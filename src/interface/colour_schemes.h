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

typedef struct {
	rct_windowclass classification;
	uint8 colours[6];
	uint8 num_colours;
	rct_string_id name;
	char *ini_name;
} window_colour_scheme;

typedef struct {
	uint8 colours[6];
} window_colours;

typedef struct {
	rct_windowclass classification;
	uint8 colours[6];
} marked_window_colours;

extern window_colour_scheme gColourSchemes[];

extern window_colours gColourSchemesRCT1[];

extern uint32 gNumColourSchemeWindows;

window_colour_scheme* colour_scheme_get_by_class(rct_windowclass classification);

void colour_scheme_update(rct_window *window);
void colour_scheme_update_by_class(rct_window *window, rct_windowclass classification);

#endif