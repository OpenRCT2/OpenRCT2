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
 
#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include "../common.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#define INVALID_HANDLE -1

typedef struct {
	const char *path;
	uint64 size;
	uint64 last_modified;
} file_info;

// Platform specific definitions
char platform_get_path_separator();
int platform_file_exists(const char *path);
int platform_directory_exists(const char *path);
int platform_ensure_directory_exists(const char *path);
int platform_lock_single_instance();
int platform_enumerate_files_begin(const char *pattern);
int platform_enumerate_files_next(int handle, file_info *outFileInfo);
void platform_enumerate_files_end(int handle);
void platform_hide_cursor();
void platform_show_cursor();
void platform_get_cursor_position(int *x, int *y);
void platform_set_cursor_position(int x, int y);
unsigned int platform_get_ticks();

#endif