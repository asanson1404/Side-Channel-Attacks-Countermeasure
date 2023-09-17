#include "clocks.h"
#include "stm32l475xx.h"

void clocks_init(void) {

    // Power Interface Clock enable (6.4.19) 
    RCC->APB1ENR1 = RCC_APB1ENR1_PWREN;

    // Reset BKP domain if different clock source selected
    RCC->BDCR = RCC_BDCR_BDRST; // Reset the entire Backup domain
    RCC->BDCR = 0;

    // Backup domain deprotection
    PWR->CR1 |= PWR_CR1_DBP;

    // STOPWUCK = 1 To allow HSI16 oscillator to be ON
    RCC->CFGR |= RCC_CFGR_STOPWUCK;
    // HSI16 oscillator ON (16 MHz)
    RCC->CR = RCC_CR_HSION;
    // Wait until HSI16 is stable
    while ((RCC->CR & RCC_CR_HSIRDY) == 0);

    // Flash setup for selected 80MHz@1.2V speed setting (4WS)
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_4WS;

    // Update PLL
    // PLLM = 1 --> 000
    // PLLN = 10 --> 10
    // PPLLR = 2 --> 00
    // PLLDIV = unused, PLLP = unused (SAI3), PLLQ = unused (48M1)
    // SRC = HSI16 --> 10
    // PLL is (HSI freq * 10) / 2 = 80 MHz
    RCC->PLLCFGR = RCC_PLLCFGR_PLLREN | (10 << RCC_PLLCFGR_PLLN_Pos) | RCC_PLLCFGR_PLLSRC_HSI;

    // PLL activation
    RCC->CR |= RCC_CR_PLLON;

    /* Waiting for PLL lock.*/
    while ((RCC->CR & RCC_CR_PLLRDY) == 0) ;

    // RCC_CFGR is OK by defaut
    RCC->CFGR = 0;

    // CCIPR : no periph clock by default
    RCC->CCIPR = 0;

    // Switches on the PLL clock source (SYSCLK ==> PLL)
    RCC->CFGR = (RCC->CFGR  & ~RCC_CFGR_SW_Msk) | RCC_CFGR_SW_PLL;
    //// Wait until SYSCLK is stable.
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) ;

    // Enable SYSCFG clock enabled
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
}