#include "main.h"
#include <avr/pgmspace.h>


static volatile uint8_t wdt_ticker = 0;

ISR(WDT_vect)
{
	wdt_ticker++;
}

void wdt_init(void) {
	/* This sequence both turns off WDT reset mode and sets the period to 32ms */
	/* Thus preparing WDT to be used as a simple timer later. */
	wdt_reset();
	WDTCR = _BV(WDCE) | _BV(WDE);
	WDTCR = _BV(WDIF) | _BV(WDIE) | _BV(WDP0);
}

void wdt_delay(uint8_t ticks) {
	if (!ticks) return; /* This would have unreliable 0 or 256 behaviour... */
	cli();
	uint8_t end = wdt_ticker + ticks;
	do {
		sys_sleep(SLEEP_MODE_PWR_DOWN);
	} while (wdt_ticker != end);
	return;
}

void sys_sleep(uint8_t mode)
{
	uint8_t cr = MCUCR & 0xC7; // Clear sleep-related bits
	uint8_t cre = cr | _BV(SE);
	cli();
	// Clear sets mode = idle
	//if (!TCCR0B) 
	cre |= mode; // Idle needed if TCCR0B to let timer run
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


void main(void) {
	CLKPR = _BV(CLKPCE);
	CLKPR = 0;
	PORTB = 0x0F;
	DDRB  = 0x07;
	wdt_init();
	sei();
	for (;;) {
		PORTB &= ~_BV(2);
		PORTB &= ~_BV(0);
		PORTB |= _BV(1);
		wdt_delay(16);
		PORTB |= _BV(2);
		PORTB |= _BV(0);
		PORTB &= ~_BV(1);
		wdt_delay(16);
	}
}
