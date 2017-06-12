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

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__)) || defined(__FreeBSD__)

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fnmatch.h>
#include <fts.h>
#include <libgen.h>
#include <locale.h>
#include <pwd.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include "../config/Config.h"
#include "../localisation/date.h"
#include "../localisation/language.h"
#include "../OpenRCT2.h"
#include "../util/util.h"
#include "platform.h"
// The name of the mutex used to prevent multiple instances of the game from running
#define SINGLE_INSTANCE_MUTEX_NAME "openrct2.lock"

#define FILE_BUFFER_SIZE 4096

utf8 _userDataDirectoryPath[MAX_PATH] = { 0 };
utf8 _openrctDataDirectoryPath[MAX_PATH] = { 0 };

void platform_get_date_utc(rct2_date *out_date)
{
    assert(out_date != NULL);
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = gmtime(&rawtime);
    out_date->day = timeinfo->tm_mday;
    out_date->month = timeinfo->tm_mon + 1;
    out_date->year = timeinfo->tm_year + 1900;
    out_date->day_of_week = timeinfo->tm_wday;
}

void platform_get_time_utc(rct2_time *out_time)
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

void platform_get_date_local(rct2_date *out_date)
{
    assert(out_date != NULL);
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    out_date->day = timeinfo->tm_mday;
    out_date->month = timeinfo->tm_mon + 1;
    out_date->year = timeinfo->tm_year + 1900;
    out_date->day_of_week = timeinfo->tm_wday;
}

void platform_get_time_local(rct2_time *out_time)
{
    assert(out_time != NULL);
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    out_time->second = timeinfo->tm_sec;
    out_time->minute = timeinfo->tm_min;
    out_time->hour = timeinfo->tm_hour;
}

static size_t platform_utf8_to_multibyte(const utf8 *path, char *buffer, size_t buffer_size)
{
    wchar_t *wpath = utf8_to_widechar(path);
    setlocale(LC_CTYPE, "UTF-8");
    size_t len = wcstombs(NULL, wpath, 0);
    bool truncated = false;
    if (len > buffer_size - 1) {
        truncated = true;
        len = buffer_size - 1;
    }
    wcstombs(buffer, wpath, len);
    buffer[len] = '\0';
    if (truncated)
        log_warning("truncated string %s", buffer);
    free(wpath);
    return len;
}

bool platform_file_exists(const utf8 *path)
{
    char buffer[MAX_PATH];
    platform_utf8_to_multibyte(path, buffer, MAX_PATH);
    bool exists = access(buffer, F_OK) != -1;
    log_verbose("file '%s' exists = %i", buffer, exists);
    return exists;
}

bool platform_directory_exists(const utf8 *path)
{
    char buffer[MAX_PATH];
    platform_utf8_to_multibyte(path, buffer, MAX_PATH);
    struct stat dirinfo;
    sint32 result = stat(buffer, &dirinfo);
    log_verbose("checking dir %s, result = %d, is_dir = %d", buffer, result, S_ISDIR(dirinfo.st_mode));
    if ((result != 0) || !S_ISDIR(dirinfo.st_mode))
    {
        return false;
    }
    return true;
}

bool platform_original_game_data_exists(const utf8 *path)
{
    char buffer[MAX_PATH];
    platform_utf8_to_multibyte(path, buffer, MAX_PATH);
    char checkPath[MAX_PATH];
    safe_strcpy(checkPath, buffer, MAX_PATH);
    safe_strcat_path(checkPath, "Data", MAX_PATH);
    safe_strcat_path(checkPath, "g1.dat", MAX_PATH);
    return platform_file_exists(checkPath);
}

static mode_t getumask()
{
    mode_t mask = umask(0);
    umask(mask);
    return 0777 & ~mask; // Keep in mind 0777 is octal
}

