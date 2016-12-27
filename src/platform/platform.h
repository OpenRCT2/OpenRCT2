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

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include "../common.h"

#ifdef __WINDOWS__
#include "../rct2.h"
	#ifndef HAVE_MATH_H
		#define HAVE_MATH_H
	#endif
#endif // __WINDOWS__

#include <SDL.h>

#include "../core/textinputbuffer.h"
#include "../drawing/font.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifdef __MACOSX__
#define KEYBOARD_PRIMARY_MODIFIER KMOD_GUI
#else
#define KEYBOARD_PRIMARY_MODIFIER KMOD_CTRL
#endif

#define INVALID_HANDLE -1

#define TOUCH_DOUBLE_TIMEOUT 300

#ifdef __WINDOWS__
#define PATH_SEPARATOR "\\"
#define PLATFORM_NEWLINE "\r\n"
#else
#define PATH_SEPARATOR "/"
#define PLATFORM_NEWLINE "\n"
#endif

#define SHIFT 0x100
#define CTRL 0x200
#define ALT 0x400
#define CMD 0x800
#ifdef __MACOSX__
	#define PLATFORM_MODIFIER CMD
#else
	#define PLATFORM_MODIFIER CTRL
#endif

typedef struct resolution {
	int width, height;
} resolution;

typedef struct file_info {
	const char *path;
	uint64 size;
	uint64 last_modified;
} file_info;

typedef struct rct2_date {
	sint16 day;
	sint16 month;
	sint16 year;
	sint16 day_of_week;
} rct2_date;

typedef struct rct2_time {
	sint16 hour;
	sint16 minute;
	sint16 second;
} rct2_time;

typedef struct openrct2_cursor {
	int x, y;
	unsigned char left, middle, right, any;
	int wheel;
	int old;
	bool touch, touchIsDouble;
	unsigned int touchDownTimestamp;
} openrct2_cursor;

enum {
	CURSOR_UP = 0,
	CURSOR_DOWN = 1,
	CURSOR_CHANGED = 2,
	CURSOR_RELEASED = CURSOR_UP | CURSOR_CHANGED,
	CURSOR_PRESSED = CURSOR_DOWN | CURSOR_CHANGED,
};

typedef enum {FD_OPEN, FD_SAVE} filedialog_type;

typedef struct file_dialog_desc {
	uint8 type;
	const utf8 *title;
	const utf8 *initial_directory;
	const utf8 *default_filename;
	struct {
		const utf8 *name;			// E.g. "Image Files"
		const utf8 *pattern;		// E.g. "*.png;*.jpg;*.gif"
	} filters[8];
} file_dialog_desc;

extern openrct2_cursor gCursorState;
extern const unsigned char *gKeysState;
extern unsigned char *gKeysPressed;
extern unsigned int gLastKeyPressed;

extern textinputbuffer gTextInput;
extern bool gTextInputCompositionActive;
extern utf8 gTextInputComposition[32];
extern int gTextInputCompositionStart;
extern int gTextInputCompositionLength;

extern int gResolutionsAllowAnyAspectRatio;
extern int gNumResolutions;
extern resolution *gResolutions;
extern SDL_Window *gWindow;

extern SDL_Color gPalette[256];

extern bool gSteamOverlayActive;

// Platform shared definitions
void platform_update_fullscreen_resolutions();
void platform_get_closest_resolution(int inWidth, int inHeight, int *outWidth, int *outHeight);
void platform_init();
void platform_draw();
void platform_draw_require_end();
void platform_free();
void platform_trigger_resize();
void platform_update_palette(const uint8 *colours, int start_index, int num_colours);
void platform_set_fullscreen_mode(int mode);
void platform_toggle_windowed_mode();
void platform_set_cursor(uint8 cursor);
void platform_refresh_video();
void platform_process_messages();
int platform_scancode_to_rct_keycode(int sdl_key);
void platform_start_text_input(utf8 *buffer, int max_length);
void platform_stop_text_input();
bool platform_is_input_active();
void platform_get_date_utc(rct2_date *out_date);
void platform_get_time_utc(rct2_time *out_time);
void platform_get_date_local(rct2_date *out_date);
void platform_get_time_local(rct2_time *out_time);

// Platform specific definitions
void platform_get_exe_path(utf8 *outPath, size_t outSize);
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
void platform_init_window_icon();

// Returns the bitmask of the GetLogicalDrives function for windows, 0 for other systems
int platform_get_drives();

bool platform_file_copy(const utf8 *srcPath, const utf8 *dstPath, bool overwrite);
bool platform_file_move(const utf8 *srcPath, const utf8 *dstPath);
bool platform_file_delete(const utf8 *path);
void platform_hide_cursor();
void platform_show_cursor();
void platform_get_cursor_position(int *x, int *y);
void platform_get_cursor_position_scaled(int *x, int *y);
void platform_set_cursor_position(int x, int y);
unsigned int platform_get_ticks();
void platform_resolve_user_data_path();
void platform_resolve_openrct_data_path();
void platform_get_openrct_data_path(utf8 *outPath, size_t outSize);
void platform_get_user_directory(utf8 *outPath, const utf8 *subDirectory, size_t outSize);
utf8* platform_get_username();
void platform_show_messagebox(utf8 *message);
bool platform_open_common_file_dialog(utf8 *outFilename, file_dialog_desc *desc, size_t outSize);
utf8 *platform_open_directory_browser(utf8 *title);
uint8 platform_get_locale_currency();
uint8 platform_get_currency_value(const char *currencyCode);
uint16 platform_get_locale_language();
uint8 platform_get_locale_measurement_format();
uint8 platform_get_locale_temperature_format();
bool platform_get_font_path(TTFFontDescriptor *font, utf8 *buffer, size_t size);

bool platform_check_steam_overlay_attached();

datetime64 platform_get_datetime_now_utc();

// Called very early in the program before parsing commandline arguments.
void core_init();

// Windows specific definitions
#ifdef __WINDOWS__
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
	#undef GetMessage

	void platform_windows_open_console();
	void platform_windows_close_console();
	int windows_get_registry_install_info(rct2_install_info *installInfo, char *source, char *font, uint8 charset);
	HWND windows_get_window_handle();
	void platform_setup_file_associations();
	void platform_remove_file_associations();
	// This function cannot be marked as 'static', even though it may seem to be,
	// as it requires external linkage, which 'static' prevents
	__declspec(dllexport) int StartOpenRCT(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
#endif // __WINDOWS__

#if defined(__LINUX__) || defined(__MACOSX__)
	void platform_posix_sub_user_data_path(char *buffer, size_t size, const char *homedir);
	void platform_posix_sub_resolve_openrct_data_path(utf8 *out, size_t size);
#endif

#endif
