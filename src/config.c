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

#include <windows.h>
#include "addresses.h"
#include "config.h"
#include "rct2.h"

static const int MagicNumber = 0x0003113A;

/**
 * 
 *  rct2: 0x006752D5
 */
void config_load()
{
	HANDLE hFile;
	DWORD bytesRead;

	hFile = CreateFile(get_file_path(PATH_ID_GAMECFG), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		// Read and check magic number
		ReadFile(hFile, RCT2_ADDRESS(0x013CE928, void), 4, &bytesRead, NULL);
		if (RCT2_GLOBAL(0x013CE928, int) == MagicNumber) {
			// Read options
			ReadFile(hFile, (void*)0x009AAC5C, 2155, &bytesRead, NULL);
			CloseHandle(hFile);
			if (RCT2_GLOBAL(0x009AB4C6, sint8) == 1)
				return;
			RCT2_GLOBAL(0x009AB4C6, sint8) = 1;
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) = 0;
			RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FAHRENHEIT, sint8) = 1;
			RCT2_GLOBAL(0x009AACBB, sint8) = 1;
			RCT2_GLOBAL(0x009AACBD, sint16) = 0;
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS))
				RCT2_GLOBAL(0x009AACBD, sint16) = (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) + 1) * 256;
			RCT2_GLOBAL(0x009AA00D, sint8) = 1;
		}
	}

	RCT2_GLOBAL(0x009AAC77, sint8) = 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_OS_TOTALPHYS, uint32) > 0x4000000) {
		RCT2_GLOBAL(0x009AAC77, sint8) = 1;
		if (RCT2_GLOBAL(RCT2_ADDRESS_OS_TOTALPHYS, uint32) > 0x8000000)
			RCT2_GLOBAL(0x009AAC77, sint8) = 2;
	}

	RCT2_GLOBAL(0x009AAC75, sint8) = RCT2_ADDRESS(0x009AF601, sint8)[RCT2_GLOBAL(0x009AAC77, sint8)];
	RCT2_GLOBAL(0x009AAC76, sint8) = RCT2_ADDRESS(0x009AF604, sint8)[RCT2_GLOBAL(0x009AAC77, sint8)];
	RCT2_GLOBAL(0x009AACBD, sint16) = 0;
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_SHOW_HEIGHT_AS_UNITS))
		RCT2_GLOBAL(0x009AACBD, sint16) = (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, sint8) + 1) * 256;
	RCT2_GLOBAL(0x009AA00D, sint8) = 1;
}

/**
 * 
 *  rct2: 0x00675487
 */
void config_save()
{
	HANDLE hFile;
	DWORD bytesWritten;

	hFile = CreateFile(get_file_path(PATH_ID_GAMECFG), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		WriteFile(hFile, &MagicNumber, 4, &bytesWritten, NULL);
		WriteFile(hFile, 0x009AAC5C, 2155, &bytesWritten, NULL);
		CloseHandle(hFile);
	}
}