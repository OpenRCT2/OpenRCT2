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

#ifndef _DIAGNOSTIC_H_
#define _DIAGNOSTIC_H_

enum {
	DIAGNOSTIC_LEVEL_FATAL,
	DIAGNOSTIC_LEVEL_ERROR,
	DIAGNOSTIC_LEVEL_WARNING,
	DIAGNOSTIC_LEVEL_VERBOSE,
	DIAGNOSTIC_LEVEL_INFORMATION,
	DIAGNOSTIC_LEVEL_COUNT
};

extern int _log_levels[DIAGNOSTIC_LEVEL_COUNT];

void diagnostic_log(int diagnosticLevel, const char *format, ...);
void diagnostic_log_with_location(int diagnosticLevel, const char *file, const char *function, int line, const char *format, ...);

#ifdef _MSC_VER

#define diagnostic_log_macro(level, format, ...)	diagnostic_log_with_location(level, __FILE__, __FUNCTION__, __LINE__, format, __VA_ARGS__)

#define log_fatal(format, ...)		diagnostic_log_macro(DIAGNOSTIC_LEVEL_FATAL, format, __VA_ARGS__)
#define log_error(format, ...)		diagnostic_log_macro(DIAGNOSTIC_LEVEL_ERROR, format, __VA_ARGS__)
#define log_warning(format, ...)	diagnostic_log_macro(DIAGNOSTIC_LEVEL_WARNING, format, __VA_ARGS__)
#define log_verbose(format, ...)	diagnostic_log(DIAGNOSTIC_LEVEL_VERBOSE, format, __VA_ARGS__)
#define log_info(format, ...)		diagnostic_log_macro(DIAGNOSTIC_LEVEL_INFORMATION, format, __VA_ARGS__)

#else

#define diagnostic_log_macro(level, format, ...)	diagnostic_log_with_location(level, __FILE__, __func__, __LINE__, format, ## __VA_ARGS__)

#define log_fatal(format, ...)		diagnostic_log_macro(DIAGNOSTIC_LEVEL_FATAL, format, ## __VA_ARGS__)
#define log_error(format, ...)		diagnostic_log_macro(DIAGNOSTIC_LEVEL_ERROR, format, ## __VA_ARGS__)
#define log_warning(format, ...)	diagnostic_log_macro(DIAGNOSTIC_LEVEL_WARNING, format, ## __VA_ARGS__)
#define log_verbose(format, ...)	diagnostic_log(DIAGNOSTIC_LEVEL_VERBOSE, format, ## __VA_ARGS__)

#endif

#endif