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

static uint8_t status = 0;

enum {
	FADE_IN = 0,
	FADE_OUT
};

// timer2 overflow
ISR(TIMER2_OVF_vect)
{
	if (status & EYES) {
		PORTB ^= _BV(GPIO_EYES);
	}

	if (status & REPULSORS_POWER) {
		PORTC ^= _BV(GPIO_REPULSORS_PWR);
	}

	if (status & REPULSOR_LEFT) {
		PORTC ^= _BV(GPIO_REPULSOR_LEFT);
	}

	if (status & REPULSOR_RIGHT) {
		PORTC ^= _BV(GPIO_REPULSOR_RIGHT);
	}

	if (status & UNIBEAM) {
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
		if (status & EYES) {
			return TRUE;
		} else {
			return FALSE;
		}
	}

	if (device & REPULSORS_POWER) {
		if (status & REPULSORS_POWER) {
			return TRUE;
		} else {
			return FALSE;
		}
	}

	if (device & REPULSOR_LEFT) {
		if (status & REPULSOR_LEFT) {
			return TRUE;
		} else {
			return FALSE;
		}
	}

	if (device & REPULSOR_RIGHT) {
		if (status & REPULSOR_RIGHT) {
			return TRUE;
		} else {
			return FALSE;
		}
	}

	if (device & UNIBEAM) {
		if (status & UNIBEAM) {
			return TRUE;
		} else {
			return FALSE;
		}
	}

	return FALSE;
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
		// inverse blinking for eyes power on
		if (device == EYES) {
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
}

// takes 1500ms
static void effect_fade(uint8_t mode, uint8_t device)
{
	uint8_t step, i, stop = 0;
	uint8_t time, off_time, on_time;

	// do fade effect in 50 steps
	for (step = 0; step <= 50; step++) {
		// calculate on/off times for PWM with period 1kHz
		if (mode == FADE_IN) {
			on_time = 2 * step;
			off_time = 100 - on_time;

			if (step < 25) {
				stop = 20;
			} else {
				stop = 10;
			}
		} else {
			off_time = 2 * step;
			on_time = 100 - off_time;

			if (step < 25) {
				stop = 10;
			} else {
				stop = 20;
			}
		}

		// each PWM step is taking 40/20ms
		for (i = 0; i < stop; i++) {
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

	// timer2 prescaler clk/128
	TCCR2 |= _BV(CS22) | _BV(CS20);

	// timer2 overflow interrupt enable
	TIMSK |= _BV(TOIE2);
}

void power_on(uint8_t device)
{
	if (device & EYES) {
		effect_blink(device);
	}

	effect_fade(FADE_IN, device);

	status |= device;
}

void power_off(uint8_t device)
{

	if (device == EYES) {
		status &= ~EYES;
		device_off(EYES);
	} else {
		// do not try to fade out device, if it's already off
		if ((device & EYES) && !device_get(EYES)) {
			device &= ~EYES;
		} else if ((device & REPULSORS_POWER) && !device_get(REPULSORS_POWER)) {
			device &= ~REPULSORS_POWER;
		} else if ((device & UNIBEAM) && !device_get(UNIBEAM)) {
			device &= ~UNIBEAM;
		}

		status &= ~device;
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

uint8_t power_state(uint8_t device)
{
	if (device_get(device)) {
		return POWER_ON;
	} else {
		return POWER_OFF;
	}
}
