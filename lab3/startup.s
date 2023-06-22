#/-------------------------------------------------------------------------------------------------
#/ 4190.308 Computer Architecture                                                       Spring 2023
#/
#// @file
#// @brief Simulator startup code to allow linking with C code
#//        Sets stack pointer to 0x80020000 and calls main with arguments 0, NULL, i.e.,
#//        'main(0, NULL)'
#//        Adjust linker script link.ld and stack base address as necessary.
#//
#// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
#// @section changelog Change Log
#// 2023/05/02 Bernhard Egger created
#//
#// @section license_section License
#// Copyright (c) 2023, Computer Systems and Platforms Laboratory, SNU
#/-------------------------------------------------------------------------------------------------

    .text
    .align 4

    .globl _start

_start:
    lui     sp, 0x80120     # set stack pointer to 0x80120000

    li      a0, 0           # no arguments (argc=0)
    li      a1, 0           # no arguments (argv=NULL)
    call    main

    ebreak
