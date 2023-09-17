#include "trigger.h"

void trigger_init() {

    // ENABLE THE GPIO PORT A CLOCK
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN ;

    // PA4 in output mode and very high speed
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE4_Msk) | (1<<GPIO_MODER_MODE4_Pos) ;
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED4 ;

    // PA4 à 0
    GPIOA->BSRR = GPIO_BSRR_BR4 ;

}

// Set PA4 to high
void trigger_high() {
    GPIOA->BSRR = GPIO_BSRR_BS4 ;
}

// Set PA4 to low
void trigger_low() {
    GPIOA->BSRR = GPIO_BSRR_BR4 ;
}