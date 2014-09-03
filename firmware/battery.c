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

#include <avr/interrupt.h>
#include <util/delay.h>

#include "common.h"
#include "helmet.h"
#include "power.h"
#include "voice.h"
#include "battery.h"

static uint16_t offset_error = 0;

static uint8_t last_capacity = 100;

static uint8_t high_reported = FALSE;
static uint8_t low_reported = FALSE;
static uint8_t dangerously_low_reported = FALSE;
static uint8_t emergency_backup_reported = FALSE;

static void simulate_power_failing(void)
{
	// blink also with eyes if helmet is closed
	if (helmet_state() == HELMET_CLOSED) {
		power_failure(ALL);
	} else {
		// blink with all devices
		power_failure(REPULSORS_POWER | UNIBEAM);
	}
}

static void report_battery_status(void)
{
	// read battery capacity
	uint8_t capacity = battery_get_capacity();

	// report only capacity changes
	if (last_capacity != capacity) {
		last_capacity = capacity;

		if (!high_reported & (capacity >= BATTERY_HIGH_CAPACITY)) {
			high_reported = TRUE;

			voice_play_sound(SOUND_BATTERY_CHARGED);
		} else if (capacity < BATTERY_LOW_CAPACITY) {
			// play warn notice that battery is almost dead (< 10%, < 20% and < 30%)
			if (capacity < BATTERY_BACKUP_CAPACITY) {
				if (!emergency_backup_reported) {
					emergency_backup_reported = TRUE;

					simulate_power_failing();

					voice_play_sound(SOUND_BUTTON_SOUND_ALARM_5);
					voice_play_sound(SOUND_TITLE_U_S);
					voice_play_sound(SOUND_BATTERY_LOW_2);

					// going to turn off all devices
					_delay_ms(1000);
					voice_play_sound(SOUND_SLEEP_2);
					voice_play_sound_no_wait(SOUND_POWER_DOWN);

					// turn off all devices
					power_off(ALL);
				}
			} else if (capacity < BATTERY_DAUNGEROUSLY_LOW_CAPACITY) {
				if (!dangerously_low_reported) {
					dangerously_low_reported = TRUE;

					simulate_power_failing();

					voice_play_sound(SOUND_BUTTON_SOUND_ALARM_5);
					voice_play_sound(SOUND_TITLE_U_S);
					voice_play_sound(SOUND_BATTERY_LOW_1);
				}
			} else {
				if (!low_reported) {
					low_reported = TRUE;

					simulate_power_failing();

					voice_play_sound(SOUND_BUTTON_SOUND_ALARM_5);
					voice_play_sound(SOUND_TITLE_U_S);
					voice_play_sound(SOUND_BATTERY_LOW_0);
				}
			}

		} else {
			low_reported = FALSE;
			dangerously_low_reported = FALSE;
			emergency_backup_reported = FALSE;
		}
	}
}

// timer0 overflow
ISR(TIMER0_OVF_vect, ISR_NOBLOCK)
{
	static uint8_t counter = 0;

	if (counter == 191) {
		// reset counter
		counter = 0;

		// check battery capacity every ~5 seconds
		report_battery_status();
	} else {
		counter++;
	}
}

void battery_init(void)
{
	// set battery sense pin as an input
	DDRC &= ~_BV(GPIO_BATTERY_SENSE);

	// enable internal 2.56V reference
	ADMUX |= _BV(REFS1) | _BV(REFS0);

	// enable ADC, set prescaler to 128 (62,5kHz)
	ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

	// set MUX channel to GND to measure offset
	ADMUX |= _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0);

	// measure ADC offset
	ADCSRA |= _BV(ADSC);

	// wait will ADC complete
	while (ADCSRA & _BV(ADSC)) {
	}

	offset_error = ADCW;

	// set MUX channel to ADC5 (battery sense pin)
	ADMUX |= _BV(MUX2) | _BV(MUX0);
	ADMUX &= ~(_BV(MUX3) | _BV(MUX1));

	// timer0 prescaler clk/1024
	TCCR0 |= _BV(CS02) | _BV(CS00);

}

uint8_t battery_get_capacity(void)
{
	uint8_t capacity = 0;

	// start ADC conversion
	ADCSRA |= _BV(ADSC);

	// wait will ADC complete (~200us)
	while (ADCSRA & _BV(ADSC)) {
	}

	// calc capacity of battery
	// 100% is 4.8 - 5.6V
	//   0% is 4V
	capacity = (ADCW - 627 - offset_error) * 100 / 126;

	// return maximum capacity 100%
	if (capacity > 100)
		capacity = 100;

	return capacity;
}

void battery_reporting_start(void)
{
	// timer0 overflow interrupt enable
	TIMSK |= _BV(TOIE0);
}

void battery_reporting_stop(void)
{
	// timer0 overflow interrupt disable
	TIMSK &= ~_BV(TOIE0);
}
