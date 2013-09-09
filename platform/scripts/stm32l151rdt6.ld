/* STM32L262RBT6 linker script */
ENTRY (reset)
/*
There will be a link error if there is not this amount of RAM free at the end.
*/
_Minimum_Stack_Size = 0x100 ;

/* Memory Spaces Definitions */
MEMORY
{
  ram      (rwx) : ORIGIN = 0x20000000, LENGTH = 48K
  flash    (rx)  : ORIGIN = 0x08000000, LENGTH = 384K
}
/* higher address of the user mode stack */
_estack = 0x20004000;

/* Enforce emition of the vector table. */
EXTERN (vectors)
EXTERN (usr_vectors)

/* Define sections. */
SECTIONS
{
	. = ORIGIN(flash);
	
	.text : 
	{
		. = ALIGN(4);
		*(.vectors)     /* Vector table: Cortex-M3 handlers */
		*(.usr_vectors)	/* Vector table: STM32-specific handlers */
		*(.text .text* .gnu.linkonce.t.*)
		*(.plt)
		*(.gnu.warning)
		*(.glue_7t) *(.glue_7) *(.vfp11_veneer)
		
		*(.ARM.extab* .gnu.linkonce.armextab.*)
		*(.gcc_except_table)
	} >flash
	
	.eh_frame_hdr : ALIGN (4)
	{
		KEEP (*(.eh_frame_hdr))
	} >flash
	
	.eh_frame : ALIGN (4)
	{
		KEEP (*(.eh_frame))
	} >flash
	
	/* .ARM.exidx is sorted, so has to go in its own output section.  */
	__exidx_start = .;
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} >flash
	__exidx_end = .;
	
	
	.rodata : ALIGN (4)
	{
		*(.rodata .rodata.* .gnu.linkonce.r.*)
		
		. = ALIGN(4);
		KEEP(*(.init))
		
		. = ALIGN(4);
		__preinit_array_start = .;
		KEEP (*(.preinit_array))
		__preinit_array_end = .;
		
		. = ALIGN(4);
		__init_array_start = .;
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array))
		__init_array_end = .;
		
		. = ALIGN(4);
		KEEP(*(.fini))
		
		. = ALIGN(4);
		__fini_array_start = .;
		KEEP (*(.fini_array))
		KEEP (*(SORT(.fini_array.*)))
		__fini_array_end = .;
		
		. = ALIGN(4);
		KEEP (*crtbegin.o(.ctors))
		KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
		KEEP (*(SORT(.ctors.*)))
		KEEP (*crtend.o(.ctors))
		
		. = ALIGN(4);
		KEEP (*crtbegin.o(.dtors))
		KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
		KEEP (*(SORT(.dtors.*)))
		KEEP (*crtend.o(.dtors))
		
		. = ALIGN(4);
		
		/* Marker to the end of the text section */
		_etext = .;
		_sidata = _etext;
	} >flash

	.data : AT ( _sidata )
	{
		. = ALIGN(4);
		/* Marker to the beginning of the data section */
		_sdata = .;
		
		KEEP(*(.jcr))
		*(.got.plt) *(.got)
		*(.shdata)
		*(.data .data.* .gnu.linkonce.d.*)
		
		. = ALIGN(4);
		/* Marker to the end of the data section */
		_edata = .;
	} >ram

	.bss : ALIGN(4)
	{
		/* Marker to the beginning of the bss section */
		_sbss = .;
		
		*(.shbss)
		*(.bss .bss.* .gnu.linkonce.b.*)
		*(COMMON)
		
		. = ALIGN(4);
		/* Marker to the end of the bss section */
		_ebss = . ;
	} >ram

	PROVIDE ( end = _ebss );
	PROVIDE ( _end = _ebss );
	
	/* This is the user stack section 
	This is just to check that there is enough RAM left for the User mode stack
	It should generate an error if it's full.
	 */
	._usrstack :
	{
		. = ALIGN(4);
		_susrstack = . ;
		
		. = . + _Minimum_Stack_Size ;
		
		. = ALIGN(4);
		_eusrstack = . ;
	} >ram
	
	
	/* after that it's only debugging information. */
	
	/* remove the debugging information from the standard libraries */
	DISCARD :
	{
		libc.a ( * )
		libm.a ( * )
		libgcc.a ( * )
	}
	
	/* Stabs debugging sections.  */
	.stab          0 : { *(.stab) }
	.stabstr       0 : { *(.stabstr) }
	.stab.excl     0 : { *(.stab.excl) }
	.stab.exclstr  0 : { *(.stab.exclstr) }
	.stab.index    0 : { *(.stab.index) }
	.stab.indexstr 0 : { *(.stab.indexstr) }
	.comment       0 : { *(.comment) }
	/* DWARF debug sections.
	   Symbols in the DWARF debugging sections are relative to the beginning
	   of the section so we begin them at 0.  */
	/* DWARF 1 */
	.debug          0 : { *(.debug) }
	.line           0 : { *(.line) }
	/* GNU DWARF 1 extensions */
	.debug_srcinfo  0 : { *(.debug_srcinfo) }
	.debug_sfnames  0 : { *(.debug_sfnames) }
	/* DWARF 1.1 and DWARF 2 */
	.debug_aranges  0 : { *(.debug_aranges) }
	.debug_pubnames 0 : { *(.debug_pubnames) }
	/* DWARF 2 */
	.debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
	.debug_abbrev   0 : { *(.debug_abbrev) }
	.debug_line     0 : { *(.debug_line) }
	.debug_frame    0 : { *(.debug_frame) }
	.debug_str      0 : { *(.debug_str) }
	.debug_loc      0 : { *(.debug_loc) }
	.debug_macinfo  0 : { *(.debug_macinfo) }
	/* SGI/MIPS DWARF 2 extensions */
	.debug_weaknames 0 : { *(.debug_weaknames) }
	.debug_funcnames 0 : { *(.debug_funcnames) }
	.debug_typenames 0 : { *(.debug_typenames) }
	.debug_varnames  0 : { *(.debug_varnames) }
	
	note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
	.ARM.attributes 0 : { KEEP (*(.ARM.attributes)) }
	/DISCARD/ : { *(.note.GNU-stack) }
	
}
