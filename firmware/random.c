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

#include "common.h"
#include "random.h"

static uint8_t EEMEM conf_seed = 27;

static uint8_t seed;

void random_init(void)
{
	seed = eeprom_read_byte(&conf_seed);
//	srand(seed);
}

uint8_t random_get(uint8_t max)
{
//	return rand() % max;
	seed = (seed * 109 + 89) % max;
	eeprom_write_byte(&conf_seed, seed);
	return seed;
}
