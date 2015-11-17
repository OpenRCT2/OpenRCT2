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

#include "addresses.h"
#include "audio/audio.h"
#include "audio/mixer.h"
#include "cmdline.h"
#include "config.h"
#include "editor.h"
#include "game.h"
#include "hook.h"
#include "interface/chat.h"
#include "interface/window.h"
#include "interface/viewport.h"
#include "localisation/localisation.h"
#include "network/http.h"
#include "network/network.h"
#include "openrct2.h"
#include "platform/platform.h"
#include "ride/ride.h"
#include "title.h"
#include "util/sawyercoding.h"
#include "util/util.h"
#include "world/mapgen.h"

#ifdef __linux__
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif // __linux__

int gOpenRCT2StartupAction = STARTUP_ACTION_TITLE;
utf8 gOpenRCT2StartupActionPath[512] = { 0 };
utf8 gExePath[MAX_PATH];
utf8 gCustomUserDataPath[MAX_PATH] = { 0 };

// This should probably be changed later and allow a custom selection of things to initialise like SDL_INIT
bool gOpenRCT2Headless = false;

bool gOpenRCT2ShowChangelog;

#if defined(__linux__)
void *gDataSegment;
void *gTextSegment;
int gExeFd;
#endif // defined(__linux__)

/** If set, will end the OpenRCT2 game loop. Intentially private to this module so that the flag can not be set back to 0. */
int _finished;

// Used for object movement tweening
static struct { sint16 x, y, z; } _spritelocations1[MAX_SPRITES], _spritelocations2[MAX_SPRITES];

static void openrct2_loop();
static bool openrct2_setup_rct2_segment();
static bool openrct2_release_rct2_segment();
static void openrct2_setup_rct2_hooks();

void openrct2_write_full_version_info(utf8 *buffer, size_t bufferSize)
{
	utf8 *ch = buffer;

	// Name and version
	strcpy(ch, OPENRCT2_NAME);
	strcat(buffer, ", v");
	strcat(buffer, OPENRCT2_VERSION);

	// Build information
	if (!str_is_null_or_empty(OPENRCT2_BRANCH)) {
		sprintf(strchr(buffer, 0), "-%s", OPENRCT2_BRANCH);
	}
	if (!str_is_null_or_empty(OPENRCT2_BUILD_NUMBER)) {
		sprintf(strchr(buffer, 0), " build %s", OPENRCT2_BUILD_NUMBER);
	}
	if (!str_is_null_or_empty(OPENRCT2_COMMIT_SHA1_SHORT)) {
		sprintf(strchr(buffer, 0), " (%s)", OPENRCT2_COMMIT_SHA1_SHORT);
	}
	if (!str_is_null_or_empty(OPENRCT2_BUILD_SERVER)) {
		sprintf(strchr(buffer, 0), " provided by %s", OPENRCT2_BUILD_SERVER);
	}
}

static void openrct2_copy_files_over(const utf8 *originalDirectory, const utf8 *newDirectory, const utf8 *extension)
{
	utf8 *ch, filter[MAX_PATH], oldPath[MAX_PATH], newPath[MAX_PATH];
	int fileEnumHandle;
	file_info fileInfo;

	if (!platform_ensure_directory_exists(newDirectory)) {
		log_error("Could not create directory %s.", newDirectory);
		return;
	}

	// Create filter path
	safe_strncpy(filter, originalDirectory, MAX_PATH);
	ch = strchr(filter, '*');
	if (ch != NULL)
		*ch = 0;
	strcat(filter, "*");
	strcat(filter, extension);

	fileEnumHandle = platform_enumerate_files_begin(filter);
	while (platform_enumerate_files_next(fileEnumHandle, &fileInfo)) {
		safe_strncpy(newPath, newDirectory, MAX_PATH);
		strcat(newPath, fileInfo.path);

		safe_strncpy(oldPath, originalDirectory, MAX_PATH);
		ch = strchr(oldPath, '*');
		if (ch != NULL)
			*ch = 0;
		strcat(oldPath, fileInfo.path);

		if (!platform_file_exists(newPath))
			platform_file_copy(oldPath, newPath, false);
	}
	platform_enumerate_files_end(fileEnumHandle);

	fileEnumHandle = platform_enumerate_directories_begin(originalDirectory);
	while (platform_enumerate_directories_next(fileEnumHandle, filter)) {
		safe_strncpy(newPath, newDirectory, MAX_PATH);
		strcat(newPath, filter);

		safe_strncpy(oldPath, originalDirectory, MAX_PATH);
		ch = strchr(oldPath, '*');
		if (ch != NULL)
			*ch = 0;
		strcat(oldPath, filter);

		if (!platform_ensure_directory_exists(newPath)) {
			log_error("Could not create directory %s.", newPath);
			return;
		}
		openrct2_copy_files_over(oldPath, newPath, extension);
	}
	platform_enumerate_directories_end(fileEnumHandle);
}

