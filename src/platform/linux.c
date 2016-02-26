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

#include "../common.h"

#ifdef __LINUX__

#include <ctype.h>
#include <dlfcn.h>
#include <errno.h>
#include <fontconfig/fontconfig.h>

#include "../localisation/language.h"
#include "../localisation/string_ids.h"
#include "../util/util.h"
#include "platform.h"

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

typedef enum { DT_NONE, DT_KDIALOG, DT_ZENITY } dialog_type;

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
	safe_strcpy(outPath, exePath, exeDelimiterIndex + 1);
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
 *   - /usr/local/share/openrct2
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
		"/usr/local/share/openrct2",
		"/var/lib/openrct2",
		"/usr/share/openrct2",
	};
	for (size_t i = 0; i < countof(searchLocations); i++)
	{
		log_verbose("Looking for OpenRCT2 data in %s", searchLocations[i]);
		if (platform_directory_exists(searchLocations[i]))
		{
			out[0] = '\0';
			safe_strcpy(out, searchLocations[i], MAX_PATH);
			return;
		}
	}
}


void execute_cmd(char *command, int *exit_value, char *buf, size_t *buf_size) {
	FILE *f;
	size_t n_chars;

	log_verbose("executing \"%s\"...\n", command);
	f = popen(command, "r");

	if (buf && buf_size) {
		n_chars = fread(buf, 1, *buf_size, f);

		// make sure string is null-terminated
		if (n_chars == *buf_size) {
			n_chars--;
		}
		buf[n_chars] = '\0';

		*buf_size = n_chars;
	} else {
		fflush(f);
	}

	if (exit_value)
		*exit_value = pclose(f);
	else
		pclose(f);
}

dialog_type get_dialog_app(char *cmd, size_t *cmd_size) {
	int exit_value;
	size_t size;
	dialog_type dtype;

	/*
	 * prefer zenity as it offers more required features, e.g., overwrite
	 * confirmation and selecting only existing files
	 */

	dtype = DT_ZENITY;
	size = *cmd_size;
	execute_cmd("which zenity", &exit_value, cmd, &size);

	if (exit_value != 0) {
		dtype = DT_KDIALOG;
		size = *cmd_size;
		execute_cmd("which kdialog", &exit_value, cmd, &size);

		if (exit_value != 0) {
			log_error("no dialog (zenity or kdialog) found\n");
			return DT_NONE;
		}
	}

	cmd[size-1] = '\0';

	*cmd_size = size;

	return dtype;
}

