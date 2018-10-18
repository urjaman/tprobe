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

static void ssx_pad(int32_t v, uint8_t x, uint8_t c) {
	uint8_t tb[12];
	ltoa(v, (char*)tb, 10);
	uint8_t l = strlen((char*)tb);
	while (l<x) { uart_tx(c); l++; }
	ss(tb);
}

static void ssx(int32_t v, uint8_t x) {
	ssx_pad(v, x, ' ');
}

static void u0x(uint16_t v, uint8_t x) {
	ssx_pad(v, x, '0');
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
#define DIAG_SSX(x) do { if (diag_en) ssx(x, 0); } while(0)

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
	if (!TCCR1) cre |= mode; // Idle needed if TCCR1 to let timer run
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
	PCMSK &= ~_BV(PCINT4);
}

void probe_on(void) {
	if (FLREG & _BV(PROBE_OFF)) {
		cli();
		PCMSK |= _BV(PCINT4);
		FLREG &= ~_BV(PROBE_OFF);
		probe_check();
		sei();
	}
}

struct settings {
	uint8_t m;
	uint8_t l;
	uint8_t altfn;
	uint8_t submode;
	uint16_t Rpu;
	uint16_t vcc_cal;
	uint8_t c;
} s;

void settings_load(void) {
//	struct settings chk;
//	const uint8_t * const crc_rp = (uint8_t*)&chk;
	/* Load defaults */
	s.altfn = 1;
	s.vcc_cal = 256;
	s.Rpu = 33160;

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
	s.submode = 1;
	probe_off();
}

void set_enter_altmode(uint8_t m) {
	s.altfn = m & 7;
	enter_altmode();
}

