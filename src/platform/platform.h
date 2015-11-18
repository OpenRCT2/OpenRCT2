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

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#ifdef _WIN32
#define HAVE_MATH_H
#endif // _WIN32

#include <SDL.h>

#include "../common.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#define INVALID_HANDLE -1

typedef struct {
	int width, height;
} resolution;

typedef struct {
	const char *path;
	uint64 size;
	uint64 last_modified;
} file_info;

typedef struct {
	sint16 day;
	sint16 month;
	sint16 year;
	sint16 day_of_week;
} rct2_date;

typedef struct {
	sint16 hour;
	sint16 minute;
	sint16 second;
} rct2_time;

typedef struct {
	int x, y;
	unsigned char left, middle, right, any;
	int wheel;
	int old;
} openrct2_cursor;

enum {
	CURSOR_UP = 0,
	CURSOR_DOWN = 1,
	CURSOR_CHANGED = 2,
	CURSOR_RELEASED = CURSOR_UP | CURSOR_CHANGED,
	CURSOR_PRESSED = CURSOR_DOWN | CURSOR_CHANGED,
};

extern openrct2_cursor gCursorState;
extern const unsigned char *gKeysState;
extern unsigned char *gKeysPressed;
extern unsigned int gLastKeyPressed;
extern int gTextInputCursorPosition;
extern int gTextInputLength;

extern bool gTextInputCompositionActive;
extern utf8 gTextInputComposition[32];
extern int gTextInputCompositionStart;
extern int gTextInputCompositionLength;

extern int gResolutionsAllowAnyAspectRatio;
extern int gNumResolutions;
extern resolution *gResolutions;
extern SDL_Window *gWindow;

extern bool gHardwareDisplay;

extern bool gSteamOverlayActive;

// Platform shared definitions
void platform_update_fullscreen_resolutions();
void platform_get_closest_resolution(int inWidth, int inHeight, int *outWidth, int *outHeight);
void platform_init();
void platform_draw();
void platform_free();
void platform_trigger_resize();
void platform_update_palette(const uint8 *colours, int start_index, int num_colours);
void platform_set_fullscreen_mode(int mode);
void platform_set_cursor(char cursor);
void platform_refresh_video();
void platform_process_messages();
int platform_scancode_to_rct_keycode(int sdl_key);
void platform_start_text_input(utf8 *buffer, int max_length);
void platform_stop_text_input();
void platform_get_date(rct2_date *out_date);
void platform_get_time(rct2_time *out_time);

// Platform specific definitions
char platform_get_path_separator();
bool platform_file_exists(const utf8 *path);
bool platform_directory_exists(const utf8 *path);
bool platform_original_game_data_exists(const utf8 *path);
time_t platform_file_get_modified_time(const utf8* path);
bool platform_ensure_directory_exists(const utf8 *path);
bool platform_directory_delete(const utf8 *path);
bool platform_lock_single_instance();
int platform_enumerate_files_begin(const utf8 *pattern);
bool platform_enumerate_files_next(int handle, file_info *outFileInfo);
void platform_enumerate_files_end(int handle);
int platform_enumerate_directories_begin(const utf8 *directory);
bool platform_enumerate_directories_next(int handle, utf8 *path);
void platform_enumerate_directories_end(int handle);

// Returns the bitmask of the GetLogicalDrives function for windows, 0 for other systems
int platform_get_drives();

bool platform_file_copy(const utf8 *srcPath, const utf8 *dstPath, bool overwrite);
bool platform_file_move(const utf8 *srcPath, const utf8 *dstPath);
bool platform_file_delete(const utf8 *path);
void platform_hide_cursor();
void platform_show_cursor();
void platform_get_cursor_position(int *x, int *y);
void platform_set_cursor_position(int x, int y);
unsigned int platform_get_ticks();
void platform_resolve_user_data_path();
void platform_get_user_directory(utf8 *outPath, const utf8 *subDirectory);
void platform_show_messagebox(utf8 *message);
int platform_open_common_file_dialog(int type, utf8 *title, utf8 *filename, utf8 *filterPattern, utf8 *filterName);
utf8 *platform_open_directory_browser(utf8 *title);
uint8 platform_get_locale_currency();
uint16 platform_get_locale_language();
uint8 platform_get_locale_measurement_format();
uint8 platform_get_locale_temperature_format();

bool platform_check_steam_overlay_attached();

// Windows specific definitions
#ifdef _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>

	int windows_get_registry_install_info(rct2_install_info *installInfo, char *source, char *font, uint8 charset);
	HWND windows_get_window_handle();
#endif // _WIN32

#ifdef __linux__
#include <unistd.h>
#include <limits.h>
#define STUB() log_warning("Function %s at %s:%d is a stub.\n", __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define _strcmpi _stricmp
#define _stricmp(x, y) strcasecmp((x), (y))
#define _strnicmp(x, y, n) strncasecmp((x), (y), (n))
#define _strdup(x) strdup((x))

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define RCT2_ENDIANESS __ORDER_LITTLE_ENDIAN__
#define LOBYTE(w) ((uint8_t)(w))
#define HIBYTE(w) ((uint8_t)(((uint16_t)(w)>>8)&0xFF))
#endif // __BYTE_ORDER__

#ifndef RCT2_ENDIANESS
#error Unknown endianess!
#endif // RCT2_ENDIANESS

#endif // __linux__

#if !(_POSIX_C_SOURCE >= 200809L || _XOPEN_SOURCE >= 700)
	char *strndup(const char *src, size_t size);
#endif // !(POSIX_C_SOURCE >= 200809L || _XOPEN_SOURCE >= 700)

#endif