bool platform_ensure_directory_exists(const utf8 *path)
{
    mode_t mask = getumask();
    char buffer[MAX_PATH];
    platform_utf8_to_multibyte(path, buffer, MAX_PATH);

    log_verbose("Create directory: %s", buffer);
    for (char *p = buffer + 1; *p != '\0'; p++) {
        if (*p == '/') {
            // Temporarily truncate
            *p = '\0';

            log_verbose("mkdir(%s)", buffer);
            if (mkdir(buffer, mask) != 0) {
                if (errno != EEXIST) {
                    return false;
                }
            }

            // Restore truncation
            *p = '/';
        }
    }

    log_verbose("mkdir(%s)", buffer);
    if (mkdir(buffer, mask) != 0) {
        if (errno != EEXIST) {
            return false;
        }
    }

    return true;
}

bool platform_directory_delete(const utf8 *path)
{
    log_verbose("Recursively deleting directory %s", path);

    FTS *ftsp;
    FTSENT *p, *chp;

    // fts_open only accepts non const paths, so we have to take a copy
    char* ourPath = _strdup(path);

    utf8* const patharray[2] = {ourPath, NULL};
    if ((ftsp = fts_open(patharray, FTS_COMFOLLOW | FTS_LOGICAL | FTS_NOCHDIR, NULL)) == NULL) {
        log_error("fts_open returned NULL");
        free(ourPath);
        return false;
    }

    chp = fts_children(ftsp, 0);
    if (chp == NULL) {
        log_verbose("No files to traverse, deleting directory %s", path);
        if (remove(path) != 0)
        {
            log_error("Failed to remove %s, errno = %d", path, errno);
        }
        free(ourPath);
        return true; // No files to traverse
    }

    while ((p = fts_read(ftsp)) != NULL) {
        switch (p->fts_info) {
            case FTS_DP: // Directory postorder, which means
                         // the directory is empty

            case FTS_F:  // File
                if(remove(p->fts_path)) {
                    log_error("Could not remove %s", p->fts_path);
                    fts_close(ftsp);
                    free(ourPath);
                    return false;
                }
                break;
            case FTS_ERR:
                log_error("Error traversing %s", path);
                fts_close(ftsp);
                free(ourPath);
                return false;
        }
    }

    free(ourPath);
    fts_close(ftsp);

    return true;
}

utf8 * platform_get_absolute_path(const utf8 * relative_path, const utf8 * base_path)
{
    utf8 path[MAX_PATH];

    if (base_path != NULL)
    {
        snprintf(path, MAX_PATH, "%s/%s", base_path, relative_path);
    }
    else
    {
        safe_strcpy(path, base_path, MAX_PATH);
    }
    return realpath(path,NULL);
}


bool platform_lock_single_instance()
{
    char pidFilePath[MAX_PATH];

    safe_strcpy(pidFilePath, _userDataDirectoryPath, sizeof(pidFilePath));
    safe_strcat_path(pidFilePath, SINGLE_INSTANCE_MUTEX_NAME, sizeof(pidFilePath));

    // We will never close this file manually. The operating system will
    // take care of that, because flock keeps the lock as long as the
    // file is open and closes it automatically on file close.
    // This is intentional.
    sint32 pidFile = open(pidFilePath, O_CREAT | O_RDWR, 0666);

    if (pidFile == -1) {
        log_warning("Cannot open lock file for writing.");
        return false;
    }
    if (flock(pidFile, LOCK_EX | LOCK_NB) == -1) {
        if (errno == EWOULDBLOCK) {
            log_warning("Another OpenRCT2 session has been found running.");
            return false;
        }
        log_error("flock returned an uncatched errno: %d", errno);
        return false;
    }
    return true;
}

typedef struct enumerate_file_info {
    char active;
    char pattern[MAX_PATH];
    struct dirent **fileListTemp;
    char **paths;
    sint32 cnt;
    sint32 handle;
    void* data;
} enumerate_file_info;
static enumerate_file_info _enumerateFileInfoList[8] = { 0 };

char *g_file_pattern;

