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
#include "drawing/drawing.h"
#include "editor.h"
#include "game.h"
#include "interface/window.h"
#include "interface/viewport.h"
#include "localisation/date.h"
#include "localisation/localisation.h"
#include "management/finance.h"
#include "management/news_item.h"
#include "object.h"
#include "peep/staff.h"
#include "ride/ride.h"
#include "util/sawyercoding.h"
#include "util/util.h"
#include "world/banner.h"
#include "world/map.h"
#include "world/park.h"
#include "world/sprite.h"

static void set_all_land_owned();
static int editor_load_landscape_from_sv4(const char *path);
static int editor_load_landscape_from_sc4(const char *path);
static int editor_read_sc4(char *src, int length);
static int editor_read_sv4(char *src, int length);
static int editor_read_s4(char *src);
static int editor_read_s6(const char *path);

/*Syntax error blah blah blat *&2)*/

/**
 *
 *  rct2: 0x0066FFE1
 */
void editor_load()
{
	rct_window *mainWindow;

	pause_sounds();
	unpause_sounds();
	object_unload_all();
	map_init();
	RCT2_CALLPROC_EBPSAFE(0x006B9CB0);
	reset_park_entrances();
	reset_saved_strings();
	reset_sprite_list();
	ride_init_all();
	window_guest_list_init_vars_a();
	sub_6BD3A4();
	park_init();
	finance_init();
	date_reset();
	window_guest_list_init_vars_b();
	window_staff_list_init_vars();
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_SCENARIO_EDITOR;
	RCT2_GLOBAL(0x0141F570, uint8) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
	window_new_ride_init_vars();
	RCT2_GLOBAL(0x0141F571, uint8) = 4;
	viewport_init_all();
	news_item_init_queue();
	RCT2_CALLPROC_EBPSAFE(0x0066EF38); // window_main_editor_create
	mainWindow = window_get_main();
	window_scroll_to_location(mainWindow, 2400, 2400, 112);
	mainWindow->flags &= ~WF_SCROLLING_TO_LOCATION;
	RCT2_CALLPROC_EBPSAFE(0x006837E3);
	gfx_invalidate_screen();
	RCT2_GLOBAL(0x009DEA66, sint16) = 0;
	// rct2_endupdate();
}

/**
 *
 *  rct2: 0x00672781
 */
void editor_convert_save_to_scenario()
{
	RCT2_CALLPROC_EBPSAFE(0x00672781);
}

/**
 *
 *  rct2: 0x00672957
 */
void trackdesigner_load()
{
	rct_window *mainWindow;

	object_unload_all();
	map_init();
	set_all_land_owned();
	RCT2_CALLPROC_EBPSAFE(0x006B9CB0);
	reset_park_entrances();
	reset_saved_strings();
	reset_sprite_list(); 
	ride_init_all();
	window_guest_list_init_vars_a();
	sub_6BD3A4();
	park_init();
	finance_init();
	date_reset();
	window_guest_list_init_vars_b();
	window_staff_list_init_vars();
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_TRACK_DESIGNER;
	RCT2_GLOBAL(0x0141F570, uint8) = 0;
	window_new_ride_init_vars();
	viewport_init_all();
	news_item_init_queue();
	RCT2_CALLPROC_EBPSAFE(0x0066EF38); // window_main_editor_create
	mainWindow = window_get_main();
	window_scroll_to_location(mainWindow, 2400, 2400, 112);
	mainWindow->flags &= ~WF_SCROLLING_TO_LOCATION;
	RCT2_CALLPROC_EBPSAFE(0x006837E3);
	gfx_invalidate_screen();
	RCT2_GLOBAL(0x009DEA66, sint16) = 0;
	rct2_endupdate();
}

/**
 *
 *  rct2: 0x006729FD
 */
void trackmanager_load()
{
	rct_window *mainWindow;

	object_unload_all();
	map_init();
	set_all_land_owned();
	RCT2_CALLPROC_EBPSAFE(0x006B9CB0);
	reset_park_entrances();
	reset_saved_strings();
	reset_sprite_list();
	ride_init_all();
	window_guest_list_init_vars_a();
	sub_6BD3A4();
	park_init();
	finance_init();
	date_reset();
	window_guest_list_init_vars_b();
	window_staff_list_init_vars();
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_TRACK_MANAGER;
	RCT2_GLOBAL(0x0141F570, uint8) = 0;
	window_new_ride_init_vars();
	viewport_init_all();
	news_item_init_queue();
	RCT2_CALLPROC_EBPSAFE(0x0066EF38); // window_main_editor_create
	mainWindow = window_get_main();
	window_scroll_to_location(mainWindow, 2400, 2400, 112);
	mainWindow->flags &= ~WF_SCROLLING_TO_LOCATION;
	RCT2_CALLPROC_EBPSAFE(0x006837E3);
	gfx_invalidate_screen();
	RCT2_GLOBAL(0x009DEA66, sint16) = 0;
	rct2_endupdate();
}

/**
 *
 *  rct2: 0x0068ABEC
 */
static void set_all_land_owned()
{
	int mapSize = RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, sint16);

	game_do_command(64, 1, 64, 2, GAME_COMMAND_56, (mapSize - 2) * 32, (mapSize - 2) * 32);
}

/**
*
*  rct2: 0x006BD3A4
*/
void sub_6BD3A4() {
	for (short i = 0; i < 200; i++) {
		RCT2_ADDRESS(RCT2_ADDRESS_STAFF_MODE_ARRAY, uint8)[i] = STAFF_MODE_NONE;
	}
	for (short i = 200; i < 204; i++) {
		RCT2_ADDRESS(RCT2_ADDRESS_STAFF_MODE_ARRAY, uint8)[i] = STAFF_MODE_WALK;
	}
	//RCT2_CALLPROC_EBPSAFE(0x006C0C3F);
	sub_6C0C3F();
}