void exit_altmode(void) {
	s.altfn &= ~0x80;
	show_val(0,1);
	show_val(7,1);
	show_val(0,0);
	TCCR1 = 0;
	GTCCR = 0;
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

static void altfn_display(void) {
	show_val(7,5);
	show_val(0,4);
	show_val(s.altfn,10);
	show_val(0,2);
}

static void altfn_transition(uint8_t m) {
	set_enter_altmode(m);
	altfn_display();
}

static void altfn_submode(uint8_t n) {
	altfn_display();
	show_val(0,2);
	show_val(n,10);
	show_val(0,2);
}

const uint8_t max_submodes[7] PROGMEM = {
	3, 2, 7, 1, 1, 1, 1
};

static uint8_t get_max_submode(void) {
	return pgm_read_byte(max_submodes + (s.altfn & 7) - 1);
}

void inc_submode(void) {
	uint8_t max_submode = get_max_submode();
	s.submode = (s.submode == max_submode) ? 1 : s.submode + 1;
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

const uint16_t adc_table[7] PROGMEM = {
	4500,
	3600,
	2700,
	2000,
	1600,
	1000,
	500
};

static uint8_t adc_disptablescan(uint16_t p, const uint16_t* tableP) {
	uint8_t r = 7;
	for (;r>0;r--) {
		uint16_t v = pgm_read_word(tableP++);
		if (p >= v) return r;
	}
	return r;
}

uint8_t adc_dispval(uint16_t p) {
	return adc_disptablescan(p, adc_table);
}


const uint16_t adc_table_lo[7] PROGMEM = {
	/* this is just 0.2 step starting from 0.4, but can be adjusted */
	1600,
	1400,
	1200,
	1000,
	 800,
	 600,
	 400
};

uint8_t adc_dispval_lo(uint16_t p) {
	return adc_disptablescan(p, adc_table_lo);
}

const uint16_t adc_table_hi[7] PROGMEM = {
	4350,
	3950,
	3650,
	3400,
	3200,
	2950,
	2000
};

uint8_t adc_dispval_hi(uint16_t p) {
	return adc_disptablescan(p, adc_table_hi);
}

/* PWM settings:
 * 1- 15.625 Hz - / 2048 / 250 (only used for PWM side)
 * 2-    250 Hz - / 128  / 250
 * 3-   1000 Hz - / 32   / 250
 * 4-   4000 Hz - / 8    / 250
 * 5-  32768 Hz - / 1    / 244 (... err 32786.8852...not like we were accurate anyways)
 * 6- 100000 Hz - / 1    / 80
 * 7- 500000 Hz - / 1    / 16
 */

const uint8_t tccr1_tab[7] PROGMEM = {
	_BV(CTC1) | 0xC,
	_BV(CTC1) | 0x8,
	_BV(CTC1) | 0x6,
	_BV(CTC1) | 0x4,
	_BV(CTC1) | 0x1,
	_BV(CTC1) | 0x1,
	_BV(CTC1) | 0x1,
};
const uint8_t ocr1c_tab[7] PROGMEM = {
	250-1, 250-1, 250-1, 250-1, 244-1, 80-1, 16-1
};

int numeric_entry(const PGM_P prompt, int dflt) {
	uint8_t edl = 0;
	uint8_t nl = 0;
	uint8_t neg = 0;
	uint16_t v = 0;
	ss_P(prompt);
	uint8_t waitbase = wdt_ticker;
	for (;;) {
		uint8_t p = wdt_ticker - waitbase;
		if ((p >= 192)||(BUTTON)) {
			if (p >= 192) {
				ss_P(PSTR("- Timeout"));
			}
			ss_P(PSTR("\r\n"));
			return dflt;
		}
		if (uart_rx_bytes()) {
			waitbase = wdt_ticker;
			uint8_t c = uart_rx();
			if (c == '\r') {
				wdt_delay(2);
				if (uart_rx_bytes()) c = uart_rx();
				if (c != '\n') uart_tx('\n');
				if (!nl) return dflt;
				if ( ((neg)&&(v > 32768)) || ((!neg)&&(v > 32767)) ) return dflt;
				if (neg) return v * -1;
				return v;
			} else if ((edl == 0)&&(c == '-')) {
				neg = 1;
				edl++;
			} else if ((nl < 5)&&(v < 3277)&&(c >= '0')&&(c <= '9')) {
				v = v * 10 + (c - '0');
				edl++;
				nl++;
			} else if ((c == '\b')||(c == 127)) {
				if (edl) {
					if (c == '\b') ss_P(PSTR(" \b"));
					else ss_P(PSTR("\b \b"));
					edl--;
					if (nl) {
						nl--;
						v = v / 10;
					}
					if (!edl) neg = 0;
				} else {
					if (c == '\b') uart_tx(pgm_read_byte(prompt + strlen_P(prompt) - 1));
				}
			} else {
				ss_P(PSTR("\b \b"));
			}
		}
		sys_sleep(SLEEP_MODE_PWR_DOWN);
	}
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
	uint8_t uart_tick = 0;
	uint16_t cal_sum = 0;
	uint16_t cal_sum2 = 0;
	uint8_t cal_state = 0;
	for (;;) {
		uint8_t c1 = prb_char();
		if (uart_rx_bytes()) {
			uint8_t c = uart_rx();
			if ((c>='1')&&(c<='7')) {
				if (s.altfn & 0x80) exit_altmode();
				set_enter_altmode(c);
			} else if (c=='0') {
				exit_altmode();
			} else if (c=='s') {
				inc_submode();
			} else if (c=='n') {
				int r = numeric_entry(PSTR("\r\nNumeric Test: "), -555);
				ss_P(PSTR("Parsed: "));
				ssx(r, 0);
				ss_P(PSTR("\r\n"));
			}

			if (c=='d') { /* This is special so that diag toggle is not activity */
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
					if ( (get_max_submode() == 1) || (bdo_since(bdw-1) >= 16) ) {
						exit_altmode();
						bdw = bdr;
					}
				} else if (bdo_since(bdw-1) >= 32) {
					bdw = bdr;
					altfn_transition(s.altfn);
				}
			}
			if (bdo_since(bdr) >= 128) { /* forget stuff before it's so old it confuses us. */
				bdr = (bdr+1) & 3;
			} else {
				if (s.altfn & 0x80) {
					if ((!b)&&(bdo_since(bdw-1) < 16)) {
						inc_submode();
						altfn_submode(s.submode);
						bdw = bdr;
					}
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
		}

		uint16_t probe = adc_sample_probe_mV();
		uint16_t vcc = adc_get_vcc_mV();

#ifdef PROBE_DELTA
		/* Detect voltage level change as activity */
		int16_t probe_delta = (int16_t)probe - (int16_t)prev_probe;
		if (probe_delta < 0) probe_delta *= -1;
		if (probe_delta > 1000) activity(2);
		prev_probe = probe;
#endif
		int16_t Udut = 0;
		int32_t Rdut = 0;
		if (s.altfn == 0x82) {
			/* Compute these before the UART section so they can be printed. */
			PORTB |= _BV(4);
			const uint16_t Rout = 7500; /* Build option, heh. */
			/* const uint32_t Rpd = 1000000UL; */
			uint16_t vccm = (((uint32_t)vcc * s.vcc_cal) + 128) / 256;
			if (probe < vcc/2) vccm = vcc;
			uint32_t Irpu = (((vccm - probe) * 100000UL)+(s.Rpu/2)) / s.Rpu;
			uint32_t Ipd =  (probe +5) / 10; /* * 1000000 / Rpd */ ; // This would end up a nop, so disabled
			uint32_t Idut = (Irpu >= Ipd) ? Irpu - Ipd : 0;
			uint16_t Urout = ((Idut * Rout) + 50000UL) / 100000UL;
			Udut = probe - Urout;
			Rdut = Idut ? ((Udut * 100000L) + (Idut/2)) / Idut : 9999999L;
			if (Rdut > 9999999L) Rdut = 9999999L;
			if (Udut < 0) Rdut = 0; /* Close to zero, or DUT is not passive. */
		}

		uint8_t uart_passed = wdt_ticker - uart_tick;
		if ((!b)&&(vcc>=4200)) { /* toggle UART usage based on power source present. */
			if (uart_passed >= 16) {
				uart_tick = wdt_ticker;
				uart_tx('\r');
				uart_tx(c1);
				ss_P(PSTR(": "));
				u0x(probe, 4);
				uart_tx('/');
				u0x(vcc, 4);
				ss_P(PSTR(" mV; A="));
				X02(s.altfn);
				ss_P(PSTR(" S="));
				X02(s.submode);
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
				if (s.altfn == 0x82) {
					ss_P(PSTR(" U:"));
					ssx(Udut, 5);
					ss_P(PSTR(" mV R: "));
					ssx(Rdut, 0);
					ss_P(PSTR(" Ohm"));
				}
				/* clr to eol */
				ss_P(PSTR("\x1B[K"));
			}
		} else {
			uart_tick = wdt_ticker;
		}

		/* Hit the hay if we've been unused too long. */
		if (inactivity_check()) deep_sleep();

		uint8_t ntick = wdt_ticker+32;
		if (altmode_check()) {
			/* alt. fn active */
			if (s.altfn == 0x87) { /* Deep sleep + Drive 0 */
				PORTB &= ~_BV(4);
				DDRB |= _BV(4);
				if (!b) { /* make sure the button is not pressed when we do it */
					deep_sleep();
				}
			} else if (s.altfn == 0x86) { /* Battery/VCC voltage check */
				show_val(adc_dispval_hi(vcc), 0);
			} else if (s.altfn == 0x85) { /* 'Flashlight' + Drive 1 */
				show_val(7, 0);
				PORTB |= _BV(4);
				DDRB |= _BV(4);
			} else if (s.altfn == 0x83) { /* 15.625Hz Output */
				if (s.submode == 1) {
					static uint8_t lt;
					/* Sync to the WDT ticker via sleeping.. */
					GTCCR = 0;
					TCCR1 = 0;
					sys_sleep(SLEEP_MODE_PWR_DOWN);
					uint8_t ct = wdt_ticker;
					if (lt != ct) {
						DDRB |= _BV(4);
						PINB = _BV(4) | _BV(0); /* Toggle */
						lt = ct;
					}
					continue; /* Skip long sleep */
				} else {
					DDRB |= _BV(4);
					PORTB &= ~_BV(2);
					uint8_t c = pgm_read_byte(ocr1c_tab + (s.submode -1));
					OCR1C = c;
					OCR1B = (c+1) >> 1;
					GTCCR |= _BV(PWM1B);
					GTCCR |= _BV(COM1B1);
					TCCR1 = pgm_read_byte(tccr1_tab + (s.submode - 1));
					PINB = _BV(0);
					ntick = wdt_ticker + 18 - (s.submode * 2);
				}
			} else if (s.altfn == 0x84) {

			} else if (s.altfn == 0x82) { /* Diode test w/ pullup */
				uint8_t v;
				if (s.submode == 2) { /* Calibration */
					if ((cal_state < 4) && (probe > 1500)) {
						cal_state = 0;
						cal_sum = 0;
						cal_sum2 = 0;
					}
					if ((cal_state > 4) && (probe < 2000)) {
						cal_state = 4;
						cal_sum = 0;
						cal_sum2 = 0;
					}
					if (cal_state < 4) {
						if (cal_state) {
							cal_sum += probe;
							cal_sum2 += vcc;
						}
						if (probe <= 1500) cal_state++;
						if (cal_state == 4) { /* Rpu calibrated */
							const uint16_t Rz = 7444;
							uint32_t Irpu3 = ((cal_sum * 100000UL) + (Rz/2)) / Rz;
								/* Urpu3 */
							s.Rpu = (((cal_sum2 - cal_sum) * 100000UL) + (Irpu3/2)) / Irpu3;
							DIAG("\r\nRpu:"); DIAG_SSX(s.Rpu);
							DIAG(" sum:"); DIAG_SSX(cal_sum);
							DIAG(" s2:"); DIAG_SSX(cal_sum2);
							DIAG(" Irpu3:"); DIAG_SSX(Irpu3);
							DIAG("\r\n");
							cal_sum = 0;
							cal_sum2 = 0;
						}
					} else {
						if (cal_state > 4) {
							cal_sum += probe;
							cal_sum2 += vcc;
						}
						if (probe >= 2000) cal_state++;
						if (cal_state == 8) { /* Vcc calibrated */
							uint32_t Irpu3 = (cal_sum + 5) / 10; /* because 1M pulldown */
							uint16_t Urpu3 = ((Irpu3 * s.Rpu) + 50000UL) / 100000UL;
							uint16_t vcc3 = cal_sum + Urpu3;
							s.vcc_cal = ((vcc3 * 256UL) + (cal_sum2/2)) / cal_sum2;
							DIAG("\r\nvcc_cal:"); DIAG_SSX(s.vcc_cal);
							DIAG(" sum:"); DIAG_SSX(cal_sum);
							DIAG(" s2:"); DIAG_SSX(cal_sum2);
							DIAG(" Urpu3:"); DIAG_SSX(Urpu3);
							DIAG(" vcc3:"); DIAG_SSX(vcc3);
							DIAG("\r\n");
							cal_sum = 0;
							cal_sum2 = 0;
							s.submode = 1;
							altfn_submode(s.submode);
						}
					}
					if (cal_state < 4) {
						v = 4 + cal_state;
					} else if (cal_state == 4) {
						v = 1;
					} else if (cal_state == 8) {
						v = 0;
						cal_state = 0;
					} else {
						v = 1 + (cal_state - 5);
					}
				} else {
					if (Udut < -100) v = 0; /* huh? */
					else if (Udut < 100) v = 4; /* red */
					else if (Udut < 1000) v = 2; /* green */
					else if (Udut < (int16_t)(vcc - 500)) v = 1; /* orange */
					else v = 0; /* nothing */
				}
				show_val(v, 0);
				ntick = wdt_ticker + 4;
			} else if (s.altfn == 0x81) {
				/* SubModes 1-3 of altfn 1 are representations of the ADC */
				/* 1: General Purpose / Voltage rail detector (1.8/3.3/5) */
				/* 2: Low voltages / 1.5V Battery analysis */
				/* 3: High voltages / Lipo check */
				if (probe >= (vcc - 20)) { /* Railed, blink 7 */
					show_val(7, 5);
					show_val(0, 3);
					continue;
				}
				uint8_t v;
				switch (s.submode) {
					default: v = adc_dispval(probe);    break;
					case 2:  v = adc_dispval_lo(probe); break;
					case 3:  v = adc_dispval_hi(probe); break;
				}
				show_val(v, 0);
				ntick -= 24;
			}
		} else {

		}

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
