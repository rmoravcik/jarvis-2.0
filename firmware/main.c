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

#include <stdlib.h>

#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "common.h"
#include "battery.h"
#include "bluetooth.h"
#include "helmet.h"
#include "power.h"
#include "voice.h"

uint8_t mcucsr __attribute__((section(".noinit")));

void get_mcucsr(void) __attribute__((naked)) __attribute__((section(".init3")));

void get_mcucsr(void)
{
	mcucsr = MCUCSR;
	MCUCSR = 0;
}

// func button
ISR(INT0_vect, ISR_NOBLOCK)
{
	uint8_t press_counter = 0;

	while (!(PIND & _BV(GPIO_FUNC_BUTTON)) && press_counter < 10) {
		_delay_ms(200);
		press_counter++;
	}

	// short press of func button
	if (press_counter == 1) {
		if (voice_is_playing()) {
			voice_stop_playback();
		} else {
			voice_play_sound(SOUND_BUTTON_SOUND_0);
			_delay_ms(500);
			voice_play_quote();
		}
	}

	// long press of func button
	if (press_counter == 10) {
		if (helmet_state() == HELMET_CLOSED) {
			// open helmet
			helmet_open();
		} else {
			// close helmet
			helmet_close();
		}
	}
}

static void check_repulsor_buttons(void)
{
	uint8_t press_counter = 0;

	if (!(PINC & _BV(GPIO_REPULSOR_BUTTON_LEFT))) {
		while (!(PINC & _BV(GPIO_REPULSOR_BUTTON_LEFT)) && press_counter < 10) {
			_delay_ms(200);
			press_counter++;
		}

		// short press of func button
		if (press_counter == 1) {
			voice_play_sound(SOUND_BUTTON_SOUND_0);
		}

		// long press of func button
		if (press_counter == 10) {
			voice_play_sound_no_wait(SOUND_REPULSOR);

			// simulate shot after 1sec
			_delay_ms(1000);

			power_blast(REPULSOR_LEFT);
		}
	}

	if (!(PINC & _BV(GPIO_REPULSOR_BUTTON_RIGHT))) {
		while (!(PINC & _BV(GPIO_REPULSOR_BUTTON_RIGHT)) && press_counter < 10) {
			_delay_ms(200);
			press_counter++;
		}

		// short press of func button
		if (press_counter == 1) {
			voice_play_sound(SOUND_BUTTON_SOUND_0);
		}

		// long press of func button
		if (press_counter == 10) {
			voice_play_sound_no_wait(SOUND_REPULSOR);

			// simulate shot after 1sec
			_delay_ms(1000);

			power_blast(REPULSOR_RIGHT);
		}
	}
}

static void init(void)
{
	OSCCAL = 0xb5;

	DDRB = 0xff;

	DDRC = 0xff;
	DDRC &= ~(_BV(GPIO_REPULSOR_BUTTON_LEFT) | _BV(GPIO_REPULSOR_BUTTON_RIGHT));

	DDRD = 0xff;
	DDRD &= ~_BV(GPIO_FUNC_BUTTON);

	// enable pull ups on repulsor buttons
	PORTC |= _BV(GPIO_REPULSOR_BUTTON_LEFT) | _BV(GPIO_REPULSOR_BUTTON_RIGHT);

	// enable pull up on func button
	PORTD |= _BV(GPIO_FUNC_BUTTON);

	// falling edge on INT0
	MCUCR |= _BV(ISC01);

	// enable INT0 interrupt
	GICR |= _BV(INT0);

	sei();
}

static void configure(void)
{
	voice_play_sound(SOUND_BD_CALIBRATING);

	bluetooth_configure();

	voice_play_sound(SOUND_SETUP_COMPLETE);
}

int main(void)
{
	uint8_t configured = FALSE;

	init();
	bluetooth_init();
	power_init();
	battery_init();
	voice_init();
	helmet_init();

	// check if configuration mode was requested
	if (!(PIND & _BV(GPIO_FUNC_BUTTON))) {
		configure();
		configured = TRUE;
	}

	// delay about 2sec after power on
	_delay_ms(2000);

	// power on repulsors, unibeam and eyes
	// if they were previously on
	if (battery_get_capacity() >= BATTERY_BACKUP_CAPACITY) {
		if (helmet_state() == HELMET_CLOSED) {
			power_on(ALL);
		} else {
			power_on(REPULSORS_POWER | UNIBEAM);
		}
	}

	if (!configured) {
		if (mcucsr & _BV(PORF)) {
			voice_play_welcome();
		} else {
			voice_play_sound(SOUND_LISTENING_ON_6);
		}
	} else {
		voice_play_sound(SOUND_INTRO_2);
	}

	// start reporting of battery status
	battery_reporting_start();

	// main loop
	while(1) {
		check_repulsor_buttons();
	}

	return 0;
}
