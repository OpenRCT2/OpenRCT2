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

#ifndef _OPENRCT2_H_
#define _OPENRCT2_H_

#include "common.h"

enum {
	STARTUP_ACTION_INTRO,
	STARTUP_ACTION_TITLE,
	STARTUP_ACTION_OPEN,
	STARTUP_ACTION_EDIT
};

extern int gOpenRCT2StartupAction;
extern char gOpenRCT2StartupActionPath[512];

bool openrct2_initialise();
void openrct2_launch();
void openrct2_dispose();
void openrct2_finish();

#endif