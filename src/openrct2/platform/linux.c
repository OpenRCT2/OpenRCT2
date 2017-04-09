#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../common.h"

// Despite the name, this file contains support for more OSs besides Linux, provided the necessary ifdefs remain small.
// Otherwise, they should be spun off into their own files.
#if defined(__LINUX__) || defined(__FREEBSD__)

#ifdef __FREEBSD__
#include <sys/sysctl.h>
#endif

#define OPENRCT2_MAX_COMMAND_LENGTH (2 * MAX_PATH)

#include <ctype.h>
#include <dlfcn.h>
#include <errno.h>
#include <fontconfig/fontconfig.h>
#include <fnmatch.h>
#include <locale.h>

#include "../config/Config.h"
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

void platform_get_exe_path(utf8 *outPath, size_t outSize)
{
	char exePath[MAX_PATH];
#ifdef __LINUX__
	ssize_t bytesRead;
	bytesRead = readlink("/proc/self/exe", exePath, MAX_PATH);
	if (bytesRead == -1) {
		log_fatal("failed to read /proc/self/exe");
	}
	exePath[bytesRead - 1] = '\0';
#elif defined(__FREEBSD__)
	size_t exeLen = sizeof(exePath);
	const sint32 mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };
	if (sysctl(mib, 4, exePath, &exeLen, NULL, 0) == -1) {
		log_fatal("failed to get process path");

	}
#else
#error "Platform does not support full path exe retrieval"
#endif
	char *exeDelimiter = strrchr(exePath, *PATH_SEPARATOR);
	if (exeDelimiter == NULL)
	{
		log_error("should never happen here");
		outPath[0] = '\0';
		return;
	}
	*exeDelimiter = '\0';

	safe_strcpy(outPath, exePath, outSize);
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
void platform_posix_sub_user_data_path(char *buffer, size_t size, const char *homedir) {
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

		safe_strcpy(buffer, homedir, size);
		safe_strcat_path(buffer, ".config", size);
	}
	else
	{
		safe_strcpy(buffer, configdir, size);
	}
	safe_strcat_path(buffer, "OpenRCT2", size);
	path_end_with_separator(buffer, size);
}

/**
 * Default directory fallback is:
 *   - (command line argument)
 *   - <exePath>/data
 *   - /usr/local/share/openrct2
 *   - /var/lib/openrct2
 *   - /usr/share/openrct2
 */
void platform_posix_sub_resolve_openrct_data_path(utf8 *out, size_t size) {
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
			safe_strcpy(out, searchLocations[i], size);
			return;
		}
	}
}

uint16 platform_get_locale_language(){
	const char *langString = setlocale(LC_MESSAGES, "");
	if(langString != NULL){
		// The locale has the following form:
		// language[_territory[.codeset]][@modifier]
		// (see https://www.gnu.org/software/libc/manual/html_node/Locale-Names.html)
		// longest on my system is 29 with codeset and modifier, so 32 for the pattern should be more than enough
		char pattern[32];
		//strip the codeset and modifier part
		sint32 length = strlen(langString);
		{
			for(sint32 i = 0; i < length; ++i){
				if(langString[i] == '.' || langString[i] == '@'){
					length = i;
					break;
				}
			}
		} //end strip
		memcpy(pattern, langString, length); //copy all until first '.' or '@'
		pattern[length] = '\0';
		//find _ if present
		const char *strip = strchr(pattern, '_');
		if(strip != NULL){
			// could also use '-', but '?' is more flexible. Maybe LanguagesDescriptors will change.
			// pattern is now "language?territory"
			pattern[strip - pattern] = '?';
		}

		// Iterate through all available languages
		for(sint32 i = 1; i < LANGUAGE_COUNT; ++i){
			if(!fnmatch(pattern, LanguagesDescriptors[i].locale, 0)){
				return i;
			}
		}

		//special cases :(
		if(!fnmatch(pattern, "en_CA", 0)){
			return LANGUAGE_ENGLISH_US;
		}
		else if (!fnmatch(pattern, "zh_CN", 0)){
			return LANGUAGE_CHINESE_SIMPLIFIED;
		}
		else if (!fnmatch(pattern, "zh_TW", 0)){
			return LANGUAGE_CHINESE_TRADITIONAL;
		}

		//no exact match found trying only language part
		if(strip != NULL){
			pattern[strip - pattern] = '*';
			pattern[strip - pattern +1] = '\0'; // pattern is now "language*"
			for(sint32 i = 1; i < LANGUAGE_COUNT; ++i){
				if(!fnmatch(pattern, LanguagesDescriptors[i].locale, 0)){
					return i;
				}
			}
		}
	}
	return LANGUAGE_ENGLISH_UK;
}

uint8 platform_get_locale_currency(){
	char *langstring = setlocale(LC_MONETARY, "");

	if (langstring == NULL) {
		return platform_get_currency_value(NULL);
	}

	struct lconv *lc = localeconv();

	return platform_get_currency_value(lc->int_curr_symbol);
}

