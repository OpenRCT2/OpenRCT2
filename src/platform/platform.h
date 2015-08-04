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

#define HAVE_MATH_H

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

// Platform shared definitions
void platform_update_fullscreen_resolutions();
void platform_get_closest_resolution(int inWidth, int inHeight, int *outWidth, int *outHeight);
void platform_init();
void platform_draw();
void platform_free();
void platform_update_palette(char* colours, int start_index, int num_colours);
void platform_set_fullscreen_mode(int mode);
void platform_set_cursor(char cursor);
void platform_refresh_video();
void platform_process_messages();
int platform_scancode_to_rct_keycode(int sdl_key);
void platform_start_text_input(char* buffer, int max_length);
void platform_stop_text_input();
SDL_RWops* platform_sdl_rwfromfile(const char* filename, const char* mode);

// Platform specific definitions
char platform_get_path_separator();
int platform_file_exists(const char *path);
int platform_directory_exists(const char *path);
int platform_original_game_data_exists(const char *path);
time_t platform_file_get_modified_time(char* path);
int platform_ensure_directory_exists(const char *path);
int platform_directory_delete(const char *path);
int platform_lock_single_instance();
int platform_enumerate_files_begin(const char *pattern);
int platform_enumerate_files_next(int handle, file_info *outFileInfo);
void platform_enumerate_files_end(int handle);
int platform_enumerate_directories_begin(const char *directory);
int platform_enumerate_directories_next(int handle, char *path);
void platform_enumerate_directories_end(int handle);

// Returns the bitmask of the GetLogicalDrives function for windows, 0 for other systems
int platform_get_drives();

int platform_file_copy(const char *srcPath, const char *dstPath);
int platform_file_move(const char *srcPath, const char *dstPath);
int platform_file_delete(const char *path);
void platform_hide_cursor();
void platform_show_cursor();
void platform_get_cursor_position(int *x, int *y);
void platform_set_cursor_position(int x, int y);
unsigned int platform_get_ticks();
void platform_get_user_directory(char *outPath, const char *subDirectory);
void platform_show_messagebox(char *message);
int platform_open_common_file_dialog(int type, char *title, char *filename, char *filterPattern, char *filterName);
utf8 *platform_open_directory_browser(utf8 *title);
uint8 platform_get_locale_currency();
uint16 platform_get_locale_language();
uint8 platform_get_locale_measurement_format();
uint8 platform_get_locale_temperature_format();

// Windows specific definitions
#ifdef _WIN32
	// Defining WIN32_LEAN_AND_MEAN breaks dsound.h in audio.h (uncomment when dsound is finally removed)
	// #ifndef WIN32_LEAN_AND_MEAN
	// 	#define WIN32_LEAN_AND_MEAN
	// #endif
	#include <windows.h>

	int windows_get_registry_install_info(rct2_install_info *installInfo, char *source, char *font, uint8 charset);
	HWND windows_get_window_handle();
#endif

#endif