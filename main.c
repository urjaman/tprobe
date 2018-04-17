#include "main.h"
#include <avr/pgmspace.h>
#include "probeuart.h"
#include "uart.h"
#include "adc.h"
#include <stdlib.h>
#include <string.h>

static uint16_t stimer_hi = 0;
static uint8_t stimer_ll = 0;

/* Uses the wdt ticker to generate a higher-width counter, and returns result of that counter. */
/* Needs to be called often enough (more often than every 4s or so) not to miss an overflow of the wdt ticker. */
static uint24_t supertimer(void) {
	uint8_t n = wdt_ticker;
	if (n < stimer_ll) stimer_hi++;
	stimer_ll = n;
	return ((uint24_t)stimer_hi << 8) | stimer_ll;
}


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


static uint8_t diag_en = 0;
#define DIAG(x) do { if (diag_en) ss_P(PSTR(x)); } while(0)

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
	DIAG("\r\nDeep breath...\r\n");
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
	if (FLREG&_BV(PROBE_OFF)) return 'N';
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

static uint24_t activity_timebase;
static uint8_t activity_type;
void activity(uint8_t t) {
	activity_type = t;
	activity_timebase = supertimer();
}

uint16_t inactivity_secs(void) {
	return (supertimer() - activity_timebase) / 32;
}

uint8_t inactivity_check(void) {
	const uint16_t timeout = 3*60*60; /* Activity timeout 3 hours */
	if (inactivity_secs() > timeout) return 1;
	return 0;
}

static uint24_t altmode_timebase;

void enter_altmode(void) {
	altmode_timebase = supertimer();
	activity(5);
	s.altfn |= 0x80;
	probe_off();
}

void set_enter_altmode(uint8_t m) {
	s.altfn = m & 7;
	enter_altmode();
}

void exit_altmode(void) {
	s.altfn &= ~0x80;
	show_val(0,0);
	DDRB &= ~_BV(4);
	PORTB &= ~_BV(4);
	probe_on();
}

uint8_t altmode_check(void) {
	if (!(s.altfn & 0x80)) return 0;
	if (s.altfn == 0x87) return 1; /* No timeout for mode 7. */
	uint24_t timeout = 2*60*60*32UL; /* Default altmode timeout 2 hours. */
	if (s.altfn == 0x86) timeout = 5*32; /* Mode 6 only 5 sec */
	uint24_t passed = supertimer() - altmode_timebase;
	if (passed >= timeout) {
		exit_altmode();
		return 0;
	}
	return 1;
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

static void altfn_transition(uint8_t m) {
	set_enter_altmode(m);
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
				show_val(0,10);
				altfn_transition(mfn);
				return;
			}
		}
	} while (1);
}

uint8_t vcc_lipo_dispval(uint16_t vcc) {
	if (vcc >= 4350) return 7; /* Ext. 4.5+ */
	if (vcc >= 3950) return 6; /* 4.0V+ Battery */
	if (vcc >= 3650) return 5; /* 3.7V+ Battery */
	if (vcc >= 3400) return 4; /* 3.5V ish */
	if (vcc >= 3200) return 3; /* 3.3V ish */
	if (vcc >= 2950) return 2; /* 3.0V ish */
	return 1; /* We're still alive... */
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
#ifdef PROBE_DELTA
	uint16_t prev_probe = 0;
#endif
	uint8_t b = BUTTON;
	for (;;) {
		uint8_t c1 = prb_char();
		if (uart_rx_bytes()) {
			uint8_t c = uart_rx();
			if ((c>='1')&&(c<='7')) {
				if (s.altfn & 0x80) exit_altmode();
				set_enter_altmode(c);
			}
			if (c=='0') {
				exit_altmode();
			}
			if (c=='d') {
				diag_en ^= 1;
			} else {
				activity(6);
			}
		}
		uint8_t bnew = BUTTON;
		if ((bnew)&&(!b)) { /* button-down event, record. */
			activity(1);
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
					exit_altmode();
					bdw = bdr;
				} else if (bdo_since(bdw-1) >= 32) {
					bdw = bdr;
					altfn_transition(s.altfn);
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

		/* Hit the hay if we've been unused too long. */
		if (inactivity_check()) deep_sleep();

		uint16_t probe = adc_sample_probe_mV();
		uint16_t vcc = adc_get_vcc_mV();

#ifdef PROBE_DELTA
		/* Detect voltage level change as activity */
		int16_t probe_delta = (int16_t)probe - (int16_t)prev_probe;
		if (probe_delta < 0) probe_delta *= -1;
		if (probe_delta > 1000) activity(2);
		prev_probe = probe;
#endif

		if (altmode_check()) {
			/* alt. fn active */
			if (s.altfn == 0x87) { /* Deep sleep */
				if (!b) { /* make sure the button is not pressed when we do it */
					deep_sleep();
				}
			} else if (s.altfn == 0x86) { /* Battery/VCC voltage check */
				show_val(vcc_lipo_dispval(vcc), 0);
			} else if (s.altfn == 0x85) { /* 'Flashlight' + Drive 1 */
				show_val(7, 0);
				PORTB |= _BV(4);
				DDRB |= _BV(4);
			} else if (s.altfn == 0x84) { /* 15.625Hz Output */
				static uint8_t lt;
				/* Sync to the WDT ticker via sleeping.. */
				sys_sleep(SLEEP_MODE_PWR_DOWN);
				uint8_t ct = wdt_ticker;
				if (lt != ct) {
					DDRB |= _BV(4);
					PINB = _BV(4) | _BV(0); /* Toggle */
					lt = ct;
				}
				continue; /* Skip UART TX */
			}
		} else {

		}


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
			if (diag_en) {
				ss_P(PSTR(" IDLE:"));
				u0x(inactivity_secs(), 5);
				ss_P(PSTR(" AT:"));
				u0x(activity_type, 1);
			}
			/* clr to eol */
			ss_P(PSTR("\x1B[K"));
		}

		uint8_t ntick = wdt_ticker+32;
		do {
			sys_sleep(SLEEP_MODE_PWR_DOWN);
			if (BUTTON != b) break;
			if (prb_char() != c1) {
				activity(4);
				break;
			}
			if (uart_rx_bytes()) break;
		} while (ntick != wdt_ticker);
	}
}
