/* 
 * GCC ARM common linker script
 * $URL$
 * $ID$

Copyright (c) 2009, 32bitmicro
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution.
Neither the name of the 32bitmicro nor the names of its contributors may be used to endorse or promote products derived from this software without 
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
ENTRY(_start)
SEARCH_DIR(.)

MEMORY
{
    flash (rx) : ORIGIN = 0x00000000, LENGTH = 512K
    ram (rwx)  : ORIGIN = 0x10000000, LENGTH = 32K
    usbram   : ORIGIN = 0x2007c000, LENGTH = 16K
    ethram   : ORIGIN = 0x20080000, LENGTH = 16K
}

_ram_end = ORIGIN(ram) + LENGTH(ram) - 4;

SECTIONS
{
  .text :
  {
    _text = . ;
    _vector = . ;
    KEEP(*(.isr_vector))
    _evector = . ;

    /* NXP only CRP location */
    . = DEFINED(CRP_Key) ?  0x02FC : . ;
    KEEP (*(.CRP.*))
    . = ALIGN(4);

    KEEP (*(.text.handler.*))

    *(.text .text.* .gnu.linkonce.t.*)
    *(.plt)
    *(.gnu.warning)
    *(.glue_7t) *(.glue_7) *(.vfp11_veneer)

    *(.ARM.extab* .gnu.linkonce.armextab.*)
    *(.gcc_except_table)
  } >flash

  .eh_frame_hdr : ALIGN (4)
  {
    KEEP (*(.eh_frame_hdr))
  } >ram

  .eh_frame : ALIGN (4)
  {
    KEEP (*(.eh_frame))
  } >flash
  
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

/*    . = ALIGN(8); */
/*    _etext = .; */
  } >flash

  __exidx_start = .;
  .ARM.exidx :
  {
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  } >flash
  __exidx_end = .; 

  . = ALIGN(8);
   _etext = .;

  . = ALIGN(8);
  _ldata = ABSOLUTE(.);

  .data :  AT (_etext)
  {
    _data = .;
   /* LOADADDR (.data); */
    KEEP(*(.jcr))
    *(.got.plt) *(.got)
    *(.shdata)
    *(.data .data.* .gnu.linkonce.d.*)
    . = ALIGN (8);
    *(.ram)
    _edata = .;
  } >ram


  .bss :
  {
    _bss = .;
    *(.shbss)
    *(.bss .bss.* .gnu.linkonce.b.*)
    _ebss = .;
    *(COMMON)
    . = ALIGN(8);
    *(.ram.b)
  } >ram

  .heap :
  {
   . = ALIGN(8);
    _heap = .;
    *(.heap)
    _eheap = .;
  } >ram

  .stack :
  {
    . = ALIGN(8);
    _stack = .;
   *(.stack)
   
  } >ram

  . = _ram_end - 16;
  /* for compatibility */
  estack = .;
  _estack = .;
  . = _ram_end;
  _end = .;
   __end = .;
  
  .usbram (NOLOAD):
  {
  __usbram_beg__ = .;
  *(.dmaram)
    __usbram_end__ = .;
  } >usbram

  .ethram (NOLOAD):
  {
  __ethram_beg__ = .;
  *(.ethram)
    __ethram_end__ = .;
  } >ethram

  .stab 0 (NOLOAD) : { *(.stab) }
  .stabstr 0 (NOLOAD) : { *(.stabstr) }
  /* DWARF debug sections.
   * Symbols in the DWARF debugging sections are relative to the beginning
   * of the section so we begin them at 0.  */
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
  /* DWARF 2.1 */
  .debug_ranges   0 : { *(.debug_ranges) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }

  .note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
  .ARM.attributes 0 : { KEEP (*(.ARM.attributes)) }
  /DISCARD/ : { *(.note.GNU-stack) }
}
