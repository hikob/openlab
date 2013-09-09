/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

// see http://lwip.wikia.com/wiki/Porting_for_an_OS for instructions
#include <stdint.h>
#include "platform.h"

#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/sys.h"
#include "lwip/stats.h"

#include "arch/cc.h"
#include "arch/sys_arch.h"

#include "debug.h"

void sys_init(void)
{
    log_debug("Init!");
}

err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
    log_debug("Sem NEW %u", count);

    *sem = xSemaphoreCreateCounting(0xFF, count);
    if (*sem == 0)
    {
        log_error("Sem NEW failed");
        SYS_STATS_INC(sem.err);
        return ERR_MEM;
    }
    else
    {
        SYS_STATS_INC_USED(sem);
        return ERR_OK;
    }
}

void sys_sem_free(sys_sem_t *sem)
{
    log_debug("Sem FREE");

    vQueueDelete(*sem);
    *sem = SYS_SEM_NULL;
    SYS_STATS_DEC(sem.used);
}

void sys_sem_signal(sys_sem_t *sem)
{
    xSemaphoreGive(*sem);
}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
    portTickType time, tmo;

    tmo = (timeout > 0) ? (configTICK_RATE_HZ * timeout) / 1000 : portMAX_DELAY;
    time = xTaskGetTickCount();

    if (xSemaphoreTake(*sem, tmo) != pdTRUE)
    {
        time = SYS_ARCH_TIMEOUT;
    }
    else
    {
        time = xTaskGetTickCount() - time;
        time = (time * 1000) / configTICK_RATE_HZ;
    }

    return time;
}

int sys_sem_valid(sys_sem_t *sem)
{
    return *sem != SYS_SEM_NULL;
}

// typically called within lwIP after freeing a semaphore
// to make sure the pointer is not left pointing to invalid data
void sys_sem_set_invalid(sys_sem_t *sem)
{
    *sem = SYS_SEM_NULL;
}

err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
    log_debug("Mbox NEW %u", size);

    *mbox = xQueueCreate(size, sizeof(void*));
    if (*mbox == NULL)
    {
        log_error("Mbox NEW failed");
        SYS_STATS_INC(mbox.err);
        return ERR_MEM;
    }
    else
    {
        SYS_STATS_INC(mbox.used);
        return ERR_OK;
    }
}

void sys_mbox_free(sys_mbox_t *mbox)
{
    log_debug("Mbox FREE");

    vQueueDelete(*mbox);
    *mbox = SYS_MBOX_NULL;
    SYS_STATS_DEC(mbox.used);
}

void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
    xQueueSendToBack(*mbox, &msg, portMAX_DELAY);
}

err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    if (xQueueSendToBack(*mbox, &msg, 0) != pdTRUE)
    {
        SYS_STATS_INC(mbox.err);
        return ERR_MEM;
    }
    return ERR_OK;
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
    portTickType time, tmo;

    tmo = (timeout > 0) ? (timeout * configTICK_RATE_HZ / 1000) : portMAX_DELAY;
    time = xTaskGetTickCount();
    if (xQueueReceive(*mbox, msg, tmo) != pdTRUE)
    {
        time = SYS_ARCH_TIMEOUT;
    }
    else
    {
        time = xTaskGetTickCount() - time;
        time = (time * 1000) / configTICK_RATE_HZ;
    }

    return time;
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    if (xQueueReceive(*mbox, msg, 0) != pdTRUE)
    {
        return SYS_MBOX_EMPTY;
    }
    return 0;
}

int sys_mbox_valid(sys_mbox_t *mbox)
{
    return *mbox != SYS_MBOX_NULL;
}

// typically called within lwIP after freeing an mbox
// to make sure the pointer is not left pointing to invalid data
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
    *mbox = SYS_MBOX_NULL;
}

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg,
        int stacksize, int prio)
{
    log_debug("Thread NEW");

    sys_thread_t task;
    if (xTaskCreate( thread, ( const signed portCHAR * ) name, stacksize, arg, prio, &task )
            != pdTRUE)
    {
        log_error("Thread NEW failed");
        return NULL;
    }
    else
    {
        return task;
    }
}

sys_prot_t sys_arch_protect(void)
{
    vPortEnterCritical();
    return 0;
}

void sys_arch_unprotect(sys_prot_t pval)
{
    vPortExitCritical();
}
