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

#ifndef _WIN32
#ifndef __APPLE__

#include "../cmdline.h"
#include "../openrct2.h"

/**
 * Unix, linux and fallback entry point to OpenRCT2.
 */
// int main(char *argv[], int argc)
// {
//     if (cmdline_run(argv, argc))
//       openrct2_launch();
// 
//     return gExitCode;
// }

char platform_get_path_separator()
{
	return '/';
}

#endif
#endif