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

#include "addresses.h"
#include "localisation/localisation.h"
#include "tutorial.h"
#include "windows/error.h"

/**
 *
 *  rct2: 0x0066ECC1
 */
void tutorial_start(int type)
{
	strcpy((char*)0x009BC677, "Tutorial not implemented.");
	window_error_open(3165, STR_NONE);

	// RCT2_CALLPROC_X(0x0066ECC1, type, 0, 0, 0, 0, 0, 0);
}

/**
 *
 *  rct2: 0x0066EE25
 */
void tutorial_stop()
{
	RCT2_CALLPROC_EBPSAFE(0x0066EE25);
}