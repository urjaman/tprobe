#include "main.h"
#include <avr/pgmspace.h>
#include "probeuart.h"
#include "uart.h"
#include "adc.h"
#include <stdlib.h>
#include <string.h>

static void ss_P(PGM_P str) {
	for(;;) {
		uint8_t val = pgm_read_byte(str);
		if (val) uart_tx(val);
		else break;
		str++;
	}
}

static void ss(const unsigned char * str) {
	for(;;) {
		uint8_t val = *str;
		if (val) uart_tx(val);
		else break;
		str++;
	}
}

static void u0x(uint16_t v, uint8_t x) {
	uint8_t tb[6];
	utoa(v, (char*)tb, 10);
	uint8_t l = strlen((char*)tb);
	while (l<x) { uart_tx('0'); l++; }
	ss(tb);
}


static uint8_t hex_char(uint8_t nib) {
	nib &= 0xF;
	nib |= 0x30;
	if (nib > 0x39) nib += 7;
	return nib;
}

static void X02(uint8_t d) {
	uart_tx(hex_char(d>>4));
	uart_tx(hex_char(d));
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



uint8_t prb_char(void) {
	if (!(FLREG&_BV(PROBE_Z))) return '0' + (FLREG&_BV(PROBE_V));
	return 'Z';
}

void probe_off(void) {
	FLREG |= _BV(PROBE_OFF);
}

void probe_on(void) {
	if (FLREG & _BV(PROBE_OFF)) {
		cli();
		FLREG &= ~_BV(PROBE_OFF);
		probe_check();
		sei();
	}
}

struct settings {
	uint8_t m;
	uint8_t l;
	uint8_t altfn;
	uint8_t c;
} s;

void settings_load(void) {
//	struct settings chk;
//	const uint8_t * const crc_rp = (uint8_t*)&chk;
	/* Load defaults */
	s.altfn = 1;

}

#if 0
void settings_save(void) {
	const uint8_t * const crc_rp = (uint8_t*)&s;
	s.m = 0xA7;
	s.l = sizeof(struct settings);

}
#endif

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
	settings_load();
	uint8_t ntick;
	for (;;) {
		uint8_t c1 = prb_char();
		if (uart_rx_bytes()) {
			uint8_t c = uart_rx();
			if ((c>='1')&&(c<='7')) s.altfn = (c - '0') | 0x80;
			if (c=='0') s.altfn &= ~0x80;
		}
		uint8_t b = BUTTON;
		if (!b) {
			uart_tx('\r');
			uart_tx(c1);
			ss_P(PSTR(": "));
			u0x(adc_sample_probe_mV(), 4);
			uart_tx('/');
			u0x(adc_get_vcc_mV(), 4);
			ss_P(PSTR(" mV; A="));
			X02(s.altfn);
			ss_P(PSTR(" ZU:"));
			u0x(pb_zup, 0);
			ss_P(PSTR(" ZD:"));
			u0x(pb_zdn, 2);
		}
		ntick = wdt_ticker+32;
		do {
			sys_sleep(SLEEP_MODE_PWR_DOWN);
			if (prb_char() != c1) break;
			if (uart_rx_bytes()) break;
		} while (ntick != wdt_ticker);
	}
}
