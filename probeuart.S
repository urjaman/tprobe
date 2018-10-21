/* Probe and UART asm in same file since shared PCINT0 */
#include <avr/io.h>
#include "probeuart.h"

.section .bss
.comm wdt_ticker, 1, 1
.lcomm trans_tick, 1
.comm uart_rxbuf, UART_RXBUFSZ, 1
.comm uart_rxwo, 1, 1
.comm pb_zup, 1, 1
.comm pb_zdn, 1, 1
.section .text
	.global probe_check
probe_check:
	sbic _SFR_IO_ADDR(FLREG), PROBE_OFF
	ret
	clr r24
	sbic _SFR_IO_ADDR(PINB), 4
	rjmp 10f
	cbi _SFR_IO_ADDR(PCMSK), 4
	sbi _SFR_IO_ADDR(PORTB), 4
	ldi r25, 4
0:	sbic _SFR_IO_ADDR(PINB), 4
	rjmp 1f
	dec r25
	brne 0b
	sts pb_zup, r25
	cbi _SFR_IO_ADDR(PORTB), 4
	sbi _SFR_IO_ADDR(PCMSK), 4
	cbi _SFR_IO_ADDR(PORTB), 2 // red on
	sbi _SFR_IO_ADDR(PORTB), 1 // green off
	rjmp 20f
1:	cbi _SFR_IO_ADDR(PORTB), 4
	ori r24, _BV(PROBE_Z)
	sbi _SFR_IO_ADDR(PORTB), 2 // red off
	sbi _SFR_IO_ADDR(PORTB), 1 // green off
	sts pb_zup, r25
	ldi r25, 40
2:	sbis _SFR_IO_ADDR(PINB), 4
	rjmp 3f
	dec r25
	brne 2b
	sbi _SFR_IO_ADDR(PCMSK), 4
	sts pb_zdn, r25
	rjmp 10f
3:	sbi _SFR_IO_ADDR(PCMSK), 4
	sts pb_zdn, r25
	rjmp 25f
10:	sbi _SFR_IO_ADDR(PORTB), 2 // red off
	cbi _SFR_IO_ADDR(PORTB), 1 // green on
	ori r24, _BV(PROBE_V)
20:	sbic _SFR_IO_ADDR(FLREG), PROBE_Z
	rjmp 25f
	in r25, _SFR_IO_ADDR(FLREG)
	cp r24, r25
	breq 25f
	cbi _SFR_IO_ADDR(PORTB), 0 // yellow on
	lds r25, wdt_ticker
	sts trans_tick, r25
25:	out _SFR_IO_ADDR(FLREG), r24
	ret


	.global WDT_vect
WDT_vect:
	push r2
	in r2, _SFR_IO_ADDR(SREG)
	push r24
	push r25
	lds r24, wdt_ticker
	ldi r25, 1
	add r25, r24
	sts wdt_ticker, r25
	sbic _SFR_IO_ADDR(FLREG), PROBE_OFF
	rjmp 10f
	lds r25, trans_tick
	cpse r24, r25
	sbi _SFR_IO_ADDR(PORTB), 0 // yellow off
	rcall probe_check
10:
	pop r25
	pop r24
	out _SFR_IO_ADDR(SREG), r2
	pop r2
	reti


// 6 cyc wake-up
// 4 from sleep / upto 4 for running instruction
// 4 interrupt entry
// 2 rjmp here
	.global PCINT0_vect
PCINT0_vect: // 6 - 16
	push r2
	in r2, _SFR_IO_ADDR(SREG) // 9
	push r24
	push r25
	push r30
	push r31		// 17
	sbic _SFR_IO_ADDR(BTNREG), BTNBIT
	rjmp 10f
	sbic _SFR_IO_ADDR(PINB), 3
	rjmp 10f
	// 21 - 31
	// UART RX
	cbi _SFR_IO_ADDR(PCMSK), 3 // disable change detection for Rx
	// 22
	ldi r25, 8 // 23

	ldi r31, 2 // 24
	nop // 25
	// okay, we will not actually arrive at the following line at 35, but we have adjusted
	// r31 such that the delay takes care of that.

	// 104 - 114 or 35 - 45
1:	// ((98 - 35) - 5) / 5 = 11.6, or 11 and 3.
	adiw r24, 0
	nop
	subi r31, -11 // allow pre-loading an offset in r31 for the first loop
		      // the delay loops leave r31 at zero
