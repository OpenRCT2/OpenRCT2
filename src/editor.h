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

#ifndef _EDITOR_H_
#define _EDITOR_H_

#include "rct2.h"
#include "addresses.h"

typedef enum {
	EDITOR_STEP_OBJECT_SELECTION,			// 0
	EDITOR_STEP_LANDSCAPE_EDITOR,			// 1
	EDITOR_STEP_INVENTIONS_LIST_SET_UP,		// 2
	EDITOR_STEP_OPTIONS_SELECTION,			// 3
	EDITOR_STEP_OBJECTIVE_SELECTION,		// 4
	EDITOR_STEP_SAVE_SCENARIO,				// 5
	EDITOR_STEP_ROLLERCOASTER_DESIGNER,		// 6
	EDITOR_STEP_TRACK_DESIGNS_MANAGER		// 7
} RCT2_EDITOR_STEP;

#define g_editor_step RCT2_GLOBAL(0x0141F570, uint8)

void editor_load();
void editor_convert_save_to_scenario();
void trackdesigner_load();
void trackmanager_load();
void editor_load_landscape(const char *path);

void sub_6BD3A4();

void editor_open_windows_for_current_step();

bool editor_check_park();
bool editor_check_object_selection();

#endif
