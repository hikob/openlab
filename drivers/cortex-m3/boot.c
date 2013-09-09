/*
 * This file is part of HiKoB Openlab.
 *
 * HiKoB Openlab is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, version 3.
 *
 * HiKoB Openlab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with HiKoB Openlab. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2011,2012 HiKoB.
 */

/*
 * boot.c
 *
 *  Created on: Jul 5, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdint.h>
#include <errno.h>
#include <sys/stat.h>
#include "boot.h"
#include "printf.h"
#include "debug.h"
#include "cm3_scb_registers.h"

/* We need from the linker the top of stack */
extern uint32_t _estack;
/* We need from the linker the beginning and end of the data section */
extern uint32_t _sdata, _edata;
/* We need from the linker the beginning and end of the bss section */
extern uint32_t _sbss, _ebss;
/* We need from the linker the end of the text section (containing the initialized data) */
extern uint32_t _etext;
/* Defined by the linker */
extern int _end;

/* Extern function needed */
extern int main();
#ifdef __cplusplus
extern void __libc_init_array();
#endif

/**
 * Define the Reset handler.
 * It initializes the bss and data sections of the RAM
 */
void __attribute__((naked)) reset()
{
    uint32_t *src, *dst;

    // Loop over the data section to copy initialized code from _etext
    src = &_etext;
    dst = &_sdata;

    while (dst < &_edata)
    {
        *dst++ = *src++;
    }

    // loop over the bss and clear
    dst = &_sbss;

    while (dst < &_ebss)
    {
        *dst++ = 0;
    }

    /* Call CTORS of static objects */
#ifdef __cplusplus
    __libc_init_array();
#endif

    // call the main function
    main();

    log_error("\n\n***** End of main *****");

    HALT();
}

/**
 * Define the default empty handler
 */
void debug_handler(const char *name)
{
    log_error("\n\n**********************\n"
              "*** Unhandled ISR ****\n"
              "*** %s ****\n"
              "**********************\n", name);

    HALT();
}

void hardfault_debug(uint32_t *hardfault_args)
{
#if RELEASE == 0
    volatile uint32_t stacked_r0;
    volatile uint32_t stacked_r1;
    volatile uint32_t stacked_r2;
    volatile uint32_t stacked_r3;
    volatile uint32_t stacked_lr;
    volatile uint32_t stacked_pc;

    stacked_r0 = ((uint32_t) hardfault_args[0]);
    stacked_r1 = ((uint32_t) hardfault_args[1]);
    stacked_r2 = ((uint32_t) hardfault_args[2]);
    stacked_r3 = ((uint32_t) hardfault_args[3]);
    stacked_lr = ((uint32_t) hardfault_args[5]);
    stacked_pc = ((uint32_t) hardfault_args[6]);

    log_printf("\n\n");
    log_printf("******** HARD FAULT *********\n");
    log_printf("Caused at pc: \t%x\tlr: %x\n", stacked_pc, stacked_lr);
    log_printf("\tr0: %x\n", stacked_r0);
    log_printf("\tr1: %x\n", stacked_r1);
    log_printf("\tr2: %x\n", stacked_r2);
    log_printf("\tr3: %x\n", stacked_r3);

    // Get the CFSR value for analysis
    uint32_t cfsr = *cm3_scb_get_CFSR();
    log_printf("*****************************\n");
    log_printf("Looking for Memory Management fault...\n");

    if (cfsr & CM3_SCB_CFSR__IACCVIOL)
    {
        log_printf("\tInstruction Access Violation\n");
    }

    if (cfsr & CM3_SCB_CFSR__DACCVIOL)
    {
        log_printf("\tData Access Violation\n");
    }

    if (cfsr & CM3_SCB_CFSR__MMFARVALID)
    {
        log_printf("\tFaulty Address: %x\n", *cm3_scb_get_MMFAR());
    }

    log_printf("done!\n");

    log_printf("*****************************\n");
    log_printf("Looking for Bus fault...\n");

    if (cfsr & CM3_SCB_CFSR__IBUSERR)
    {
        log_printf("\tInstruction Bus Error\n");
    }

    if (cfsr & CM3_SCB_CFSR__PRECISERR)
    {
        log_printf("\tPrecise Data Bus Error\n");
    }

    if (cfsr & CM3_SCB_CFSR__IMPRECISERR)
    {
        log_printf("\tImprecise Data Bus Error\n");
    }

    if (cfsr & CM3_SCB_CFSR__BFARVALID)
    {
        log_printf("\tFaulty Address: %x\n", *cm3_scb_get_BFAR());
    }

    log_printf("done!\n");

    log_printf("*****************************\n");
    log_printf("Looking for Usage fault...\n");

    if (cfsr & CM3_SCB_CFSR__UNDEFINSTR)
    {
        log_printf("\tUndefined Instruction Error\n");
    }

    if (cfsr & CM3_SCB_CFSR__INVSTATE)
    {
        log_printf("\tInvalid State Error\n");
    }

    if (cfsr & CM3_SCB_CFSR__INVPC)
    {
        log_printf("\tInvalid PC load Error\n");
    }

    if (cfsr & CM3_SCB_CFSR__UNALIGNED)
    {
        log_printf("\tUnaligned access Error\n");
    }

    if (cfsr & CM3_SCB_CFSR__DIVBYZERO)
    {
        log_printf("\tDivide by Zero Error\n");
    }

    log_printf("done!\n");

    log_printf("*****************************\n");
#endif // RELEASE == 0
    HALT();
}

