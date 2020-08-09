/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__)) || defined(__FreeBSD__)

#    include <cstring>
#    include <ctype.h>
#    include <dirent.h>
#    include <errno.h>
#    include <fcntl.h>
#    include <fnmatch.h>
#    if !defined(__EMSCRIPTEN__) && __has_include(<fts.h>)
#        include <fts.h>
#    endif
#    include "../OpenRCT2.h"
#    include "../config/Config.h"
#    include "../core/Path.hpp"
#    include "../core/String.hpp"
#    include "../localisation/Date.h"
#    include "../localisation/Language.h"
#    include "../util/Util.h"
#    include "Platform2.h"

#    include <libgen.h>
#    include <locale.h>
#    include <locale>
#    include <pwd.h>
#    include <stdlib.h>
#    include <sys/file.h>
#    include <sys/stat.h>
#    include <sys/time.h>
#    include <time.h>
#    include <unistd.h>
// The name of the mutex used to prevent multiple instances of the game from running
#    define SINGLE_INSTANCE_MUTEX_NAME "openrct2.lock"

#    define FILE_BUFFER_SIZE 4096

static utf8 _userDataDirectoryPath[MAX_PATH] = { 0 };

void platform_get_date_utc(rct2_date* out_date)
{
    assert(out_date != nullptr);
    time_t rawtime;
    struct tm* timeinfo;
    struct tm buf;
    time(&rawtime);
    timeinfo = gmtime_r(&rawtime, &buf);
    out_date->day = timeinfo->tm_mday;
    out_date->month = timeinfo->tm_mon + 1;
    out_date->year = timeinfo->tm_year + 1900;
    out_date->day_of_week = timeinfo->tm_wday;
}

void platform_get_time_utc(rct2_time* out_time)
{
    assert(out_time != nullptr);
    time_t rawtime;
    struct tm* timeinfo;
    struct tm buf;
    time(&rawtime);
    timeinfo = gmtime_r(&rawtime, &buf);
    out_time->second = timeinfo->tm_sec;
    out_time->minute = timeinfo->tm_min;
    out_time->hour = timeinfo->tm_hour;
}

bool platform_directory_exists(const utf8* path)
{
    struct stat dirinfo;
    int32_t result = stat(path, &dirinfo);
    log_verbose("checking dir %s, result = %d, is_dir = %d", path, result, S_ISDIR(dirinfo.st_mode));
    return result == 0 && S_ISDIR(dirinfo.st_mode);
}

bool platform_original_game_data_exists(const utf8* path)
{
    char checkPath[MAX_PATH];
    safe_strcpy(checkPath, path, MAX_PATH);
    safe_strcat_path(checkPath, "Data", MAX_PATH);
    safe_strcat_path(checkPath, "g1.dat", MAX_PATH);
    return Platform::FileExists(checkPath);
}

// Implement our own version of getumask(), as it is documented being
// "a vaporware GNU extension".
static mode_t openrct2_getumask()
{
    mode_t mask = umask(0);
    umask(mask);
    return 0777 & ~mask; // Keep in mind 0777 is octal
}

bool platform_ensure_directory_exists(const utf8* path)
{
    mode_t mask = openrct2_getumask();
    char buffer[MAX_PATH];
    safe_strcpy(buffer, path, sizeof(buffer));

    log_verbose("Create directory: %s", buffer);
    for (char* p = buffer + 1; *p != '\0'; p++)
    {
        if (*p == '/')
        {
            // Temporarily truncate
            *p = '\0';

            log_verbose("mkdir(%s)", buffer);
            if (mkdir(buffer, mask) != 0)
            {
                if (errno != EEXIST)
                {
                    return false;
                }
            }

            // Restore truncation
            *p = '/';
        }
    }

    log_verbose("mkdir(%s)", buffer);
    if (mkdir(buffer, mask) != 0)
    {
        if (errno != EEXIST)
        {
            return false;
        }
    }

    return true;
}

