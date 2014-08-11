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

#include <avr/eeprom.h>

#include <avr/interrupt.h>
#include <util/delay.h>

#include "common.h"
#include "battery.h"
#include "bluetooth.h"
#include "helmet.h"
#include "power.h"
#include "random.h"
#include "voice.h"

#define VOICE_SILENT 1

static void init(void);
static void configure(void);
static void battery_report_capacity(void);
static void battery_warn_notice(void);

// func button
ISR(INT0_vect)
{
	uint8_t press_counter = 0;

	while (!(PIND & _BV(GPIO_FUNC_BUTTON)) && press_counter < 10) {
		_delay_ms(200);
		press_counter++;
	}

	// short press of func button
	if (press_counter == 1) {
		voice_play_sound(SOUND_BUTTON_SOUND_0);

		if (helmet_state() == HELMET_CLOSED) {
		} else {
			voice_play_random();
		}
	}

	// long press of func button
	if (press_counter == 10) {
		voice_play_sound(SOUND_BUTTON_SOUND_0);

		if (helmet_state() == HELMET_CLOSED) {
			// turn off eyes
			power_off(EYES);

			// open helmet
			helmet_open();
		} else {
			// close helmet
			helmet_close();

			// turn on eyes
			power_on(EYES);

			// check if battery is not dead
			battery_warn_notice();
		}
	}
}

int main(void)
{
	uint8_t configured = 0;

	init();
	bluetooth_init();
	random_init();
	power_init();
	battery_init();
	helmet_init();
	voice_init();

#ifdef VOICE_SILENT
	voice_set_volume(SOUND_VOLUME_7);
#endif

	// check if configuration mode was requested
	if (!(PIND & _BV(GPIO_FUNC_BUTTON))) {
		configure();
		configured = 1;
	}

	power_on(ALL);

	if (!configured) {
		voice_play_welcome();
	}

	_delay_ms(1000);

	// report battery capacity after power on
	battery_report_capacity();

	// main loop
	while(1) {
	}

	return 0;
}

static void init(void)
{
	OSCCAL = 0xb5;

	DDRB = 0xff;
	DDRC = 0xff;

	DDRD = 0xff;
	DDRD &= ~_BV(GPIO_FUNC_BUTTON);

	// enable pull up on func button
	PORTD |= _BV(GPIO_FUNC_BUTTON);

	// falling edge on INT0
	MCUCR |= _BV(ISC01);

	// enable INT0 interrupt
	GIMSK |= _BV(INT0);

	sei();
}

static void configure(void)
{
	voice_play_sound(SOUND_BD_CALIBRATING);

	bluetooth_configure();

	voice_play_sound(SOUND_CONFIRM_6);

	_delay_ms(1000);
	voice_play_sound(SOUND_INTRO_2);
}

static void battery_report_capacity(void)
{
	// read battery capacity
	uint8_t capacity = battery_get_capacity();

	if (capacity >= 90) {
		voice_play_sound(SOUND_BATTERY_CHARGED);
	} else if (capacity < 20) {
		voice_play_sound(SOUND_BATTERY_LOW_0);
	}
}

static void battery_warn_notice(void)
{
	// read battery capacity
	uint8_t capacity = battery_get_capacity();

	if (capacity < 20) {
		// blink also with eyes if helmet is closed
		if (helmet_state() == HELMET_CLOSED) {
			power_failure(ALL | EYES);
		} else {
			// blink with all devices
			power_failure(ALL);
		}

		// play warn notice that battery is almost dead
		voice_play_sound(SOUND_BATTERY_LOW_1);
	}
}
