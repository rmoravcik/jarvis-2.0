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

#include <util/delay.h>

#include "common.h"
#include "helmet.h"
#include "power.h"
#include "voice.h"
#include "battery.h"

uint16_t adc_offset = 0;

uint8_t last_capacity = 100;

uint8_t low_reported = FALSE;
uint8_t dangerously_low_reported = FALSE;
uint8_t emergency_backup_reported = FALSE;

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

	adc_offset = ADCW;

	// set MUX channel to ADC5 (battery sense pin)
	ADMUX |= _BV(MUX2) | _BV(MUX0);
	ADMUX &= ~(_BV(MUX3) | _BV(MUX1));
}

uint8_t battery_get_capacity(void)
{
	uint8_t capacity = 0;

	// start ADC conversion
	ADCSRA |= _BV(ADSC);

	// wait will ADC complete
	while (ADCSRA & _BV(ADSC)) {
	}

	// calc capacity of battery
	// 100% is 4.8 - 5.6V
	//   0% is 4V
	capacity = (ADCW - 654 - adc_offset) * 100 / 130;

	// return maximum capacity 100%
	if (capacity > 100)
		capacity = 100;

	return capacity;
}

void battery_report_capacity(uint8_t report_high)
{
	// read battery capacity
	uint8_t capacity = battery_get_capacity();

	// report only capacity changes
	if (last_capacity != capacity) {
		last_capacity = capacity;

		if (report_high & (capacity >= BATTERY_HIGH_CAPACITY)) {
			voice_play_sound(SOUND_BATTERY_CHARGED);
		} else if (capacity < BATTERY_LOW_CAPACITY) {
			// blink also with eyes if helmet is closed
			if (helmet_state() == HELMET_CLOSED) {
				power_failure(ALL | EYES);
			} else {
				// blink with all devices
				power_failure(ALL);
			}

			// play warn notice that battery is almost dead (< 10%, < 20% and < 30%)
			if (capacity < BATTERY_BACKUP_CAPACITY) {
				if (!emergency_backup_reported) {
					emergency_backup_reported = TRUE;
					voice_play_sound(SOUND_BATTERY_LOW_2);

					// going to turn off all devices
					_delay_ms(1000);
					voice_play_sound(SOUND_SLEEP_2);
					voice_play_sound_no_wait(SOUND_POWER_DOWN);

					// turn off all devices
					power_off(ALL | EYES);
				}
			} else if (capacity < BATTERY_DAUNGEROUSLY_LOW_CAPACITY) {
				if (!dangerously_low_reported) {
					dangerously_low_reported = TRUE;
					voice_play_sound(SOUND_BATTERY_LOW_1);
				}

				emergency_backup_reported = FALSE;
			} else {
				if (!low_reported) {
					low_reported = TRUE;
					voice_play_sound(SOUND_BATTERY_LOW_0);
				}

				dangerously_low_reported = FALSE;
				emergency_backup_reported = FALSE;
			}
		} else {
			low_reported = FALSE;
			dangerously_low_reported = FALSE;
			emergency_backup_reported = FALSE;
		}
	}
}
