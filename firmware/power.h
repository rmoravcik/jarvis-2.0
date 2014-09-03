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

#ifndef POWER_H
#define POWER_H

enum {
	EYES		= 0x0001,
	REPULSORS_POWER	= 0x0002,
	REPULSOR_LEFT	= 0x0004,
	REPULSOR_RIGHT	= 0x0008,
	UNIBEAM		= 0x0010,
};

enum {
	POWER_OFF = 0,
	POWER_ON
};

#define ALL		EYES | REPULSORS_POWER | UNIBEAM

void power_init(void);

void power_on(uint8_t devices);
void power_off(uint8_t devices);

void power_failure(uint8_t devices);

void power_blast(uint8_t devices);

uint8_t power_state(uint8_t device);

void power_set_intensity(uint8_t devices, uint8_t value);
int8_t power_get_intensity(uint8_t device);

#endif // POWER_H
