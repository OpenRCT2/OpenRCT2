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

#ifndef _ADDRESSES_H_
#define _ADDRESSES_H_

#pragma warning(disable : 4731)

#define RCT2_ADDRESS(address, type)				((type*)address)
#define RCT2_GLOBAL(address, type)				(*((type*)address))
#define RCT2_CALLPROC(address)					(((void(*)())address)())
#define RCT2_CALLFUNC(address, returnType)		(((returnType(*)())address)())

#define RCT2_ADDRESS_CMDLINE						0x009E2D98

#define RCT2_ADDRESS_APP_PATH						0x009AA214
#define RCT2_ADDRESS_APP_PATH_SLASH					0x009AB4D9
#define RCT2_ADDRESS_SAVED_GAMES_PATH				0x009AB5DA
#define RCT2_ADDRESS_SCENARIOS_PATH					0x009AB6E9
#define RCT2_ADDRESS_LANDSCAPES_PATH				0x009AB7FB
#define RCT2_ADDRESS_OBJECT_DATA_PATH				0x009AB90E
#define RCT2_ADDRESS_TRACKS_PATH					0x009ABA1E
#define RCT2_ADDRESS_SAVED_GAMES_PATH_2				0x009ABB37

#define RCT2_ADDRESS_SCREEN_DPI						0x009ABDC8
#define RCT2_ADDRESS_SCREEN_WIDTH					0x009ABDD8
#define RCT2_ADDRESS_SCREEN_HEIGHT					0x009ABDDA

#define RCT2_ADDRESS_RUN_INTRO_TICK_PART			0x009AC319
#define RCT2_ADDRESS_SCREEN_FLAGS					0x009DEA68
#define RCT2_ADDRESS_PLACE_OBJECT_MODIFIER			0x009DEA70
#define RCT2_ADDRESS_ON_TUTORIAL					0x009DEA71

#define RCT2_ADDRESS_OS_TOTALPHYS					0x01423B5C

static void RCT2_CALLPROC_EBPSAFE(int address)
{
	__asm push ebp
	__asm call address
	__asm pop ebp
}

static void RCT2_CALLPROC_X(int address, int _eax, int _ebx, int _ecx, int _edx, int _esi, int _edi, int _ebp)
{
	__asm {
		push address
		mov eax, _eax
		mov ebx, _ebx
		mov ecx, _ecx
		mov edx, _edx
		mov esi, _esi
		mov edi, _edi
		mov ebp, _ebp
		call [esp]
		add esp, 4
	}
}

#endif