#include <stdint.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void sys_sleep(uint8_t mode);

extern uint8_t boot_sr;

typedef __int24 int24_t;
typedef __uint24 uint24_t;
