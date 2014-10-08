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

#ifndef LOCALISATION_H
#define LOCALISATION_H

#include "currency.h"
#include "format_codes.h"
#include "language.h"
#include "string_ids.h"

void format_string(char *dest, rct_string_id format, void *args);
void generate_string_file();
void reset_saved_strings();
void error_string_quit(int error, rct_string_id format);
int get_string_length(char* buffer);

// Real name data
extern const char real_name_initials[16];
extern const char *real_names[1024];

#endif
