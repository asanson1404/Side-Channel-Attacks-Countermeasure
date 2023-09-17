#include "../Inc/trng.h"
#include "stm32l475xx.h"

void trng_init() {

    // True random number generator is enabled.
    RNG->CR |= RNG_CR_RNGEN;

}

unsigned char get_random() {

    // Wait for a valid value
    while((RNG->SR & RNG_SR_DRDY_Msk) == 0);

    // Return a 32-bit random value
    return ((RNG->DR % 256) & 0xff);

}