bool platform_directory_delete(const utf8* path)
{
#    ifdef _FTS_H
    log_verbose("Recursively deleting directory %s", path);

    FTS* ftsp;
    FTSENT *p, *chp;

    // fts_open only accepts non const paths, so we have to take a copy
    char* ourPath = _strdup(path);

    utf8* const patharray[2] = { ourPath, NULL };
    if ((ftsp = fts_open(patharray, FTS_COMFOLLOW | FTS_LOGICAL | FTS_NOCHDIR, NULL)) == nullptr)
    {
        log_error("fts_open returned NULL");
        free(ourPath);
        return false;
    }

    chp = fts_children(ftsp, 0);
    if (chp == nullptr)
    {
        log_verbose("No files to traverse, deleting directory %s", path);
        if (remove(path) != 0)
        {
            log_error("Failed to remove %s, errno = %d", path, errno);
        }
        free(ourPath);
        return true; // No files to traverse
    }

    while ((p = fts_read(ftsp)) != nullptr)
    {
        switch (p->fts_info)
        {
            case FTS_DP: // Directory postorder, which means
                         // the directory is empty

            case FTS_F: // File
                if (remove(p->fts_path))
                {
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

#    else
    log_warning("OpenRCT2 was compiled without fts.h, deleting '%s' not done.", path);
#    endif // _FTS_H
    return true;
}

std::string platform_get_absolute_path(const utf8* relative_path, const utf8* base_path)
{
    std::string result;
    if (relative_path != nullptr)
    {
        std::string pathToResolve;
        if (base_path == nullptr)
        {
            pathToResolve = std::string(relative_path);
        }
        else
        {
            pathToResolve = std::string(base_path) + std::string("/") + relative_path;
        }

        auto realpathResult = realpath(pathToResolve.c_str(), nullptr);
        if (realpathResult != nullptr)
        {
            result = std::string(realpathResult);
            free(realpathResult);
        }
    }
    return result;
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
    int32_t pidFile = open(pidFilePath, O_CREAT | O_RDWR, 0666);

    if (pidFile == -1)
    {
        log_warning("Cannot open lock file for writing.");
        return false;
    }

    struct flock lock;

    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;

    if (fcntl(pidFile, F_SETLK, &lock) == -1)
    {
        if (errno == EWOULDBLOCK)
        {
            log_warning("Another OpenRCT2 session has been found running.");
            return false;
        }
        log_error("flock returned an uncatched errno: %d", errno);
        return false;
    }
    return true;
}

int32_t platform_get_drives()
{
    // POSIX systems do not know drives. Return 0.
    return 0;
}

bool platform_file_copy(const utf8* srcPath, const utf8* dstPath, bool overwrite)
{
    log_verbose("Copying %s to %s", srcPath, dstPath);

    FILE* dstFile;

    if (overwrite)
    {
        dstFile = fopen(dstPath, "wb");
    }
    else
    {
        // Portability note: check your libc's support for "wbx"
        dstFile = fopen(dstPath, "wbx");
    }

    if (dstFile == nullptr)
    {
        if (errno == EEXIST)
        {
            log_warning("platform_file_copy: Not overwriting %s, because overwrite flag == false", dstPath);
            return false;
        }

        log_error("Could not open destination file %s for copying", dstPath);
        return false;
    }

    // Open both files and check whether they are opened correctly
    FILE* srcFile = fopen(srcPath, "rb");
    if (srcFile == nullptr)
    {
        fclose(dstFile);
        log_error("Could not open source file %s for copying", srcPath);
        return false;
    }

    size_t amount_read = 0;
    size_t file_offset = 0;

    // Copy file in FILE_BUFFER_SIZE-d chunks
    char* buffer = static_cast<char*>(malloc(FILE_BUFFER_SIZE));
    while ((amount_read = fread(buffer, FILE_BUFFER_SIZE, 1, srcFile)))
    {
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

bool platform_file_move(const utf8* srcPath, const utf8* dstPath)
{
    return rename(srcPath, dstPath) == 0;
}

bool platform_file_delete(const utf8* path)
{
    int32_t ret = unlink(path);
    return ret == 0;
}

time_t platform_file_get_modified_time(const utf8* path)
{
    struct stat buf;
    if (stat(path, &buf) == 0)
    {
        return buf.st_mtime;
    }
    return 100;
}

TemperatureUnit platform_get_locale_temperature_format()
{
// LC_MEASUREMENT is GNU specific.
#    ifdef LC_MEASUREMENT
    const char* langstring = setlocale(LC_MEASUREMENT, "");
#    else
    const char* langstring = setlocale(LC_ALL, "");
#    endif

    if (langstring != nullptr)
    {
        if (!fnmatch("*_US*", langstring, 0) || !fnmatch("*_BS*", langstring, 0) || !fnmatch("*_BZ*", langstring, 0)
            || !fnmatch("*_PW*", langstring, 0))
        {
            return TemperatureUnit::Fahrenheit;
        }
    }
    return TemperatureUnit::Celsius;
}

uint8_t platform_get_locale_date_format()
{
    const std::time_base::dateorder dateorder = std::use_facet<std::time_get<char>>(std::locale()).date_order();

    switch (dateorder)
    {
        case std::time_base::mdy:
            return DATE_FORMAT_MONTH_DAY_YEAR;

        case std::time_base::ymd:
            return DATE_FORMAT_YEAR_MONTH_DAY;

        case std::time_base::ydm:
            return DATE_FORMAT_YEAR_DAY_MONTH;

        default:
            return DATE_FORMAT_DAY_MONTH_YEAR;
    }
}

datetime64 platform_get_datetime_now_utc()
{
    const datetime64 epochAsTicks = 621355968000000000;

    struct timeval tv;
    gettimeofday(&tv, NULL);

    // Epoch starts from: 1970-01-01T00:00:00Z
    // Convert to ticks from 0001-01-01T00:00:00Z
    uint64_t utcEpochTicks = static_cast<uint64_t>(tv.tv_sec) * 10000000ULL + tv.tv_usec * 10;
    datetime64 utcNow = epochAsTicks + utcEpochTicks;
    return utcNow;
}

std::string platform_get_username()
{
    std::string result;
    auto pw = getpwuid(getuid());
    if (pw != nullptr)
    {
        result = std::string(pw->pw_name);
    }
    return result;
}

bool platform_process_is_elevated()
{
#    ifndef __EMSCRIPTEN__
    return (geteuid() == 0);
#    else
    return false;
#    endif // __EMSCRIPTEN__
}

std::string platform_get_rct1_steam_dir()
{
    return "app_285310" PATH_SEPARATOR "depot_285311";
}

std::string platform_get_rct2_steam_dir()
{
    return "app_285330" PATH_SEPARATOR "depot_285331";
}

#endif