static sint32 winfilter(const struct dirent *d)
{
    sint32 entry_length = strnlen(d->d_name, MAX_PATH);
    char *name_upper = malloc(entry_length + 1);
    if (name_upper == NULL)
    {
        log_error("out of memory");
        return 0;
    }
    for (sint32 i = 0; i < entry_length; i++)
    {
        name_upper[i] = (char)toupper(d->d_name[i]);
    }
    name_upper[entry_length] = '\0';
    bool match = fnmatch(g_file_pattern, name_upper, FNM_PATHNAME) == 0;
    //log_verbose("trying matching filename %s, result = %d", name_upper, match);
    free(name_upper);
    return match;
}

sint32 platform_enumerate_files_begin(const utf8 *pattern)
{
    char npattern[MAX_PATH];
    platform_utf8_to_multibyte(pattern, npattern, MAX_PATH);
    enumerate_file_info *enumFileInfo;
    log_verbose("begin file search, pattern: %s", npattern);

    char *file_name = strrchr(npattern, *PATH_SEPARATOR);
    char *dir_name;
    if (file_name != NULL)
    {
        dir_name = strndup(npattern, file_name - npattern);
        file_name = &file_name[1];
    } else {
        file_name = npattern;
        dir_name = strdup(".");
    }


    sint32 pattern_length = strlen(file_name);
    g_file_pattern = strndup(file_name, pattern_length);
    for (sint32 j = 0; j < pattern_length; j++)
    {
        g_file_pattern[j] = (char)toupper(g_file_pattern[j]);
    }
    log_verbose("looking for file matching %s", g_file_pattern);
    sint32 cnt;
    for (sint32 i = 0; i < countof(_enumerateFileInfoList); i++) {
        enumFileInfo = &_enumerateFileInfoList[i];
        if (!enumFileInfo->active) {
            safe_strcpy(enumFileInfo->pattern, npattern, sizeof(enumFileInfo->pattern));
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
            const sint32 dir_name_len = strnlen(dir_name, MAX_PATH);
            for (sint32 idx = 0; idx < cnt; idx++)
            {
                struct dirent *d = enumFileInfo->fileListTemp[idx];
                const sint32 entry_len = strnlen(d->d_name, MAX_PATH);
                // 1 for separator, 1 for trailing null
                size_t path_len = sizeof(char) * min(MAX_PATH, entry_len + dir_name_len + 2);
                paths[idx] = malloc(path_len);
                log_verbose("dir_name: %s", dir_name);
                safe_strcpy(paths[idx], dir_name, path_len);
                safe_strcat_path(paths[idx], d->d_name, path_len);
                log_verbose("paths[%d] = %s", idx, paths[idx]);

// macOS uses decomposed Unicode strings (e.g. an 'e' and a combining accent) in filenames
// This causes problems with the sprite font, as the font only contains precomposed characters
// The block below converts all filename strings to their precomposed form, preventing mojibake
#ifdef __MACOSX__
                utf8* precomp_path = macos_str_decomp_to_precomp(paths[idx]);
                size_t precomp_len = sizeof(utf8) * min(MAX_PATH, strnlen(precomp_path, MAX_PATH) + 2);
                paths[idx] = malloc(precomp_len);
                safe_strcpy(paths[idx], precomp_path, precomp_len);
                log_verbose("macOS decomp-to-precomp fix - paths[%d] = %s", idx, paths[idx]);
#endif
            }
            enumFileInfo->handle = 0;
            enumFileInfo->active = 1;
            free(dir_name);
            free(g_file_pattern);
            g_file_pattern = NULL;
            return i;
        }
    }

    free(dir_name);
    free(g_file_pattern);
    g_file_pattern = NULL;
    return -1;
}

bool platform_enumerate_files_next(sint32 handle, file_info *outFileInfo)
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
        sint32 entryIdx = enumFileInfo->handle++;
        struct stat fileInfo;
        log_verbose("trying handle %d", entryIdx);
        char *fileName = enumFileInfo->paths[entryIdx];
        sint32 statRes;
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

