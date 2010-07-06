/***********************************************************************************
*  Copyright (c) 2010 by Michael Fischer. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*  
*  1. Redistributions of source code must retain the above copyright 
*     notice, this list of conditions and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the 
*     documentation and/or other materials provided with the distribution.
*  3. Neither the name of the author nor the names of its contributors may 
*     be used to endorse or promote products derived from this software 
*     without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
*  THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
*  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
*  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
*  SUCH DAMAGE.
*
************************************************************************************
*  History:
*
*  18.02.10  mifi   First Version, this file based on an anglia example 
*                   and was modified for the LPC1768.
***********************************************************************************/

/***********************************************************************************
*  Copyright 2005 Anglia Design
*  This demo code and associated components are provided as is and has no warranty,
*  implied or otherwise.  You are free to use/modify any of the provided
*  code at your own risk in your applications with the expressed limitation
*  of liability (see below)
* 
*  LIMITATION OF LIABILITY:   ANGLIA OR ANGLIA DESIGNS SHALL NOT BE LIABLE FOR ANY
*  LOSS OF PROFITS, LOSS OF USE, LOSS OF DATA, INTERRUPTION OF BUSINESS, NOR FOR
*  INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES OF ANY KIND WHETHER UNDER
*  THIS AGREEMENT OR OTHERWISE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*
*  Author         : Spencer Oliver
*  Web         : www.anglia-designs.com
*
***********************************************************************************/

/* Stack Sizes */

   _STACKSIZE = 512;
   _HEAPSIZE = 64;

/* Memory Definitions */

MEMORY
{
    flash (rx) : ORIGIN = 0x00000000, LENGTH = 512K
    ram (rwx)  : ORIGIN = 0x10000000, LENGTH = 32K
    usbram   : ORIGIN = 0x2007c000, LENGTH = 16K
    ethram   : ORIGIN = 0x20080000, LENGTH = 16K
}

/* Section Definitions */

SECTIONS
{
   /* first section is .text which is used for code */

   .text :
   {
      CREATE_OBJECT_SYMBOLS
      KEEP(*(.vectors))
      *(.text .text.*)
      *(.gnu.linkonce.t.*)
      *(.glue_7t) *(.glue_7) *(.vfp11_veneer)
      KEEP(*(.fini))
      *(.gcc_except_table)
   } >flash =0
   . = ALIGN(4);

   /* .ctors .dtors are used for c++ constructors/destructors */
   
   .ctors :
   {
      PROVIDE(__ctors_start__ = .);
      KEEP(*(SORT(.ctors.*)))
      KEEP(*(.ctors))
      PROVIDE(__ctors_end__ = .);
   } >flash

   .dtors :
   {
      PROVIDE(__dtors_start__ = .); 
      KEEP(*(SORT(.dtors.*)))
      KEEP(*(.dtors))
      PROVIDE(__dtors_end__ = .);
   } >flash
   
   /* .rodata section which is used for read-only data (constants) */

   .rodata :
   {
      *(.rodata .rodata.*)
      *(.gnu.linkonce.r.*)
   } >flash
   . = ALIGN(4);

   .init_array :
   {
      *(.init)
        *(.fini)
      PROVIDE_HIDDEN (__preinit_array_start = .);
      KEEP (*(.preinit_array))
      PROVIDE_HIDDEN (__preinit_array_end = .);
      PROVIDE_HIDDEN (__init_array_start = .);
      KEEP (*(SORT(.init_array.*)))
      KEEP (*(.init_array))
      PROVIDE_HIDDEN (__init_array_end = .);
      PROVIDE_HIDDEN (__fini_array_start = .);
      KEEP (*(.fini_array))
      KEEP (*(SORT(.fini_array.*)))
      PROVIDE_HIDDEN (__fini_array_end = .);
   } >flash

   . = ALIGN(4);

   /* .ARM.exidx is sorted, so has to go in its own output section.  */
   __exidx_start = .;
   .ARM.exidx :
   {
      *(.ARM.exidx* .gnu.linkonce.armexidx.*)
   } >flash
   __exidx_end = .;

   _etext = .;
   PROVIDE (etext = .);

   /* .data section which is used for initialized data */

   .data : AT (_etext)
   {
      __data_start = .;
      *(.data .data.*)
      *(.gnu.linkonce.d.*)
      SORT(CONSTRUCTORS)
      . = ALIGN(4);
      *(.fastrun .fastrun.*)
   } >flash
   . = ALIGN(4);
   
   _edata = .;
   PROVIDE (edata = .);

   /* .bss section which is used for uninitialized data */

   .bss :
   {
      __bss_start = .;
      __bss_start__ = .;
      *(.bss .bss.*)
      *(.gnu.linkonce.b.*)
      *(COMMON)
      . = ALIGN(4);
   } >flash
   __bss_end__ = .;
   
   _end = .;
   PROVIDE(end = .);

   /* .heap section which is used for memory allocation */
   
   .heap (NOLOAD) :
   {
      __heap_start__ = .;
      *(.heap)
      . = MAX(__heap_start__ + _HEAPSIZE , .);
   } >ram
   __heap_end__ = __heap_start__ + SIZEOF(.heap);
   
   /* .stack section - user mode stack */
   
   .stack (__heap_end__ + 3) / 4 * 4 (NOLOAD) :
   {
      . = ALIGN(8);
      __stack_start__ = .;
      *(.stack)
      . = ALIGN(8);
      . = MAX(__stack_start__ + _STACKSIZE , .);
   } >ram
   __stack_end__ = __stack_start__ + SIZEOF(.stack);

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
}
