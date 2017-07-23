#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "ringbuffer.h"

#define BAUD    57600
#define BRC     ((F_CPU / 16 / BAUD) - 1)

#define T_CLK   PD2
#define T_CS    PD3
#define T_D     PD4
#define S_TEST  PD5

#define DELAY_INTERVAL 1000

volatile ringbuffer_t tx_buffer = {{}, 0, 0};

void init(void);
void write_string_serial(char c[]);
uint16_t read_temperature(void);
void run_in_test_mode(void);

int main(void) {
    char celsius_string[10] = "";
    init();
    
    int test_mode = (PIND & (1 << S_TEST)) != 0;
    
    while (1) {
        if (test_mode) {
            run_in_test_mode();
        }
        else {
            // sample thermocouple
            uint16_t sensor_value = read_temperature();
            
            if (sensor_value == -1) {
                write_string_serial("-1\n");
            }
            else {
                float celsius = sensor_value * 0.25;
                char celsius_string[10];
                dtostrf(celsius, 7, 2, celsius_string);
            
                // transmit temperature
                write_string_serial(celsius_string);
                write_string_serial("\n");
            }
            
            _delay_ms(DELAY_INTERVAL);
        }
    }
    
    return 0;
}

void init(void) {
    
    //set up thermocouple
    DDRD = (1 << T_CLK) | (1 << T_CS);
    
    // set initial state (CS->High, CLK->Low)
    PORTD |= _BV(T_CS);
    PORTD &= ~_BV(T_CLK);
    
    // set up serial transmission
    UBRR0H = (BRC >> 8);
    UBRR0L = BRC;
    
    UCSR0B = (1 << TXEN0) | (1 << TXCIE0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    
    // enable global interrupts
    sei();
}

uint16_t read_temperature(void) {
    uint16_t value = 0;
    
    // initialize conversion process on MAX6675
    PORTD &= ~_BV(T_CS);
    _delay_ms(2);
    PORTD |= _BV(T_CS);
    _delay_ms(220);
    
    // start the data reading process
    PORTD &= ~_BV(T_CS);
    
    // bounce the clock to read dummy bit 15
    PORTD |= _BV(T_CLK);
    _delay_ms(1);
    PORTD &= ~_BV(T_CLK);
    
    // read the rest of the 14 bits
    for (int i = 14; i >= 0; i--) {
        PORTD |= _BV(T_CLK);
        value |= ((PIND & (1 << T_D)) != 0) << i;
        PORTD &= ~_BV(T_CLK);
    }
    
    // check if sensor is present
    if ((value & 0x04) == 0x04) {
        return -1;
    }
    
    return value >> 3;
}

void write_string_serial(char c[]) {
    for (uint8_t i = 0; i < strlen(c); i++) {
        buffer_add(&tx_buffer, c[i]);
    }
    
    // kick off the interrupt initially (and after an empty buffer is written to)
    if (UCSR0A & (1 << UDRE0)) {
        UDR0 = 0;
    }
}

ISR(USART_TX_vect) {
    uint8_t byte = 0;
    buffer_get(&tx_buffer, &byte);
    // HAXX
    if (byte != 0) {
        UDR0 = byte;
    }
}

void run_in_test_mode(void) {
    char test_celsius_string[10];
    int i;
    
    for (i = 30; i <= 190; i++) {
        float celsius = i * 1.0;
        dtostrf(celsius, 7, 2, test_celsius_string);
        
        // transmit temperature
        write_string_serial(test_celsius_string);
        write_string_serial("\n");
        
        if (i == 190) {
            i = 30;
        }
        
        _delay_ms(DELAY_INTERVAL);
    }
}
