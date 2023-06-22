/*-------------------------------------------------------------------------------------------------
// 4190.308 Computer Architecture                                                       Spring 2023
//
/// @file
/// @brief Linker script
///        Instruction memory: 0x80000000 - 0x8000ffff
///        Data memory:        0x80100000 - 0x802fffff
///        Must match startup.s
///
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2023/05/02 Bernhard Egger created
///
/// @section license_section License
/// Copyright (c) 2023, Computer Systems and Platforms Laboratory, SNU
//-----------------------------------------------------------------------------------------------*/



OUTPUT_ARCH( "riscv" )
ENTRY(_start)

SECTIONS
{
  . = 0x80000000;
  .text.init : { *(.text.init) }
  . = ALIGN(0x1000);
  .tohost : { *(.tohost) }
  . = ALIGN(0x1000);
  .text : { *(.text) }
  . = 0x80100000;
  .data : { *(.data) }
  .bss : { *(.bss) }
  _end = .;
}

