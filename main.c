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

/* Disables WDT and the brownout detector and sleeeps. Forever, or well until a pin changes. */
void deep_sleep(void)
{
	uint8_t cr = MCUCR & 0xC7; // Clear sleep-related bits
	uint8_t cre2 = cr | _BV(SE) | _BV(BODS) | _BV(SM1);
	uint8_t cre1 = cre2 | _BV(BODSE);
	cli();
	wdt_reset();
	WDTCR = _BV(WDCE) | _BV(WDE);
	WDTCR = 0;
	MCUCR = cre1;
	MCUCR = cre2;
	sei();
	sleep_cpu();
	MCUCR = cr; /* sleep_disable */
	wdt_init();
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

void show_val(uint8_t v, uint8_t d) {
	/* Happily enough, the LEDs are PORTB0-2 in the logical bit order, just inverted output */
	PORTB = (PORTB & 0xF8) | ((~v) & 7);
	wdt_delay(d);
}

/* Button (-down transition) state tracker */
static uint8_t bdr = 0;
static uint8_t bdw = 0;
static uint8_t bdt[4];

static uint8_t bdo_since(uint8_t off) {
	uint8_t dt = bdt[off & 3];
	uint8_t passed = wdt_ticker - dt;
	return passed;
}

static void altfn_transition(void) {
	show_val(7,5);
	show_val(0,4);
	show_val(s.altfn,10);
	show_val(0,2);
}

void tri_tap_menu(void) {
	uint8_t waitbase = wdt_ticker;
	uint8_t mfn = s.altfn & 0x7F;
	uint8_t sv = 0;
	int butt = -2;
	do {
		uint8_t p = wdt_ticker - waitbase;
		if (p >= 192) break;
		if (!sv) sv = mfn;
		else sv = 0;
		show_val(sv,2);
		uint8_t b = BUTTON;
		if ((b)&&(butt == -1)) {
			butt = wdt_ticker;
			waitbase = wdt_ticker;
		} else if ((!b)&&(butt == -2)) butt = -1;
		else if ((!b)&&(butt >= 0)) {
			waitbase = wdt_ticker;
			mfn = (mfn==7) ? 1 : mfn + 1;
			butt = -1;
		} else if ((b)&&(butt >= 0)) {
			uint8_t bp = wdt_ticker - butt;
			if (bp > 32) {
				s.altfn = 0x80 | mfn;
				show_val(0,10);
				altfn_transition();
				return;
			}
		}
	} while (1);
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
	settings_load();

	uint8_t b = BUTTON;
	for (;;) {
		uint8_t c1 = prb_char();
		if (uart_rx_bytes()) {
			uint8_t c = uart_rx();
			if ((c>='1')&&(c<='7')) {
				probe_off();
				s.altfn = (c - '0') | 0x80;
			}
			if (c=='0') {
				probe_on();
				s.altfn &= ~0x80;
			}

		}
		uint8_t bnew = BUTTON;
		if ((bnew)&&(!b)) { /* button-down event, record. */
			bdt[bdw] = wdt_ticker;
			bdw = (bdw+1) & 3;
			if (bdr == bdw) { /* avoid overflow, forget oldest */
				/* also, this buffer very tactically holds 3 down events :P */
				bdr = (bdr+1) & 3;
			}
		}
		b = bnew;
		if (bdw != bdr) {
			if (b) {
				if (s.altfn & 0x80) {
					probe_on();
					bdw = bdr;
					s.altfn &= ~0x80;
				} else if (bdo_since(bdw-1) >= 32) {
					probe_off();
					s.altfn |= 0x80;
					bdw = bdr;
					altfn_transition();
				}
			}
			if (bdo_since(bdr) >= 128) { /* forget stuff before it's so old it confuses us. */
				bdr = (bdr+1) & 3;
			} else {
				uint8_t cnt = (bdw - bdr) & 3;
				if ((cnt == 3)&&(bdo_since(bdr) < 32)) {
					probe_off();
					/* tri-tap detected */
					tri_tap_menu();
					if (!(s.altfn & 0x80)) probe_on();
					bdw = bdr;
				}
			}
		}

		if (s.altfn & 0x80) {
			/* alt. fn active */
			if (s.altfn == 0x87) { /* special deep sleep mode ... */
				if (!b) { /* make sure the button is not pressed when we do it */
					deep_sleep();
				}
			}
		} else {

		}

		uint16_t probe = adc_sample_probe_mV();
		uint16_t vcc = adc_get_vcc_mV();
		if ((!b)&&(vcc>=4200)) { /* toggle UART usage based on power source present. */
			uart_tx('\r');
			uart_tx(c1);
			ss_P(PSTR(": "));
			u0x(probe, 4);
			uart_tx('/');
			u0x(vcc, 4);
			ss_P(PSTR(" mV; A="));
			X02(s.altfn);
			ss_P(PSTR(" ZU:"));
			u0x(pb_zup, 0);
			ss_P(PSTR(" ZD:"));
			u0x(pb_zdn, 2);
			/* clr to eol */
			ss_P(PSTR("\x1B[K"));
		}

		uint8_t ntick = wdt_ticker+32;
		do {
			sys_sleep(SLEEP_MODE_PWR_DOWN);
			if (BUTTON != b) break;
			if (prb_char() != c1) break;
			if (uart_rx_bytes()) break;
		} while (ntick != wdt_ticker);
	}
}
