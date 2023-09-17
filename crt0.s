/*
    crt0.s for exection in RAM
*/


    .syntax unified
    .global _start 
    .thumb

_start:
    ldr sp, =_STACK
    bl init_bss
    bl main

_exit:
    b _exit



/*
    SERANDOUR
*/
/*
.syntax unified
.cpu cortex-m4
.arch armv7-m
.thumb


.global _start

.section .boottext, "x"
// setup the stack 
// then call main
.thumb_func
_start:
    ldr sp, =_STACK
    bl init_bss
    bl init_data
    bl init_code
    ldr r0, =main 
    ldr lr, _exit
    mov pc, r0
_exit:
    b .
*/