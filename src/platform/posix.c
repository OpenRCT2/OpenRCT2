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

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))

#include <libgen.h>
#include <SDL_syswm.h>
#include <sys/stat.h>
#include <errno.h>
#include <pwd.h>
#include "../addresses.h"
#include "../cmdline.h"
#include "../openrct2.h"
#include "../localisation/language.h"
#include "../localisation/currency.h"
#include "../config.h"
#include "platform.h"
#include "../util/util.h"
#include <dirent.h>
#include <fnmatch.h>
#include <locale.h>
#include <sys/time.h>
#include <time.h>

// The name of the mutex used to prevent multiple instances of the game from running
#define SINGLE_INSTANCE_MUTEX_NAME "RollerCoaster Tycoon 2_GSKMUTEX"

utf8 _userDataDirectoryPath[MAX_PATH] = { 0 };
utf8 _openrctDataDirectoryPath[MAX_PATH] = { 0 };

/**
 * The function that is called directly from the host application (rct2.exe)'s WinMain. This will be removed when OpenRCT2 can
 * be built as a stand alone application.
 */
int main(int argc, const char **argv)
{
	int run_game = cmdline_run(argv, argc);
	if (run_game == 1)
	{
		openrct2_launch();
	}

	exit(gExitCode);
	return gExitCode;
}

void platform_get_date(rct2_date *out_date)
{
	assert(out_date != NULL);
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = gmtime(&rawtime);
	out_date->day = timeinfo->tm_mday;
	out_date->month = timeinfo->tm_mon;
	out_date->year = timeinfo->tm_year;
	out_date->day_of_week = timeinfo->tm_wday;
}

void platform_get_time(rct2_time *out_time)
{
	assert(out_time != NULL);
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = gmtime(&rawtime);
	out_time->second = timeinfo->tm_sec;
	out_time->minute = timeinfo->tm_min;
	out_time->hour = timeinfo->tm_hour;
}

char platform_get_path_separator()
{
	return '/';
}

bool platform_file_exists(const utf8 *path)
{
	wchar_t *wPath = utf8_to_widechar(path);
	int len = min(MAX_PATH - 1, utf8_length(path));
	char buffer[MAX_PATH];
	wcstombs(buffer, wPath, len);
	buffer[len] = '\0';
	free(wPath);
	bool exists = access(buffer, F_OK) != -1;
	log_verbose("file '%s' exists = %i", buffer, exists);
	return exists;
}

bool platform_directory_exists(const utf8 *path)
{
	wchar_t *wPath = utf8_to_widechar(path);
	int len = min(MAX_PATH - 1, utf8_length(path));
	char buffer[MAX_PATH];
	wcstombs(buffer, wPath, len);
	buffer[len] = '\0';
	free(wPath);
	struct stat dirinfo;
	int result = stat(buffer, &dirinfo);
	log_verbose("checking dir %s, result = %d, is_dir = %d", buffer, result, S_ISDIR(dirinfo.st_mode));
	if ((result != 0) || !S_ISDIR(dirinfo.st_mode))
	{
		return false;
	}
	return true;
}

bool platform_original_game_data_exists(const utf8 *path)
{
	wchar_t *wPath = utf8_to_widechar(path);
	int len = min(MAX_PATH - 1, utf8_length(path));
	char buffer[MAX_PATH];
	wcstombs(buffer, wPath, len);
	buffer[len] = '\0';
	free(wPath);
	char checkPath[MAX_PATH];
	sprintf(checkPath, "%s%c%s%c%s", buffer, platform_get_path_separator(), "Data", platform_get_path_separator(), "g1.dat");
	return platform_file_exists(checkPath);
}

mode_t getumask()
{
	mode_t mask = umask(0);
	umask(mask);
	return 0777 & ~mask; // Keep in mind 0777 is octal
}

