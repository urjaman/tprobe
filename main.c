#include "main.h"
#include <avr/pgmspace.h>


static volatile uint8_t wdt_ticker = 0;

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

static volatile uint8_t button = 0;

#define UART_RXBUFSZ 8
static uint8_t uart_rxbuf[UART_RXBUFSZ];
static volatile uint8_t uart_rxwo = 0;
static uint8_t uart_rxro = 0;

enum probe_s {
	PROBE_Z,
	PROBE_0,
	PROBE_1,
	PROBE_OFF
};

static uint8_t probe_state = PROBE_Z;
static uint8_t trans_tick = 0;

static void probe_check(void) {
	enum probe_s n_state;
	if (probe_state == PROBE_OFF) return;
	if (PINB&_BV(4)) {
		n_state = PROBE_1;
	} else {
		PCMSK &= ~_BV(PCINT4); // disable change detection during purposeful change
		PORTB |= _BV(4);
		n_state = PROBE_0;
		_delay_us(1); // TBD
		if (PINB&_BV(4)) {
			PORTB &= ~_BV(4);
			n_state = PROBE_Z;
			uint8_t cnt=0; // TBD, need to wait for it to go 0...
			do {
				_delay_us(1);
				if (!(PINB&_BV(4))) break;
			} while (--cnt);
		}
		PORTB &= ~_BV(4);
		PCMSK |= _BV(PCINT4);

	}
	if (n_state != probe_state) { /* transition detection */
		PORTB &= ~_BV(0); // yellow on
		trans_tick = wdt_ticker;
	}
	probe_state = n_state;
	if (n_state == PROBE_Z) {
		PORTB |= _BV(1); // green off
		PORTB |= _BV(2); // red off
	} else if (n_state == PROBE_0) {
		PORTB &= ~_BV(2); // red on
		PORTB |= _BV(1); // green off
	} else if (n_state == PROBE_1) {
		PORTB &= ~_BV(1); // green on
		PORTB |= _BV(2); // red off
	}
}


ISR(WDT_vect)
{
	uint8_t ex_ticker = wdt_ticker;
	wdt_ticker = ex_ticker + 1;
	if (probe_state != PROBE_OFF) {
		if (ex_ticker != trans_tick) PORTB |= _BV(0); // yellow off
		probe_check();
	}
}



ISR(PCINT0_vect)
{
	if ((!(PINB&_BV(3)))&&(!button)) {
		// uart rx
	}
	if (PINB&_BV(3)) button = 0;
	probe_check();
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
	PCMSK = _BV(PCINT4) | _BV(PCINT3); // probe, uart
	GIFR |= _BV(PCIF);
	GIMSK = _BV(PCIE);
	sei();
	for (;;) {
		wdt_delay(1);
	}
}
