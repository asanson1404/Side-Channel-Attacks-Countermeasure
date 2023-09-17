#include "../Inc/led.h"
#include "stm32l475xx.h"

void led_init(){

    /* SYNTAXE POUR LA PARTIE GPIO
    //activation clock PortB (bit1 à 1)
    RCC_AHB2ENR = (RCC_AHB2ENR & ~(1<<1)) | (1<<1) ;

    //activation clock PortC (bit2 à 1)
    RCC_AHB2ENR = (RCC_AHB2ENR & ~(1<<2)) | (1<<2) ;    

    //Pin PB14 in output mode
    GPIOB_MODER = (GPIOB_MODER & ~(3<<28)) | (1<<28) ;*/

    //activation clock PortB
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN ;
    //activation clock PortC
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN ;
    //Pin PB14 in output mode
    GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODE14_Msk) | (1<<GPIO_MODER_MODE14_Pos);
}

void led_g_on(){
    
    // SYNTAXE POUR LA PARTIE GPIO
    //GPIOB_BSRR = (GPIOB_BSRR & ~(1<<14)) | (1<<14) ;

    GPIOB->BSRR |= GPIO_BSRR_BS14 ; 
}

void led_g_off(){

    // SYNTAXE POUR LA PARTIE GPIO
    //GPIOB_BSRR = (GPIOB_BSRR & ~(1<<30)) | (1<<30) ;

    GPIOB->BSRR |= GPIO_BSRR_BR14 ;
}

void led(state_t state){

    switch(state){

        case LED_OFF:

            //SYNTAXE POUR LA PARTIE GPIO
            //Pin PC9 in input mode
            //GPIOC_MODER = (GPIOC_MODER & ~(3<<18)) ;

            //Pin PC9 in input mode
            GPIOC->MODER &= ~GPIO_MODER_MODE9_Msk ;
            break;

        case LED_YELLOW:

            /*SYNTAXE POUR LA PARTIE GPIO
            //Pin PC9 in output mode
            GPIOC_MODER = (GPIOC_MODER & ~(3<<18)) | (1<<18) ;
            GPIOC_BSRR = (GPIOC_BSRR & ~(1<<9)) | (1<<9) ;*/
            
            //Pin PC9 in output mode
            GPIOC->MODER |= 1<<GPIO_MODER_MODE9_Pos ;
            GPIOC->BSRR  |= 1<<GPIO_BSRR_BS9_Pos ;
            break;

        case LED_BLUE:

            /*SYNTAXE POUR LA PARTIE GPIO
            //Pin PC9 in output mode
            GPIOC_MODER = (GPIOC_MODER & ~(3<<18)) | (1<<18) ;
            GPIOC_BSRR = (GPIOC_BSRR & ~(1<<25)) | (1<<25) ;*/

            //Pin PC9 in output mode
            GPIOC->MODER |= 1<<GPIO_MODER_MODE9_Pos ;
            GPIOC->BSRR  |= 1<<GPIO_BSRR_BR9_Pos ;
            break;
    }
}