uint8 platform_get_locale_measurement_format(){
	// LC_MEASUREMENT is GNU specific.
	#ifdef LC_MEASUREMENT
	const char *langstring = setlocale(LC_MEASUREMENT, "");
	#else
	const char *langstring = setlocale(LC_ALL, "");
	#endif

	if(langstring != NULL){
		//using https://en.wikipedia.org/wiki/Metrication#Chronology_and_status_of_conversion_by_country as reference
		if(!fnmatch("*_US*", langstring, 0) || !fnmatch("*_MM*", langstring, 0) || !fnmatch("*_LR*", langstring, 0)){
			return MEASUREMENT_FORMAT_IMPERIAL;
		}
	}
	return MEASUREMENT_FORMAT_METRIC;
}

uint8 platform_get_locale_date_format()
{
	return DATE_FORMAT_DAY_MONTH_YEAR;
}

static void execute_cmd(char *command, sint32 *exit_value, char *buf, size_t *buf_size) {
	FILE *f;
	size_t n_chars;

	log_verbose("executing \"%s\"...\n", command);
	f = popen(command, "r");

	if (buf && buf_size) {
		n_chars = fread(buf, 1, *buf_size, f);

		// some commands may return a new-line terminated result, trim that
		if (n_chars > 0 && buf[n_chars - 1] == '\n') {
			buf[n_chars - 1] = '\0';
		}
		// make sure string is null-terminated
		if (n_chars == *buf_size) {
			n_chars--;
		}
		buf[n_chars] = '\0';

		// account for null terminator
		*buf_size = n_chars + 1;
	} else {
		fflush(f);
	}

	if (exit_value)
		*exit_value = pclose(f);
	else
		pclose(f);
}

