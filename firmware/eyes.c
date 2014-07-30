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
#include "eyes.h"

enum {
	FADE_IN = 0,
	FADE_OUT
};

static void eyes_blink(void);
static void eyes_fade_effect(uint8_t mode);

void eyes_init()
{
	// set eyes pin as in output
	DDRB |= _BV(GPIO_EYES);

	// set eyes pin to low
	PORTB &= ~_BV(GPIO_EYES);
}

void eyes_power_up(void)
{
	eyes_blink();
	eyes_fade_effect(FADE_IN);
}

void eyes_power_down(void)
{
	PORTB &= ~_BV(GPIO_EYES);

	// wait for 200ms before opening helmet
	_delay_ms(200);
}

void eyes_power_failure(void)
{
	eyes_blink();
}

// takes 200ms
static void eyes_blink(void)
{
	PORTB &= ~_BV(GPIO_EYES);
	_delay_ms(50);
	PORTB |= _BV(GPIO_EYES);
	_delay_ms(50);
	PORTB &= ~_BV(GPIO_EYES);
	_delay_ms(50);
	PORTB |= _BV(GPIO_EYES);
	_delay_ms(50);
}

// takes 3000ms
static void eyes_fade_effect(uint8_t mode)
{
	uint8_t step, i = 0;
	uint8_t time, off_time, on_time;

	// do fade effect in 50 steps
	for (step = 0; step <= 50; step++) {
		// calculate on/off times for PWM with period 1kHz
		if (mode == FADE_IN) {
			on_time = 2 * step;
			off_time = 100 - on_time;
		} else {
			off_time = 2 * step;
			on_time = 100 - off_time;
		}

		// each PWM step is taking 60ms
		for (i = 0; i < 30; i++) {
			PORTB &= ~_BV(GPIO_EYES);

			for (time = 0; time < off_time; time++) {
				_delay_us(20);
			}

			PORTB |= _BV(GPIO_EYES);

			for (time = 0; time < on_time; time++) {
				_delay_us(20);
			}
		}
	}

	// if fade in, leave gpio asserted high
	// else set it low
	if (mode == FADE_IN) {
		PORTB |= _BV(GPIO_EYES);
	} else {
		PORTB &= ~_BV(GPIO_EYES);
	}
}
