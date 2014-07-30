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
#include "helmet.h"

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

	TCCR1A = _BV(WGM11);
	TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);

	DDRB |= _BV(GPIO_SERVO1) | _BV(GPIO_SERVO2);

	helmet_open();
}

void helmet_open(void)
{
	pwm_enable();

	OCR1A = 2200;
	OCR1B = 900;

	// wait till helmet is open
	_delay_ms(700);

	pwm_disable();
}

void helmet_close(void)
{
	pwm_enable();

	OCR1A = 900;
	OCR1B = 2200;

	// wait till helmet is closed
	_delay_ms(700);

	pwm_disable();
}

uint8_t helmet_state(void)
{
	if ((OCR1A == 900) && (OCR1B == 2200)) {
		return HELMET_CLOSED;
	} else {
		return HELMET_OPEN;
	}
}
