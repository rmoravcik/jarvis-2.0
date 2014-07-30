/*
 *  iron-man-helmet-controller
 *  Copyright (C) 2013 Roman Moravcik
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

#include <util/delay.h>

#include "common.h"
#include "voice.h"

static void voice_send_command(uint8_t command);

void voice_init(void)
{
	// set DATA pin as an output
	DDRD |= _BV(GPIO_WT588_DATA);

	// set BUSY and RESET pins as an inputs
	DDRB &= ~_BV(GPIO_WT588_BUSY);
	DDRD &= ~_BV(GPIO_WT588_RESET);

	// set DATA pin to high
	PORTD |= _BV(GPIO_WT588_DATA);

	_delay_ms(100);
}

void voice_play_sound(uint8_t sound)
{
	// send command to play a sound
	voice_send_command(sound);

	// wait till busy signal is asserted low by voice module
	while (PINB & _BV(GPIO_WT588_BUSY)) {
	}

	// busy signal is held down for 32ms
	_delay_ms(40);
}

void voice_play_sound_no_wait(uint8_t sound)
{
	// send command to play a sound
	voice_send_command(sound);
}

void voice_set_volume(uint8_t volume)
{
	voice_send_command(volume);
}

static void voice_send_command(uint8_t command)
{
	uint8_t i;

	PORTD &= ~_BV(GPIO_WT588_DATA);
	_delay_ms(5);
	PORTD |= _BV(GPIO_WT588_DATA);

	for (i = 0; i < 8; i++) {
		if (command & 1) {
			PORTD |= _BV(GPIO_WT588_DATA);
			_delay_us(600);
			PORTD &= ~_BV(GPIO_WT588_DATA);
			_delay_us(200);
		} else {
			PORTD |= _BV(GPIO_WT588_DATA);
			_delay_us(200);
			PORTD &= ~_BV(GPIO_WT588_DATA);
			_delay_us(600);
		}

		command >>= 1;
	}

	PORTD |= _BV(GPIO_WT588_DATA);
	_delay_ms(20);
}

