#ifndef UART_H
#define UART_H

#include "stm32l475xx.h"
#include <string.h>

void uart_init() ;
void uart_putchar(uint8_t c) ;
uint8_t uart_getchar() ;
void uart_puts(char *s) ;
void uart_puts_sized(const unsigned char *s, size_t size) ;
void uart_gets(unsigned char *s, size_t size) ;
int32_t sum();

#endif //UART_H