void platform_enumerate_files_end(sint32 handle)
{
    if (handle < 0)
    {
        return;
    }
    enumerate_file_info *enumFileInfo = &_enumerateFileInfoList[handle];
    sint32 cnt = enumFileInfo->cnt;
    for (sint32 i = 0; i < cnt; i++) {
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

static sint32 dirfilter(const struct dirent *d)
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

sint32 platform_enumerate_directories_begin(const utf8 *directory)
{
    char npattern[MAX_PATH];
    sint32 length = platform_utf8_to_multibyte(directory, npattern, MAX_PATH) + 1;
    enumerate_file_info *enumFileInfo;
    log_verbose("begin directory listing, path: %s", npattern);

    // TODO: add some checking for stringness and directoryness

    sint32 cnt;
    for (sint32 i = 0; i < countof(_enumerateFileInfoList); i++) {
        enumFileInfo = &_enumerateFileInfoList[i];
        if (!enumFileInfo->active) {
            safe_strcpy(enumFileInfo->pattern, npattern, length);
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
            const sint32 dir_name_len = strnlen(npattern, MAX_PATH);
            for (sint32 idx = 0; idx < cnt; idx++)
            {
                struct dirent *d = enumFileInfo->fileListTemp[idx];
                const sint32 entry_len = strnlen(d->d_name, MAX_PATH);
                // 1 for separator, 1 for trailing null
                size_t path_len = sizeof(char) * min(MAX_PATH, entry_len + dir_name_len + 2);
                paths[idx] = malloc(path_len);
                log_verbose("dir_name: %s", npattern);
                safe_strcpy(paths[idx], npattern, path_len);
                safe_strcat_path(paths[idx], d->d_name, path_len);
                log_verbose("paths[%d] = %s", idx, paths[idx]);
            }
            enumFileInfo->handle = 0;
            enumFileInfo->active = 1;
            return i;
        }
    }

    return -1;
}

bool platform_enumerate_directories_next(sint32 handle, utf8 *path)
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
        sint32 entryIdx = enumFileInfo->handle++;
        struct stat fileInfo;
        char *fileName = enumFileInfo->paths[entryIdx];
        sint32 statRes;
        statRes = stat(fileName, &fileInfo);
        if (statRes == -1) {
            log_error("failed to stat file '%s'! errno = %i", fileName, errno);
            return false;
        }
        // so very, very wrong
        safe_strcpy(path, basename(fileName), MAX_PATH);
        path_end_with_separator(path, MAX_PATH);
        return true;
    } else {
        return false;
    }
}

