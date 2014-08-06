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

#include <avr/io.h>

#include "common.h"
#include "bluetooth.h"

#define UART_BAUD 9600

const uint8_t at_uart[] = "AT+UART=9600,0,0\r\n";
const uint8_t at_name[] = "AT+NAME=Iron Man Mark IV\r\n";

static uint8_t usart_getc()
{
	// Wait untill a data are available
	while (!(UCSRA & (1 << RXC)));

	return UDR;
}

static void usart_putc(const uint8_t c)
{
	// Wait untill the transmitter is ready
	while (!(UCSRA & (1 << UDRE)));

	UDR = c;
}

static void usart_puts(const uint8_t *s)
{
	while (*s) {
		usart_putc(*s++);
	}
}

void bluetooth_init(void)
{
	// Double transmission speed
	UCSRA = _BV(U2X);

	// Set baud rate
	UBRRL = (F_CPU / (8UL * UART_BAUD)) - 1;

	// Asynchronous mode, no parity, 8N1
	UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);

	// Enable TX & RX
	UCSRB = _BV(TXEN) | _BV(RXEN);
}

void bluetooth_configure(void)
{
	usart_puts(at_name);
}
