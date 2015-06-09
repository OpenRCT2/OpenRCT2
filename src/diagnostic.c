/*****************************************************************************
* Copyright (c) 2014 Ted John, Duncan Frost
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

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include "diagnostic.h"

int _log_levels[DIAGNOSTIC_LEVEL_COUNT] = { 1, 1, 1, 0, 1 };
int _log_location_enabled = 1;

const char * _level_strings[] = {
	"FATAL",
	"ERROR",
	"WARNING",
	"VERBOSE",
	"INFO"
};

void diagnostic_log(int diagnosticLevel, const char *format, ...)
{
	FILE *stream;
	va_list args;

	if (!_log_levels[diagnosticLevel])
		return;

	stream = stderr;

	// Level
	fprintf(stream, "%s: ", _level_strings[diagnosticLevel]);

	// Message
	va_start(args, format);
	vfprintf(stream, format, args);
	va_end(args);

	// Line terminator
	fprintf(stream, "\n");
}

void diagnostic_log_with_location(int diagnosticLevel, const char *file, const char *function, int line, const char *format, ...)
{
	FILE *stream;
	va_list args;

	if (!_log_levels[diagnosticLevel])
		return;

	stream = stderr;

	// Level and source code information
	if (_log_location_enabled)
		fprintf(stream, "%s[%s:%d (%s)]: ", _level_strings[diagnosticLevel], file, line, function);
	else
		fprintf(stream, "%s: ", _level_strings[diagnosticLevel]);

	// Message
	va_start(args, format);
	vfprintf(stream, format, args);
	va_end(args);

	// Line terminator
	fprintf(stream, "\n");
}