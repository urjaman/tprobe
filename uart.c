#include "main.h"
#include "uart.h"

/* This is basically just a few C helpers to deal with the RX buffer, rest is
 * done by probeuart.S */

static uint8_t uart_rxro;

uint8_t uart_rx_bytes(void) {
	int8_t used = uart_rxwo - uart_rxro;
	if (used<0) used += UART_RXBUFSZ;
	return used;
}

static void uart_wait_sleep(void) {
	cli();
	while (uart_rxro == uart_rxwo) {
		sys_sleep(SLEEP_MODE_PWR_DOWN);
		cli();
	}
	sei();
}

uint8_t uart_rx(void) {
	if (uart_rxro == uart_rxwo) uart_wait_sleep();
	uint8_t tmp = uart_rxro;
	uint8_t d = uart_rxbuf[tmp++];
	uart_rxro = tmp & (UART_RXBUFSZ-1);
	return d;
}
