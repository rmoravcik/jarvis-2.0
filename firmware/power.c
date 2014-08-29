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

enum {
	FADE_IN = 0,
	FADE_OUT
};

// timer2 overflow
ISR(TIMER2_OVF_vect)
{
	if (enabled & EYES) {
		PORTB ^= _BV(GPIO_EYES);
	}

	if (enabled & REPULSORS_POWER) {
		PORTC ^= _BV(GPIO_REPULSORS_PWR);
	}

	if (enabled & REPULSOR_LEFT) {
		PORTC ^= _BV(GPIO_REPULSOR_LEFT);
	}

	if (enabled & REPULSOR_RIGHT) {
		PORTC ^= _BV(GPIO_REPULSOR_RIGHT);
	}

	if (enabled & UNIBEAM) {
		PORTD ^= _BV(GPIO_UNIBEAM);
	}
}

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
	uint8_t start, stop, step, i, end = 0;
	uint8_t time, off_time, on_time;

	if (mode == FADE_IN) {
		start = 0;
		stop = 50;
	} else {
		start = 50;
		stop = 100;
	}

	// do fade effect in 50 steps
	for (step = start; step <= stop; step = step + 2) {
		// calculate on/off times for PWM with period 1kHz
		if (mode == FADE_IN) {
			on_time = step;
			off_time = 100 - on_time;

			if (step < 50) {
				end = 20;
			} else {
				end = 10;
			}
		} else {
			off_time = step;
			on_time = 100 - off_time;

			if (step < 50) {
				end = 10;
			} else {
				end = 20;
			}
		}

		// each PWM step is taking 40/20ms
		for (i = 0; i < end; i++) {
			device_off(devices);

			for (time = 0; time < off_time; time++) {
				_delay_us(20);
			}

			device_on(devices);

			for (time = 0; time < on_time; time++) {
				_delay_us(20);
			}
		}
	}

	// if fade in, leave gpio asserted high
	// else set it low
	if (mode == FADE_IN) {
		device_on(devices);
	} else {
		device_off(devices);
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

	// timer2 prescaler clk/128
	TCCR2 |= _BV(CS22) | _BV(CS20);

	// timer2 overflow interrupt enable
	TIMSK |= _BV(TOIE2);
}

void power_on(uint8_t devices)
{
	if (devices & EYES) {
		effect_blink(devices);
	}

	effect_fade(FADE_IN, devices);

	enabled |= devices;
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

		enabled &= ~devices;
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
