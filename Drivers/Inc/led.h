#ifndef LED_H
#define LED_H

#include<stdint.h>

//UTILE POUR LA PARTIE GPIO
/*
#define RCC  0x40021000
#define RCC_AHB2ENR (*(volatile uint32_t*)(RCC + 0x4C))
#define GPIOB 0x48000400
#define GPIOB_MODER (*(volatile uint32_t*)(GPIOB))
#define GPIOB_BSRR (*(volatile uint32_t*)(GPIOB + 0x18))
#define GPIOC 0x48000800
#define GPIOC_MODER (*(volatile uint32_t*)(GPIOC))
#define GPIOC_BSRR (*(volatile uint32_t*)(GPIOC + 0x18))*/

typedef enum{
    LED_OFF,
    LED_YELLOW,
    LED_BLUE
}state_t;

void led_init();
void led_g_on();
void led_g_off();
void led(state_t state);

#endif //LED_H