// This is the bit-transition-aware delay mechanism (labels 1xx)
	sbic _SFR_IO_ADDR(PINB), 3 // pick correct wait while value loop
	rjmp 110f
	nop
	// 3
100:	sbic _SFR_IO_ADDR(PINB), 3 // wait while zero
	rjmp 120f
	dec r31
	brne 100b
	rjmp 130f // 5

110:	sbis _SFR_IO_ADDR(PINB), 3 // wait while one
	rjmp 120f
	dec r31
	brne 110b
	rjmp 130f

// transition detected 2 to 6 cycles ago, so say 4 cycles, 1 for lsr, (35 - 4 - 1) / 3 = 10 loops
120:	ldi r31, 10
121:	dec r31
	brne 121b
130:
	// 98 - 108
	lsr r24
	sbic _SFR_IO_ADDR(PINB), 3
	ori r24, 0x80
	dec r25
	brne 1b
	// 4 cycles past the mid-point of last bit
	ldi r30, lo8(uart_rxbuf) // 1
	ldi r31, hi8(uart_rxbuf) // 2
	lds r25, uart_rxwo // 4
	add r30, r25 // 5
	brcc 4f // 6
	inc r31 // 7
4:	st Z, r24
	inc r25 // 8
	andi r25, (UART_RXBUFSZ-1) // 9

	// this is a tiny version of the bit-change-aware delay loop, that
	// instead just exits on the transition, and we do not wait if
	// the bit is already 1 (no transition expected).
	ldi r31, 13
5:	sbic _SFR_IO_ADDR(PINB), 3
	rjmp 6f
	dec r31
	brne 5b
6:

	sbi _SFR_IO_ADDR(PCMSK), 3 // changing shooould be done ;p (or if later, we want the ISR)
	sbis _SFR_IO_ADDR(PINB), 3
	rjmp 8f

	sts uart_rxwo, r25
	rjmp 12f // we make a hasty exit so we're ready for next byte in time, hopefully
8:	sbi _SFR_IO_ADDR(BTNREG), BTNBIT
10:	sbic _SFR_IO_ADDR(PINB), 3
	cbi _SFR_IO_ADDR(BTNREG), BTNBIT
	rcall probe_check
12:	pop r31
	pop r30
	pop r25
	pop r24
	out _SFR_IO_ADDR(SREG), r2
	pop r2
	reti


// 8Mhz/115200 = 69[.444] cyc/bit => 34[.722] = 35 cyc/half
// dly takes rcall(3) + ret(4) + X*3 (ldi is counted by brne taking 1 on exit loop)
// fuzz is correction for the outside loops, for now 6 / 2 = 3.
// X = (35 - (7 + fuzz)) / 3
// that is 8[.333] => 8, with the .333 converted to a nop

dly_half_bit: // Note: this fn doesnt touch carry
	nop // 8.333
	ldi r25, 8 // this is X above
0:	dec r25
	brne 0b
	ret

	.global uart_tx
uart_tx:
	cli
	cbi _SFR_IO_ADDR(PCMSK), 3 // disable change detection for Tx
	sbi _SFR_IO_ADDR(DDRB), 3
	rcall dly_half_bit // receiver only checks for half of the stop bit, and we only send half, so
	cbi _SFR_IO_ADDR(PORTB), 3 // start bit
	sec		// inject end marker bit to r16
	ror r24		// and pull bit0 to C(arry)
0:	rcall dly_half_bit	// dlys dont touch carry
	rcall dly_half_bit
	cbi _SFR_IO_ADDR(PORTB), 3
	brcc 1f			// this works like a skip now, so cycles effectively 1
	sbi _SFR_IO_ADDR(PORTB), 3
1:	lsr r24		// pull next bit to Carry and test result for 0 (if so, end marker eaten)
	brne 0b
	nop	// brne normalization to 2 cycles :P
	// wait to send last bit
	rcall dly_half_bit
	rcall dly_half_bit
	nop // cbi
	nop // brcc
	// send the stop bit
	sbi _SFR_IO_ADDR(PORTB), 3
	nop // lsr
	nop // brne1
	nop // brne2
	rcall dly_half_bit
	sbi _SFR_IO_ADDR(PCMSK), 3 // enable change detect :p
	sei
	cbi _SFR_IO_ADDR(DDRB), 3
	ret