// TODO move to platform
static void openrct2_set_exe_path()
{
#ifdef _WIN32
	wchar_t exePath[MAX_PATH];
	wchar_t tempPath[MAX_PATH];
	wchar_t *exeDelimiter;
	int exeDelimiterIndex;

	GetModuleFileNameW(NULL, exePath, MAX_PATH);
	exeDelimiter = wcsrchr(exePath, platform_get_path_separator());
	exeDelimiterIndex = (int)(exeDelimiter - exePath);
	lstrcpynW(tempPath, exePath, exeDelimiterIndex + 1);
	tempPath[exeDelimiterIndex] = L'\0';
	_wfullpath(exePath, tempPath, MAX_PATH);
	WideCharToMultiByte(CP_UTF8, 0, exePath, countof(exePath), gExePath, countof(gExePath), NULL, NULL);
#else
	char exePath[MAX_PATH];
	ssize_t bytesRead;
	bytesRead = readlink("/proc/self/exe", exePath, MAX_PATH);
	if (bytesRead == -1) {
		log_fatal("failed to read /proc/self/exe");
	}
	exePath[bytesRead] = '\0';
	log_verbose("######################################## Setting exe path to %s", exePath);
	char *exeDelimiter = strrchr(exePath, platform_get_path_separator());
	if (exeDelimiter == NULL)
	{
		log_error("should never happen here");
		gExePath[0] = '\0';
		return;
	}
	int exeDelimiterIndex = (int)(exeDelimiter - exePath);

	safe_strncpy(gExePath, exePath, exeDelimiterIndex + 1);
	gExePath[exeDelimiterIndex] = '\0';
#endif // _WIN32
}

/**
 * Copy saved games and landscapes to user directory
 */
static void openrct2_copy_original_user_files_over()
{
	utf8 path[MAX_PATH];

	platform_get_user_directory(path, "save");
	openrct2_copy_files_over((utf8*)RCT2_ADDRESS_SAVED_GAMES_PATH, path, ".sv6");

	platform_get_user_directory(path, "landscape");
	openrct2_copy_files_over((utf8*)RCT2_ADDRESS_LANDSCAPES_PATH, path, ".sc6");
}

bool openrct2_initialise()
{
	utf8 userPath[MAX_PATH];

	platform_resolve_user_data_path();
	platform_get_user_directory(userPath, NULL);
	if (!platform_ensure_directory_exists(userPath)) {
		log_fatal("Could not create user directory (do you have write access to your documents folder?)");
		return false;
	}

	if (!openrct2_setup_rct2_segment()) {
		log_fatal("Unable to load RCT2 data sector");
		return false;
	}

	openrct2_set_exe_path();

	config_set_defaults();
	if (!config_open_default()) {
		if (!config_find_or_browse_install_directory()) {
			log_fatal("An RCT2 install directory must be specified!");
			return false;
		}
	}

	gOpenRCT2ShowChangelog = true;
	if (gConfigGeneral.last_run_version != NULL && (strcmp(gConfigGeneral.last_run_version, OPENRCT2_VERSION) == 0))
		gOpenRCT2ShowChangelog = false;
	gConfigGeneral.last_run_version = OPENRCT2_VERSION;
	config_save_default();

	// TODO add configuration option to allow multiple instances
	// if (!gOpenRCT2Headless && !platform_lock_single_instance()) {
	// 	log_fatal("OpenRCT2 is already running.");
	// 	return false;
	// }

	get_system_info();
	if (!gOpenRCT2Headless) {
		audio_init();
		audio_populate_devices();
	}
	if (!language_open(gConfigGeneral.language))
	{
		log_fatal("Failed to open language, exiting.");
		return false;
	}
	http_init();

	themes_set_default();
	themes_load_presets();
	title_sequences_set_default();
	title_sequences_load_presets();

	openrct2_setup_rct2_hooks();

	if (!rct2_init())
		return false;

	chat_init();

	openrct2_copy_original_user_files_over();

	// TODO move to audio initialise function
	if (str_is_null_or_empty(gConfigSound.device)) {
		Mixer_Init(NULL);
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, uint32) = 0;
	} else {
		Mixer_Init(gConfigSound.device);
		for (int i = 0; i < gAudioDeviceCount; i++) {
			if (strcmp(gAudioDevices[i].name, gConfigSound.device) == 0) {
				RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, uint32) = i;
			}
		}
	}

	return true;
}

