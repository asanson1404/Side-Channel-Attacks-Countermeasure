#include "../Inc/uart.h"

void uart_init(){

    //activation horloge portB
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN ;

    //activation horloge port série USART1
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN ;
    //PCLK est selectionnée comme l'horloge d'USART1
    RCC->CCIPR &= ~RCC_CCIPR_USART1SEL_Msk ;

    //Désactivation USART1
    USART1->CR1 = 0 ;
    USART1->CR2 = 0 ;

    //Pin PB6 (TX de l'USART1) en mode alternate fonction
    GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODE6_Msk) | GPIO_MODER_MODE6_1 ;
    //Alternate fonction 7 pour l'utiliser en mode USART1_TX
    GPIOB->AFR[0] = (GPIOB->AFR[0] & ~GPIO_AFRL_AFSEL6_Msk) | (7<<GPIO_AFRL_AFSEL6_Pos) ;
    

    //Pin PB7 (RX de l'USART1) en mode alternate fonction
    GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODE7_Msk) | GPIO_MODER_MODE7_1 ;
    //Alternate fonction 7 pour l'utiliser en mode USART1_RX
    GPIOB->AFR[0] = (GPIOB->AFR[0] & ~GPIO_AFRL_AFSEL7_Msk) | (7<<GPIO_AFRL_AFSEL7_Pos) ;

    //Reset du port série USART1 (précaution)
    RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST ;
    //Dereset du port série USART (essentiel pour une bonne mise en oeuvre)
    RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST_Msk ;

    //vitesse du port série à 115200 bauds
    USART1->BRR = 80000000/115200 ;

    //Oversampling à 16
    USART1->CR1 &= ~USART_CR1_OVER8_Msk ;
    //Configurer le port série en mode 8 bits de data
    USART1->CR1 &= ~USART_CR1_M_Msk ;
    //Désactiver le bit de contrôle de parité
    USART1->CR1 &= ~USART_CR1_PCE_Msk ;
    //Configurer le port série avec 1 bit stop
    USART1->CR2 &= ~USART_CR2_STOP_Msk ;

    //Activation USART1
    USART1->CR1 |= USART_CR1_UE ;
    //Activation du transmetteur
    USART1->CR1 |= USART_CR1_TE ;
    //Activation du récepteur 
    USART1->CR1 |= USART_CR1_RE ;

}

void uart_putchar(uint8_t c){
    //Tant que TXE n'est pas à 1, on attend
    while(!(USART1->ISR & USART_ISR_TXE_Msk));
    USART1->TDR = c ;
}

uint8_t uart_getchar(){
    //Lorsque RXNE est à 1, la data est prête à être lue
    while(!(USART1->ISR & USART_ISR_RXNE_Msk));
    return (USART1->RDR & 0xFF);
}

void uart_puts(char *s){
    int i = 0 ;
    while(s[i] != '\0'){
        uart_putchar(s[i]);
        i++;
    }
}

void uart_puts_sized(const unsigned char *s, size_t size){
    for(size_t i = 0; i< size; i++) {
        uart_putchar(s[i]);
    }
}

void uart_gets(unsigned char *s, size_t size){
    for(size_t i = 0; i< size; i++){
        s[i] = uart_getchar() ;
        if(s[i] == 13){
            s[i] = '\0' ;
            break;
        }
    } 
}

int32_t sum(){
    int32_t s = 0;
    //on ne pourra envoyer que maximum 1000 octets (default)
    for(int i = 0; i<1000; i++){
        s += uart_getchar();
    }
    return s ;
}