bool platform_ensure_directory_exists(const utf8 *path)
{
	mode_t mask = getumask();

	wchar_t *wPath = utf8_to_widechar(path);
	int len = min(MAX_PATH, utf8_length(path));
	char buffer[MAX_PATH];
	wcstombs(buffer, wPath, len);
	buffer[len - 1] = '\0';
	free(wPath);
	log_verbose("%s", buffer);
	const int result = mkdir(buffer, mask);
	if (result == 0 || (result == -1 && errno == EEXIST))
		return true;
	return false;
}

bool platform_directory_delete(const utf8 *path)
{
	STUB();
	return true;
}

bool platform_lock_single_instance()
{
	STUB();
	return true;
}

typedef struct {
	char active;
	char pattern[MAX_PATH];
	struct dirent **fileListTemp;
	char **paths;
	int cnt;
	int handle;
	void* data;
} enumerate_file_info;
static enumerate_file_info _enumerateFileInfoList[8] = { 0 };

char *g_file_pattern;

static int winfilter(const struct dirent *d)
{
	int entry_length = strnlen(d->d_name, MAX_PATH);
	char *name_upper = malloc(entry_length + 1);
	if (name_upper == NULL)
	{
		log_error("out of memory");
		return 0;
	}
	for (int i = 0; i < entry_length; i++)
	{
		name_upper[i] = (char)toupper(d->d_name[i]);
	}
	name_upper[entry_length] = '\0';
	bool match = fnmatch(g_file_pattern, name_upper, FNM_PATHNAME) == 0;
	//log_verbose("trying matching filename %s, result = %d", name_upper, match);
	free(name_upper);
	return match;
}

int platform_enumerate_files_begin(const utf8 *pattern)
{
	enumerate_file_info *enumFileInfo;
	wchar_t *wpattern = utf8_to_widechar(pattern);
	int length = min(utf8_length(pattern), MAX_PATH);
	char *npattern = malloc(length+1);
	int converted;
	converted = wcstombs(npattern, wpattern, length);
	npattern[length] = '\0';
	if (converted == MAX_PATH) {
		log_warning("truncated string %s", npattern);
	}
	log_verbose("begin file search, pattern: %s", npattern);

	char *file_name = strrchr(npattern, platform_get_path_separator());
	char *dir_name;
	if (file_name != NULL)
	{
		dir_name = strndup(npattern, file_name - npattern);
		file_name = &file_name[1];
	} else {
		file_name = npattern;
		dir_name = strdup(".");
	}


	int pattern_length = strlen(file_name);
	g_file_pattern = strndup(file_name, pattern_length);
	for (int j = 0; j < pattern_length; j++)
	{
		g_file_pattern[j] = (char)toupper(g_file_pattern[j]);
	}
	log_verbose("looking for file matching %s", g_file_pattern);
	int cnt;
	for (int i = 0; i < countof(_enumerateFileInfoList); i++) {
		enumFileInfo = &_enumerateFileInfoList[i];
		if (!enumFileInfo->active) {
			safe_strncpy(enumFileInfo->pattern, npattern, sizeof(enumFileInfo->pattern));
			cnt = scandir(dir_name, &enumFileInfo->fileListTemp, winfilter, alphasort);
			if (cnt < 0)
			{
				break;
			}
			log_verbose("found %d files matching in dir '%s'", cnt, dir_name);
			enumFileInfo->cnt = cnt;
			enumFileInfo->paths = malloc(cnt * sizeof(char *));
			char **paths = enumFileInfo->paths;
			// 256 is size of dirent.d_name
			const int buf_len = min(MAX_PATH, 256);
			const int dir_name_len = strnlen(dir_name, MAX_PATH);
			char separator[] = {platform_get_path_separator(), 0};
			for (int idx = 0; idx < cnt; idx++)
			{
				struct dirent *d = enumFileInfo->fileListTemp[idx];
				const int entry_len = strnlen(d->d_name, MAX_PATH);
				// 1 for separator, 1 for trailing null
				size_t path_len = sizeof(char) * min(MAX_PATH, entry_len + dir_name_len + 2);
				paths[idx] = malloc(path_len);
				paths[idx][0] = '\0';
				log_verbose("dir_name: %s", dir_name);
				strncat(paths[idx], dir_name, path_len - 2);
				strncat(paths[idx], separator, path_len - strnlen(paths[idx], path_len) - 1);
				strncat(paths[idx], d->d_name, path_len - strnlen(paths[idx], path_len) - 1);
				log_verbose("paths[%d] = %s", idx, paths[idx]);
			}
			enumFileInfo->handle = 0;
			enumFileInfo->active = 1;
			free(dir_name);
			free(g_file_pattern);
			g_file_pattern = NULL;
			free(wpattern);
			free(npattern);
			return i;
		}
	}

	free(dir_name);
	free(g_file_pattern);
	g_file_pattern = NULL;
	free(wpattern);
	free(npattern);
	return -1;
}

