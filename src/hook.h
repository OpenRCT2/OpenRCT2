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

#ifndef _HOOK_H_
#define _HOOK_H_

enum REGISTER_ARGS {
	EAX = 1 << 0,
	EBX = 1 << 1,
	ECX = 1 << 2,
	EDX = 1 << 3,
	ESI = 1 << 4,
	EDI = 1 << 5,
	EBP = 1 << 6,
	END = 0
};

void addhook(int address, int newaddress, int stacksize, int registerargs[], int registersreturned);

#endif