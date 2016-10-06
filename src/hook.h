#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef _HOOK_H_
#define _HOOK_H_

#ifndef NO_RCT2

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

void addhook(int address, int newaddress, int stacksize, int registerargs[], int registersreturned, int eaxDestinationRegister);

#endif

#endif