bool platform_enumerate_files_next(int handle, file_info *outFileInfo)
{

	if (handle < 0)
	{
		return false;
	}
	enumerate_file_info *enumFileInfo = &_enumerateFileInfoList[handle];
	bool result;

	if (enumFileInfo->handle < enumFileInfo->cnt) {
		result = true;
	} else {
		result = false;
	}

	if (result) {
		int entryIdx = enumFileInfo->handle++;
		struct stat fileInfo;
		log_verbose("trying handle %d", entryIdx);
		char *fileName = enumFileInfo->paths[entryIdx];
		int statRes;
		statRes = stat(fileName, &fileInfo);
		if (statRes == -1) {
			log_error("failed to stat file '%s'! errno = %i", fileName, errno);
			return false;
		}
		outFileInfo->path = basename(fileName);
		outFileInfo->size = fileInfo.st_size;
		outFileInfo->last_modified = fileInfo.st_mtime;
		return true;
	} else {
		return false;
	}
}

void platform_enumerate_files_end(int handle)
{
	if (handle < 0)
	{
		return;
	}
	enumerate_file_info *enumFileInfo = &_enumerateFileInfoList[handle];
	int cnt = enumFileInfo->cnt;
	for (int i = 0; i < cnt; i++) {
		free(enumFileInfo->fileListTemp[i]);
		free(enumFileInfo->paths[i]);
	}
	free(enumFileInfo->fileListTemp);
	free(enumFileInfo->paths);
	// FIXME: this here could have a bug
	enumFileInfo->fileListTemp = NULL;
	enumFileInfo->handle = 0;
	enumFileInfo->active = 0;
}

static int dirfilter(const struct dirent *d)
{
	if (d->d_name[0] == '.') {
		return 0;
	}
#if defined(_DIRENT_HAVE_D_TYPE) || defined(DT_UNKNOWN)
	if (d->d_type == DT_DIR || d->d_type == DT_LNK)
	{
		return 1;
	} else {
		return 0;
	}
#else
#error implement dirfilter!
#endif // defined(_DIRENT_HAVE_D_TYPE) || defined(DT_UNKNOWN)
}

