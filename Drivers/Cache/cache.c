#include "cache.h"
#include "stm32l475xx.h"

void disable_cache(void) {

    // Disable data cache
    FLASH->ACR &= ~FLASH_ACR_DCEN; 
    // Disable instruction cache
    FLASH->ACR &= ~FLASH_ACR_ICEN;
    
}