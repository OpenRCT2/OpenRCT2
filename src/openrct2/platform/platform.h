/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../config/Config.h"

#include <string>
#include <time.h>

#ifdef __ANDROID__
#    include <jni.h>
#endif // __ANDROID__

struct TTFFontDescriptor;
namespace OpenRCT2::Ui
{
    struct FileDialogDesc;
}

#ifndef MAX_PATH
#    define MAX_PATH 260
#endif

#define INVALID_HANDLE (-1)

#ifdef _WIN32
#    define PATH_SEPARATOR "\\"
#    define PLATFORM_NEWLINE "\r\n"
#else
#    define PATH_SEPARATOR "/"
#    define PLATFORM_NEWLINE "\n"
#endif

struct resolution_t
{
    int32_t width, height;
};

struct file_info
{
    const char* path;
    uint64_t size;
    uint64_t last_modified;
};

struct rct2_date
{
    uint8_t day;
    uint8_t month;
    int16_t year;
    uint8_t day_of_week;
};

struct rct2_time
{
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

// Platform shared definitions
void platform_toggle_windowed_mode();
void platform_refresh_video(bool recreate_window);

// Platform specific definitions
time_t platform_file_get_modified_time(const utf8* path);
bool platform_ensure_directory_exists(const utf8* path);
bool platform_lock_single_instance();

// Returns the bitmask of the GetLogicalDrives function for windows, 0 for other systems
int32_t platform_get_drives();
uint32_t platform_get_ticks();
void platform_sleep(uint32_t ms);
bool platform_open_common_file_dialog(utf8* outFilename, OpenRCT2::Ui::FileDialogDesc& desc, size_t outSize);
std::string platform_get_rct1_steam_dir();
std::string platform_get_rct2_steam_dir();

datetime64 platform_get_datetime_now_utc();

// Called very early in the program before parsing commandline arguments.
void core_init();

// Windows specific definitions
#ifdef _WIN32
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#    ifndef WIN32_LEAN_AND_MEAN
#        define WIN32_LEAN_AND_MEAN
#    endif
#    include <windows.h>
#    undef CreateDirectory
#    undef CreateWindow
#    undef GetMessage

bool platform_setup_uri_protocol();
// This function cannot be marked as 'static', even though it may seem to be,
// as it requires external linkage, which 'static' prevents
__declspec(dllexport) int32_t StartOpenRCT2(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int32_t nCmdShow);
#endif // _WIN32