int platform_enumerate_directories_begin(const utf8 *directory)
{
	enumerate_file_info *enumFileInfo;
	wchar_t *wpattern = utf8_to_widechar(directory);
	int length = min(utf8_length(directory), MAX_PATH);
	char *npattern = malloc(length+1);
	int converted;
	converted = wcstombs(npattern, wpattern, length);
	npattern[length - 1] = '\0';
	if (converted == MAX_PATH) {
		log_warning("truncated string %s", npattern);
	}
	log_verbose("begin directory listing, path: %s", npattern);

	// TODO: add some checking for stringness and directoryness

	int cnt;
	for (int i = 0; i < countof(_enumerateFileInfoList); i++) {
		enumFileInfo = &_enumerateFileInfoList[i];
		if (!enumFileInfo->active) {
			safe_strncpy(enumFileInfo->pattern, npattern, length);
			cnt = scandir(npattern, &enumFileInfo->fileListTemp, dirfilter, alphasort);
			if (cnt < 0)
			{
				break;
			}
			log_verbose("found %d files in dir '%s'", cnt, npattern);
			enumFileInfo->cnt = cnt;
			enumFileInfo->paths = malloc(cnt * sizeof(char *));
			char **paths = enumFileInfo->paths;
			// 256 is size of dirent.d_name
			const int buf_len = min(MAX_PATH, 256);
			const int dir_name_len = strnlen(npattern, MAX_PATH);
			char separator[] = {platform_get_path_separator(), 0};
			for (int idx = 0; idx < cnt; idx++)
			{
				struct dirent *d = enumFileInfo->fileListTemp[idx];
				const int entry_len = strnlen(d->d_name, MAX_PATH);
				// 1 for separator, 1 for trailing null
				size_t path_len = sizeof(char) * min(MAX_PATH, entry_len + dir_name_len + 2);
				paths[idx] = malloc(path_len);
				paths[idx][0] = '\0';
				log_verbose("dir_name: %s", npattern);
				strncat(paths[idx], npattern, path_len - 2);
				strncat(paths[idx], separator, path_len - strnlen(paths[idx], path_len) - 1);
				strncat(paths[idx], d->d_name, path_len - strnlen(paths[idx], path_len) - 1);
				log_verbose("paths[%d] = %s", idx, paths[idx]);
			}
			enumFileInfo->handle = 0;
			enumFileInfo->active = 1;
			free(wpattern);
			free(npattern);
			return i;
		}
	}

	free(wpattern);
	free(npattern);
	return -1;
}

bool platform_enumerate_directories_next(int handle, utf8 *path)
{
	if (handle < 0)
	{
		return false;
	}

	bool result;
	enumerate_file_info *enumFileInfo = &_enumerateFileInfoList[handle];

	log_verbose("handle = %d", handle);
	if (enumFileInfo->handle < enumFileInfo->cnt) {
		result = true;
	} else {
		result = false;
	}

	if (result) {
		int entryIdx = enumFileInfo->handle++;
		struct stat fileInfo;
		char *fileName = enumFileInfo->paths[entryIdx];
		int statRes;
		statRes = stat(fileName, &fileInfo);
		if (statRes == -1) {
			log_error("failed to stat file '%s'! errno = %i", fileName, errno);
			return false;
		}
		// so very, very wrong
		safe_strncpy(path, basename(fileName), MAX_PATH);
		strncat(path, "/", MAX_PATH - strlen(path) - 1);
		return true;
	} else {
		return false;
	}
}

void platform_enumerate_directories_end(int handle)
{
	if (handle < 0)
	{
		return;
	}
	enumerate_file_info *enumFileInfo = &_enumerateFileInfoList[handle];
	int cnt = enumFileInfo->cnt;
	for (int i = 0; i < cnt; i++) {
		free(enumFileInfo->fileListTemp[i]);
		free(enumFileInfo->paths[i]);
	}
	free(enumFileInfo->fileListTemp);
	free(enumFileInfo->paths);
	// FIXME: this here could have a bug
	enumFileInfo->fileListTemp = NULL;
	enumFileInfo->handle = 0;
	enumFileInfo->active = 0;
}

int platform_get_drives(){
	/*
	return GetLogicalDrives();
	*/
	STUB();
	return 0;
}

bool platform_file_copy(const utf8 *srcPath, const utf8 *dstPath, bool overwrite)
{
	STUB();
	return 0;
}

bool platform_file_move(const utf8 *srcPath, const utf8 *dstPath)
{
	STUB();
	return 0;
}

bool platform_file_delete(const utf8 *path)
{
	STUB();
	return 0;
}

wchar_t *regular_to_wchar(const char* src)
{
	int len = strnlen(src, MAX_PATH);
	wchar_t *w_buffer = malloc((len + 1) * sizeof(wchar_t));
	mbtowc (NULL, NULL, 0);  /* reset mbtowc */

	int max = len;
	int i = 0;
	while (max > 0)
	{
		int length;
		length = mbtowc(&w_buffer[i], &src[i], max);
		if (length < 1)
		{
			w_buffer[i + 1] = '\0';
			break;
		}
		i += length;
		max -= length;
	}
	return w_buffer;
}

