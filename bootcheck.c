#include <avr/io.h>

uint8_t boot_sr __attribute__((section(".noinit"))) __attribute__((used));

void boot(void);

/* If you change this code, check that it fits within page 0 (64 bytes) of the flash. */
/* => half-written program wont break the jump to bootloader logic. */

void check_bl(void) __attribute__((naked)) __attribute__((section(".init3"))) __attribute__((used));
void check_bl(void) {
	uint8_t s = MCUSR;
	MCUSR = 0;
	boot_sr = s;
	if (s == _BV(EXTRF)) boot();
}
