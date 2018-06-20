/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include <time.h>
#include "../common.h"

struct TTFFontDescriptor;
struct rct2_install_info;

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#define INVALID_HANDLE (-1)

#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#define PLATFORM_NEWLINE "\r\n"
#else
#define PATH_SEPARATOR "/"
#define PLATFORM_NEWLINE "\n"
#endif

struct resolution_t {
    int32_t width, height;
};

struct file_info {
    const char *path;
    uint64_t size;
    uint64_t last_modified;
};

struct rct2_date {
    uint8_t day;
    uint8_t month;
    int16_t year;
    uint8_t day_of_week;
};

struct rct2_time {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

enum FILEDIALOG_TYPE
{
    FD_OPEN,
    FD_SAVE
};

struct file_dialog_desc {
    uint8_t type;
    const utf8 *title;
    const utf8 *initial_directory;
    const utf8 *default_filename;
    struct {
        const utf8 *name;           // E.g. "Image Files"
        const utf8 *pattern;        // E.g. "*.png;*.jpg;*.gif"
    } filters[8];
};

// Platform shared definitions
void platform_update_palette(const uint8_t *colours, int32_t start_index, int32_t num_colours);
void platform_toggle_windowed_mode();
void platform_refresh_video(bool recreate_window);
void platform_get_date_utc(rct2_date *out_date);
void platform_get_time_utc(rct2_time *out_time);
void platform_get_date_local(rct2_date *out_date);
void platform_get_time_local(rct2_time *out_time);

// Platform specific definitions
bool platform_file_exists(const utf8 *path);
bool platform_directory_exists(const utf8 *path);
bool platform_original_game_data_exists(const utf8 *path);
time_t platform_file_get_modified_time(const utf8* path);
bool platform_ensure_directory_exists(const utf8 *path);
bool platform_directory_delete(const utf8 *path);
utf8 * platform_get_absolute_path(const utf8 * relative_path, const utf8 * base_path);
bool platform_lock_single_instance();
bool platform_place_string_on_clipboard(utf8* target);

// Returns the bitmask of the GetLogicalDrives function for windows, 0 for other systems
int32_t platform_get_drives();

bool platform_file_copy(const utf8 *srcPath, const utf8 *dstPath, bool overwrite);
bool platform_file_move(const utf8 *srcPath, const utf8 *dstPath);
bool platform_file_delete(const utf8 *path);
uint32_t platform_get_ticks();
void platform_sleep(uint32_t ms);
void platform_get_openrct_data_path(utf8 *outPath, size_t outSize);
void platform_get_user_directory(utf8 *outPath, const utf8 *subDirectory, size_t outSize);
utf8* platform_get_username();
bool platform_open_common_file_dialog(utf8 *outFilename, file_dialog_desc *desc, size_t outSize);
utf8 *platform_open_directory_browser(const utf8 *title);
uint8_t platform_get_locale_currency();
uint8_t platform_get_currency_value(const char *currencyCode);
uint16_t platform_get_locale_language();
uint8_t platform_get_locale_measurement_format();
uint8_t platform_get_locale_temperature_format();
uint8_t platform_get_locale_date_format();
bool platform_process_is_elevated();
bool platform_get_steam_path(utf8 * outPath, size_t outSize);

#ifndef NO_TTF
bool platform_get_font_path(TTFFontDescriptor *font, utf8 *buffer, size_t size);
#endif // NO_TTF

datetime64 platform_get_datetime_now_utc();

float platform_get_default_scale();

// Called very early in the program before parsing commandline arguments.
void core_init();

// Windows specific definitions
#ifdef _WIN32
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #undef CreateDirectory
    #undef CreateWindow
    #undef GetMessage

    void platform_setup_file_associations();
    void platform_remove_file_associations();
    bool platform_setup_uri_protocol();
    // This function cannot be marked as 'static', even though it may seem to be,
    // as it requires external linkage, which 'static' prevents
    __declspec(dllexport) int32_t StartOpenRCT(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int32_t nCmdShow);
#endif // _WIN32

#if defined(__APPLE__) && defined(__MACH__)
    void macos_disallow_automatic_window_tabbing();
    utf8* macos_str_decomp_to_precomp(utf8 *input);
#endif

#endif
