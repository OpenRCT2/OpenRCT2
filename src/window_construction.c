/*****************************************************************************
 * Copyright (c) 2014 Kevin Burke
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


/**
 * rct2: 0x006C78CD
 */
void window_construction_ondropdown()
{
    short widgetIndex;
    short a1;
    int a2;

    __asm mov widgetIndex, dx;
    __asm mov a1, ax;

    if widgetIndex != WIDX_CONSTRUCT_RIDE {
        return;
    }
    
    if a1 == 0xffff {
        // what is nullsub?
    }
    RCT2_GLOBAL(0x00F44070, sint16) = 0x80000000;
    RCT2_CALLPROC_EBPSAFE(0x006C9627);

    // in the code, this is movzx eax, ax
    __asm movzx a2, ax;
    return RCT2_GLOBAL(0x00F4407C, sint16)[a2]
}
