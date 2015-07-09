/*****************************************************************************
 * Copyright (c) 2014 Ted John
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
 
#ifndef _TITLE_H_
#define _TITLE_H_

enum {
	TITLE_SCRIPT_WAIT,
	TITLE_SCRIPT_LOADMM,
	TITLE_SCRIPT_LOCATION,
	TITLE_SCRIPT_ROTATE,
	TITLE_SCRIPT_ZOOM,
	TITLE_SCRIPT_RESTART,
	TITLE_SCRIPT_LOAD,
	TITLE_SCRIPT_END,
	TITLE_SCRIPT_SPEED,
	TITLE_SCRIPT_LOOP,
	TITLE_SCRIPT_ENDLOOP
} TITLE_SCRIPT_COMMANDS;

extern sint32 gTitleScriptCommand;
extern uint8 gTitleScriptSave;
extern sint32 gTitleScriptSkipTo;
extern sint32 gTitleScriptSkipLoad;

void title_load();
void title_update();
void title_skip_from_beginning();
void title_script_get_line(FILE *file, char *parts);
bool title_refresh_sequence();
void DrawOpenRCT2(int x, int y);

#endif
