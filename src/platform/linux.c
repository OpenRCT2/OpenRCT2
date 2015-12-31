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

#if defined(__linux__)

#include "platform.h"
#include <dlfcn.h>
#include <stdlib.h>
#include "../util/util.h"
#include "fontconfig/fontconfig.h"

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

void platform_get_exe_path(utf8 *outPath)
{
	char exePath[MAX_PATH];
	ssize_t bytesRead;
	bytesRead = readlink("/proc/self/exe", exePath, MAX_PATH);
	if (bytesRead == -1) {
		log_fatal("failed to read /proc/self/exe");
	}
	exePath[bytesRead - 1] = '\0';
	char *exeDelimiter = strrchr(exePath, platform_get_path_separator());
	if (exeDelimiter == NULL)
	{
		log_error("should never happen here");
		outPath[0] = '\0';
		return;
	}
	int exeDelimiterIndex = (int)(exeDelimiter - exePath);

	exePath[exeDelimiterIndex] = '\0';
	safe_strncpy(outPath, exePath, exeDelimiterIndex + 1);
}

bool platform_check_steam_overlay_attached() {
	void* processHandle = dlopen(NULL, RTLD_NOW);

	struct dummy* p = (struct dummy*) processHandle;
	p = p->ptr;

	struct lmap* pl = (struct lmap*) p->ptr;

	while (pl != NULL) {
		if (strstr(pl->path, "gameoverlayrenderer.so") != NULL) {
			dlclose(processHandle);
			return true;
		}
		pl = pl->next;
	}
	dlclose(processHandle);

	return false;
}

/**
 * Default directory fallback is:
 *   - (command line argument)
 *   - $XDG_CONFIG_HOME/OpenRCT2
 *   - /home/[uid]/.config/OpenRCT2
 */
void platform_posix_sub_user_data_path(char *buffer, const char *homedir, const char *separator) {
	const char *configdir = getenv("XDG_CONFIG_HOME");
	log_verbose("configdir = '%s'", configdir);
	if (configdir == NULL)
	{
		log_verbose("configdir was null, used getuid, now is = '%s'", homedir);
		if (homedir == NULL)
		{
			log_fatal("Couldn't find user data directory");
			exit(-1);
			return;
		}

		strncat(buffer, homedir, MAX_PATH - 1);
		strncat(buffer, separator, MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
		strncat(buffer, ".config", MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
	}
	else
	{
		strncat(buffer, configdir, MAX_PATH - 1);
	}
	strncat(buffer, separator, MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
	strncat(buffer, "OpenRCT2", MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
	strncat(buffer, separator, MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
}

/**
 * Default directory fallback is:
 *   - (command line argument)
 *   - <exePath>/data
 *   - /var/lib/openrct2
 *   - /usr/share/openrct2
 */
void platform_posix_sub_resolve_openrct_data_path(utf8 *out) {
	static const utf8 *searchLocations[] = {
		"../share/openrct2",
#ifdef ORCT2_RESOURCE_DIR
		// defined in CMakeLists.txt
		ORCT2_RESOURCE_DIR,
#endif // ORCT2_RESOURCE_DIR
		"/var/lib/openrct2",
		"/usr/share/openrct2",
	};
	for (size_t i = 0; i < countof(searchLocations); i++)
	{
		log_verbose("Looking for OpenRCT2 data in %s", searchLocations[i]);
		if (platform_directory_exists(searchLocations[i]))
		{
			out[0] = '\0';
			safe_strncpy(out, searchLocations[i], MAX_PATH);
			return;
		}
	}
}

utf8 *platform_open_directory_browser(utf8 *title)
{
	STUB();
	return NULL;
}

void platform_show_messagebox(char *message)
{
	STUB();
	log_verbose(message);
}

/**
 *
 *  rct2: 0x004080EA
 */
int platform_open_common_file_dialog(int type, utf8 *title, utf8 *filename, utf8 *filterPattern, utf8 *filterName)
{
	STUB();
	return 0;
}

bool platform_get_font_path(TTFFontDescriptor *font, utf8 *buffer)
{
	assert(buffer != NULL);
	assert(font != NULL);

	log_verbose("Looking for font %s with FontConfig.", font->font_name);
	FcConfig* config = FcInitLoadConfigAndFonts();
	if (!config)
	{
		log_error("Failed to initialize FontConfig library");
		FcFini();
		return false;
	}
	FcPattern* pat = FcNameParse((const FcChar8*) font->font_name);

	FcConfigSubstitute(config, pat, FcMatchPattern);
	FcDefaultSubstitute(pat);

	bool found = false;
	FcResult result = FcResultNoMatch;
	FcPattern* match = FcFontMatch(config, pat, &result);

	if (match)
	{
		FcChar8* filename = NULL;
		if (FcPatternGetString(match, FC_FILE, 0, &filename) == FcResultMatch)
		{
			found = true;
			safe_strncpy(buffer, (utf8*) filename, MAX_PATH);
			log_verbose("FontConfig provided font %s", filename);
		}
		FcPatternDestroy(match);
	} else {
		log_warning("Failed to find required font.");
	}

	FcPatternDestroy(pat);
	FcConfigDestroy(config);
	FcFini();
	return found;
}

#endif
