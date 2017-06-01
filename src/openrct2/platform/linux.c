#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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
