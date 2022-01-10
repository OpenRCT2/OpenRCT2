/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__)) || defined(__FreeBSD__)

#    include "../OpenRCT2.h"
#    include "../config/Config.h"
#    include "../core/FileSystem.hpp"
#    include "../core/Path.hpp"
#    include "../core/String.hpp"
#    include "../localisation/Date.h"
#    include "../localisation/Language.h"
#    include "../util/Util.h"
#    include "Platform2.h"

#    include <cstring>
#    include <ctype.h>
#    include <dirent.h>
#    include <errno.h>
#    include <fcntl.h>
#    include <fnmatch.h>
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

static utf8 _userDataDirectoryPath[MAX_PATH] = { 0 };

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

time_t platform_file_get_modified_time(const utf8* path)
{
    struct stat buf;
    if (stat(path, &buf) == 0)
    {
        return buf.st_mtime;
    }
    return 100;
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

std::string platform_get_rct1_steam_dir()
{
    return "app_285310" PATH_SEPARATOR "depot_285311";
}

std::string platform_get_rct2_steam_dir()
{
    return "app_285330" PATH_SEPARATOR "depot_285331";
}

#endif
