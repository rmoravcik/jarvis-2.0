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

#include <avr/interrupt.h>
#include <util/delay.h>

#include "common.h"
#include "battery.h"
#include "eyes.h"
#include "helmet.h"
#include "repulsor.h"
#include "voice.h"

#define VOICE_SILENT 1

static void init(void);
static void battery_report_capacity(void);
static void battery_warn_notice(void);

// helmet button
ISR(INT0_vect)
{
	uint8_t press_counter = 0;

	while (!(PIND & _BV(GPIO_HELMET_BUTTON)) && press_counter < 10) {
		_delay_ms(200);
		press_counter++;
	}

	// short press of helmet button
	if (press_counter == 1) {
		if (helmet_state() == HELMET_CLOSED) {
			voice_play_sound(SOUND_JARVIS_BATTERY_FULL_2);
		} else {
			voice_play_sound(SOUND_JARVIS_EARLY_TO_RISE);
		}
	}

	// long press of helmet button
	if (press_counter == 10) {
		if (helmet_state() == HELMET_CLOSED) {
			// turn off eyes
			eyes_power_down();

			// open helmet
			helmet_open();
		} else {
			// close helmet
			helmet_close();

			// turn on eyes
			eyes_power_up();

			// check if battery is not dead
			battery_warn_notice();
		}
	}

	sei();
}

// repulsor button
ISR(INT1_vect)
{
	uint8_t press_counter = 0;

	while (!(PIND & _BV(GPIO_REPULSOR_BUTTON)) && press_counter < 10) {
		_delay_ms(200);
		press_counter++;
	}

	// short press of repulsor button
	if (press_counter == 1) {
		if (helmet_state() == HELMET_CLOSED) {
			voice_play_sound(SOUND_JARVIS_BATTERY_FULL_3);
		} else {
			voice_play_sound(SOUND_JARVIS_ARC_REACTOR_MODE);
		}
	}

	// long press of repulsor button
	if (press_counter == 10) {
		if (helmet_state() == HELMET_CLOSED) {
			// helmet is closed, we can shot from repulsor
			voice_play_sound_no_wait(SOUND_REPULSOR);

			// simulate shot after 1sec
			_delay_ms(1000);

			repulsor_blast();

			// check if battery is not dead
			battery_warn_notice();
		} else {
			// helmet is open
			// FIXME: what to do now?
		}
	}

	sei();
}

int main(void)
{
	init();

	battery_init();
	eyes_init();
	repulsor_init();
	helmet_init();
	voice_init();

	// enable repulsor
	repulsor_power_up();

#ifdef VOICE_SILENT
	voice_set_volume(SOUND_VOLUME_7);
#endif

	// report battery capacity after power on
	battery_report_capacity();

	// main loop
	while(1) {
	}

	return 0;
}

static void init(void)
{
	DDRB = 0xff;
	DDRC = 0xff;

	DDRD = 0xff;
	DDRD &= ~(_BV(GPIO_HELMET_BUTTON) | _BV(GPIO_REPULSOR_BUTTON));

	// enable pull ups on helmet and repulsor inputs
	PORTD |= _BV(GPIO_HELMET_BUTTON) | _BV(GPIO_REPULSOR_BUTTON);

	// falling edge on INT0, falling edge on INT1
	MCUCR |= _BV(ISC11) | _BV(ISC01);

	// enable INT0 and INT1 interrupts
	GIMSK |= _BV(INT1) | _BV(INT0);

	sei();
}

static void battery_report_capacity(void)
{
	// read battery capacity
	uint8_t capacity = battery_get_capacity();

	if (capacity == 0) {
		voice_play_sound(SOUND_JARVIS_BATTERY_LOW_1);
	} else {
		voice_play_sound(SOUND_JARVIS_BATTERY_POWER_AT);

		if (capacity <= 20) {
			voice_play_sound(SOUND_JARVIS_BATTERY_1 + (capacity - 1));
		} else {
			uint8_t remainder = capacity % 10;

			voice_play_sound(SOUND_JARVIS_BATTERY_20 + ((capacity / 10) - 2));

			if (remainder > 0) {
				voice_play_sound(SOUND_JARVIS_BATTERY_1 + (remainder - 1));
			}
		}
		voice_play_sound(SOUND_JARVIS_BATTERY_PERCENT);
	}
}

static void battery_warn_notice(void)
{
	// read battery capacity
	uint8_t capacity = battery_get_capacity();

	if (capacity < 20) {
		// blink with eyes if helmet is closed
		if (helmet_state() == HELMET_CLOSED) {
			eyes_power_failure();
		}

		// blink with repulsor
		repulsor_power_failure();

		// play warn notice that battery is almost dead
		voice_play_sound(SOUND_JARVIS_BATTERY_LOW_1);
	}
}
