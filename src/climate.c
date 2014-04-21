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
#include "date.h"
#include "rct2.h"

int climate_celcius_to_fahrenheit(int celcius)
{
	return (celcius * 29) / 16 + 32;
}

/**
 *
 *  rct2: 0x006C45ED
 */
void climate_reset(int climate)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;

	RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE, sint8) = climate;

	eax = 1;
	RCT2_CALLFUNC_X(0x006C4672, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER, sint8) = eax & 0xFF;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TEMPERATURE, sint8) = ebx & 0xFF;

	RCT2_GLOBAL(0x013CA74E, sint8) = (ebx >> 8) & 0xFF;
	RCT2_GLOBAL(0x013CA750, sint8) = ecx & 0xFF;
	RCT2_GLOBAL(0x013CA752, sint8) = (ecx >> 8) & 0xFF;
	RCT2_CALLPROC_X(0x6C461C, 0, 0, 0, 0, 0, 0, 0);
}
