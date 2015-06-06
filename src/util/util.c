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

#include "util.h"

int squaredmetres_to_squaredfeet(int squaredMetres)
{
	// 1 metre squared = 10.7639104 feet squared
	// RCT2 approximates as 11
	return squaredMetres * 11;
}

int metres_to_feet(int metres)
{
	// 1 metre = 3.2808399 feet
	// RCT2 approximates as 3.28125
	return (metres * 840) / 256;
}

int mph_to_kmph(int mph)
{
	// 1 mph = 1.60934 kmph
	// RCT2 approximates as 1.609375
	return (mph * 1648) / 1024;
}

const char *path_get_filename(const char *path)
{
	const char *result, *ch;

	result = path;
	for (ch = path; *ch != 0; ch++) {
		if (*ch == '/' || *ch == '\\') {
			if (*(ch + 1) != 0)
				result = ch + 1;
		}
	}

	return result;
}

void path_set_extension(char *path, const char *extension)
{
	char *ch = path;
	while (*ch != '.' && *ch != 0) {
		ch++;
	}

	if (extension[0] != '.')
		*ch++ = '.';

	strcpy(ch, extension);
}

void path_remove_extension(char *path)
{
	char *ch = path;

	while (*ch != '\0') ++ch;
	for (--ch; ch >= path; --ch) {
		if (*ch == '.') {
			*ch = '\0';
			break;
		}
	}
}

long fsize(FILE *fp)
{
	long originalPosition, size;

	originalPosition = ftell(fp);
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, originalPosition, SEEK_SET);

	return size;
}

bool readentirefile(const char *path, void **outBuffer, long *outLength)
{
	FILE *fp;
	long fpLength;
	void *fpBuffer;

	// Open file
	fp = fopen(path, "rb");
	if (fp == NULL)
		return 0;

	// Get length
	fseek(fp, 0, SEEK_END);
	fpLength = ftell(fp);
	rewind(fp);

	// Read whole file into a buffer
	fpBuffer = malloc(fpLength);
	fread(fpBuffer, fpLength, 1, fp);
	fclose(fp);

	*outBuffer = fpBuffer;
	*outLength = fpLength;
	return 1;
}

int bitscanforward(int source)
{
	int i;

	for (i = 0; i < 32; i++)
		if (source & (1 << i))
			return i;

	return -1;
}

bool strequals(const char *a, const char *b, int length, bool caseInsensitive)
{
	return caseInsensitive ?
		_strnicmp(a, b, length) == 0 :
		strncmp(a, b, length) == 0;
}

/* case insensitve compare */
int strcicmp(char const *a, char const *b)
{
	for (;; a++, b++) {
		int d = tolower(*a) - tolower(*b);
		if (d != 0 || !*a)
			return d;
	}
}