void platform_enumerate_directories_end(sint32 handle)
{
    if (handle < 0)
    {
        return;
    }
    enumerate_file_info *enumFileInfo = &_enumerateFileInfoList[handle];
    sint32 cnt = enumFileInfo->cnt;
    for (sint32 i = 0; i < cnt; i++) {
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

sint32 platform_get_drives(){
    // POSIX systems do not know drives. Return 0.
    return 0;
}

bool platform_file_copy(const utf8 *srcPath, const utf8 *dstPath, bool overwrite)
{
    log_verbose("Copying %s to %s", srcPath, dstPath);

    FILE *dstFile;

    if (overwrite) {
        dstFile = fopen(dstPath, "wb");
    } else {
        // Portability note: check your libc's support for "wbx"
        dstFile = fopen(dstPath, "wbx");
    }

    if (dstFile == NULL) {
        if (errno == EEXIST) {
            log_warning("platform_file_copy: Not overwriting %s, because overwrite flag == false", dstPath);
            return false;
        }

        log_error("Could not open destination file %s for copying", dstPath);
        return false;
    }

    // Open both files and check whether they are opened correctly
    FILE *srcFile = fopen(srcPath, "rb");
    if (srcFile == NULL) {
        fclose(dstFile);
        log_error("Could not open source file %s for copying", srcPath);
        return false;
    }

    size_t amount_read = 0;
    size_t file_offset = 0;

    // Copy file in FILE_BUFFER_SIZE-d chunks
    char* buffer = (char*) malloc(FILE_BUFFER_SIZE);
    while ((amount_read = fread(buffer, FILE_BUFFER_SIZE, 1, srcFile))) {
        fwrite(buffer, amount_read, 1, dstFile);
        file_offset += amount_read;
    }

    // Finish the left-over data from file, which may not be a full
    // FILE_BUFFER_SIZE-d chunk.
    fseek(srcFile, file_offset, SEEK_SET);
    amount_read = fread(buffer, 1, FILE_BUFFER_SIZE, srcFile);
    fwrite(buffer, amount_read, 1, dstFile);

    fclose(srcFile);
    fclose(dstFile);
    free(buffer);

    return true;
}

bool platform_file_move(const utf8 *srcPath, const utf8 *dstPath)
{
    return rename(srcPath, dstPath) == 0;
}

bool platform_file_delete(const utf8 *path)
{
    sint32 ret = unlink(path);
    return ret == 0;
}

static wchar_t *regular_to_wchar(const char* src)
{
    sint32 len = strnlen(src, MAX_PATH);
    wchar_t *w_buffer = malloc((len + 1) * sizeof(wchar_t));
    mbtowc (NULL, NULL, 0);  /* reset mbtowc */

    sint32 max = len;
    sint32 i = 0;
    while (max > 0)
    {
        sint32 length;
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

/**
 * Default directory fallback is:
 *   - (command line argument)
 *   - <platform dependent>
 */
void platform_resolve_user_data_path()
{

    if (gCustomUserDataPath[0] != 0) {
        if (!platform_ensure_directory_exists(gCustomUserDataPath)) {
            log_error("Failed to create directory \"%s\", make sure you have permissions.", gCustomUserDataPath);
            return;
        }
        char *path;
        if ((path = realpath(gCustomUserDataPath, NULL)) == NULL) {
            log_error("Could not resolve path \"%s\"", gCustomUserDataPath);
            return;
        }

        safe_strcpy(_userDataDirectoryPath, path, MAX_PATH);
        free(path);

        // Ensure path ends with separator
        path_end_with_separator(_userDataDirectoryPath, MAX_PATH);
        log_verbose("User data path resolved to: %s", _userDataDirectoryPath);
        if (!platform_directory_exists(_userDataDirectoryPath)) {
            log_error("Custom user data directory %s does not exist", _userDataDirectoryPath);
        }
        return;
    }

    char buffer[MAX_PATH];
    log_verbose("buffer = '%s'", buffer);

    const char *homedir = getpwuid(getuid())->pw_dir;
    platform_posix_sub_user_data_path(buffer, MAX_PATH, homedir);

    log_verbose("OpenRCT2 user data directory = '%s'", buffer);
    sint32 len = strnlen(buffer, MAX_PATH);
    wchar_t *w_buffer = regular_to_wchar(buffer);
    w_buffer[len] = '\0';
    utf8 *path = widechar_to_utf8(w_buffer);
    free(w_buffer);
    safe_strcpy(_userDataDirectoryPath, path, MAX_PATH);
    free(path);
    log_verbose("User data path resolved to: %s", _userDataDirectoryPath);
}

void platform_get_openrct_data_path(utf8 *outPath, size_t outSize)
{
    safe_strcpy(outPath, _openrctDataDirectoryPath, outSize);
}

/**
 * Default directory fallback is:
 *   - (command line argument)
 *   - <exePath>/data
 *   - <platform dependent>
 */
void platform_resolve_openrct_data_path()
{
    if (gCustomOpenrctDataPath[0] != 0) {
        // NOTE: second argument to `realpath` is meant to either be NULL or `PATH_MAX`-sized buffer,
        // since our `MAX_PATH` macro is set to some other value, pass NULL to have `realpath` return
        // a `malloc`ed buffer.
        char *resolved_path = realpath(gCustomOpenrctDataPath, NULL);
        if (resolved_path == NULL) {
            log_error("Could not resolve path \"%s\", errno = %d", gCustomOpenrctDataPath, errno);
            return;
        } else {
            safe_strcpy(_openrctDataDirectoryPath, resolved_path, MAX_PATH);
            free(resolved_path);
        }

        path_end_with_separator(_openrctDataDirectoryPath, MAX_PATH);
        return;
    }

    char buffer[MAX_PATH];
    platform_get_exe_path(buffer, sizeof(buffer));

    safe_strcat_path(buffer, "data", MAX_PATH);
    log_verbose("Looking for OpenRCT2 data in %s", buffer);
    if (platform_directory_exists(buffer))
    {
        _openrctDataDirectoryPath[0] = '\0';
        safe_strcpy(_openrctDataDirectoryPath, buffer, MAX_PATH);
        log_verbose("Found OpenRCT2 data in %s", _openrctDataDirectoryPath);
        return;
    }

    platform_posix_sub_resolve_openrct_data_path(_openrctDataDirectoryPath, sizeof(_openrctDataDirectoryPath));
    log_verbose("Trying to use OpenRCT2 data in %s", _openrctDataDirectoryPath);
}

void platform_get_user_directory(utf8 *outPath, const utf8 *subDirectory, size_t outSize)
{
    char buffer[MAX_PATH];
    safe_strcpy(buffer, _userDataDirectoryPath, sizeof(buffer));
    if (subDirectory != NULL && subDirectory[0] != 0) {
        log_verbose("adding subDirectory '%s'", subDirectory);
        safe_strcat_path(buffer, subDirectory, sizeof(buffer));
        path_end_with_separator(buffer, sizeof(buffer));
    }
    sint32 len = strnlen(buffer, MAX_PATH);
    wchar_t *w_buffer = regular_to_wchar(buffer);
    w_buffer[len] = '\0';
    utf8 *path = widechar_to_utf8(w_buffer);
    free(w_buffer);
    safe_strcpy(outPath, path, outSize);
    free(path);
    log_verbose("outPath + subDirectory = '%s'", buffer);
}

time_t platform_file_get_modified_time(const utf8* path){
    struct stat buf;
    if (stat(path, &buf) == 0) {
        return buf.st_mtime;
    }
    return 100;
}

uint8 platform_get_locale_temperature_format(){
// LC_MEASUREMENT is GNU specific.
#ifdef LC_MEASUREMENT
    const char *langstring = setlocale(LC_MEASUREMENT, "");
#else
    const char *langstring = setlocale(LC_ALL, "");
#endif

    if(langstring != NULL){
        if (!fnmatch("*_US*", langstring, 0) ||
            !fnmatch("*_BS*", langstring, 0) ||
            !fnmatch("*_BZ*", langstring, 0) ||
            !fnmatch("*_PW*", langstring, 0))
        {
            return TEMPERATURE_FORMAT_F;
        }
    }
    return TEMPERATURE_FORMAT_C;
}

uint8 platform_get_locale_date_format()
{
    return DATE_FORMAT_DAY_MONTH_YEAR;
}

datetime64 platform_get_datetime_now_utc()
{
    const datetime64 epochAsTicks = 621355968000000000;

    struct timeval tv;
    gettimeofday(&tv, NULL);

    // Epoch starts from: 1970-01-01T00:00:00Z
    // Convert to ticks from 0001-01-01T00:00:00Z
    uint64 utcEpochTicks = (uint64)tv.tv_sec * 10000000ULL + tv.tv_usec * 10;
    datetime64 utcNow = epochAsTicks + utcEpochTicks;
    return utcNow;
}

utf8* platform_get_username() {
    struct passwd* pw = getpwuid(getuid());

    if (pw) {
        return pw->pw_name;
    } else {
        return NULL;
    }
}

#endif
