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
#include "power.h"

static uint8_t enabled = 0;

static uint8_t duty[3] = {0, 0, 0};

#define DUTY_EYES	0
#define DUTY_REPULSORS	1
#define DUTY_UNIBEAM	2

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
		if (enabled & EYES) {
			return TRUE;
		} else {
			return FALSE;
		}
	}

	if (device & REPULSORS_POWER) {
		if (enabled & REPULSORS_POWER) {
			return TRUE;
		} else {
			return FALSE;
		}
	}

	if (device & REPULSOR_LEFT) {
		if (enabled & REPULSOR_LEFT) {
			return TRUE;
		} else {
			return FALSE;
		}
	}

	if (device & REPULSOR_RIGHT) {
		if (enabled & REPULSOR_RIGHT) {
			return TRUE;
		} else {
			return FALSE;
		}
	}

	if (device & UNIBEAM) {
		if (enabled & UNIBEAM) {
			return TRUE;
		} else {
			return FALSE;
		}
	}

	return FALSE;
}

// timer2 overflow
ISR(TIMER2_OVF_vect)
{
	static uint8_t cycle = 0;

	if (cycle == 0) {
		if (enabled & EYES) {
			PORTB |= _BV(GPIO_EYES);
		}

		if (enabled & REPULSORS_POWER) {
			PORTC |= _BV(GPIO_REPULSORS_PWR);
		}

		if (enabled & UNIBEAM) {
			PORTD |= _BV(GPIO_UNIBEAM);
		}
	} else if (cycle == duty[DUTY_EYES]) {
		if (enabled & EYES) {
			PORTB &= ~_BV(GPIO_EYES);
		}
	} else if (cycle == duty[DUTY_REPULSORS]) {
		if (enabled & REPULSORS_POWER) {
			PORTC &= ~_BV(GPIO_REPULSORS_PWR);
		}
	} else if (cycle == duty[DUTY_UNIBEAM]) {
		if (enabled & UNIBEAM) {
			PORTD &= ~_BV(GPIO_UNIBEAM);
		}
	}

	cycle++;

	if (cycle >= 100) {
		cycle = 0;
	}
}

// takes 200ms
static void effect_blink(uint8_t devices)
{
	if (device_get(devices)) {
		// temporary disable PWM on requested devices
		enabled &= ~devices;

		device_off(devices);
		_delay_ms(50);
		device_on(devices);
		_delay_ms(50);
		device_off(devices);
		_delay_ms(50);
		device_on(devices);
		_delay_ms(50);

		// re-enable disabled PWM
		enabled |= devices;
	} else {
		// inverse blinking is used only eyes power on effect
		if (devices == EYES) {
			device_on(devices);
			_delay_ms(50);
			device_off(devices);
			_delay_ms(50);
			device_on(devices);
			_delay_ms(50);
			device_off(devices);
			_delay_ms(50);
		}
	}
}

// takes 1500ms
static void effect_fade(uint8_t mode, uint8_t devices)
{
	uint8_t step;

	enabled |= devices;

	// do fade effect in 50 steps
	for (step = 0; step <= 100; step = step + 2) {

		if (mode == FADE_IN) {
			if (devices & EYES) {
				duty[DUTY_EYES] = step;
			}

			if (devices & REPULSORS_POWER) {
				duty[DUTY_REPULSORS] = step;
			}

			if (devices & UNIBEAM) {
				duty[DUTY_UNIBEAM] = step;
			}
		} else {
			if (devices & EYES) {
				duty[DUTY_EYES] = 100 - step;
			}

			if (devices & REPULSORS_POWER) {
				duty[DUTY_REPULSORS] = 100 - step;
			}

			if (devices & UNIBEAM) {
				duty[DUTY_UNIBEAM] = 100 - step;
			}
		}

		_delay_ms(15);
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

	// timer2 prescaler clk (no prescaler)
	TCCR2 |= _BV(CS20);

	// timer2 overflow interrupt enable
	TIMSK |= _BV(TOIE2);
}

void power_on(uint8_t devices)
{
	if (devices & EYES) {
		effect_blink(devices);
	}

	effect_fade(FADE_IN, devices);
}

void power_off(uint8_t devices)
{

	if (devices == EYES) {
		enabled &= ~EYES;
		device_off(EYES);
	} else {
		// do not try to fade out device, if it's already off
		if ((devices & EYES) && !device_get(EYES)) {
			devices &= ~EYES;
		} else if ((devices & REPULSORS_POWER) && !device_get(REPULSORS_POWER)) {
			devices &= ~REPULSORS_POWER;
		} else if ((devices & UNIBEAM) && !device_get(UNIBEAM)) {
			devices &= ~UNIBEAM;
		}

		effect_fade(FADE_OUT, devices);
	}
}

void power_failure(uint8_t devices)
{
	effect_blink(devices);
}

void power_blast(uint8_t device)
{
	// simulate power blast
	device_on(device);
	_delay_ms(100);
	device_off(device);
}

uint8_t power_state(uint8_t device)
{
	if (device_get(device)) {
		return POWER_ON;
	} else {
		return POWER_OFF;
	}
}