static dialog_type get_dialog_app(char *cmd, size_t *cmd_size) {
	sint32 exit_value;
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

bool platform_open_common_file_dialog(utf8 *outFilename, file_dialog_desc *desc, size_t outSize) {
	sint32 exit_value;
	char executable[MAX_PATH];
	char cmd[OPENRCT2_MAX_COMMAND_LENGTH];
	char result[OPENRCT2_MAX_COMMAND_LENGTH];
	size_t size;
	dialog_type dtype;
	char *action = NULL;
	char *flags = NULL;
	char filter[OPENRCT2_MAX_COMMAND_LENGTH] = { 0 };
	char filterPatternRegex[64];

	size = OPENRCT2_MAX_COMMAND_LENGTH;
	dtype = get_dialog_app(executable, &size);

	switch (dtype) {
		case DT_KDIALOG:
			switch (desc->type) {
				case FD_OPEN:
					action = "--getopenfilename";
					break;
				case FD_SAVE:
					action = "--getsavefilename";
					break;
			}

			{
		        bool first = true;
				for (sint32 j = 0; j < countof(desc->filters); j++) {
					if (desc->filters[j].pattern && desc->filters[j].name) {
						char filterTemp[100] = { 0 };
						if (first) {
							snprintf(filterTemp, countof(filterTemp), "%s | %s", desc->filters[j].pattern, desc->filters[j].name);
							first = false;
						} else {
							snprintf(filterTemp, countof(filterTemp), "\\n%s | %s", desc->filters[j].pattern, desc->filters[j].name);
						}
						safe_strcat(filter, filterTemp, countof(filter));
					}
				}
				char filterTemp[100] = { 0 };
				if (first) {
					snprintf(filterTemp, countof(filterTemp), "*|%s", (char *)language_get_string(STR_ALL_FILES));
				} else {
					snprintf(filterTemp, countof(filterTemp), "\\n*|%s", (char *)language_get_string(STR_ALL_FILES));
				}
				safe_strcat(filter, filterTemp, countof(filter));

				// kdialog wants filters space-delimited and we don't expect ';' anywhere else,
				// this is much easier and quicker to do than being overly careful about replacing
				// it only where truly needed.
				sint32 filterSize = strlen(filter);
				for (sint32 i = 0; i < filterSize + 3; i++) {
					if (filter[i] == ';') {
						filter[i] = ' ';
					}
				}
			}

			snprintf(cmd, OPENRCT2_MAX_COMMAND_LENGTH, "%s --title '%s' %s '%s/' ~ '%s'", executable, desc->title, action, desc->initial_directory, filter);
			break;
		case DT_ZENITY:
			action = "--file-selection";
			switch (desc->type) {
				case FD_SAVE:
					flags = "--confirm-overwrite --save";
					break;
				case FD_OPEN:
					flags = "";
					break;
			}

			// Zenity seems to be case sensitive, while Kdialog isn't.
			for (sint32 j = 0; j < countof(desc->filters); j++) {
				if (desc->filters[j].pattern && desc->filters[j].name) {
					sint32 regexIterator = 0;
					for(sint32 i = 0; i <= strlen(desc->filters[j].pattern); i++) {
						if (isalpha(desc->filters[j].pattern[i])) {
							filterPatternRegex[regexIterator+0] = '[';
							filterPatternRegex[regexIterator+1] = (char)toupper(desc->filters[j].pattern[i]);
							filterPatternRegex[regexIterator+2] = (char)tolower(desc->filters[j].pattern[i]);
							filterPatternRegex[regexIterator+3] = ']';
							regexIterator += 3;
						}
						else if(desc->filters[j].pattern[i] == ';') {
							filterPatternRegex[regexIterator] = ' ';
						}
						else {
							filterPatternRegex[regexIterator] = (char)desc->filters[j].pattern[i];
						}
						regexIterator++;
					}
					filterPatternRegex[regexIterator+1] = 0;

					char filterTemp[100] = { 0 };
					snprintf(filterTemp, countof(filterTemp), " --file-filter='%s | %s'", desc->filters[j].name, filterPatternRegex);
					safe_strcat(filter, filterTemp, countof(filter));
				}
			}
			char filterTemp[100] = { 0 };
			snprintf(filterTemp, countof(filterTemp), " --file-filter='%s | *'", (char *)language_get_string(STR_ALL_FILES));
			safe_strcat(filter, filterTemp, countof(filter));

			snprintf(cmd, OPENRCT2_MAX_COMMAND_LENGTH, "%s %s --filename='%s/' %s --title='%s' / %s", executable, action, desc->initial_directory, flags, desc->title, filter);
			break;
		default: return 0;
	}

	size = OPENRCT2_MAX_COMMAND_LENGTH;
	execute_cmd(cmd, &exit_value, result, &size);

	if (exit_value != 0) {
		return 0;
	}

	result[size-1] = '\0';
	log_verbose("filename = %s", result);

	if (desc->type == FD_OPEN && access(result, F_OK) == -1) {
		char msg[OPENRCT2_MAX_COMMAND_LENGTH];

		snprintf(msg, OPENRCT2_MAX_COMMAND_LENGTH, "\"%s\" not found: %s, please choose another file\n", result, strerror(errno));
		platform_show_messagebox(msg);

		return platform_open_common_file_dialog(outFilename, desc, outSize);
	} else
	if (desc->type == FD_SAVE && access(result, F_OK) != -1 && dtype == DT_KDIALOG) {
		snprintf(cmd, OPENRCT2_MAX_COMMAND_LENGTH, "%s --yesno \"Overwrite %s?\"", executable, result);

		size = OPENRCT2_MAX_COMMAND_LENGTH;
		execute_cmd(cmd, &exit_value, 0, 0);

		if (exit_value != 0) {
			return 0;
		}
	}

	safe_strcpy(outFilename, result, outSize);

	return 1;
}

utf8 *platform_open_directory_browser(const utf8 *title) {
	size_t size;
	dialog_type dtype;
	sint32 exit_value;
	char cmd[OPENRCT2_MAX_COMMAND_LENGTH];
	char executable[MAX_PATH];
	char result[OPENRCT2_MAX_COMMAND_LENGTH];
	char *return_value;

	size = MAX_PATH;
	dtype = get_dialog_app(executable, &size);

	switch (dtype) {
		case DT_KDIALOG:
			snprintf(cmd, OPENRCT2_MAX_COMMAND_LENGTH, "%s --title '%s' --getexistingdirectory /", executable, title);
			break;
		case DT_ZENITY:
			snprintf(cmd, OPENRCT2_MAX_COMMAND_LENGTH, "%s --title='%s' --file-selection --directory /", executable, title);
			break;
		default: return 0;
	}

	size = OPENRCT2_MAX_COMMAND_LENGTH;
	execute_cmd(cmd, &exit_value, result, &size);

	if (exit_value != 0) {
		return NULL;
	}

	assert(size - 1 < countof(result));
	result[size-1] = '\0';

	return_value = _strdup(result);

	return return_value;
}

void platform_show_messagebox(const char * message) {
	size_t size;
	dialog_type dtype;
	char cmd[OPENRCT2_MAX_COMMAND_LENGTH];
	char executable[MAX_PATH];

	log_verbose(message);

	size = OPENRCT2_MAX_COMMAND_LENGTH;
	dtype = get_dialog_app(executable, &size);

	switch (dtype) {
		case DT_KDIALOG:
			snprintf(cmd, OPENRCT2_MAX_COMMAND_LENGTH, "%s --title \"OpenRCT2\" --msgbox \"%s\"", executable, message);
			break;
		case DT_ZENITY:
			snprintf(cmd, OPENRCT2_MAX_COMMAND_LENGTH, "%s --title=\"OpenRCT2\" --info --text=\"%s\"", executable, message);
			break;
		default:
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "OpenRCT2", message, gWindow);
			return;
	}

	size = OPENRCT2_MAX_COMMAND_LENGTH;
	execute_cmd(cmd, 0, 0, 0);
}

#ifndef NO_TTF
bool platform_get_font_path(TTFFontDescriptor *font, utf8 *buffer, size_t size)
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
			safe_strcpy(buffer, (utf8*) filename, size);
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
#endif // NO_TTF

sint32 platform_get_non_window_flags()
{
	return SDL_WINDOW_MAXIMIZED | SDL_WINDOW_MINIMIZED | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP;
}

#endif
