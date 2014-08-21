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

#define BLUETOOTH_PROMPT		"IRONMAN> "

#define BLUETOOTH_RESPONSE_OK		"OK\r\n"
#define BLUETOOTH_RESPONSE_ERROR	"ERROR\r\n"

#define BLUETOOTH_CMD_ON		"ON"
#define BLUETOOTH_CMD_OFF		"OFF"

#define BLUETOOTH_CMD_OPEN		"OPEN"
#define BLUETOOTH_CMD_CLOSE		"CLOSE"

#define BLUETOOTH_CMD_LEFT		"LEFT"
#define BLUETOOTH_CMD_RIGHT		"RIGHT"

#define BLUETOOTH_CMD_BATTERY		"BATTERY"

#define BLUETOOTH_CMD_EYES		"EYES"
#define BLUETOOTH_CMD_EYES_HELP		BLUETOOTH_CMD_EYES " [" BLUETOOTH_CMD_ON "/" BLUETOOTH_CMD_OFF "]"

#define BLUETOOTH_CMD_FORTUNE		"FORTUNE"

#define BLUETOOTH_CMD_HELMET		"HELMET"
#define BLUETOOTH_CMD_HELMET_HELP	BLUETOOTH_CMD_HELMET " [" BLUETOOTH_CMD_OPEN "/" BLUETOOTH_CMD_CLOSE "]"

#define BLUETOOTH_CMD_HELP		"HELP"

#define BLUETOOTH_CMD_REBOOT		"REBOOT"

#define BLUETOOTH_CMD_REPULSOR		"REPULSOR"
#define BLUETOOTH_CMD_REPULSOR_HELP	BLUETOOTH_CMD_REPULSOR " [" BLUETOOTH_CMD_LEFT "/" BLUETOOTH_CMD_RIGHT "]"

#define BLUETOOTH_CMD_REPULSORS		"REPULSORS"
#define BLUETOOTH_CMD_REPULSORS_HELP	BLUETOOTH_CMD_REPULSORS " [" BLUETOOTH_CMD_ON "/" BLUETOOTH_CMD_OFF "]"

#define BLUETOOTH_CMD_UNIBEAM		"UNIBEAM"
#define BLUETOOTH_CMD_UNIBEAM_HELP	BLUETOOTH_CMD_UNIBEAM " [" BLUETOOTH_CMD_ON "/" BLUETOOTH_CMD_OFF "]"

#define BLUETOOTH_CMD_VERSION		"VERSION"
#define BLUETOOTH_RESPONSE_VERSION	"HW Rev. v2.0, SW Rev. " VERSION "\r\nBuild: " __DATE__ " " __TIME__ "\r\n"

#define BLUETOOTH_CMD_VOLUME		"VOLUME"
#define BLUETOOTH_CMD_VOLUME_HELP	"VOLUME [0..7]"

char *commands[] = {
	"",
	BLUETOOTH_CMD_BATTERY,
	BLUETOOTH_CMD_EYES_HELP,
	BLUETOOTH_CMD_FORTUNE,
	BLUETOOTH_CMD_HELMET_HELP,
	BLUETOOTH_CMD_HELP,
	BLUETOOTH_CMD_REBOOT,
	BLUETOOTH_CMD_REPULSOR_HELP,
	BLUETOOTH_CMD_REPULSORS_HELP,
	BLUETOOTH_CMD_UNIBEAM_HELP,
	BLUETOOTH_CMD_VERSION,
	BLUETOOTH_CMD_VOLUME_HELP
};

#endif // STRINGS_H
