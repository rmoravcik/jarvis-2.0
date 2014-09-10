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

#define BLUETOOTH_RESPONSE_OK		"OK"
#define BLUETOOTH_RESPONSE_ERROR	"ERROR"

#define BLUETOOTH_PARAM_ON		"ON"
#define BLUETOOTH_PARAM_OFF		"OFF"

#define BLUETOOTH_PARAM_OPEN		"OPEN"
#define BLUETOOTH_PARAM_CLOSE		"CLOSE"

#define BLUETOOTH_PARAM_LEFT		"LEFT"
#define BLUETOOTH_PARAM_RIGHT		"RIGHT"

#define BLUETOOTH_CMD_BATTERY		"BATTERY"

#define BLUETOOTH_CMD_ECHO		"ECHO"
#define BLUETOOTH_CMD_ECHO_HELP		BLUETOOTH_CMD_ECHO

#define BLUETOOTH_CMD_EYES		"EYES"
#define BLUETOOTH_CMD_EYES_HELP		BLUETOOTH_CMD_EYES " [" BLUETOOTH_PARAM_ON "/" BLUETOOTH_PARAM_OFF "]"

#define BLUETOOTH_CMD_HELMET		"HELMET"
#define BLUETOOTH_CMD_HELMET_HELP	BLUETOOTH_CMD_HELMET " [" BLUETOOTH_PARAM_OPEN "/" BLUETOOTH_PARAM_CLOSE "]"

#define BLUETOOTH_CMD_HELP		"HELP"

#define BLUETOOTH_CMD_INTENSITY		"INTENSITY"
#define BLUETOOTH_CMD_INTENSITY_HELP	BLUETOOTH_CMD_INTENSITY " [0..9]"

#define BLUETOOTH_CMD_QUOTE		"QUOTE"

#define BLUETOOTH_CMD_REBOOT		"REBOOT"

#define BLUETOOTH_CMD_REPULSOR		"REPULSOR"
#define BLUETOOTH_CMD_REPULSOR_HELP	BLUETOOTH_CMD_REPULSOR " [" BLUETOOTH_PARAM_LEFT "/" BLUETOOTH_PARAM_RIGHT "]"

#define BLUETOOTH_CMD_REPULSOR		"REPULSOR"
#define BLUETOOTH_CMD_REPULSOR_HELP	BLUETOOTH_CMD_REPULSOR " [" BLUETOOTH_PARAM_LEFT "/" BLUETOOTH_PARAM_RIGHT "]"

#define BLUETOOTH_CMD_REPULSORS		"REPULSORS"
#define BLUETOOTH_CMD_REPULSORS_HELP	BLUETOOTH_CMD_REPULSORS " [" BLUETOOTH_PARAM_ON "/" BLUETOOTH_PARAM_OFF "]"

#define BLUETOOTH_CMD_UNIBEAM		"UNIBEAM"
#define BLUETOOTH_CMD_UNIBEAM_HELP	BLUETOOTH_CMD_UNIBEAM " [" BLUETOOTH_PARAM_ON "/" BLUETOOTH_PARAM_OFF "]"

#define BLUETOOTH_CMD_VERSION		"VERSION"
#define BLUETOOTH_RESPONSE_VERSION	"HW Rev. v2.0, SW Rev. " VERSION "\r\nBuild: " __DATE__ " " __TIME__

#define BLUETOOTH_CMD_VOLUME		"VOLUME"
#define BLUETOOTH_CMD_VOLUME_HELP	BLUETOOTH_CMD_VOLUME " [0..7]"

#ifndef QT_VERSION
char *commands[] = {
	"",
	BLUETOOTH_CMD_BATTERY,
	BLUETOOTH_CMD_ECHO_HELP,
	BLUETOOTH_CMD_EYES_HELP,
	BLUETOOTH_CMD_HELMET_HELP,
	BLUETOOTH_CMD_HELP,
	BLUETOOTH_CMD_INTENSITY_HELP,
	BLUETOOTH_CMD_QUOTE,
	BLUETOOTH_CMD_REBOOT,
	BLUETOOTH_CMD_REPULSOR_HELP,
	BLUETOOTH_CMD_REPULSORS_HELP,
	BLUETOOTH_CMD_UNIBEAM_HELP,
	BLUETOOTH_CMD_VERSION,
	BLUETOOTH_CMD_VOLUME_HELP
};
#endif

#endif // STRINGS_H