void __attribute__((naked, weak)) hardfault_handler()
{
    __asm("MOVS   R0, #4  \n"
          "MOV    R1, LR  \n"
          "TST    R0, R1  \n"
          "BEQ    _MSP    \n"
          "MRS    R0, PSP \n"
          "B      hardfault_debug      \n"
          "_MSP:  \n"
          "MRS    R0, MSP \n"
          "B      hardfault_debug      \n");
}

DEBUG_HANDLER(nmi_handler)
;
DEBUG_HANDLER(memmanage_handler)
;
DEBUG_HANDLER(busfault_handler)
;
DEBUG_HANDLER(usagefault_handler)
;
DEBUG_HANDLER(svc_handler)
;
DEBUG_HANDLER(debugmon_handler)
;
DEBUG_HANDLER(pendsv_handler)
;
DEBUG_HANDLER(systick_handler)
;

/*
 * Place the vector handlers pointers in order at the beginning
 * of the .vectors section
 */
__attribute__((section(".vectors"))) const vector_t vectors[] =
{
    // Address 0x0: stack pointer
    (vector_t) &_estack,

    // Then the handlers
    reset, nmi_handler, hardfault_handler, memmanage_handler,
    busfault_handler, usagefault_handler, 0, // 0x1C: reserved
    0, // 0x20: reserved
    0, // 0x24: reserved
    0, // 0x28: reserved
    svc_handler, debugmon_handler, 0, // 0x34: reserved
    pendsv_handler, systick_handler

    // Then the ISRs (STM32 specific)
};

int _kill(int pid, int sig)
{
    (void) pid;
    (void) sig;
    errno = EINVAL;
    return -1;
}

void _exit(int status)
{
    (void) status;

    while (1)
    {
        ;
    }
}

int _getpid(void)
{
    return 1;
}

caddr_t _sbrk(int incr)
{
    static unsigned char *heap;
    unsigned char *prev_heap;

    if (heap == 0)
    {
        heap = (unsigned char *) &_end;
    }

    prev_heap = heap;

    heap += incr;
    return (caddr_t) prev_heap;
}

int _close(int file)
{
    (void) file;
    return -1;
}

int _fstat(int file, struct stat *st)
{
    (void) file;
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file)
{
    (void) file;
    return 1;
}

int _lseek(int file, int ptr, int dir)
{
    (void) file;
    (void) ptr;
    (void) dir;
    return 0;
}

int _read(int file, char *ptr, int len)
{
    (void) file;
    (void) ptr;
    (void) len;
    return 0;
}

void __attribute__((weak)) xputc(char c)
{
    (void) c;
}

int _write(int file, char *ptr, int len)
{
    (void) file;
    int i;

    for (i = 0; i < len; i++)
    {
        xputc(*ptr++);
    }

    return len;
}
