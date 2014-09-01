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

#include <avr/eeprom.h>
#include <util/delay.h>

#include "bluetooth.h"

#include "common.h"
#include "power.h"
#include "voice.h"
#include "helmet.h"

static uint8_t EEMEM eeprom_state = HELMET_OPEN;
static uint8_t state = HELMET_OPEN;

extern uint8_t mcucsr;

static void pwm_enable(void)
{
	TCCR1A |= _BV(COM1A1) | _BV(COM1B1);
}

static void pwm_disable(void)
{
	TCCR1A &= ~(_BV(COM1A1) | _BV(COM1B1));
}

void helmet_init()
{
	ICR1 = 20000;

	// Fast PWM, top is ICR1
	TCCR1A = _BV(WGM11);
	// clk/8 (From prescaler)
	TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11);

	DDRB |= _BV(GPIO_SERVO1) | _BV(GPIO_SERVO2);

	state = eeprom_read_byte(&eeprom_state);

	// open helmet only if was power on reset
	if (mcucsr & _BV(PORF)) {
		state = HELMET_CLOSED;
		helmet_open();
	} else {
		if (state == HELMET_OPEN) {
			OCR1A = 2200;
			OCR1B = 900;
		} else {
			OCR1A = 900;
			OCR1B = 2200;
		}
	}
}

void helmet_open(void)
{
	if (state == HELMET_CLOSED) {
		// turn off eyes
		power_off(EYES);

		// wait for 200ms before opening helmet
		_delay_ms(200);

		voice_play_sound_no_wait(SOUND_IM_SUIT_03);

		pwm_enable();

		OCR1A = 2200;
		OCR1B = 900;

		// wait till helmet is open
		_delay_ms(700);

		pwm_disable();

		state = HELMET_OPEN;
		eeprom_write_byte(&eeprom_state, state);
	}
}

void helmet_close(void)
{
	if (state == HELMET_OPEN) {
		pwm_enable();

		OCR1A = 900;
		OCR1B = 2200;

		// wait till helmet is closed
		_delay_ms(700);

		pwm_disable();

		// turn on eyes
		power_failure(EYES);
		power_on(EYES);

		state = HELMET_CLOSED;
		eeprom_write_byte(&eeprom_state, state);
	}
}

uint8_t helmet_state(void)
{
	if (state == HELMET_CLOSED) {
		return HELMET_CLOSED;
	} else {
		return HELMET_OPEN;
	}
}
