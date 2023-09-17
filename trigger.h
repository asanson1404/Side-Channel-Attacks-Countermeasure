#ifndef TRIGGER_H
#define TRIGGER_H

#include "cmsis/stm32l475xx.h"

/*
    /!\ TRIGGER ON PIN PA4 /!\
    Configure an output pin to have a trigger
    Usefull yo know when the encryption has started
*/

void trigger_init();
void trigger_high();
void trigger_low();

#endif