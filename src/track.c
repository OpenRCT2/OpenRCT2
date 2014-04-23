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
#include "track.h"

/**
 *
 *  rct2: 0x00997C9D
 */
const rct_trackdefinition gTrackDefinitions[] = {
	// TYPE			VANGLE END		VANGLE START	BANK END			BANK START			SPECIAL
	{ TRACK_FLAT,	TRACK_NONE,		TRACK_NONE,		TRACK_BANK_NONE,	TRACK_BANK_NONE,	TRACK_NONE },		// flat
	{ TRACK_FLAT,	TRACK_NONE,		TRACK_NONE,		TRACK_BANK_NONE,	TRACK_BANK_NONE,	TRACK_NONE },		// station end
	{ TRACK_FLAT,	TRACK_NONE,		TRACK_NONE,		TRACK_BANK_NONE,	TRACK_BANK_NONE,	TRACK_NONE },		// station begin
	{ TRACK_FLAT,	TRACK_NONE,		TRACK_NONE,		TRACK_BANK_NONE,	TRACK_BANK_NONE,	TRACK_NONE },		// station middle
	{ TRACK_FLAT,	TRACK_UP_25,	TRACK_UP_25,	TRACK_BANK_NONE,	TRACK_BANK_NONE,	TRACK_NONE },		// 25 deg up
	{ TRACK_FLAT,	TRACK_UP_60,	TRACK_UP_60,	TRACK_BANK_NONE,	TRACK_BANK_NONE,	TRACK_NONE },		// 60 deg up
	// TODO the rest
};

/**
 *
 *  rct2: 0x006CED50
 */
void track_load_list(int edx)
{
	RCT2_CALLPROC_X(0x006CED50, 0, 0, 0, edx, 0, 0, 0);
}