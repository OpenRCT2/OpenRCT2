/*****************************************************************************
* Copyright (c) 2014 Ted John, Peter Hill
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* This file is part of OpenRCT2.
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <windows.h>
#include <stdio.h>
#include "rct2.h"


void settings_parse_settings(FILE *fp);
int settings_get_line(FILE *fp, char *setting, char *value);


void settings_init();
void settings_create_default(char *path);


typedef struct settings{
	uint8 SCREENSHOT_AS_PNG;
	char GAME_PATH[MAX_PATH];

} settings_t;

extern settings_t settings;



#endif