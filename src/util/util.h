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

#ifndef _UTIL_H_
#define _UTIL_H_

#include "../common.h"

int squaredmetres_to_squaredfeet(int squaredMetres);
int metres_to_feet(int metres);
int mph_to_kmph(int mph);

const char *path_get_filename(const char *path);
const char *path_get_extension(const char *path);
void path_set_extension(char *path, const char *extension);
void path_remove_extension(char *path);
long fsize(FILE *fp);
bool readentirefile(const char *path, void **outBuffer, long *outLength);

int bitscanforward(int source);
bool strequals(const char *a, const char *b, int length, bool caseInsensitive);
int strcicmp(char const *a, char const *b);

bool utf8_is_bom(const char *str);

#endif
