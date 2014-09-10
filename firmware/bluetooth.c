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
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "battery.h"
#include "common.h"
#include "helmet.h"
#include "power.h"
#include "strings.h"
#include "voice.h"
#include "bluetooth.h"

#define UART_BAUD 38400

static uint8_t echo = FALSE;

#define RXBUFF_LEN 40
char rxbuff[RXBUFF_LEN];

enum {
	RESPONSE_OK = 0,
	RESPONSE_ERROR,
	RESPONSE_NO_RESPONSE,
};

static void bluetooth_parse_command(uint8_t size);

static void wdt_reboot(void)
{
	wdt_reset();
	wdt_enable(WDTO_2S);
}

static void uart_flush(void)
{
	char tmp __attribute__((unused));

	// Read until data are available
	while (UCSRA & _BV(RXC))
		tmp = UDR;
}

static void uart_putc(char c)
{
	// Wait until the transmitter is ready
	while (!(UCSRA & _BV(UDRE)));

	UDR = c;
}

static void uart_puts(char *str)
{
	while (*str) {
		uart_putc(*str++);
	}
}

static void hc05_send_command(char *command)
{
	uart_puts(command);
	uart_puts("\r\n");

	_delay_ms(100);
}

// UART RX complete
ISR(USART_RXC_vect)
{
	static uint8_t i = 0;

	rxbuff[i] = UDR;

	// uppercase input characters
	if ((rxbuff[i] >= 'a') && (rxbuff[i] <= 'z')) {
		rxbuff[i] = rxbuff[i] - 'a' + 'A';
	}

	if (rxbuff[i] == '\r') {
		if (echo) {
			// send 'new line' character
			uart_puts("\r\n");
		}

		// replace '\r' character with string termination character
		rxbuff[i] = '\0';

		bluetooth_parse_command(i);

		i = 0;
	} else if (rxbuff[i] == '\b') {
		if (i > 0) {
			if (echo) {
				// clear deleted character
				uart_puts("\b \b");
			}

			rxbuff[i] = 0;
			i--;
		}
	} else {
		if (echo) {
			// echo back received character
			uart_putc(rxbuff[i]);
		}

		i++;
		if (i == RXBUFF_LEN) {
			i = 0;
		}
	}
}

