#include "main.h"
#include <avr/pgmspace.h>

void sys_sleep(uint8_t mode)
{
	uint8_t cr = MCUCR & 0xC7; // Clear sleep-related bits
	uint8_t cre = cr | _BV(SE);
	cli();
	// Clear sets mode = idle
	if (!TCCR0B) cre |= mode; // Idle needed if TCCR0B to let timer run
	MCUCR = cre; /* sleep enable and set mode */
	sei();
	sleep_cpu();
	MCUCR = cr; /* sleep_disable */
}

static uint8_t hex_char(uint8_t nib) {
	nib &= 0xF;
	nib |= 0x30;
	if (nib > 0x39) nib += 7;
	return nib;
}

void hex_out(uint8_t d) {
//	USI_UART_Transmit_Byte(hex_char(d>>4));
//	USI_UART_Transmit_Byte(hex_char(d));
//	USI_UART_Transmit_Byte(' ');
}

uint8_t duhdata = 0x54;


const char stringy[] PROGMEM __attribute__((used)) = "OMG";

void main(void) {
	CLKPR = _BV(CLKPCE);
	CLKPR = 0;
	PORTB = 0x0F;
	DDRB  = 0x07;
	sei();
	for (;;) {
		duhdata++;
		PORTB &= ~_BV(2);
		PORTB |= _BV(0);
		PORTB |= _BV(1);
		_delay_ms(500);
		PORTB |= _BV(2);
		PORTB &= ~_BV(0);
		PORTB &= ~_BV(1);
		_delay_ms(500);
		if (duhdata == 0) _delay_ms(1);
	}
}
