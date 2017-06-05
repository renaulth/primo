#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "ringbuffer.h"

#define LED_BLINK_DELAY 1000

void initIO(void) {
    DDRB |= _BV(DDB5);
}

int main(void) {
	initIO();

	while (1) {
        PORTB |= _BV(PORTB5);
        _delay_ms(LED_BLINK_DELAY);
        PORTB &= ~ _BV(PORTB5);
        _delay_ms(LED_BLINK_DELAY);
    }
    
	return 0;
}
