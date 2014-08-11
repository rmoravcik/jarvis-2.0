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
	SOUND_ASK_0 = 0,
	SOUND_BATTERY_CHARGED,
	SOUND_BATTERY_LOW_0,
	SOUND_BATTERY_LOW_1,
	SOUND_BATTERY_LOW_2, // FIXME
	SOUND_BD_CALIBRATING,
	SOUND_BUTTON_SOUND_0,
	SOUND_CLOCK_AFTERNOON, // FIXME
	SOUND_CLOCK_ALARM_WAKE_3,
	SOUND_CLOCK_LATE_1,
	SOUND_CONFIRM_6,
	SOUND_INTRO_2,
	SOUND_JUST_KIDDING,
	SOUND_LISTENING_ON_1,
	SOUND_LISTENING_ON_4,
	SOUND_LISTENING_ON_6,
	SOUND_LISTENING_ON_AFTERNOON,
	SOUND_LISTENING_ON_EVENING,
	SOUND_LISTENING_ON_MORNING,
	SOUND_NETWORK_LOST_WIFI,
	SOUND_NETWORK_NO_WIFI,
	SOUND_SELF_DESTRUCT_1,
	SOUND_SLEEP_0, // FIXME
	SOUND_SLEEP_2, // FIXME
	SOUND_TRS4,
	SOUND_TRS7_1,
	SOUND_TRS9,
	SOUND_TRS10,
	SOUND_TRS11,
	SOUND_WELCOME,
	SOUND_IM_SUIT_01,
	SOUND_IM_SUIT_02,
	SOUND_IM_SUIT_03,
	SOUND_IM_SUIT_04,
	SOUND_IM_SUIT_05,
	SOUND_IM_SUIT_06,
	SOUND_IM_SUIT_07,
	SOUND_REPULSOR,
	SOUND_ACDC
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

void voice_play_sound(uint8_t sound);
void voice_play_sound_no_wait(uint8_t sound);

void voice_set_volume(uint8_t volume);

#endif // VOICE_H
