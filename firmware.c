#include "led.h"
#include "uart.h"
#include "trigger.h"
#include "trng.h"
#include "Drivers/Clocks/clocks.h"
#include "Drivers/Cache/cache.h"
#include "Implementation/ref/skinny_reference.h"
#include "Implementation/affine_mask/skinny_aff.h"


#define P_LEN 16
#define K_LEN 48

void tempo (int t);
int echo_test(unsigned char* data, size_t size);
int skinny_ref(unsigned char* data);
int skinny_aff(unsigned char* data);
int ascii_to_hex(int len, unsigned char* ascii_buf, unsigned char* data_buf);
int hex_to_ascii(int len, unsigned char* data_buf, unsigned char* ascii_buf);


extern unsigned char r0; // Additive mask
extern unsigned char r1; // Multiplicative mask

int main(){

    clocks_init();
    trng_init();
    disable_cache();
    led_init();
    trigger_init();
    uart_init();

    int t = 0 ;
    while(t<5){
        led_g_on();
        tempo(1000000);
        led_g_off();
        tempo(1000000);
        led(LED_YELLOW);
        tempo(1000000);
        led(LED_OFF);
        tempo(1000000);
        led(LED_BLUE);
        tempo(1000000);
        led(LED_OFF);
        tempo(1000000);
        t++;
    }

    //uart_puts("En attente d'un plaintext (16 octets) et d'une clé (48 octets) :\n\r\n\r");

    unsigned char ascii_buf[2 * (P_LEN + K_LEN) + 1];
    unsigned char data_buf[P_LEN + K_LEN];

    while(1){

        /*uart_gets(ascii_buf, 2 * (P_LEN + K_LEN));
        uart_puts("Plaintext : ");
        uart_puts_sized(ascii_buf, 2 * P_LEN);
        uart_putchar('\n');
        uart_putchar('\r');
        uart_puts("Key : ");
        uart_puts_sized(ascii_buf + 2 * P_LEN, 2 * K_LEN);
        uart_putchar('\n');
        uart_putchar('\r');*/

        uart_gets(ascii_buf, 2 * (P_LEN + K_LEN) + 1);

        // 'r' as ref
        if (ascii_buf[128] == 'r') {
            ascii_to_hex((P_LEN + K_LEN), ascii_buf, data_buf);
            skinny_ref(data_buf);
        }

        // 's' as secure
        else if (ascii_buf[128] == 's') {
            ascii_to_hex((P_LEN + K_LEN), ascii_buf, data_buf);
            skinny_aff(data_buf);
        }

    }

    return 0;  
}

int skinny_ref(unsigned char* data) {

    unsigned char plaintext [P_LEN] = {0};
    unsigned char ascii_plaintext [2*P_LEN] = {0};
    const unsigned char tweakey[K_LEN] = {0};

    memcpy(plaintext, data, P_LEN);
    memcpy(tweakey, (data + P_LEN), K_LEN);

    trigger_high();
    skinny_128_384_plus_enc(plaintext, tweakey);
    trigger_low();

    hex_to_ascii(P_LEN, plaintext, ascii_plaintext);
    //uart_puts("Cyphertext : ");
    uart_puts_sized(ascii_plaintext, 2*P_LEN);
    uart_putchar('\n');

    return 0x00;
}

int skinny_aff(unsigned char* data) {

    unsigned char plaintext [P_LEN] = {0};
    unsigned char ascii_plaintext [2*P_LEN] = {0};
    const unsigned char tweakey[K_LEN] = {0};

    memcpy(plaintext, data, P_LEN);
    memcpy(tweakey, (data + P_LEN), K_LEN);

    trigger_high();
    Aff_skinny_128_384_plus_enc(plaintext, tweakey);
    trigger_low();

    hex_to_ascii(P_LEN, plaintext, ascii_plaintext);
    //uart_puts("Cyphertext : ");
    uart_puts_sized(ascii_plaintext, 2*P_LEN);
    uart_putchar('\n');

    return 0x00;
}


int echo_test(unsigned char* data, size_t size) {
    uart_gets(data, size);
    uart_puts_sized(data, size);
    uart_putchar('\n');
    uart_putchar('\r');
    return 0x00;
}


int ascii_to_hex(int len, unsigned char* ascii_buf, unsigned char* data_buf)
{
	for(int i = 0; i < len; i++)
	{
		unsigned char n_hi = ascii_buf[2*i];
		unsigned char n_lo = ascii_buf[2*i+1];

		if(n_lo <= '9')
			data_buf[i] = n_lo - '0';
		else if(n_lo >= 'A' && n_lo <= 'F')
			data_buf[i] = n_lo - 'A' + 10;
		else if(n_lo >= 'a' && n_lo <= 'f')
			data_buf[i] = n_lo - 'a' + 10;
		else
			return 1;

		if(n_hi <= '9')
			data_buf[i] |= (n_hi - '0') << 4;
		else if(n_hi >= 'A' && n_hi <= 'F')
			data_buf[i] |= (n_hi - 'A' + 10) << 4;
		else if(n_hi >= 'a' && n_hi <= 'f')
			data_buf[i] |= (n_hi - 'a' + 10) << 4;
		else
			return 1;
	}

	return 0;
}

int hex_to_ascii(int len, unsigned char* data_buf, unsigned char* ascii_buf)
{
	for(int i = 0; i < len; i++)
	{
        unsigned char hex_hi = (data_buf[i] & 0xf0) >> 4;
        unsigned char hex_lo = data_buf[i] & 0x0f;

        if (hex_hi <= 9)
            ascii_buf[2*i] = hex_hi + '0';
        else if(hex_hi >= 10 && hex_hi <= 15)
            ascii_buf[2*i] = hex_hi - 10 + 'a';
        else
            return 1;

        if (hex_lo <= 9)
            ascii_buf[2*i+1] = hex_lo + '0';
        else if(hex_lo >= 10 && hex_lo <= 15)
            ascii_buf[2*i+1] = hex_lo - 10 + 'a';
        else
            return 1;
	}

	return 0;
}

void tempo(int t){
    for (int j=0; j<t; j++){
            asm volatile("nop");
        }
}