/**
 * Launches the game, after command line arguments have been parsed and processed.
 */
void openrct2_launch()
{
	if (openrct2_initialise()) {
		RCT2_GLOBAL(RCT2_ADDRESS_RUN_INTRO_TICK_PART, uint8) = 0;
		if((gOpenRCT2StartupAction == STARTUP_ACTION_TITLE) && gConfigGeneral.play_intro)
			gOpenRCT2StartupAction = STARTUP_ACTION_INTRO;

		switch (gOpenRCT2StartupAction) {
		case STARTUP_ACTION_INTRO:
			RCT2_GLOBAL(RCT2_ADDRESS_RUN_INTRO_TICK_PART, uint8) = 1;
			break;
		case STARTUP_ACTION_TITLE:
			RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_TITLE_DEMO;
			break;
		case STARTUP_ACTION_OPEN:
			assert(gOpenRCT2StartupActionPath != NULL);
			if (rct2_open_file(gOpenRCT2StartupActionPath) == 0)
				break;

			RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_PLAYING;

#ifndef DISABLE_NETWORK
			if (gNetworkStart == NETWORK_MODE_SERVER) {
				network_begin_server(gNetworkStartPort);
			}
#endif // DISABLE_NETWORK
			break;
		case STARTUP_ACTION_EDIT:
			if (strlen(gOpenRCT2StartupActionPath) == 0) {
				editor_load();
			} else {
				editor_load_landscape(gOpenRCT2StartupActionPath);
			}
			break;
		}

#ifndef DISABLE_NETWORK
		if (gNetworkStart == NETWORK_MODE_CLIENT) {
			network_begin_client(gNetworkStartHost, gNetworkStartPort);
		}
#endif // DISABLE_NETWORK

		openrct2_loop();
	}
	openrct2_dispose();

	// HACK Some threads are still running which causes the game to not terminate. Investigation required!
	exit(gExitCode);
}

void openrct2_dispose()
{
	network_close();
	http_dispose();
	language_close_all();
	openrct2_release_rct2_segment();
	platform_free();
}

/**
 * Determines whether its worth tweening a sprite or not when frame smoothing is on.
 */
static bool sprite_should_tween(rct_sprite *sprite)
{
	if (sprite->unknown.linked_list_type_offset == SPRITE_LINKEDLIST_OFFSET_VEHICLE)
		return true;
	if (sprite->unknown.linked_list_type_offset == SPRITE_LINKEDLIST_OFFSET_PEEP)
		return true;
	if (sprite->unknown.linked_list_type_offset == SPRITE_LINKEDLIST_OFFSET_UNKNOWN)
		return true;

	return false;
}

/**
 * Run the main game loop until the finished flag is set at 40fps (25ms interval).
 */
