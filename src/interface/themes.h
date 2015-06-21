/*****************************************************************************
* Copyright (c) 2014 Ted John, Peter Hill, Duncan Frost
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

#ifndef _COLOUR_SCHEMES_H_
#define _COLOUR_SCHEMES_H_

#include "../common.h"
#include "window.h"
#include "../config.h"

typedef struct {
	rct_windowclass classification;
	char *section_name;
	rct_string_id name;
	uint8 num_colours;
	theme_window window;
} theme_window_definition;

typedef struct {
	rct_windowclass classification;
	theme_window window;
} theme_window_preset;

// The definitions for window themes as well as the RCT2 preset
extern theme_window_definition gThemeWindowDefinitions[];
// The preset for RCT1 window themes
extern theme_window_preset gThemeWindowsRCT1[];

// The index of the current theme
extern uint16 gCurrentTheme;
// The number of theme-able windows
extern uint32 gNumThemeWindows;

theme_preset* theme_get_preset();
theme_window_definition* theme_window_definition_get_by_class(rct_windowclass classification);
theme_window* theme_window_get_by_class(rct_windowclass classification);

void colour_scheme_update(rct_window *window);
void colour_scheme_update_by_class(rct_window *window, rct_windowclass classification);

void theme_change_preset(int preset);
void theme_create_preset(int duplicate, const char *name);
void theme_delete_preset(int preset);
void theme_rename_preset(int preset, const char *newName);


#endif