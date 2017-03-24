#pragma once

#define FLREG GPIOR0
#define PROBE_V 0
#define PROBE_Z 1
#define PROBE_OFF 2

#define UART_RXBUFSZ 32

#define BTNREG GPIOR1
#define BTNBIT 0

#define BUTTON (BTNREG&_BV(BTNBIT))

#ifndef __ASSEMBLER__
extern volatile uint8_t wdt_ticker;
void probe_check(void);
extern volatile uint8_t uart_rxbuf[UART_RXBUFSZ];
extern volatile uint8_t uart_rxwo;
void uart_tx(uint8_t c);
#endif