static void openrct2_loop()
{
	uint32 currentTick, ticksElapsed, lastTick = 0;
	static uint32 uncapTick = 0;
	static int fps = 0;
	static uint32 secondTick = 0;

	log_verbose("begin openrct2 loop");

	_finished = 0;
	do {
		if (gConfigGeneral.uncap_fps && gGameSpeed <= 4) {
			currentTick = SDL_GetTicks();
			if (uncapTick == 0) {
				// Reset sprite locations
				uncapTick = SDL_GetTicks();
				openrct2_reset_object_tween_locations();
			}

			// Limit number of updates per loop (any long pauses or debugging can make this update for a very long time)
			if (currentTick - uncapTick > 25 * 60) {
				uncapTick = currentTick - 25 - 1;
			}

			platform_process_messages();

			while (uncapTick <= currentTick && currentTick - uncapTick > 25) {
				// Get the original position of each sprite
				for (uint16 i = 0; i < MAX_SPRITES; i++) {
					_spritelocations1[i].x = g_sprite_list[i].unknown.x;
					_spritelocations1[i].y = g_sprite_list[i].unknown.y;
					_spritelocations1[i].z = g_sprite_list[i].unknown.z;
				}

				// Update the game so the sprite positions update
				rct2_update();

				// Get the next position of each sprite
				for (uint16 i = 0; i < MAX_SPRITES; i++) {
					_spritelocations2[i].x = g_sprite_list[i].unknown.x;
					_spritelocations2[i].y = g_sprite_list[i].unknown.y;
					_spritelocations2[i].z = g_sprite_list[i].unknown.z;
				}

				uncapTick += 25;
			}

			// Tween the position of each sprite from the last position to the new position based on the time between the last
			// tick and the next tick.
			float nudge = 1 - ((float)(currentTick - uncapTick) / 25);
			for (uint16 i = 0; i < MAX_SPRITES; i++) {
				if (!sprite_should_tween(&g_sprite_list[i]))
					continue;

				sprite_move(
					_spritelocations2[i].x + (sint16)((_spritelocations1[i].x - _spritelocations2[i].x) * nudge),
					_spritelocations2[i].y + (sint16)((_spritelocations1[i].y - _spritelocations2[i].y) * nudge),
					_spritelocations2[i].z + (sint16)((_spritelocations1[i].z - _spritelocations2[i].z) * nudge),
					&g_sprite_list[i]
				);
				invalidate_sprite_2(&g_sprite_list[i]);
			}

			if ((SDL_GetWindowFlags(gWindow) & (SDL_WINDOW_MINIMIZED | SDL_WINDOW_HIDDEN)) == 0) {
				rct2_draw();
				platform_draw();
			}

			fps++;
			if (SDL_GetTicks() - secondTick >= 1000) {
				fps = 0;
				secondTick = SDL_GetTicks();
			}

			// Restore the real positions of the sprites so they aren't left at the mid-tween positions
			for (uint16 i = 0; i < MAX_SPRITES; i++) {
				if (!sprite_should_tween(&g_sprite_list[i]))
					continue;

				invalidate_sprite_2(&g_sprite_list[i]);
				sprite_move(_spritelocations2[i].x, _spritelocations2[i].y, _spritelocations2[i].z, &g_sprite_list[i]);
			}
			network_update();
		} else {
			uncapTick = 0;
			currentTick = SDL_GetTicks();
			ticksElapsed = currentTick - lastTick;
			if (ticksElapsed < 25) {
				if (ticksElapsed < 15)
					SDL_Delay(15 - ticksElapsed);
				continue;
			}

			lastTick = currentTick;

			platform_process_messages();

			rct2_update();

			if ((SDL_GetWindowFlags(gWindow) & (SDL_WINDOW_MINIMIZED | SDL_WINDOW_HIDDEN)) == 0) {
				rct2_draw();
				platform_draw();
			}
		}
	} while (!_finished);
}

/**
 * Causes the OpenRCT2 game loop to finish.
 */
void openrct2_finish()
{
	_finished = 1;
}

void openrct2_reset_object_tween_locations()
{
	for (uint16 i = 0; i < MAX_SPRITES; i++) {
		_spritelocations1[i].x = _spritelocations2[i].x = g_sprite_list[i].unknown.x;
		_spritelocations1[i].y = _spritelocations2[i].y = g_sprite_list[i].unknown.y;
		_spritelocations1[i].z = _spritelocations2[i].z = g_sprite_list[i].unknown.z;
	}
}

/**
 * Loads RCT2's data model and remaps the addresses.
 * @returns true if the data integrity check succeeded, otherwise false.
 */