void platform_posix_sub_user_data_path(char *buffer, const char *homedir, const char *separator);

/**
 * Default directory fallback is:
 *   - (command line argument)
 *   - <platform dependent>
 */
void platform_resolve_user_data_path()
{
	const char separator[2] = { platform_get_path_separator(), 0 };

	if (gCustomUserDataPath[0] != 0) {
		realpath(gCustomUserDataPath, _userDataDirectoryPath);

		// Ensure path ends with separator
		int len = strlen(_userDataDirectoryPath);
		if (_userDataDirectoryPath[len - 1] != separator[0]) {
			strncat(_userDataDirectoryPath, separator, MAX_PATH - 1);
		}
		return;
	}

	char buffer[MAX_PATH];
	buffer[0] = '\0';
	log_verbose("buffer = '%s'", buffer);

	const char *homedir = getpwuid(getuid())->pw_dir;
	platform_posix_sub_user_data_path(buffer, homedir, separator);

	log_verbose("OpenRCT2 user data directory = '%s'", buffer);
	int len = strnlen(buffer, MAX_PATH);
	wchar_t *w_buffer = regular_to_wchar(buffer);
	w_buffer[len] = '\0';
	utf8 *path = widechar_to_utf8(w_buffer);
	free(w_buffer);
	safe_strncpy(_userDataDirectoryPath, path, MAX_PATH);
	free(path);
}

void platform_get_openrct_data_path(utf8 *outPath)
{
	safe_strncpy(outPath, _openrctDataDirectoryPath, sizeof(_openrctDataDirectoryPath));
}

void platform_posix_sub_resolve_openrct_data_path(utf8 *out);

/**
 * Default directory fallback is:
 *   - (command line argument)
 *   - <exePath>/data
 *   - <platform dependent>
 */
