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

#ifndef COMMON_H
#define COMMON_H

#include <avr/io.h>

#define GPIO_EYES		PB0
#define GPIO_SERVO1		PB1
#define GPIO_SERVO2		PB2
#define GPIO_WT588_BUSY		PB6

#define GPIO_BATTERY_SENSE	PC5

#define GPIO_REPULSOR_BLAST	PD1
#define GPIO_HELMET_BUTTON	PD2
#define GPIO_REPULSOR_BUTTON	PD3
#define GPIO_REPULSOR_PWR	PD5
#define GPIO_WT588_RESET	PD6
#define GPIO_WT588_DATA		PD7

#endif // COMMON_H
