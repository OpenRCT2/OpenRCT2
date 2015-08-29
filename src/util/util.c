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
#include <SDL.h>

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

bool filename_valid_characters(const utf8 *filename)
{
	for (int i = 0; filename[i] != '\0'; i++) {
		if (filename[i] == '\\' || filename[i] == '/' || filename[i] == ':' || filename[i] == '?' ||
			filename[i] == '*' || filename[i] == '<' || filename[i] == '>' || filename[i] == '|')
			return false;
	}
	return true;
}

const char *path_get_filename(const utf8 *path)
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

const char *path_get_extension(const utf8 *path)
{
	const char *extension = NULL;
	const char *ch = path;
	while (*ch != 0) {
		if (*ch == '.')
			extension = ch;

		ch++;
	}
	if (extension == NULL)
		extension = ch;
	return extension;
}

void path_set_extension(utf8 *path, const utf8 *newExtension)
{
	char *extension = NULL;
	char *ch = path;
	while (*ch != 0) {
		if (*ch == '.')
			extension = ch;

		ch++;
	}
	if (extension == NULL)
		extension = ch;

	if (newExtension[0] != '.')
		*extension++ = '.';

	strcpy(extension, newExtension);
}

void path_remove_extension(utf8 *path)
{
	char *ch = path + strlen(path);
	for (--ch; ch >= path; --ch) {
		if (*ch == '.') {
			*ch = '\0';
			break;
		}
	}
}

bool readentirefile(const utf8 *path, void **outBuffer, int *outLength)
{
	SDL_RWops *fp;
	int fpLength;
	void *fpBuffer;

	// Open file
	fp = SDL_RWFromFile(path, "rb");
	if (fp == NULL)
		return 0;

	// Get length
	SDL_RWseek(fp, 0, RW_SEEK_END);
	fpLength = (int)SDL_RWtell(fp);
	SDL_RWseek(fp, 0, RW_SEEK_SET);

	// Read whole file into a buffer
	fpBuffer = malloc(fpLength);
	SDL_RWread(fp, fpBuffer, fpLength, 1);
	SDL_RWclose(fp);

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

int bitcount(int source)
{
	int result = 0;
	for (int i = 0; i < 32; i++) {
		if (source & (1 << i)) {
			result++;
		}
	}
	return result;
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

bool utf8_is_bom(const char *str)
{
	return str[0] == (char)0xEF && str[1] == (char)0xBB && str[2] == (char)0xBF;
}

bool str_is_null_or_empty(const char *str)
{
	return str == NULL || str[0] == 0;
}