/*
 *  jarvis-2.0
 *  Copyright (C) 2014 Roman Moravcik
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdlib.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "common.h"
#include "bluetooth.h"

#define UART_BAUD 38400

// UART RX complete
ISR(USART_RXC_vect)
{
    uint8_t tmp __attribute__((unused));
    tmp = UDR;
}

static void uart_flush(void)
{
	char tmp __attribute__((unused));

	// Read until data are available
	while (UCSRA & (1 << RXC))
		tmp = UDR;
}

static char uart_getc(void)
{
	// Wait until data are available
	while (!(UCSRA & (1 << RXC)));

	return UDR;
}

static void uart_putc(char c)
{
	// Wait until the transmitter is ready
	while (!(UCSRA & (1 << UDRE)));

	UDR = c;
}

static void uart_puts(char *str)
{
	while (*str) {
		uart_putc(*str++);
	}
}

static void hc05_send_cmd(char *cmd)
{
	uart_puts(cmd);
	uart_puts("\r\n");

	_delay_ms(100);
}

void bluetooth_init(void)
{
	// Double transmission speed
	UCSRA = _BV(U2X);

	// Set baud rate
	UBRRL = (F_CPU / (8UL * UART_BAUD)) - 1;

	// Asynchronous mode, no parity, 8N1
	UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);

	// Enable TX & RX, Enable RXC interrupt
	UCSRB = _BV(TXEN) | _BV(RXEN) | _BV(RXCIE);

	uart_flush();
}

void bluetooth_configure(void)
{
	uart_flush();
	_delay_ms(500);

	hc05_send_cmd("AT");
	_delay_ms(500);

	hc05_send_cmd("AT+NAME=\"Iron Man Mark IV\"");
	hc05_send_cmd("AT+PSWD=1234");
	hc05_send_cmd("AT+UART=38400,0,0");
	hc05_send_cmd("AT+CMODE=1");
	hc05_send_cmd("AT+CLASS=800804");
	hc05_send_cmd("AT+RESET");
}

void bluetooth_send(char *str)
{
	uart_puts(str);
	uart_puts("\r\n");
}