static void bluetooth_parse_command(uint8_t size)
{
	uint8_t response = RESPONSE_OK;

	sei();

	if (size == 0) {
		response = RESPONSE_NO_RESPONSE;
	} else if (strncmp(rxbuff, BLUETOOTH_CMD_HELP, size) == 0) {
		uint8_t i = 0;

		for (i = 1; i < sizeof(commands) / sizeof(commands[0]); i ++) {
			uart_puts(commands[i]);

			if ((i % 3) == 0) {
				uart_puts("\r\n");
			} else {
				uint8_t padding = 0, j = 0;
				padding = 26 - strlen(commands[i]);
				for (j = 0; j < padding; j++) {
					uart_putc(' ');
				}
			}
		}

		if (((i - 1) % 3) != 0) {
			uart_puts("\r\n");
		}

		response = RESPONSE_NO_RESPONSE;
	} else if (strncmp(rxbuff, BLUETOOTH_CMD_BATTERY, size) == 0) {
		uint8_t capacity = 0;
		char resp[16];

		capacity = battery_get_capacity();
		itoa(capacity, resp, 10);

		uart_puts(BLUETOOTH_CMD_BATTERY);
		uart_puts(": ");
		uart_puts(resp);
		uart_puts("%\r\n");

		response = RESPONSE_NO_RESPONSE;
	} else if (strncmp(rxbuff, BLUETOOTH_CMD_ECHO, size) == 0) {
		echo = TRUE;
		uart_puts("\r\n");
		response = RESPONSE_NO_RESPONSE;
	} else if (strncmp(rxbuff, BLUETOOTH_CMD_EYES, strlen(BLUETOOTH_CMD_EYES)) == 0) {
		char *param = rxbuff + strlen(BLUETOOTH_CMD_EYES) + 1;

		if (size == strlen(BLUETOOTH_CMD_EYES) + strlen(BLUETOOTH_PARAM_ON) + 1) {
			if (strncmp(param, BLUETOOTH_PARAM_ON, strlen(BLUETOOTH_PARAM_ON)) == 0) {
				power_on(EYES);
			} else {
				response = RESPONSE_ERROR;
			}
		} else if (size == strlen(BLUETOOTH_CMD_EYES) + strlen(BLUETOOTH_PARAM_OFF) + 1) {
			if (strncmp(param, BLUETOOTH_PARAM_OFF, strlen(BLUETOOTH_PARAM_OFF)) == 0) {
				power_off(EYES);
			} else {
				response = RESPONSE_ERROR;
			}
		} else if (size == strlen(BLUETOOTH_CMD_EYES)) {
			uint8_t state = power_state(EYES);

			uart_puts(BLUETOOTH_CMD_EYES);
			uart_puts(": ");

			if (state == POWER_ON) {
				uart_puts(BLUETOOTH_PARAM_ON);
			} else {
				uart_puts(BLUETOOTH_PARAM_OFF);
			}
			uart_puts("\r\n");

			response = RESPONSE_NO_RESPONSE;
		} else {
			response = RESPONSE_ERROR;
		}
	} else if (strncmp(rxbuff, BLUETOOTH_CMD_FORTUNE, size) == 0) {
		voice_play_random();
	} else if (strncmp(rxbuff, BLUETOOTH_CMD_HELMET, strlen(BLUETOOTH_CMD_HELMET)) == 0) {
		char *param = rxbuff + strlen(BLUETOOTH_CMD_HELMET) + 1;

		if (size == strlen(BLUETOOTH_CMD_HELMET) + strlen(BLUETOOTH_PARAM_OPEN) + 1) {
			if (strncmp(param, BLUETOOTH_PARAM_OPEN, strlen(BLUETOOTH_PARAM_OPEN)) == 0) {
				helmet_open();
			} else {
				response = RESPONSE_ERROR;
			}
		} else if (size == strlen(BLUETOOTH_CMD_HELMET) + strlen(BLUETOOTH_PARAM_CLOSE) + 1) {
			if (strncmp(param, BLUETOOTH_PARAM_CLOSE, strlen(BLUETOOTH_PARAM_CLOSE)) == 0) {
				helmet_close();
			} else {
				response = RESPONSE_ERROR;
			}
		} else if (size == strlen(BLUETOOTH_CMD_HELMET)) {
			uint8_t state = helmet_state();

			uart_puts(BLUETOOTH_CMD_HELMET);
			uart_puts(": ");

			if (state == HELMET_OPEN) {
				uart_puts(BLUETOOTH_PARAM_OPEN);
			} else {
				uart_puts(BLUETOOTH_PARAM_CLOSE);
			}
			uart_puts("\r\n");

			response = RESPONSE_NO_RESPONSE;
		} else {
			response = RESPONSE_ERROR;
		}
	} else if (strncmp(rxbuff, BLUETOOTH_CMD_INTENSITY, strlen(BLUETOOTH_CMD_INTENSITY)) == 0) {
		uint8_t device = 0;
		char *param = rxbuff + strlen(BLUETOOTH_CMD_INTENSITY) + 1;

		if (strncmp(param, BLUETOOTH_CMD_EYES, strlen(BLUETOOTH_CMD_EYES)) == 0) {
			if (size == strlen(BLUETOOTH_CMD_INTENSITY) + strlen(BLUETOOTH_CMD_EYES) + 3) {
				device = EYES;
			} else if (size == strlen(BLUETOOTH_CMD_INTENSITY) + strlen(BLUETOOTH_CMD_EYES) + 1) {
				device = EYES;
				response = RESPONSE_NO_RESPONSE;
			} else {
				response = RESPONSE_ERROR;
			}
		} else if (strncmp(param, BLUETOOTH_CMD_REPULSORS, strlen(BLUETOOTH_CMD_REPULSORS)) == 0) {
			if (size == strlen(BLUETOOTH_CMD_INTENSITY) + strlen(BLUETOOTH_CMD_REPULSORS) + 3) {
				device = REPULSORS_POWER;
			} else if (size == strlen(BLUETOOTH_CMD_INTENSITY) + strlen(BLUETOOTH_CMD_REPULSORS) + 1) {
				device = REPULSORS_POWER;
				response = RESPONSE_NO_RESPONSE;
			} else {
				response = RESPONSE_ERROR;
			}
		} else if (strncmp(param, BLUETOOTH_CMD_UNIBEAM, strlen(BLUETOOTH_CMD_UNIBEAM)) == 0) {
			if (size == strlen(BLUETOOTH_CMD_INTENSITY) + strlen(BLUETOOTH_CMD_UNIBEAM) + 3) {
				device = UNIBEAM;
			} else if (size == strlen(BLUETOOTH_CMD_INTENSITY) + strlen(BLUETOOTH_CMD_UNIBEAM) + 1) {
				device = UNIBEAM;
				response = RESPONSE_NO_RESPONSE;
			} else {
				response = RESPONSE_ERROR;
			}
		} else {
			response = RESPONSE_ERROR;
		}

		if (response == RESPONSE_OK) {
			// convert number in ascii to integer
			uint8_t intensity = rxbuff[size - 1] - '0';

			if ((intensity >= 0) && (intensity <= 9)) {
				power_set_intensity(device, intensity);
			} else {
				response = RESPONSE_ERROR;
			}
		} else if (response == RESPONSE_NO_RESPONSE) {
			int8_t intensity = power_get_intensity(device);

			uart_puts(BLUETOOTH_CMD_INTENSITY);
			uart_puts(": ");
			uart_putc('0' + intensity);
			uart_puts("\r\n");
		}
	} else if (strncmp(rxbuff, BLUETOOTH_CMD_REBOOT, size) == 0) {
		response = RESPONSE_NO_RESPONSE;

		// stop reporting of battery status
		battery_reporting_stop();

		voice_play_sound(SOUND_SLEEP_0);
		_delay_ms(1000);
		voice_play_sound(SOUND_SLEEP_2);
		voice_play_sound_no_wait(SOUND_POWER_DOWN);

		// turn off all devices
		power_off(ALL);

		// restart mcu
		wdt_reboot();
	} else if (strncmp(rxbuff, BLUETOOTH_CMD_REPULSORS, strlen(BLUETOOTH_CMD_REPULSORS)) == 0) {
		char *param = rxbuff + strlen(BLUETOOTH_CMD_REPULSORS) + 1;

		if (size == strlen(BLUETOOTH_CMD_REPULSORS) + strlen(BLUETOOTH_PARAM_ON) + 1) {
			if (strncmp(param, BLUETOOTH_PARAM_ON, strlen(BLUETOOTH_PARAM_ON)) == 0) {
				power_on(REPULSORS_POWER);
			} else {
				response = RESPONSE_ERROR;
			}
		} else if (size == strlen(BLUETOOTH_CMD_REPULSORS) + strlen(BLUETOOTH_PARAM_OFF) + 1) {
			if (strncmp(param, BLUETOOTH_PARAM_OFF, strlen(BLUETOOTH_PARAM_OFF)) == 0) {
				power_off(REPULSORS_POWER);
			} else {
				response = RESPONSE_ERROR;
			}
		} else if (size == strlen(BLUETOOTH_CMD_REPULSORS)) {
			uint8_t state = power_state(REPULSORS_POWER);

			uart_puts(BLUETOOTH_CMD_REPULSORS);
			uart_puts(": ");

			if (state == POWER_ON) {
				uart_puts(BLUETOOTH_PARAM_ON);
			} else {
				uart_puts(BLUETOOTH_PARAM_OFF);
			}
			uart_puts("\r\n");

			response = RESPONSE_NO_RESPONSE;
		} else {
			response = RESPONSE_ERROR;
		}
	} else if (strncmp(rxbuff, BLUETOOTH_CMD_REPULSOR, strlen(BLUETOOTH_CMD_REPULSOR)) == 0) {
		char *param = rxbuff + strlen(BLUETOOTH_CMD_REPULSOR) + 1;

		if (size == strlen(BLUETOOTH_CMD_REPULSOR) + strlen(BLUETOOTH_PARAM_LEFT) + 1) {
			if (strncmp(param, BLUETOOTH_PARAM_LEFT, strlen(BLUETOOTH_PARAM_LEFT)) == 0) {
				power_blast(REPULSOR_LEFT);
			} else {
				response = RESPONSE_ERROR;
			}
		} else if (size == strlen(BLUETOOTH_CMD_REPULSOR) + strlen(BLUETOOTH_PARAM_RIGHT) + 1) {
			if (strncmp(param, BLUETOOTH_PARAM_RIGHT, strlen(BLUETOOTH_PARAM_RIGHT)) == 0) {
				power_blast(REPULSOR_RIGHT);
			} else {
				response = RESPONSE_ERROR;
			}
		} else {
			response = RESPONSE_ERROR;
		}
	} else if (strncmp(rxbuff, BLUETOOTH_CMD_UNIBEAM, strlen(BLUETOOTH_CMD_UNIBEAM)) == 0) {
		char *param = rxbuff + strlen(BLUETOOTH_CMD_UNIBEAM) + 1;

		if (size == strlen(BLUETOOTH_CMD_UNIBEAM) + strlen(BLUETOOTH_PARAM_ON) + 1) {
			if (strncmp(param, BLUETOOTH_PARAM_ON, strlen(BLUETOOTH_PARAM_ON)) == 0) {
				power_on(UNIBEAM);
			} else {
				response = RESPONSE_ERROR;
			}
		} else if (size == strlen(BLUETOOTH_CMD_UNIBEAM) + strlen(BLUETOOTH_PARAM_OFF) + 1) {
			if (strncmp(param, BLUETOOTH_PARAM_OFF, strlen(BLUETOOTH_PARAM_OFF)) == 0) {
				power_off(UNIBEAM);
			} else {
				response = RESPONSE_ERROR;
			}
		} else if (size == strlen(BLUETOOTH_CMD_UNIBEAM)) {
			uint8_t state = power_state(UNIBEAM);

			uart_puts(BLUETOOTH_CMD_UNIBEAM);
			uart_puts(": ");

			if (state == POWER_ON) {
				uart_puts(BLUETOOTH_PARAM_ON);
			} else {
				uart_puts(BLUETOOTH_PARAM_OFF);
			}
			uart_puts("\r\n");

			response = RESPONSE_NO_RESPONSE;
		} else {
			response = RESPONSE_ERROR;
		}
	} else if (strncmp(rxbuff, BLUETOOTH_CMD_VERSION, size) == 0) {
		uart_puts(BLUETOOTH_RESPONSE_VERSION);
		uart_puts("\r\n");
		response = RESPONSE_NO_RESPONSE;
	} else if (strncmp(rxbuff, BLUETOOTH_CMD_VOLUME, strlen(BLUETOOTH_CMD_VOLUME)) == 0) {
		if (size == strlen(BLUETOOTH_CMD_VOLUME) + 2) {
			// convert number in ascii to integer
			uint8_t volume = rxbuff[size - 1] - '0';

			if ((volume >= 0) && (volume <= 7)) {
				voice_set_volume(SOUND_VOLUME_0 + volume);
			} else {
				response = RESPONSE_ERROR;
			}
		} else if (size == strlen(BLUETOOTH_CMD_VOLUME)) {
			uint8_t volume = voice_get_volume() - SOUND_VOLUME_0;

			uart_puts(BLUETOOTH_CMD_VOLUME);
			uart_puts(": ");
			uart_putc('0' + volume);
			uart_puts("\r\n");

			response = RESPONSE_NO_RESPONSE;
		} else {
			response = RESPONSE_ERROR;
		}
	} else {
		response = RESPONSE_ERROR;
	}

	if (response == RESPONSE_OK) {
		uart_puts(BLUETOOTH_RESPONSE_OK);
		uart_puts("\r\n");
	} else if (response == RESPONSE_ERROR) {
		uart_puts(BLUETOOTH_RESPONSE_ERROR);
		uart_puts("\r\n");
	}

	if (echo) {
		uart_puts(BLUETOOTH_PROMPT);
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

	// Enable TX & RX, Enable RXC interrupt
	UCSRB = _BV(TXEN) | _BV(RXEN) | _BV(RXCIE);

	uart_flush();
}

void bluetooth_configure(void)
{
	uart_flush();
	_delay_ms(500);

	hc05_send_command("AT");
	_delay_ms(500);

	hc05_send_command("AT+NAME=\"Iron Man Mark IV\"");
	hc05_send_command("AT+PSWD=1234");
	hc05_send_command("AT+UART=38400,0,0");
	hc05_send_command("AT+CMODE=1");
	hc05_send_command("AT+CLASS=800804");
	hc05_send_command("AT+RESET");
}

void bluetooth_send(char *str)
{
	uart_puts(str);
	uart_puts("\r\n");
}
