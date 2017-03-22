/*
 * Copyright (C) 2010,2011,2017 Urja Rannikko <urjaman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <avr/io.h>
#if SPM_PAGESIZE > 256
#error Too big SPM pagesize for the protocol
#endif

zero = 1

utx = 16
urx = 17
utmp = 18
udly = 19

cnt = 20
cmd = 21
tmp = 22

Zlo = 30
Zhi = 31


#define TAG_CHAR 0x55
#define INIT_CMD 0xAA

/* The exit command is 0xFF, but we protect the bootloader, so this is how many app pages. */
#define EXIT_LIMIT 124

.section .bootloader, "ax"
	.global boot
boot:   /* N.B. We're called after basic C init (cli, r1 is zero, stack is ok) */
	ldi tmp, 0x80
	out _SFR_IO_ADDR(CLKPR), tmp
	out _SFR_IO_ADDR(CLKPR), zero
	ldi tmp, 0x09
	out _SFR_IO_ADDR(PORTB), tmp
	ldi tmp, 0x0F
	out _SFR_IO_ADDR(DDRB), tmp

	rcall uart_recv
	cpi urx, INIT_CMD
	brne out
	ldi utx, TAG_CHAR
	rcall uart_send
#if SPM_PAGESIZE == 256
	clr utx
#else
	ldi utx, SPM_PAGESIZE
#endif

bootlp: /* N.B. Dont use "zero" past this point, r1 gets overwritten when flashing. */
	rcall uart_send
	rcall uart_recv
	cpi urx, EXIT_LIMIT
	brsh out

	mov Zhi, urx
	clr Zlo
#if SPM_PAGESIZE == 64
	lsr Zhi
	ror Zlo
	lsr Zhi
	ror Zlo
#elif SPM_PAGESIZE == 128
	lsr Zhi
	ror Zlo
#elif SPM_PAGESIZE == 256
#else
#error unexpected SPM_PAGESIZE
#endif

	ldi cmd, 0x01 // page load
	ldi cnt, (SPM_PAGESIZE/2)
dataloop:
	rcall uart_recv
	mov r0, urx
	rcall uart_recv
	mov r1, urx
	out _SFR_IO_ADDR(SPMCSR), cmd
	spm
	adiw Zlo, 2
	dec cnt
	brne dataloop

	sbiw Zlo, 2 // get back to the right page area
	ldi cmd, 0x03 // erase
	rcall do_spm_wait
	ldi cmd, 0x05 // write page
	rcall do_spm_wait
	ldi utx, TAG_CHAR
	rjmp bootlp

uart_recv:
	// clr urx is not necessary since we lsr before each bit, so we'll write the whole reg :)
	ldi utmp, 8
	cbi _SFR_IO_ADDR(DDRB), 3 // end TX, now we're listening
0:	sbic _SFR_IO_ADDR(PINB), 3
	rjmp 0b
	rcall dly_half_bit
	//nop // fuzz correction (disabled because we'd like fractional speedup)
	sbic _SFR_IO_ADDR(PINB), 3 // noise check, is it a real start bit?
	rjmp 0b
1:	rcall dly_half_bit
	rcall dly_half_bit
	lsr urx // 1
	sbic _SFR_IO_ADDR(PINB), 3 // 1
	ori urx, 0x80 // 1
	dec utmp // 1
	brne 1b // 2
2:	// Data done, now test the stop bit
	rcall dly_half_bit
	rcall dly_half_bit
	//nop // fuzz correction
	sbis _SFR_IO_ADDR(PINB), 3
	rjmp 3f // break/err processing
	sbi _SFR_IO_ADDR(DDRB), 3 // enable drive, we're no longer listening
	// toggle the yellow led on a valid byte ;)
	sbi _SFR_IO_ADDR(PINB), 0
	ret // all dandy, pass the byte onwards :)

3:	// for now, we signal 'E'RR and exit on any framing error or a break
	sbi _SFR_IO_ADDR(DDRB), 3 // enable drive
	ldi utx, 0x45
	rcall uart_send

out:
#ifdef RWWSRE
	ldi cmd, 0x11
	rcall do_spm_wait
#endif
	rjmp entry
.set entry,0

uart_send:
	rcall dly_half_bit // receiver only checks for half of the stop bit, and we only send half, so
	cbi _SFR_IO_ADDR(PORTB), 3 // start bit
	sec		// inject end marker bit to r16
	ror utx		// and pull bit0 to C(arry)
0:	rcall dly_half_bit	// dlys dont touch carry
	rcall dly_half_bit
	cbi _SFR_IO_ADDR(PORTB), 3
	brcc 1f			// this works like a skip now, so cycles effectively 1
	sbi _SFR_IO_ADDR(PORTB), 3
1:	lsr utx		// pull next bit to Carry and test result for 0 (if so, end marker eaten)
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
	ret

// 8Mhz/115200 = 69[.444] cyc/bit => 34[.722] = 35 cyc/half
// dly takes rcall(3) + ret(4) + X*3 (ldi is counted by brne taking 1 on exit loop)
// fuzz is correction for the outside loops, for now 6 / 2 = 3.
// X = (35 - (7 + fuzz)) / 3
// that is 8[.333] => 8, with the .333 converted to a nop

dly_half_bit: // Note: this fn doesnt touch carry
	nop // 8.333
	ldi udly, 8 // this is X above
0:	dec udly
	brne 0b
	ret

// X for other baud rates
// 57600: 19 + 2nop
// 38400: 31 + 1nop
// 19200: 66
// 9600: 135 + 2nop

do_spm_wait:
	out _SFR_IO_ADDR(SPMCSR), cmd
	spm
wait_spm_ready:
	in cmd, _SFR_IO_ADDR(SPMCSR)
	sbrc cmd, SPMEN
	rjmp wait_spm_ready
	ret