static bool openrct2_setup_rct2_segment()
{
	// Linux will run OpenRCT2 as a native application and then load in the Windows PE, mapping the appropriate addresses as
	// necessary. Windows does not need to do this as OpenRCT2 runs as a DLL loaded from the Windows PE.
#ifdef __linux__
	#define RDATA_OFFSET 0x004A4000
	#define DATASEG_OFFSET 0x005E2000

	const char *exepath = "openrct2.exe";
	gExeFd = open(exepath, O_RDONLY);
	if (gExeFd < 0) {
		log_fatal("failed to open %s, errno = %d", exepath, errno);
		exit(1);
	}

	// Using PE-bear I was able to figure out all the needed addresses to be filled.
	// There are three sections to be loaded: .rdata, .data and .text, plus another
	// one to be mapped: DATASEG.
	// Out of the three, two can simply be mmapped into memory, while the third one,
	// .data has a virtual size which is much completely different to its file size
	// (even when taking page-alignment into consideration)
	//
	// The sections are as follows (dump from gdb)
	// [0]     0x401000->0x6f7000 at 0x00001000: .text ALLOC LOAD READONLY CODE HAS_CONTENTS
	// [1]     0x6f7000->0x8a325d at 0x002f7000: CODESEG ALLOC LOAD READONLY CODE HAS_CONTENTS
	// [2]     0x8a4000->0x9a5894 at 0x004a4000: .rdata ALLOC LOAD DATA HAS_CONTENTS
	// [3]     0x9a6000->0x9e2000 at 0x005a6000: .data ALLOC LOAD DATA HAS_CONTENTS
	// [4]     0x1428000->0x14282bc at 0x005e2000: DATASEG ALLOC LOAD DATA HAS_CONTENTS
	// [5]     0x1429000->0x1452000 at 0x005e3000: .cms_t ALLOC LOAD READONLY CODE HAS_CONTENTS
	// [6]     0x1452000->0x14aaf3e at 0x0060c000: .cms_d ALLOC LOAD DATA HAS_CONTENTS
	// [7]     0x14ab000->0x14ac58a at 0x00665000: .idata ALLOC LOAD READONLY DATA HAS_CONTENTS
	// [8]     0x14ad000->0x14b512f at 0x00667000: .rsrc ALLOC LOAD DATA HAS_CONTENTS
	//
	// .data section, however, has virtual size of 0xA81C3C, and so
	// 0x9a6000 + 0xA81C3C = 0x1427C3C, which after alignment to page size becomes
	// 0x1428000, which can be seen as next section, DATASEG
	//
	// Since mmap does not provide a way to create a mapping with virtual size,
	// I resorted to creating a one large map for data and memcpy'ing data where
	// required.
	// Another section is needed for .text, as it requires PROT_EXEC flag.

	// TODO: UGLY, UGLY HACK!
	off_t file_size = 6750208;

	int len = 0x01429000 - 0x8a4000; // 0xB85000, 12079104 bytes or around 11.5MB
	// section: rw data
	gDataSegment = mmap((void *)0x8a4000, len, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_ANONYMOUS | MAP_SHARED, 0, 0);
	if (gDataSegment != (void *)0x8a4000) {
		log_fatal("mmap failed to get required offset for data segment! got %p, expected %p, errno = %d", gDataSegment, (void *)(0x8a4000), errno);
		exit(1);
	}

	len = 0x004A3000;
	// section: text
	gTextSegment = mmap((void *)(0x401000), len, PROT_EXEC | PROT_WRITE | PROT_READ, MAP_FIXED | MAP_PRIVATE, gExeFd, 0x1000);
	if (gTextSegment != (void *)(0x401000))
	{
		log_fatal("mmap failed to get required offset for text segment! got %p, expected %p, errno = %d", gTextSegment, (void *)(0x401000), errno);
		exit(1);
	}

	void *fbase = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, gExeFd, 0);
	int err = errno;
	log_warning("mmapped file to %p", fbase);
	if (fbase == MAP_FAILED)
	{
		log_fatal("mmap failed to get required offset! got %p, errno = %d", fbase, err);
		exit(1);
	}
	// .rdata and real part of .data
	// 0x9e2000 - 0x8a4000 = 0x13e000
	memcpy(gDataSegment, fbase + RDATA_OFFSET, 0x13e000);
	// 0x8a4000 + 0xb84000 = 0x1428000 aka DATASEG
	memcpy(gDataSegment + 0xB84000, fbase + DATASEG_OFFSET, 0x1000);
	err = munmap(fbase, file_size);
	if (err != 0)
	{
		err = errno;
		log_error("Failed to unmap file! errno = %d", err);
	}
