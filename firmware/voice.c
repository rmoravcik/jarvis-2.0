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

#include <avr/eeprom.h>
#include <util/delay.h>

#include "common.h"

#include "voice.h"

#define STOP_PLAYING		0xFE

static uint8_t EEMEM eeprom_seed = 27;

static uint8_t seed;

uint8_t random_get(uint8_t max)
{
	static uint8_t old = 0;
	uint8_t new = 0;

	do {
		seed = (seed * 109 + 89) % 256;
		new = seed % max;
	} while (new == old);

	eeprom_write_byte(&eeprom_seed, seed);
	old = new;

	return new;
}

static void wt588d_send_command(uint8_t command)
{
	uint8_t i;

	PORTD &= ~_BV(GPIO_WT588_RESET);
	_delay_ms(5);
	PORTD |= _BV(GPIO_WT588_RESET);

	_delay_ms(17);

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
}

void voice_init(void)
{
	seed = eeprom_read_byte(&eeprom_seed);

	// set DATA and RESET pins as an outputs
	DDRD |= _BV(GPIO_WT588_DATA) | _BV(GPIO_WT588_RESET);

	// set BUSY pin as an input
	DDRB &= ~_BV(GPIO_WT588_BUSY);

	// set DATA and RESET pins to high
	PORTD |= _BV(GPIO_WT588_DATA) | _BV(GPIO_WT588_RESET);

	_delay_ms(100);
}

void voice_play_welcome(void)
{
	uint8_t i = 0;

	i = random_get(5);

	switch (i) {
		case 0:
			voice_play_sound(SOUND_LISTENING_ON_1);
			break;

		case 1:
			voice_play_sound(SOUND_LISTENING_ON_4);
			voice_play_sound(SOUND_LISTENING_ON_6);
			break;

		case 2:
			voice_play_sound(SOUND_WELCOME);
			break;

		case 3:
			voice_play_sound(SOUND_LISTENING_ON_AFTERNOON);
			voice_play_sound(SOUND_ASK_0);
			break;

		case 4:
			voice_play_sound(SOUND_LISTENING_ON_EVENING);
			voice_play_sound(SOUND_ASK_0);
			break;

		case 5:
			voice_play_sound(SOUND_LISTENING_ON_MORNING);
			voice_play_sound(SOUND_ASK_0);
			break;
	}
}

void voice_play_random(void)
{
	uint8_t i = 0;

	i = random_get(9);

	switch (i) {
		case 0:
			voice_play_sound(SOUND_CLOCK_AFTERNOON);
			break;

		case 1:
			voice_play_sound(SOUND_CLOCK_ALARM_WAKE_3);
			voice_play_sound_no_wait(SOUND_ACDC);
			break;

		case 2:
			voice_play_sound(SOUND_CLOCK_LATE_1);
			break;

		case 3:
			voice_play_sound(SOUND_NETWORK_LOST_WIFI);
			voice_play_sound(SOUND_NETWORK_NO_WIFI);
			break;

		case 4:
			voice_play_sound(SOUND_SELF_DESTRUCT_1);
			break;

		case 5:
			voice_play_sound(SOUND_TRS4);
			break;

		case 6:
			voice_play_sound(SOUND_TRS7_1);
			break;

		case 7:
			voice_play_sound(SOUND_TRS9);
			break;

		case 8:
			voice_play_sound(SOUND_TRS10);
			break;

		case 9:
			voice_play_sound(SOUND_TRS11);
			voice_play_sound(SOUND_JUST_KIDDING);
			break;
	}
}

uint8_t voice_is_playing(void)
{
	// busy is held down during the playback
	if (PINB & _BV(GPIO_WT588_BUSY)) {
		return FALSE;
	} else {
		return TRUE;
	}
}

void voice_play_sound(uint8_t sound)
{
	// send command to play a sound
	wt588d_send_command(sound);

	// wait till wt588d start playing sample
	while (!voice_is_playing());

	// wait till wt588d finish playing sample
	while (voice_is_playing());

	// busy signal is held down for 32ms
	_delay_ms(32);
}

void voice_play_sound_no_wait(uint8_t sound)
{
	// send command to play a sound
	wt588d_send_command(sound);
}

void voice_set_volume(uint8_t volume)
{
	wt588d_send_command(volume);
}

void voice_stop_playback(void)
{
	wt588d_send_command(STOP_PLAYING);
}
