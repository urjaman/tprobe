#include "main.h"
#include <avr/pgmspace.h>
#include "probeuart.h"
#include "uart.h"
#include "adc.h"
#include <stdio.h>

static int mputch(char c, FILE *stream);
static FILE mstdout = FDEV_SETUP_STREAM(mputch, NULL, _FDEV_SETUP_WRITE);

static int mputch(char c, FILE *stream) {
	if (c == '\n') mputch('\r', stream);
	uart_tx(c);
	return 0;
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
	uart_tx(hex_char(d>>4));
	uart_tx(hex_char(d));
	uart_tx(' ');
}

uint8_t prb_char(void) {
	if (!(FLREG&_BV(PROBE_Z))) return '0' + (FLREG&_BV(PROBE_V));
	return 'Z';
}

void main(void) {
	CLKPR = _BV(CLKPCE);
	CLKPR = 0;
	PORTB = 0x0F;
	DDRB  = 0x07;
	wdt_init();
	PCMSK = _BV(PCINT4) | _BV(PCINT3); // probe, uart
	GIFR |= _BV(PCIF);
	GIMSK = _BV(PCIE);
	sei();
	stdout = &mstdout;
	uint8_t ntick;
	for (;;) {
		uint8_t c1 = prb_char();
		uint8_t c2 = 0;
		if (uart_rx_bytes()) c2 = uart_rx();
		printf("\r%c: %04d/%04d mV; IO=%02X ZU:%d ZD:%d", c1, adc_sample_probe_mV(), adc_get_vcc_mV(), c2, pb_zup, pb_zdn);
		ntick = wdt_ticker+32;
		do {
			sys_sleep(SLEEP_MODE_PWR_DOWN);
			if (prb_char() != c1) break;
			if (uart_rx_bytes()) break;
		} while (ntick != wdt_ticker);
	}
}
