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
#include "editor.h"

/**
 *
 *  rct2: 0x0066FFE1
 */
void editor_load()
{
	RCT2_CALLPROC_EBPSAFE(0x0066FFE1);
}

/**
 *
 *  rct2: 0x00672781
 */
void editor_convert_save_to_scenario()
{
	RCT2_CALLPROC_EBPSAFE(0x00672781);
}

/**
 *
 *  rct2: 0x00672957
 */
void trackdesigner_load()
{
	RCT2_CALLPROC_EBPSAFE(0x00672957);
}

/**
 *
 *  rct2: 0x006729FD
 */
void trackmanager_load()
{
	RCT2_CALLPROC_EBPSAFE(0x006729FD);
}