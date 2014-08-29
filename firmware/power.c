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

static uint8_t curr[3] = { 0, 0, 0};
static uint8_t duty[3] = { 0, 0, 0};

#define DUTY_EYES	0
#define DUTY_REPULSORS	1
#define DUTY_UNIBEAM	2

enum {
	FADE_IN = 0,
	FADE_OUT
};

static void device_on(uint8_t device)
{
	if (device) {
		PORTB |= _BV(GPIO_EYES);
	}

	if (device & REPULSORS_POWER) {
		PORTC |= _BV(GPIO_REPULSORS_PWR);
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

	if (device & UNIBEAM) {
		PORTD &= ~_BV(GPIO_UNIBEAM);
	}
}

static uint8_t device_get(uint8_t device)
{
	if (device & EYES) {
		if (duty[DUTY_EYES] > 0) {
			return TRUE;
		} else {
			return FALSE;
		}
	}

	if (device & REPULSORS_POWER) {
		if (duty[DUTY_REPULSORS] > 0) {
			return TRUE;
		} else {
			return FALSE;
		}
	}

	if (device & UNIBEAM) {
		if (duty[DUTY_UNIBEAM] > 0) {
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
		if ((duty[DUTY_EYES] > 0) && (curr[DUTY_EYES]) > 0) {
			PORTB |= _BV(GPIO_EYES);
		} else {
			PORTB &= ~_BV(GPIO_EYES);
		}

		if ((duty[DUTY_REPULSORS] > 0) && (curr[DUTY_REPULSORS]) > 0) {
			PORTC |= _BV(GPIO_REPULSORS_PWR);
		} else {
			PORTC &= ~_BV(GPIO_REPULSORS_PWR);
		}

		if ((duty[DUTY_UNIBEAM] > 0) && (curr[DUTY_UNIBEAM]) > 0) {
			PORTD |= _BV(GPIO_UNIBEAM);
		} else {
			PORTD &= ~_BV(GPIO_UNIBEAM);
		}
	} else if (cycle == curr[DUTY_EYES]) {
		if (duty[DUTY_EYES] > 0) {
			PORTB &= ~_BV(GPIO_EYES);
		}
	} else if (cycle == curr[DUTY_REPULSORS]) {
		if (duty[DUTY_REPULSORS] > 0) {
			PORTC &= ~_BV(GPIO_REPULSORS_PWR);
		}
	} else if (cycle == curr[DUTY_UNIBEAM]) {
		if (duty[DUTY_UNIBEAM] > 0) {
			PORTD &= ~_BV(GPIO_UNIBEAM);
		}
	}

	cycle++;

	if (cycle >= 100) {
		cycle = 0;
	}
}

// takes 1500ms
static void effect_fade(uint8_t mode, uint8_t devices)
{
	uint8_t step;

	// do fade effect in 50 steps
	for (step = 0; step <= 100; step = step + 2) {

		if (mode == FADE_IN) {
			if (devices & EYES) {
				if (step > duty[DUTY_EYES]) {
					curr[DUTY_EYES] = duty[DUTY_EYES];
				} else {
					curr[DUTY_EYES] = step;
				}
			}

			if (devices & REPULSORS_POWER) {
				if (step > duty[DUTY_REPULSORS]) {
					curr[DUTY_REPULSORS] = duty[DUTY_REPULSORS];
				} else {
					curr[DUTY_REPULSORS] = step;
				}
			}

			if (devices & UNIBEAM) {
				if (step > duty[DUTY_UNIBEAM]) {
					curr[DUTY_UNIBEAM] = duty[DUTY_UNIBEAM];
				} else {
					curr[DUTY_UNIBEAM] = step;
				}
			}
		} else {
			if (devices & EYES) {
				if (100 - step > duty[DUTY_EYES]) {
					curr[DUTY_EYES] = duty[DUTY_EYES];
				} else {
					curr[DUTY_EYES] = 100 - step;
				}
			}

			if (devices & REPULSORS_POWER) {
				if (100 - step > duty[DUTY_REPULSORS]) {
					curr[DUTY_REPULSORS] = duty[DUTY_REPULSORS];
				} else {
					curr[DUTY_REPULSORS] = 100 - step;
				}
			}

			if (devices & UNIBEAM) {
				if (100 - step > duty[DUTY_UNIBEAM]) {
					curr[DUTY_UNIBEAM] = duty[DUTY_UNIBEAM];
				} else {
					curr[DUTY_UNIBEAM] = 100 - step;
				}
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
		duty[DUTY_EYES] = 50;
	}

	if (devices & REPULSORS_POWER) {
		duty[DUTY_REPULSORS] = 50;
	}

	if (devices & UNIBEAM) {
		duty[DUTY_UNIBEAM] = 50;
	}

	effect_fade(FADE_IN, devices);
}

void power_off(uint8_t devices)
{

	if (devices == EYES) {
		duty[DUTY_EYES] = 0;
		device_off(EYES);
	} else {
		effect_fade(FADE_OUT, devices);

		if (devices & EYES) {
			duty[DUTY_EYES] = 0;
		}

		if (devices & REPULSORS_POWER) {
			duty[DUTY_REPULSORS] = 0;
		}

		if (devices & UNIBEAM) {
			duty[DUTY_UNIBEAM] = 0;
		}
	}
}

void power_failure(uint8_t devices)
{
	if (device_get(devices)) {
		uint8_t tmp_eyes, tmp_repulsors, tmp_unibeam;

		// temporary disable PWM on requested devices
		if (devices & EYES) {
			tmp_eyes = duty[DUTY_EYES];
			duty[DUTY_EYES] = 0;
		}

		if (devices & REPULSORS_POWER) {
			tmp_repulsors = duty[DUTY_REPULSORS];
			duty[DUTY_REPULSORS] = 0;
		}

		if (devices & UNIBEAM) {
			tmp_unibeam = duty[DUTY_UNIBEAM];
			duty[DUTY_UNIBEAM] = 0;
		}

		device_off(devices);
		_delay_ms(50);
		device_on(devices);
		_delay_ms(50);
		device_off(devices);
		_delay_ms(50);
		device_on(devices);
		_delay_ms(50);

		// re-enable previously disabled PWM
		if (devices & EYES) {
			duty[DUTY_EYES] = tmp_eyes;
		}

		if (devices & REPULSORS_POWER) {
			duty[DUTY_REPULSORS] = tmp_repulsors;
		}

		if (devices & UNIBEAM) {
			duty[DUTY_UNIBEAM] = tmp_unibeam;
		}
	} else {
		// inverse blinking is used only eyes power on effect
		if (devices == EYES) {
			device_on(EYES);
			_delay_ms(50);
			device_off(EYES);
			_delay_ms(50);
			device_on(EYES);
			_delay_ms(50);
			device_off(EYES);
			_delay_ms(50);
		}
	}
}

void power_blast(uint8_t device)
{
	if (device & REPULSOR_LEFT) {
		PORTC |= _BV(GPIO_REPULSOR_LEFT);
	}

	if (device & REPULSOR_RIGHT) {
		PORTC |= _BV(GPIO_REPULSOR_RIGHT);
	}

	_delay_ms(100);

	if (device & REPULSOR_LEFT) {
		PORTC &= ~_BV(GPIO_REPULSOR_LEFT);
	}

	if (device & REPULSOR_RIGHT) {
		PORTC &= ~_BV(GPIO_REPULSOR_RIGHT);
	}
}

uint8_t power_state(uint8_t device)
{
	if (device_get(device)) {
		return POWER_ON;
	} else {
		return POWER_OFF;
	}
}
