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

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <windows.h>
#include "rct2.h"

/**
 * Object entry structure.
 * size: 0x10
 */
typedef struct {
	uint32 var_00;
	char name[8];		// 0x04
	uint32 var_0C;
} rct_object_entry;

void object_list_load();
void object_read_and_load_entries(HANDLE hFile);
int object_load_packed();

int object_load(int ecx, rct_object_entry *ebp);

#endif