void platform_resolve_openrct_data_path()
{
	const char separator[2] = { platform_get_path_separator(), 0 };

	if (gCustomOpenrctDataPath[0] != 0) {
		realpath(gCustomOpenrctDataPath, _openrctDataDirectoryPath);

		// Ensure path ends with separator
		int len = strlen(_openrctDataDirectoryPath);
		if (_openrctDataDirectoryPath[len - 1] != separator[0]) {
			strncat(_openrctDataDirectoryPath, separator, MAX_PATH - 1);
		}
		return;
	}

	char buffer[MAX_PATH] = { 0 };
	platform_get_exe_path(buffer);

	strncat(buffer, separator, MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
	strncat(buffer, "data", MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
	log_verbose("Looking for OpenRCT2 data in %s", buffer);
	if (platform_directory_exists(buffer))
	{
		_openrctDataDirectoryPath[0] = '\0';
		safe_strncpy(_openrctDataDirectoryPath, buffer, MAX_PATH);
		log_verbose("Found OpenRCT2 data in %s", _openrctDataDirectoryPath);
		return;
	}

	platform_posix_sub_resolve_openrct_data_path(_openrctDataDirectoryPath);
	log_verbose("Trying to use OpenRCT2 data in %s", _openrctDataDirectoryPath);
}

void platform_get_user_directory(utf8 *outPath, const utf8 *subDirectory)
{
	const char separator[2] = { platform_get_path_separator(), 0 };
	char buffer[MAX_PATH];
	safe_strncpy(buffer, _userDataDirectoryPath, sizeof(buffer));
	if (subDirectory != NULL && subDirectory[0] != 0) {
		log_verbose("adding subDirectory '%s'", subDirectory);
		strncat(buffer, subDirectory, MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
		strncat(buffer, separator, MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
	}
	int len = strnlen(buffer, MAX_PATH);
	wchar_t *w_buffer = regular_to_wchar(buffer);
	w_buffer[len] = '\0';
	utf8 *path = widechar_to_utf8(w_buffer);
	free(w_buffer);
	safe_strncpy(outPath, path, MAX_PATH);
	free(path);
	log_verbose("outPath + subDirectory = '%s'", buffer);
}

uint16 platform_get_locale_language(){
	const char *langString = setlocale(LC_MESSAGES, "");
	if(langString != NULL){
		// The locale has the following form:
		// language[_territory[.codeset]][@modifier] (see https://www.gnu.org/software/libc/manual/html_node/Locale-Names.html)
		char pattern[32]; // longest on my system is 29 with codeset and modifier, so 32 for the pattern should be more than enough
		//strip the codeset and modifier part
		int length = strlen(langString);
		{
			for(int i = 0; i < length; ++i){
				if(langString[i] == '.' || langString[i] == '@'){
					length = i;
					break;
				}
			}
		} //end strip
		strncpy(pattern,langString, length); //copy all until first '.' or '@'
		pattern[length] = '\0';
		//find _ if present
		const char *strip = strchr(pattern, '_');
		if(strip != NULL){
			pattern[strip - pattern] = '?'; // could also use '-', but '?' is more flexible. Maybe LanguagesDescriptors will change. pattern is now "language?territory"
		}

		// Iterate through all available languages
		for(int i = 1; i < LANGUAGE_COUNT; ++i){
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
			for(int i = 1; i < LANGUAGE_COUNT; ++i){
				if(!fnmatch(pattern, LanguagesDescriptors[i].locale, 0)){
					return i;
				}
			}
		}
	}
	return LANGUAGE_ENGLISH_UK;
}

time_t platform_file_get_modified_time(const utf8* path){
	struct stat buf;
	if (stat(path, &buf) == 0) {
		return buf.st_mtime;
	}
	return 100;
}

uint8 platform_get_locale_currency(){
	char *langstring = setlocale(LC_MONETARY, "");

	if(langstring != NULL){
		struct lconv *lc = localeconv();

		//Only works if g_currency_specs contains the actual (local) symbol
		for(int i = 0; i < CURRENCY_END; ++i){
			if(!strcmp(lc->currency_symbol, CurrencyDescriptors[i].symbol_unicode)){
				return i;
			}
		}
		//TODO: can be removed when CurrencyDescriptors contains the actual symbols for won and rubel
		//Won should remain a special case, beacause some (or all?) systems use the full width won sign (e.g. Gentoo)
		if(!strncmp(lc->int_curr_symbol, "KRW", 3)){
			return CURRENCY_WON;
		}
		else if(!strncmp(lc->int_curr_symbol, "RUB", 3)){
			return CURRENCY_ROUBLE;
		}
	}
	//All other currencies are historic
	return CURRENCY_POUNDS;
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

uint8 platform_get_locale_temperature_format(){
	// LC_MEASUREMENT is GNU specific.
	#ifdef LC_MEASUREMENT
	const char *langstring = setlocale(LC_MEASUREMENT, "");
	#else
	const char *langstring = setlocale(LC_ALL, "");
	#endif

	if(langstring != NULL){
		if(!fnmatch("*_US*", langstring, 0) || !fnmatch("*_BS*", langstring, 0) || !fnmatch("*_BZ*", langstring, 0) || !fnmatch("*_PW*", langstring, 0)){
			return TEMPERATURE_FORMAT_F;
		}
	}
	return TEMPERATURE_FORMAT_C;
}

datetime64 platform_get_datetime_now_utc()
{
	const datetime64 epochAsTicks = 621355968000000000;

	struct timeval tv;
	gettimeofday(&tv, NULL);

	uint64 utcEpoch = tv.tv_sec;

	// Epoch starts from: 1970-01-01T00:00:00Z
	// Convert to ticks from 0001-01-01T00:00:00Z
	uint64 utcEpochTicks = (uint64)tv.tv_sec * 10000000ULL + tv.tv_usec * 10;
	datetime64 utcNow = epochAsTicks + utcEpochTicks;
	return utcNow;
}

#endif
