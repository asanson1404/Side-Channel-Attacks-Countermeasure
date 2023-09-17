/* LDSCRIPT FOR EXECUTION IN RAM */

ENTRY(_start)

MEMORY {
    rom     (rx) : ORIGIN = 0X08000000, LENGTH = 1M
    sram1   (wx) : ORIGIN = 0X20000000, LENGTH = 96K
    sram2   (wx) : ORIGIN = 0X10000000, LENGTH = 32K
}

_STACK = ORIGIN(sram2) + LENGTH(sram2) ;

SECTIONS {
    .text : {
        *(.text)
        _etext = . ;
    } > sram1

    .rodata : {
        *(.rodata)
    } > sram1

    .data : {
        *(.data)
    } > sram1

    .bss : {
        _bstart = . ;
        *(.bss) *(COMMON)
        _bend = . ;
    } > sram1
}
