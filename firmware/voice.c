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
static uint8_t EEMEM eeprom_volume = SOUND_VOLUME_7;

static uint8_t seed;
static uint8_t volume;

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

static void wt588d_reset(void)
{
	PORTD &= ~_BV(GPIO_WT588_RESET);
	_delay_ms(5);
	PORTD |= _BV(GPIO_WT588_RESET);

	_delay_ms(17);
}

static void wt588d_send_command(uint8_t command)
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
}

void voice_init(void)
{
	seed = eeprom_read_byte(&eeprom_seed);

	// initialize with default value if eeprom is empty
	if (seed == 0xFF) {
		seed = 27;
		eeprom_write_byte(&eeprom_seed, seed);
	}

	volume = eeprom_read_byte(&eeprom_volume);

	// initialize with default value if eeprom is corrupted
	if ((volume < SOUND_VOLUME_0) || (volume > SOUND_VOLUME_7)) {
		volume = SOUND_VOLUME_7;
		eeprom_write_byte(&eeprom_volume, volume);
	}

	// set DATA and RESET pins as an outputs
	DDRD |= _BV(GPIO_WT588_DATA) | _BV(GPIO_WT588_RESET);

	// set BUSY pin as an input
	DDRB &= ~_BV(GPIO_WT588_BUSY);

	// set DATA and RESET pins to high
	PORTD |= _BV(GPIO_WT588_DATA) | _BV(GPIO_WT588_RESET);

	_delay_ms(100);

	wt588d_reset();

	wt588d_send_command(volume);
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
			voice_play_sound(SOUND_ASK_0);
			break;

		case 2:
			voice_play_sound(SOUND_LISTENING_ON_AFTERNOON);
			voice_play_sound(SOUND_LISTENING_ON_7);
			break;

		case 3:
			voice_play_sound(SOUND_LISTENING_ON_EVENING);
			voice_play_sound(SOUND_LISTENING_ON_7);
			break;

		case 4:
			voice_play_sound(SOUND_LISTENING_ON_MORNING);
			voice_play_sound(SOUND_LISTENING_ON_7);
			break;

		case 5:
			voice_play_sound(SOUND_WELCOME);
			voice_play_sound(SOUND_LISTENING_ON_3);
			break;
	}
}

void voice_play_quote(void)
{
	uint8_t i = 0;

	i = random_get(24);

	switch (i) {
		case 0:
			voice_play_sound(SOUND_CLOCK_AFTERNOON); // 3.20
			break;

		case 1:
			voice_play_sound(SOUND_CLOCK_ALARM_SNOOZE_0); // 1.94
			break;

		case 2:
			voice_play_sound(SOUND_CLOCK_ALARM_SNOOZE_2); // 3.81
			break;

		case 3:
			voice_play_sound(SOUND_CLOCK_ALARM_WAKE_3); // 2.17
			voice_play_sound_no_wait(SOUND_ACDC); // 1:11.41
			break;

		case 4:
			voice_play_sound(SOUND_CLOCK_LATE_0); // 3.70
			break;

		case 5:
			voice_play_sound(SOUND_CLOCK_LATE_1); // 4.61
			break;

		case 6:
			voice_play_sound(SOUND_LISTENING_OFF_2); // 2.93
			break;

		case 7:
			voice_play_sound(SOUND_MESSAGE_NEW_6); // 2.76
			break;

		case 8:
			voice_play_sound(SOUND_MESSAGE_NEW_7); // 3.45
			break;

		case 9:
			voice_play_sound(SOUND_NETWORK_LOST_WIFI); // 1.89
			voice_play_sound(SOUND_NETWORK_NO_WIFI); // 3.75
			break;

		case 10:
			voice_play_sound(SOUND_REPEAT_1); // 2.04
			break;

		case 11:
			voice_play_sound(SOUND_REPEAT_2); // 3.77
			break;

		case 12:
			voice_play_sound(SOUND_REPEAT_3); // 4.03
			break;

		case 13:
			voice_play_sound(SOUND_SELF_DESTRUCT_1); // 5.82
			break;

		case 14:
			voice_play_sound(SOUND_SUITS_DECRYPT_1); // 2.99
			_delay_ms(1500);
			voice_play_sound(SOUND_SUITS_DECRYPT_2); // 5.37
			_delay_ms(2000);
			voice_play_sound(SOUND_SUITS_DECRYPT_3); // 2.06
			_delay_ms(1000);
			voice_play_sound(SOUND_SUITS_DECRYPT_4); // 3.48
			_delay_ms(200);
			voice_play_sound(SOUND_SUITS_DECRYPT_5); // 3.55
			_delay_ms(300);
			voice_play_sound(SOUND_SUITS_DECRYPT_6); // 1.78
			_delay_ms(100);

			i = random_get(1);
			if (i == 1) {
				voice_play_sound(SOUND_SUITS_DECRYPT_8); // 8.18
			} else {
				voice_play_sound(SOUND_SUITS_DECRYPT_7); // 11.22
			}
			break;

		case 15:
			voice_play_sound(SOUND_TRS3_0); // 3.74
			break;

		case 16:
			voice_play_sound(SOUND_TRS4); // 4.29
			break;

		case 17:
			voice_play_sound(SOUND_TRS6); // 4.47
			break;

		case 18:
			voice_play_sound(SOUND_TRS7_1); // 2.97
			break;

		case 19:
			voice_play_sound(SOUND_TRS8); // 1.83
			break;

		case 20:
			voice_play_sound(SOUND_TRS9); // 4.92
			break;

		case 21:
			voice_play_sound(SOUND_TRS10); // 6.71
			break;

		case 22:
			voice_play_sound(SOUND_TRS11); // 4.90
			_delay_ms(1000);
			voice_play_sound(SOUND_JUST_KIDDING); // 0.62
			break;

		case 23:
			voice_play_sound(SOUND_UNAVAILABLE_0); // 3.31
			break;

		case 24:
			voice_play_sound(SOUND_UNAVAILABLE_3); // 2.96
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

void voice_set_volume(uint8_t level)
{
	if (volume != level) {
		volume = level;
		eeprom_write_byte(&eeprom_volume, volume);
	}

	wt588d_send_command(level);

}

uint8_t voice_get_volume(void)
{
	return volume;
}

void voice_stop_playback(void)
{
	wt588d_send_command(STOP_PLAYING);
}