#endif // __linux__

	// Check that the expected data is at various addresses.
	// Start at 0x9a6000, which is start of .data, to skip the region containing addresses to DLL
	// calls, which can be changed by windows/wine loader.
	const uint32 c1 = sawyercoding_calculate_checksum((void *)0x009A6000, 0x009E0000 - 0x009A6000);
	const uint32 c2 = sawyercoding_calculate_checksum((void *)0x01428000, 0x014282BC - 0x01428000);
	const uint32 exp_c1 = 10114815;
	const uint32 exp_c2 = 23564;
	if (c1 != exp_c1 || c2 != exp_c2) {
		log_warning("c1 = %u, expected %u, match %d", c1, exp_c1, c1 == exp_c1);
		log_warning("c2 = %u, expected %u, match %d", c2, exp_c2, c2 == exp_c2);
		return false;
	}

	return true;
}

/**
 * Releases segments created with @ref openrct2_setup_rct2_segment, if any.
 */
static bool openrct2_release_rct2_segment()
{
	bool result = true;
#if defined(__linux__)
	int len = 0x01429000 - 0x8a4000; // 0xB85000, 12079104 bytes or around 11.5MB
	int err;
	err = munmap(gDataSegment, len);
	if (err != 0)
	{
		err = errno;
		log_error("Failed to unmap data segment! errno = %d", err);
		result = false;
	}
	len = 0x004A3000;
	err = munmap(gTextSegment, len);
	if (err != 0)
	{
		err = errno;
		log_error("Failed to unmap text segment! errno = %d", err);
		result = false;
	}
	err = close(gExeFd);
	if (err != 0)
	{
		err = errno;
		log_error("Failed to close file! errno = %d", err);
		result = false;
	}
#endif // defined(__linux__)
	return result;
}

/**
 * Setup hooks to allow RCT2 to call OpenRCT2 functions instead.
 */
static void openrct2_setup_rct2_hooks()
{
	addhook(0x006E732D, (int)gfx_set_dirty_blocks, 0, (int[]){ EAX, EBX, EDX, EBP, END }, 0, 0);			// remove when all callers are decompiled
	addhook(0x006E7499, (int)gfx_redraw_screen_rect, 0, (int[]){ EAX, EBX, EDX, EBP, END }, 0, 0);			// remove when 0x6E7FF3 is decompiled
	addhook(0x006B752C, (int)ride_crash, 0, (int[]){ EDX, EBX, END }, 0, 0);								// remove when all callers are decompiled
	addhook(0x0069A42F, (int)peep_window_state_update, 0, (int[]){ ESI, END }, 0, 0);						// remove when all callers are decompiled
	addhook(0x006BB76E, (int)audio_play_sound_panned, 0, (int[]){EAX, EBX, ECX, EDX, EBP, END}, EAX, 0);	// remove when all callers are decompiled
	addhook(0x006C42D9, (int)scrolling_text_setup, 0, (int[]){EAX, ECX, EBP, END}, 0, EBX);					// remove when all callers are decompiled
	addhook(0x006C2321, (int)gfx_get_string_width, 0, (int[]){ESI, END}, 0, ECX);							// remove when all callers are decompiled
	addhook(0x006C2555, (int)format_string, 0, (int[]){EDI, EAX, ECX, END}, 0, 0);							// remove when all callers are decompiled
}

#if _MSC_VER >= 1900
/**
 * Temporary fix for libraries not compiled with VS2015
 */
FILE **__iob_func()
{
	static FILE* streams[3];
	streams[0] = stdin;
	streams[1] = stdout;
	streams[2] = stderr;
	return streams;
}
#endif
