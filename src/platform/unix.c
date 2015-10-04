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
#include <dlfcn.h>

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

/*
char platform_get_path_separator()
{
	return '/';
}
*/

// See http://syprog.blogspot.ru/2011/12/listing-loaded-shared-objects-in-linux.html
struct lmap {
	void* base_address;
	char* path;
	void* unused;
	struct lmap *next, *prev;
};

struct dummy {
	void* pointers[3];
	struct dummy* ptr;
};

bool platform_check_steam_overlay_attached() {
	void* processHandle = dlopen(NULL, RTLD_NOW);

	struct dummy* p = (struct dummy*) processHandle;
	p = p->ptr;

	struct lmap* pl = (struct lmap*) p->ptr;

	while (pl != NULL) {
		if (strstr(pl->path, "gameoverlayrenderer.so") != NULL) {
			return true;
		}
		pl = pl->next;
	}

	return false;
}

#endif
#endif
