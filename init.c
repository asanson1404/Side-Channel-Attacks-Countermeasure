#include <stdint.h>

void init_bss(){

    extern uint8_t _bstart;
    extern uint8_t _bend;
    uint8_t * p = &_bstart;

    while( p < &_bend ){
        *p++ = 0;
    }
}

