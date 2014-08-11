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
#include "power.h"

enum {
	FADE_IN = 0,
	FADE_OUT
};

static void device_on(uint8_t device)
{
	if (device & EYES) {
		PORTB |= _BV(GPIO_EYES);
	}

	if (device & REPULSORS_POWER) {
		PORTC |= _BV(GPIO_REPULSORS_PWR);
	}

	if (device & REPULSOR_LEFT) {
		PORTC |= _BV(GPIO_REPULSOR_LEFT);
	}

	if (device & REPULSOR_RIGHT) {
		PORTC |= _BV(GPIO_REPULSOR_RIGHT);
	}

	if (device & UNIBEAM) {
		PORTD |= _BV(GPIO_UNIBEAM);
	}
}

static void device_off(uint8_t device)
{
	if (device & EYES) {
		PORTB &= ~_BV(GPIO_EYES);
	}

	if (device & REPULSORS_POWER) {
		PORTC &= ~_BV(GPIO_REPULSORS_PWR);
	}

	if (device & REPULSOR_LEFT) {
		PORTC &= ~_BV(GPIO_REPULSOR_LEFT);
	}

	if (device & REPULSOR_RIGHT) {
		PORTC &= ~_BV(GPIO_REPULSOR_RIGHT);
	}

	if (device & UNIBEAM) {
		PORTD &= ~_BV(GPIO_UNIBEAM);
	}
}

static uint8_t device_get(uint8_t device)
{
	if (device & EYES) {
		if (PORTB & _BV(GPIO_EYES)) {
			return 1;
		} else {
			return 0;
		}
	}

	if (device & REPULSORS_POWER) {
		if (PORTC & _BV(GPIO_REPULSORS_PWR)) {
			return 1;
		} else {
			return 0;
		}
	}

	if (device & REPULSOR_LEFT) {
		if (PORTC & _BV(GPIO_REPULSOR_LEFT)) {
			return 1;
		} else {
			return 0;
		}
	}

	if (device & REPULSOR_RIGHT) {
		if (PORTC & _BV(GPIO_REPULSOR_RIGHT)) {
			return 1;
		} else {
			return 0;
		}
	}

	if (device & UNIBEAM) {
		if (PORTD & _BV(GPIO_UNIBEAM)) {
			return 1;
		} else {
			return 0;
		}
	}

	return 0;
}

// takes 200ms
static void effect_blink(uint8_t device)
{
	if (device_get(device)) {
		device_off(device);
		_delay_ms(50);
		device_on(device);
		_delay_ms(50);
		device_off(device);
		_delay_ms(50);
		device_on(device);
		_delay_ms(50);
	} else {
		device_on(device);
		_delay_ms(50);
		device_off(device);
		_delay_ms(50);
		device_on(device);
		_delay_ms(50);
		device_off(device);
		_delay_ms(50);
	}
}

// takes 3000ms
static void effect_fade(uint8_t mode, uint8_t device)
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
			device_off(device);

			for (time = 0; time < off_time; time++) {
				_delay_us(20);
			}

			device_on(device);

			for (time = 0; time < on_time; time++) {
				_delay_us(20);
			}
		}
	}

	// if fade in, leave gpio asserted high
	// else set it low
	if (mode == FADE_IN) {
		device_on(device);
	} else {
		device_off(device);
	}
}

void power_init(void)
{
	// set eyes pin as an output
	DDRB |= _BV(GPIO_EYES);

	// set eyes pin to low
	PORTB &= ~_BV(GPIO_EYES);

	// set repuslor blast and pwr pins as an output
	DDRC |= _BV(GPIO_REPULSORS_PWR) | _BV(GPIO_REPULSOR_LEFT) | _BV(GPIO_REPULSOR_RIGHT);

	// set power blast and pwr pins to low
	PORTC &= ~(_BV(GPIO_REPULSORS_PWR) | _BV(GPIO_REPULSOR_LEFT) | _BV(GPIO_REPULSOR_RIGHT));

	// set unibeam pin as an output
	DDRD |= _BV(GPIO_UNIBEAM);

	// set unibeam pin to low
	PORTD &= ~_BV(GPIO_UNIBEAM);
}

void power_on(uint8_t device)
{
	if (device & EYES) {
		effect_blink(device);
	}

	effect_fade(FADE_IN, device);
}

void power_off(uint8_t device)
{
	if (device & EYES) {
		device_off(EYES);

		// wait for 200ms before opening helmet
		_delay_ms(200);
	} else {
		effect_fade(FADE_OUT, device);
	}

}

void power_failure(uint8_t device)
{
	effect_blink(device);
}

void power_blast(uint8_t device)
{
	// simulate power blast
	device_on(device);
	_delay_ms(100);
	device_off(device);
}
