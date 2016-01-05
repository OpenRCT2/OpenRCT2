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
#include "../platform/platform.h"

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
	return (mph * 1648) >> 10;
}

int mph_to_dmps(int mph)
{
	// 1 mph = 4.4704 decimeters/s
	return (mph * 73243) >> 14;
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
	// Find last slash or backslash in the path
	char *filename = strrchr(path, platform_get_path_separator());

	// Checks if the path is valid (e.g. not just a file name)
	if (filename == NULL)
	{
		// Return the input string to keep things working
		return path;
	}

	// Increase pointer by one, to get rid of the slashes
	filename++;

	return filename;
}

// Returns the extension (dot inclusive) from the given path, or the end of the
// string when no extension was found.
const char *path_get_extension(const utf8 *path)
{
	// Get the filename from the path
	const char *filename = path_get_filename(path);

	// Try to find the most-right dot in the filename
	char *extension = strrchr(filename, '.');

	// When no dot was found, return a pointer to the null-terminator
	if (extension == NULL)
		extension = strrchr(filename, '\0');

	return extension;
}

void path_set_extension(utf8 *path, const utf8 *newExtension)
{
	// Remove existing extension (check first if there is one)
	if (path_get_extension(path) < strrchr(path, '\0'))
		path_remove_extension(path);
	// Append new extension
	path_append_extension(path, newExtension);
}

void path_append_extension(utf8 *path, const utf8 *newExtension)
{
	// Append a dot to the filename if the new extension doesn't start with it
	char *endOfString = strrchr(path, '\0');
	if (newExtension[0] != '.')
		*endOfString++ = '.';

	// Append the extension to the path
	safe_strncpy(endOfString, newExtension, MAX_PATH - (endOfString - path) - 1);
}

void path_remove_extension(utf8 *path)
{
	// Find last dot in filename, and replace it with a null-terminator
	char *lastDot = strrchr(path_get_filename(path), '.');
	if (lastDot != NULL)
		*lastDot = '\0';
	else
		log_warning("No extension found. (path = %s)", path);
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
	fpLength = (int)SDL_RWsize(fp);

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

	#if _MSC_VER >= 1400 // Visual Studio 2005
		uint8 success = _BitScanForward(&i, source);
		return success != 0 ? i : -1;
	#else
	for (i = 0; i < 32; i++)
		if (source & (1u << i))
			return i;

	return -1;
	#endif
}

int bitcount(int source)
{
	int result = 0;
	for (int i = 0; i < 32; i++) {
		if (source & (1u << i)) {
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

char *safe_strncpy(char * destination, const char * source, size_t size)
{
	assert(destination != NULL);
	assert(source != NULL);

	if (size == 0)
	{
		return destination;
	}
	char *result = destination;
	bool terminated = false;
	for (size_t i = 0; i < size; i++)
	{
		if (*source != '\0')
		{
			*destination++ = *source++;
		} else {
			*destination = *source;
			terminated = true;
			break;
		}
	}
	if (!terminated)
	{
		result[size - 1] = '\0';
		log_warning("Truncating string \"%s\" to %d bytes.", result, size);
	}
	return result;
}

char *safe_strcat(char *destination, const char *source, size_t size)
{
	assert(destination != NULL);
	assert(source != NULL);

	if (size == 0) {
		return destination;
	}

	char *result = destination;

	size_t i;
	for (i = 0; i < size; i++) {
		if (*destination == '\0') {
			break;
		} else {
			destination++;
		}
	}

	bool terminated = false;
	for (; i < size; i++) {
		if (*source != '\0') {
			*destination++ = *source++;
		} else {
			*destination = *source;
			terminated = true;
			break;
		}
	}

	if (!terminated) {
		result[size - 1] = '\0';
		log_warning("Truncating string \"%s\" to %d bytes.", result, size);
	}

	return result;
}

char *safe_strcat_path(char *destination, const char *source, size_t size)
{
	const char pathSeparator = platform_get_path_separator();

	size_t length = strlen(destination);
	if (length >= size - 1) {
		return destination;
	}

	if (destination[length - 1] != pathSeparator) {
		destination[length] = pathSeparator;
		destination[length + 1] = '\0';
	}

	return safe_strcat(destination, source, size);
}

char *safe_strtrimleft(char *destination, const char *source, size_t size)
{
	while (*source == ' ' && *source != '\0') {
		source++;
	}
	return safe_strncpy(destination, source, size);
}

bool utf8_is_bom(const char *str)
{
	return str[0] == (char)0xEF && str[1] == (char)0xBB && str[2] == (char)0xBF;
}

bool str_is_null_or_empty(const char *str)
{
	return str == NULL || str[0] == 0;
}

uint32 srand0, srand1, srand2, srand3;

void util_srand(int source) {
	srand0 = source;
	srand1 = srand0 ^ (source >> 24);
	srand2 = srand1 ^ (source >> 16);
	srand3 = srand2 ^ (source >> 8);
}

uint32 util_rand() {
	uint32 temp = srand0 ^ (srand0 << 11);
	srand0 = srand1;
	srand1 = srand2;
	srand2 = srand3;
	srand3 = srand3 ^ (srand3 >> 19) ^ temp ^ (temp >> 8);
	return srand3;
}
