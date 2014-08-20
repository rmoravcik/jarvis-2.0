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

#ifndef VOICE_H
#define VOICE_H

#include <stdint.h>

enum {
	SOUND_ACDC = 0,
	SOUND_ASK_0,
	SOUND_BATTERY_CHARGED,
	SOUND_BATTERY_LOW_0,
	SOUND_BATTERY_LOW_1,
	SOUND_BATTERY_LOW_2,
	SOUND_BD_CALIBRATING,
	SOUND_BUTTON_SOUND_0,
	SOUND_CLOCK_AFTERNOON,
	SOUND_CLOCK_ALARM_SNOOZE_0,
	SOUND_CLOCK_ALARM_SNOOZE_2,
	SOUND_CLOCK_ALARM_WAKE_3,
	SOUND_CLOCK_LATE_0,
	SOUND_CLOCK_LATE_1,
	SOUND_CONFIRM_6,		// Done!
	SOUND_INTRO_2,
	SOUND_JUST_KIDDING,
	SOUND_LISTENING_OFF_2,
	SOUND_LISTENING_ON_1,
	SOUND_LISTENING_ON_3,
	SOUND_LISTENING_ON_4,
	SOUND_LISTENING_ON_5,		// Ready when you are!
	SOUND_LISTENING_ON_6,
	SOUND_LISTENING_ON_7,
	SOUND_LISTENING_ON_AFTERNOON,
	SOUND_LISTENING_ON_EVENING,
	SOUND_LISTENING_ON_MORNING,
	SOUND_MESSAGE_NEW_6,
	SOUND_MESSAGE_NEW_7,
	SOUND_NETWORK_LOST_WIFI,
	SOUND_NETWORK_NO_WIFI,
	SOUND_POWER_DOWN,
	SOUND_REPEAT_1,
	SOUND_REPEAT_2,
	SOUND_REPEAT_3,
	SOUND_SELF_DESTRUCT_1,
	SOUND_SETUP_COMPLETE,
	SOUND_SLEEP_0,
	SOUND_SLEEP_2,
	SOUND_SUITS_DECRYPT_1,
	SOUND_SUITS_DECRYPT_2,
	SOUND_SUITS_DECRYPT_3,
	SOUND_SUITS_DECRYPT_4,
	SOUND_SUITS_DECRYPT_5,
	SOUND_SUITS_DECRYPT_6,
	SOUND_SUITS_DECRYPT_7,
	SOUND_SUITS_DECRYPT_8,
	SOUND_TRS3_0,
	SOUND_TRS4,
	SOUND_TRS6,
	SOUND_TRS7_1,
	SOUND_TRS8,
	SOUND_TRS9,
	SOUND_TRS10,
	SOUND_TRS11,
	SOUND_UNAVAILABLE_0,
	SOUND_UNAVAILABLE_3,
	SOUND_WELCOME,
	SOUND_IM_SUIT_01,		// Remove?
	SOUND_IM_SUIT_02,		// Remove?
	SOUND_IM_SUIT_03,
	SOUND_IM_SUIT_04,		// Remove?
	SOUND_IM_SUIT_05,		// Remove?
	SOUND_IM_SUIT_06,		// Remove?
	SOUND_IM_SUIT_07,		// Remove?
	SOUND_REPULSOR
};

enum {
	SOUND_VOLUME_0 = 0xE0,
	SOUND_VOLUME_1,
	SOUND_VOLUME_2,
	SOUND_VOLUME_3,
	SOUND_VOLUME_4,
	SOUND_VOLUME_5,
	SOUND_VOLUME_6,
	SOUND_VOLUME_7,
};

void voice_init(void);

void voice_play_welcome(void);
void voice_play_random(void);

uint8_t voice_is_playing(void);

void voice_play_sound(uint8_t sound);
void voice_play_sound_no_wait(uint8_t sound);

void voice_set_volume(uint8_t value);

void voice_stop_playback(void);

#endif // VOICE_H
