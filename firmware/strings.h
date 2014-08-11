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

#ifndef STRINGS_H
#define STRINGS_H

#include <stdint.h>

#define BLUETOOTH_RESPONSE_PROMPT	"IRONMAN> "

#define BLUETOOTH_RESPONSE_OK		"OK\r\n"
#define BLUETOOTH_RESPONSE_ERROR	"ERROR\r\n"
#define BLUETOOTH_RESPONSE_VERSION	"Iron Man Mark IV, HW Rev. 2.0, SW Rev. 1.0\r\nBuild: " __DATE__ " " __TIME__ "\r\n"

#define BLUETOOTH_CMD_HELP		"?"

#define BLUETOOTH_CMD_EYES_ON		"EYES ON"
#define BLUETOOTH_CMD_EYES_OFF		"EYES OFF"

#define BLUETOOTH_CMD_HELMET_OPEN	"HELMET OPEN"
#define BLUETOOTH_CMD_HELMET_CLOSE	"HELMET CLOSE"

#define BLUETOOTH_CMD_REPULSORS_ON	"REPULSORS ON"
#define BLUETOOTH_CMD_REPULSORS_OFF	"REPULSORS OFF"
#define BLUETOOTH_CMD_REPULSOR_LEFT	"REPULSOR LEFT BLAST"
#define BLUETOOTH_CMD_REPULSOR_RIGHT	"REPULSOR RIGHT BLAST"

#define BLUETOOTH_CMD_UNIBEAM_ON	"UNIBEAM ON"
#define BLUETOOTH_CMD_UNIBEAM_OFF	"UNIBEAM OFF"

#define BLUETOOTH_CMD_VERSION		"VERSION"

char *commands[] = {
	BLUETOOTH_CMD_HELP,
	BLUETOOTH_CMD_EYES_ON,
	BLUETOOTH_CMD_EYES_OFF,
	BLUETOOTH_CMD_HELMET_OPEN,
	BLUETOOTH_CMD_HELMET_CLOSE,
	BLUETOOTH_CMD_REPULSORS_ON,
	BLUETOOTH_CMD_REPULSORS_OFF,
	BLUETOOTH_CMD_REPULSOR_LEFT,
	BLUETOOTH_CMD_REPULSOR_RIGHT,
	BLUETOOTH_CMD_UNIBEAM_ON,
	BLUETOOTH_CMD_UNIBEAM_OFF,
	BLUETOOTH_CMD_VERSION
};

#endif // STRINGS_H