static void read(void *dst, void **src, int length)
{
	memcpy(dst, *src, length);
	*((char**)src) += length;
}

/**
 *
 *  rct2: 0x006758C0
 */
void editor_load_landscape(const char *path)
{
	window_close_construction_windows();
	
	char *extension = strrchr(path, '.');
	if (extension != NULL) {
		if (_stricmp(extension, ".sv4") == 0) {
			editor_load_landscape_from_sv4(path);
			return;
		} else if (_stricmp(extension, ".sc4") == 0) {
			editor_load_landscape_from_sc4(path);
			return;
		}
	}

	// Load SC6 / SV6
	editor_read_s6(path);
}

/**
 *
 *  rct2: 0x006A2B02
 */
static int editor_load_landscape_from_sv4(const char *path)
{
	FILE *fp;
	long fpLength;
	char *fpBuffer;

	// Open file
	fp = fopen(path, "rb");
	if (fp == NULL) {
		RCT2_GLOBAL(0x009AC31B, uint8) = 255;
		RCT2_GLOBAL(0x009AC31C, uint16) = 3011;
		return 0;
	}

	// Read whole file into a buffer
	fpLength = fsize(fp);
	fpBuffer = malloc(fpLength);
	fread(fpBuffer, fpLength, 1, fp);
	fclose(fp);

	editor_read_sv4(fpBuffer, fpLength);
	free(fpBuffer);

	RCT2_CALLPROC_EBPSAFE(0x006A2B62);
	return 1;
}

static int editor_load_landscape_from_sc4(const char *path)
{
	FILE *fp;
	long fpLength;
	char *fpBuffer;

	// Open file
	fp = fopen(path, "rb");
	if (fp == NULL) {
		RCT2_GLOBAL(0x009AC31B, uint8) = 255;
		RCT2_GLOBAL(0x009AC31C, uint16) = 3011;
		return 0;
	}

	// Get length
	fseek(fp, 0, SEEK_END);
	fpLength = ftell(fp);
	rewind(fp);

	// Read whole file into a buffer
	fpBuffer = malloc(fpLength);
	fread(fpBuffer, fpLength, 1, fp);
	fclose(fp);

	editor_read_sc4(fpBuffer, fpLength);
	free(fpBuffer);

	RCT2_CALLPROC_EBPSAFE(0x006A2B62);
	return 1;
}

static int editor_read_sc4(char *src, int length)
{
	int decodedLength;
	char *decodedBuffer;

	decodedBuffer = malloc(2065676);
	decodedLength = sawyercoding_decode_sc4(src, decodedBuffer, length);
	if (decodedLength != 2065676) {
		free(decodedBuffer);
		return 0;
	}

	editor_read_s4(decodedBuffer);
	free(decodedBuffer);
	return 1;
}

static int editor_read_sv4(char *src, int length)
{
	int decodedLength;
	char *decodedBuffer;

	decodedBuffer = malloc(2065676);
	decodedLength = sawyercoding_decode_sv4(src, decodedBuffer, length);
	if (decodedLength != 2065676) {
		free(decodedBuffer);
		return 0;
	}

	editor_read_s4(decodedBuffer);
	free(decodedBuffer);
	return 1;
}

/**
 *
 *  rct2: 0x0069EEA0
 */
static int editor_read_s4(char *src)
{
	int i;
	rct_banner *banner;

	read((void*)RCT2_ADDRESS_CURRENT_MONTH_YEAR, &src, 16);
	memset((void*)RCT2_ADDRESS_MAP_ELEMENTS, 0, 0x60000 * 4);
	read((void*)RCT2_ADDRESS_MAP_ELEMENTS, &src, 0x60000);
	read((void*)0x010E63B8, &src, 0x138804);

	for (i = 0; i < MAX_BANNERS; i++)
		gBanners[i].var_00 = 255;

	read((void*)0x013573BC, &src, 12424);

	for (i = 0; i < MAX_BANNERS; i++) {
		banner = &gBanners[i];
		if (banner->var_00 != 255 && banner->string_idx != 3458)
			banner->string_idx = 778;
	}

	read((void*)0x0135A8F4, &src, 0x2F51C);
	memset((void*)0x013CA672, 0, 204);
	read((void*)0x0138B580, &src, 0x258F2);
	read((void*)0x013C6A72, &src, 0x3C00);

	char *esi = (char*)0x13C6A72;
	char *edi = (char*)0x13B0E72;
	int ebx, edx = 116;
	do {
		ebx = 32;
		do {
			memcpy(edi, esi, 4); esi += 4; edi += 4;
			memset(edi, 0, 4); edi += 4;
		} while (--ebx > 0);
		memset(edi, 0, 64); edi += 64;
	} while (--edx > 0);
	edi += 0xA800;
	
	edx = 4;
	do {
		ebx = 32;
		do {
			memcpy(edi, esi, 4); esi += 4; edi += 4;
			memset(edi, 0, 4); edi += 4;
		} while (--ebx);
		memset(edi, 0, 64); edi += 64;
	} while (--edx);

	read((void*)0x013CA672, &src, 116);
	read((void*)0x013CA73A, &src, 4);
	read((void*)0x013CA73E, &src, 0x41EA);
	return 1;
}

/**
 *
 *  rct2: 0x006758FE
 */
static int editor_read_s6(const char *path)
{
	strcpy((char *)0x0141EF68, path);
	RCT2_CALLPROC_EBPSAFE(0x006758FE);
	return 1;
}
