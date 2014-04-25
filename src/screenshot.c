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
#include "rct2.h"
#include "screenshot.h"
#include "strings.h"
#include "window_error.h"

/**
 * 
 *  rct2: 0x006E3AEC
 */
void screenshot_check()
{
	int screenshotIndex;

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREENSHOT_COUNTDOWN, uint8) != 0) {
		RCT2_GLOBAL(RCT2_ADDRESS_SCREENSHOT_COUNTDOWN, uint8)--;
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCREENSHOT_COUNTDOWN, uint8) == 0) {
			RCT2_CALLPROC_EBPSAFE(0x00684218);
			screenshotIndex = screenshot_dump();
			RCT2_GLOBAL(0x013CE952, uint16) = STR_SCR_BMP;
			RCT2_GLOBAL(0x013CE952 + 2, uint16) = screenshotIndex;
			RCT2_GLOBAL(0x009A8C29, uint8) |= 1;

			window_error_open(screenshotIndex == -1 ? STR_SCREENSHOT_FAILED : STR_SCREENSHOT_SAVED_AS, -1);
			RCT2_GLOBAL(0x009A8C29, uint8) &= ~1;
			RCT2_CALLPROC_EBPSAFE(0x006843DC);
		}
	}
}

/**
 * 
 *  rct2: 0x00683D20
 */
int screenshot_dump()
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	RCT2_CALLFUNC_X(0x00683D20, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	return eax & 0xFFFF;
}