int platform_open_common_file_dialog(filedialog_type type, utf8 *title, utf8 *filename, utf8 *filterPattern, utf8 *filterName) {
	int exit_value;
	char executable[MAX_PATH];
	char cmd[MAX_PATH];
	char result[MAX_PATH];
	size_t size;
	dialog_type dtype;
	char *action;
	char *flags;
	char *filter = NULL;
	char filterPatternRegex[64];
	char *allFilesPatternDescription;

	size = MAX_PATH;
	dtype = get_dialog_app(executable, &size);

	switch (dtype) {
		case DT_KDIALOG:
			switch (type) {
				case FD_OPEN:
					action = "--getopenfilename";
					break;
				case FD_SAVE:
					action = "--getsavefilename";
					break;
			}

			if (filterPattern && filterName) {
				filter = (char*) malloc(1 + strlen(filterPattern) + 3 + strlen(filterName) + 1);
				sprintf(filter, "\"%s | %s\"", filterPattern, filterName);
			}

			snprintf(cmd, MAX_PATH, "%s --title \"%s\" %s ~ %s", executable, title, action, filter?filter:"");
			break;
		case DT_ZENITY:
			action = "--file-selection";
			switch (type) {
				case FD_SAVE:
					flags = "--confirm-overwrite --save";
					break;
				case FD_OPEN:
					flags = "";
					break;
			}

			// Zenity seems to be case sensitive, while Kdialog isn't.
			if (filterPattern && filterName) {
				int regexIterator = 0;
				for(int i = 0; i <= sizeof(filterPattern); i++) {
					if (isalpha(filterPattern[i])) {
						filterPatternRegex[regexIterator+0] = '[';
						filterPatternRegex[regexIterator+1] = (char)toupper(filterPattern[i]);
						filterPatternRegex[regexIterator+2] = (char)tolower(filterPattern[i]);
						filterPatternRegex[regexIterator+3] = ']';
						regexIterator += 3;
					}
					else {
						filterPatternRegex[regexIterator] = (char)filterPattern[i];
					}
					regexIterator++;
				}
				filterPatternRegex[regexIterator+1] = 0;

				allFilesPatternDescription = (char *)language_get_string(STR_ALL_FILES);
				filter = (char*) malloc(strlen("--file-filter=\"") + strlen(filterPatternRegex) + 3 + strlen(filterName) + 2 + strlen(" --file-filter=\"") + strlen(allFilesPatternDescription) + strlen(" | *\""));
				sprintf(filter, "--file-filter=\"%s | %s\" --file-filter=\"%s | *\"", filterName, filterPatternRegex, allFilesPatternDescription);
			}

			snprintf(cmd, MAX_PATH, "%s %s %s --title=\"%s\" / %s", executable, action, flags, title, filter?filter:"");
			break;
		default: return 0;
	}

	size = MAX_PATH;
	execute_cmd(cmd, &exit_value, result, &size);

	if (exit_value != 0) {
		free(filter);
		return 0;
	}

	result[size-1] = '\0';
	log_verbose("filename = %s", result);

	if (type == FD_OPEN && access(result, F_OK) == -1) {
		char msg[MAX_PATH];

		snprintf(msg, MAX_PATH, "\"%s\" not found: %s, please choose another file\n", result, strerror(errno));
		platform_show_messagebox(msg);

		if (filter != NULL)
			free(filter);
		return platform_open_common_file_dialog(type, title, filename, filterPattern, filterName);
	} else
	if (type == FD_SAVE && access(result, F_OK) != -1 && dtype == DT_KDIALOG) {
		snprintf(cmd, MAX_PATH, "%s --yesno \"Overwrite %s?\"", executable, result);

		size = MAX_PATH;
		execute_cmd(cmd, &exit_value, 0, 0);

		if (exit_value != 0) {
			if (filter != NULL)
				free(filter);
			return 0;
		}
	}

	strncpy(filename, result, MAX_PATH);

	if (filter != NULL)
		free(filter);

	return 1;
}

utf8 *platform_open_directory_browser(utf8 *title) {
	size_t size;
	dialog_type dtype;
	int exit_value;
	char cmd[MAX_PATH];
	char executable[MAX_PATH];
	char result[MAX_PATH];
	char *return_value;

	size = MAX_PATH;
	dtype = get_dialog_app(executable, &size);

	switch (dtype) {
		case DT_KDIALOG:
			snprintf(cmd, MAX_PATH, "%s --title \"%s\" --getexistingdirectory /", executable, title);
			break;
		case DT_ZENITY:
			snprintf(cmd, MAX_PATH, "%s --title=\"%s\" --file-selection --directory /", executable, title);
			break;
		default: return 0;
	}

	size = MAX_PATH;
	execute_cmd(cmd, &exit_value, result, &size);

	if (exit_value != 0) {
		return NULL;
	}

	result[size-1] = '\0';

	return_value = (char*) malloc(strlen(result)+1);
	strcpy(return_value, result);

	return return_value;
}

void platform_show_messagebox(char *message) {
	size_t size;
	dialog_type dtype;
	int exit_value;
	char cmd[MAX_PATH];
	char executable[MAX_PATH];

	log_verbose(message);

	size = MAX_PATH;
	dtype = get_dialog_app(executable, &size);

	switch (dtype) {
		case DT_KDIALOG:
			snprintf(cmd, MAX_PATH, "%s --title \"OpenRCT2\" --msgbox \"%s\"", executable, message);
			break;
		case DT_ZENITY:
			snprintf(cmd, MAX_PATH, "%s --title=\"OpenRCT2\" --info --text=\"%s\"", executable, message);
			break;
		default:
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "OpenRCT2", message, gWindow);
			return;
	}

	size = MAX_PATH;
	execute_cmd(cmd, 0, 0, 0);
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
			safe_strcpy(buffer, (utf8*) filename, MAX_PATH);
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
