/*****************************************************************************
 * Copyright (c) 2014 Ted John, Peter Hill
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
#include "config.h"
#include "game.h"
#include "editor.h"
#include "world/footpath.h"
#include "input.h"
#include "localisation/localisation.h"
#include "interface/screenshot.h"
#include "interface/viewport.h"
#include "interface/widget.h"
#include "interface/window.h"
#include "management/finance.h"
#include "management/marketing.h"
#include "management/news_item.h"
#include "management/research.h"
#include "object.h"
#include "peep/peep.h"
#include "peep/staff.h"
#include "platform/platform.h"
#include "ride/ride.h"
#include "ride/ride_ratings.h"
#include "ride/vehicle.h"
#include "scenario.h"
#include "title.h"
#include "tutorial.h"
#include "util/sawyercoding.h"
#include "util/util.h"
#include "windows/error.h"
#include "windows/tooltip.h"
#include "world/climate.h"
#include "world/park.h"
#include "world/sprite.h"

int gGameSpeed = 1;

void game_increase_game_speed()
{
	gGameSpeed = min(8, gGameSpeed + 1);
}

void game_reduce_game_speed()
{
	gGameSpeed = max(1, gGameSpeed - 1);
}

/**
 * 
 *  rct2: 0x0066B5C0 (part of 0x0066B3E8)
 */
void game_create_windows()
{
	window_main_open();
	window_top_toolbar_open();
	window_game_bottom_toolbar_open();
	RCT2_CALLPROC_EBPSAFE(0x0066B905);
}

/**
*
*  rct2: 0x006838BD
*/
void update_palette_effects()
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_LIGHTNING_ACTIVE, uint8) == 1) {
		// change palette to lighter color during lightning
		int palette = 1532;
		if (RCT2_GLOBAL(0x009ADAE0, sint32) != -1) {
			palette = RCT2_GLOBAL(RCT2_GLOBAL(0x009ADAE0, int) + 2, int);
		}
		rct_g1_element g1_element = RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[palette];
		int xoffset = g1_element.x_offset;
		xoffset = xoffset * 4;
		for (int i = 0; i < g1_element.width; i++) {
			RCT2_ADDRESS(0x01424680 + xoffset, uint8)[(i * 4) + 0] = -((0xFF - g1_element.offset[(i * 3) + 0]) / 2) - 1;
			RCT2_ADDRESS(0x01424680 + xoffset, uint8)[(i * 4) + 1] = -((0xFF - g1_element.offset[(i * 3) + 1]) / 2) - 1;
			RCT2_ADDRESS(0x01424680 + xoffset, uint8)[(i * 4) + 2] = -((0xFF - g1_element.offset[(i * 3) + 2]) / 2) - 1;
		}
		RCT2_GLOBAL(0x014241BC, uint32) = 2;
		platform_update_palette(RCT2_ADDRESS(0x01424680, uint8), 10, 236);
		RCT2_GLOBAL(0x014241BC, uint32) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_LIGHTNING_ACTIVE, uint8)++;
	} else {
		if (RCT2_GLOBAL(RCT2_ADDRESS_LIGHTNING_ACTIVE, uint8) == 2) {
			// change palette back to normal after lightning
			int palette = 1532;
			if (RCT2_GLOBAL(0x009ADAE0, sint32) != -1) {
				palette = RCT2_GLOBAL(RCT2_GLOBAL(0x009ADAE0, int) + 2, int);
			}
			rct_g1_element g1_element = RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[palette];
			int xoffset = g1_element.x_offset;
			xoffset = xoffset * 4;
			for (int i = 0; i < g1_element.width; i++) {
				RCT2_ADDRESS(0x01424680 + xoffset, uint8)[(i * 4) + 0] = g1_element.offset[(i * 3) + 0];
				RCT2_ADDRESS(0x01424680 + xoffset, uint8)[(i * 4) + 1] = g1_element.offset[(i * 3) + 1];
				RCT2_ADDRESS(0x01424680 + xoffset, uint8)[(i * 4) + 2] = g1_element.offset[(i * 3) + 2];
			}
		}

		// animate the water/lava/chain movement palette
		int q = 0;
		int weather_colour = RCT2_ADDRESS(0x98195C, uint32)[RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER_GLOOM, uint8)];
		if (weather_colour != -1) {
			q = 1;
			if (weather_colour != 0x2000031) {
				q = 2;
			}
		}
		uint32 j = RCT2_GLOBAL(RCT2_ADDRESS_PALETTE_EFFECT_FRAME_NO, uint32);
		j = (((uint16)((~j / 2) * 128) * 15) >> 16);
		int p = 1533;
		if (RCT2_GLOBAL(0x009ADAE0, int) != -1) {
			p = RCT2_GLOBAL(RCT2_GLOBAL(0x009ADAE0, int) + 0x6, int);
		}
		rct_g1_element g1_element = RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[q + p];
		uint8* vs = &g1_element.offset[j * 3];
		uint8* vd = RCT2_ADDRESS(0x01424A18, uint8);
		int n = 5;
		for (int i = 0; i < n; i++) {
			vd[0] = vs[0];
			vd[1] = vs[1];
			vd[2] = vs[2];
			vs += 9;
			if (vs >= &g1_element.offset[9 * n]) {
				vs -= 9 * n;
			}
			vd += 4;
		}

		p = 1536;
		if (RCT2_GLOBAL(0x009ADAE0, int) != -1) {
			p = RCT2_GLOBAL(RCT2_GLOBAL(0x009ADAE0, int) + 0xA, int);
		}
		g1_element = RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[q + p];
		vs = &g1_element.offset[j * 3];
		n = 5;
		for (int i = 0; i < n; i++) {
			vd[0] = vs[0];
			vd[1] = vs[1];
			vd[2] = vs[2];
			vs += 9;
			if (vs >= &g1_element.offset[9 * n]) {
				vs -= 9 * n;
			}
			vd += 4;
		}

		j = ((uint16)(RCT2_GLOBAL(RCT2_ADDRESS_PALETTE_EFFECT_FRAME_NO, uint32) * -960) * 3) >> 16;
		p = 1539;
		g1_element = RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element)[q + p];
		vs = &g1_element.offset[j * 3];
		vd += 12;
		n = 3;
		for (int i = 0; i < n; i++) {
			vd[0] = vs[0];
			vd[1] = vs[1];
			vd[2] = vs[2];
			vs += 3;
			if (vs >= &g1_element.offset[3 * n]) {
				vs -= 3 * n;
			}
			vd += 4;
		}

		RCT2_GLOBAL(0x014241BC, uint32) = 2;
		platform_update_palette(RCT2_ADDRESS(0x01424680, uint8), 230, 16);
		RCT2_GLOBAL(0x014241BC, uint32) = 0;
		if (RCT2_GLOBAL(RCT2_ADDRESS_LIGHTNING_ACTIVE, uint8) == 2) {
			RCT2_GLOBAL(0x014241BC, uint32) = 2;
			platform_update_palette(RCT2_ADDRESS(0x01424680, uint8), 10, 236);
			RCT2_GLOBAL(0x014241BC, uint32) = 0;
			RCT2_GLOBAL(RCT2_ADDRESS_LIGHTNING_ACTIVE, uint8) = 0;
		}
	}
	if (RCT2_GLOBAL(0x009E2C4C, uint32) == 2 || RCT2_GLOBAL(0x009E2C4C, uint32) == 1) {
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_CAP_BPP, uint32) != 8) {
			RCT2_GLOBAL(0x009E2C78, int) = 1;
		}
	}
}

void game_update()
{
	int i, numUpdates, tmp;

	// Handles picked-up peep and rain redraw
	redraw_peep_and_rain();

	// 0x006E3AEC // screen_game_process_mouse_input();
	// RCT2_CALLPROC_EBPSAFE(0x006E3AEC); // screen_game_process_keyboard_input();
	screenshot_check();
	game_handle_keyboard_input();

	// Determine how many times we need to update the game
	if (gGameSpeed > 1) {
		numUpdates = 1 << (gGameSpeed - 1);
	} else {
		numUpdates = RCT2_GLOBAL(0x009DE588, uint16) / 31;
		numUpdates = clamp(1, numUpdates, 4);
	}

	// Update the game one or more times
	if (RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) == 0) {
		for (i = 0; i < numUpdates; i++) {
			game_logic_update();
			start_title_music();

			if (gGameSpeed > 1)
				continue;

			// Possibly smooths viewport scrolling, I don't see a difference though
			if (RCT2_GLOBAL(0x009E2D74, uint32) == 1) {
				RCT2_GLOBAL(0x009E2D74, uint32) = 0;
				break;
			} else {
				if (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) == INPUT_STATE_RESET ||
					RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) == INPUT_STATE_NORMAL
				) {
					if (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32)) {
						RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) &= ~INPUT_FLAG_VIEWPORT_SCROLLING;
						break;
					}
				} else {
					break;
				}
			}
		}
	}

	RCT2_GLOBAL(0x009A8C28, uint8) = 0;

	RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) &= ~INPUT_FLAG_VIEWPORT_SCROLLING;
	RCT2_GLOBAL(0x009AC861, uint16) ^= (1 << 15);
	RCT2_GLOBAL(0x009AC861, uint16) &= ~(1 << 1);
	tmp = RCT2_GLOBAL(0x009AC861, uint16) & (1 << 0);
	RCT2_GLOBAL(0x009AC861, uint16) &= ~(1 << 0);
	if (!tmp)
		RCT2_GLOBAL(0x009AC861, uint16) |= (1 << 1);
	RCT2_GLOBAL(0x009AC861, uint16) &= ~(1 << 3);
	tmp = RCT2_GLOBAL(0x009AC861, uint16) & (1 << 2);
	RCT2_GLOBAL(0x009AC861, uint16) &= ~(1 << 2);
	if (!tmp)
		RCT2_GLOBAL(0x009AC861, uint16) |= (1 << 2);

	window_map_tooltip_update_visibility();
	window_update_all();

	RCT2_GLOBAL(0x01388698, uint16)++;

	// Input
	RCT2_GLOBAL(0x0141F568, uint8) = RCT2_GLOBAL(0x0013CA740, uint8);
	game_handle_input();

	update_palette_effects();
	update_rain_animation();

	stop_completed_sounds(); // removes other sounds that are no longer playing, this is normally called somewhere in rct2_init

	if (RCT2_GLOBAL(0x009AAC73, uint8) != 255) {
		RCT2_GLOBAL(0x009AAC73, uint8)++;
		if (RCT2_GLOBAL(0x009AAC73, uint8) == 255)
			config_save();
	}
}

void game_logic_update()
{
	short stringId, _dx;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, sint32)++;
	RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, sint32)++;
	RCT2_GLOBAL(0x009DEA66, sint16)++;
	if (RCT2_GLOBAL(0x009DEA66, sint16) == 0)
		RCT2_GLOBAL(0x009DEA66, sint16)--;

	sub_68B089();
	scenario_update();
	climate_update();
	fountain_update_all();
	sub_6A876D();
	peep_update_all();
	vehicle_update_all();
	texteffect_update_all();
	ride_update_all();
	park_update();
	research_update();
	ride_ratings_update_all();
	ride_measurements_update();
	map_invalidate_animations();
	vehicle_sounds_update();
	peep_update_crowd_noise();
	climate_update_sound();
	news_item_update_current();
	editor_open_windows_for_current_step();

	// Update windows
	window_dispatch_update_all();

	if (RCT2_GLOBAL(0x009AC31B, uint8) != 0) {
		stringId = STR_UNABLE_TO_LOAD_FILE;
		_dx = RCT2_GLOBAL(0x009AC31C, uint16);
		if (RCT2_GLOBAL(0x009AC31B, uint8) != 254) {
			stringId = RCT2_GLOBAL(0x009AC31C, uint16);
			_dx = 0xFFFF;
		}
		RCT2_GLOBAL(0x009AC31B, uint8) = 0;

		window_error_open(stringId, _dx);
	}
}

/**
 * 
 *  rct2: 0x0069C62C
 *
 * @param cost (ebp)
 */
static int game_check_affordability(int cost)
{
	if (cost <= 0)return cost;
	if (RCT2_GLOBAL(0x141F568, uint8) & 0xF0)return cost;
	
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32)&(1 << 8))){
		if (cost <= (sint32)(DECRYPT_MONEY(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, sint32))))return cost;
	}
	RCT2_GLOBAL(0x13CE952, uint32) = cost;
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = 827;
	return 0x80000000;
}

static uint32 game_do_command_table[58];
static GAME_COMMAND_POINTER* new_game_command_table[58];

/**
 * 
 *  rct2: 0x006677F2
 *
 * @param flags (ebx)
 * @param command (esi)
 */
int game_do_command(int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp)
{
	return game_do_command_p(esi, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
}

/**
*
*  rct2: 0x006677F2 with pointers as arguments
*
* @param flags (ebx)
* @param command (esi)
*/
int game_do_command_p(int command, int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	int cost, flags, insufficientFunds;
	int original_ebx, original_edx, original_esi, original_edi, original_ebp;

	*esi = command;
	original_ebx = *ebx;
	original_edx = *edx;
	original_esi = *esi;
	original_edi = *edi;
	original_ebp = *ebp;

	flags = *ebx;
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = 0xFFFF;

	// Increment nest count
	RCT2_GLOBAL(0x009A8C28, uint8)++;

	*ebx &= ~GAME_COMMAND_FLAG_APPLY;
	
	// Primary command
	if (game_do_command_table[command] == 0) {
		new_game_command_table[command](eax, ebx, ecx, edx, esi, edi, ebp);
	} else {
		RCT2_CALLFUNC_X(game_do_command_table[command], eax, ebx, ecx, edx, esi, edi, ebp);
	}
	cost = *ebx;

	if (cost != 0x80000000) {
		// Check funds
		insufficientFunds = 0;
		if (RCT2_GLOBAL(0x009A8C28, uint8) == 1 && !(flags & 4) && !(flags & 0x20) && cost != 0)
			insufficientFunds = game_check_affordability(cost);

		if (insufficientFunds != 0x80000000) {
			*ebx = original_ebx;
			*edx = original_edx;
			*esi = original_esi;
			*edi = original_edi;
			*ebp = original_ebp;

			if (!(flags & 1)) {
				// Decrement nest count
				RCT2_GLOBAL(0x009A8C28, uint8)--;
				return cost;
			}

			// Secondary command
			if (game_do_command_table[command] == 0) {
				new_game_command_table[command](eax, ebx, ecx, edx, esi, edi, ebp);
			} else {
				RCT2_CALLFUNC_X(game_do_command_table[command], eax, ebx, ecx, edx, esi, edi, ebp);
			}
			*edx = *ebx;

			if (*edx != 0x80000000 && *edx < cost)
				cost = *edx;

			// Decrement nest count
			RCT2_GLOBAL(0x009A8C28, uint8)--;
			if (RCT2_GLOBAL(0x009A8C28, uint8) != 0)
				return cost;

			// 
			if (!(flags & 0x20)) {
				// Update money balance
				finance_payment(cost, RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) / 4);
				if (RCT2_GLOBAL(0x0141F568, uint8) == RCT2_GLOBAL(0x013CA740, uint8)) {
					// Create a +/- money text effect
					if (cost != 0)
						RCT2_CALLPROC_X(0x0069C5D0, 0, cost, 0, 0, 0, 0, 0);
				}
			}

			return cost;
		}
	}

	// Error occured

	// Decrement nest count
	RCT2_GLOBAL(0x009A8C28, uint8)--;

	// Show error window
	if (RCT2_GLOBAL(0x009A8C28, uint8) == 0 && (flags & 1) && RCT2_GLOBAL(0x0141F568, uint8) == RCT2_GLOBAL(0x013CA740, uint8) && !(flags & 8))
		window_error_open(RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16), RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16));

	return 0x80000000;
}


/**
 * 
 *  rct2: 0x00667C15
 */
void game_pause_toggle(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	if (*ebx & GAME_COMMAND_FLAG_APPLY) {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint32) ^= 1;
		window_invalidate_by_class(WC_TOP_TOOLBAR);
		if (RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint32) & 1)
			pause_sounds();
		else
			unpause_sounds();
	}
	*ebx = 0;
}

/**
 * 
 *  rct2: 0x0066DB5F
 */
static void game_load_or_quit(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	if (*ebx & GAME_COMMAND_FLAG_APPLY) {
		switch (*edx & 0xFF) {
		case 0:
			RCT2_GLOBAL(RCT2_ADDRESS_SAVE_PROMPT_MODE, uint16) = *edi & 0xFF;
			window_save_prompt_open();
			break;
		case 1:
			window_close_by_class(WC_SAVE_PROMPT);
			break;
		default:
			game_load_or_quit_no_save_prompt();
			break;
		}
	}
	*ebx = 0;
}

/**
 *
 *  rct2: 0x00674F40
 */
static int open_landscape_file_dialog()
{
	int result;
	format_string((char*)RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, STR_LOAD_LANDSCAPE_DIALOG_TITLE, 0);
	strcpy((char*)0x0141EF68, (char*)RCT2_ADDRESS_LANDSCAPES_PATH);
	format_string((char*)0x0141EE68, STR_RCT2_LANDSCAPE_FILE, 0);
	pause_sounds();
	result = platform_open_common_file_dialog(1, (char*)RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, (char*)0x0141EF68, "*.SV6;*.SV4;*.SC6", (char*)0x0141EE68);
	unpause_sounds();
	// window_proc
	return result;
}

/**
 *
 *  rct2: 0x00674EB6
 */
static int open_load_game_dialog()
{
	int result;
	format_string((char*)RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, STR_LOAD_GAME_DIALOG_TITLE, 0);
	strcpy((char*)0x0141EF68, (char*)RCT2_ADDRESS_SAVED_GAMES_PATH);
	format_string((char*)0x0141EE68, STR_RCT2_SAVED_GAME, 0);
	pause_sounds();
	result = platform_open_common_file_dialog(1, (char*)RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, (char*)0x0141EF68, "*.SV6", (char*)0x0141EE68);
	unpause_sounds();
	// window_proc
	return result;
}

/**
 *
 *  rct2: 0x0066DC0F
 */
static void load_landscape()
{
	window_loadsave_open(LOADSAVETYPE_LOAD | LOADSAVETYPE_LANDSCAPE);
	return;

	if (open_landscape_file_dialog() == 0) {
		gfx_invalidate_screen();
	} else {
		// Set default filename
		char *esi = (char*)0x0141EF67;
		while (1) {
			esi++;
			if (*esi == '.')
				break;
			if (*esi != 0)
				continue;
			strcpy(esi, ".SC6");
			break;
		}
		strcpy((char*)RCT2_ADDRESS_SAVED_GAMES_PATH_2, (char*)0x0141EF68);

		editor_load_landscape((char*)0x0141EF68);
		if (1) {
			gfx_invalidate_screen();
			rct2_endupdate();
		} else {
			RCT2_GLOBAL(0x009DEA66, uint16) = 0;
			rct2_endupdate();
		}
	}
}

/**
 * 
 *  rct2: 0x00675E1B
 */
int game_load_save(const char *path)
{
	rct_window *mainWindow;
	FILE *file;
	int i, j;

	log_verbose("loading saved game, %s", path);

	strcpy((char*)0x0141EF68, path);
	file = fopen(path, "rb");
	if (file == NULL) {
		log_error("unable to open %s", path);

		RCT2_GLOBAL(0x009AC31B, uint8) = 255;
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, uint16) = STR_FILE_CONTAINS_INVALID_DATA;
		return 0;
	}

	if (!sawyercoding_validate_checksum(file)) {
		fclose(file);

		log_error("invalid checksum, %s", path);

		RCT2_GLOBAL(0x009AC31B, uint8) = 255;
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, uint16) = STR_FILE_CONTAINS_INVALID_DATA;
		return 0;
	}

	rct_s6_header *s6Header = (rct_s6_header*)0x009E34E4;
	rct_s6_info *s6Info = (rct_s6_info*)0x0141F570;

	// Read first chunk
	sawyercoding_read_chunk(file, (uint8*)s6Header);
	if (s6Header->type == S6_TYPE_SAVEDGAME) {
		// Read packed objects
		if (s6Header->num_packed_objects > 0) {
			j = 0;
			for (i = 0; i < s6Header->num_packed_objects; i++)
				j += object_load_packed(file);
			if (j > 0)
				object_list_load();
		}
	}

	uint8 load_success = object_read_and_load_entries(file);

	// Read flags (16 bytes)
	sawyercoding_read_chunk(file, (uint8*)RCT2_ADDRESS_CURRENT_MONTH_YEAR);

	// Read map elements
	memset((void*)RCT2_ADDRESS_MAP_ELEMENTS, 0, MAX_MAP_ELEMENTS * sizeof(rct_map_element));
	sawyercoding_read_chunk(file, (uint8*)RCT2_ADDRESS_MAP_ELEMENTS);

	// Read game data, including sprites
	sawyercoding_read_chunk(file, (uint8*)0x010E63B8);

	fclose(file);

	// Check expansion pack
	// RCT2_CALLPROC_EBPSAFE(0x006757E6);

	if (!load_success){
		set_load_objects_fail_reason();
		if (RCT2_GLOBAL(0x9DE518,uint32) & (1<<5)){
			RCT2_GLOBAL(0x14241BC, uint32) = 2;
			//call 0x0040705E Sets cursor position and something else. Calls maybe wind func 8 probably pointless
			RCT2_GLOBAL(0x14241BC, uint32) = 0;
			RCT2_GLOBAL(0x9DE518, uint32) &= ~(1<<5);
		}
		title_load();
		rct2_endupdate();
		return 0;//This never gets called
	}

	// The rest is the same as in scenario load and play
	RCT2_CALLPROC_EBPSAFE(0x006A9FC0);
	map_update_tile_pointers();
	reset_0x69EBE4();// RCT2_CALLPROC_EBPSAFE(0x0069EBE4);
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_PLAYING;
	viewport_init_all();
	game_create_windows();
	mainWindow = window_get_main();

	mainWindow->viewport_target_sprite = -1;
	mainWindow->saved_view_x = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_X, sint16);
	mainWindow->saved_view_y = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_Y, sint16);
	uint8 _cl = (RCT2_GLOBAL(0x0138869E, sint16) & 0xFF) - mainWindow->viewport->zoom;
	mainWindow->viewport->zoom = RCT2_GLOBAL(0x0138869E, sint16) & 0xFF;
	*((char*)(&RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, sint32))) = RCT2_GLOBAL(0x0138869E, sint16) >> 8;
	if (_cl != 0) {
		if (_cl < 0) {
			_cl = -_cl;
			mainWindow->viewport->view_width >>= _cl;
			mainWindow->viewport->view_height >>= _cl;
		} else {
			mainWindow->viewport->view_width <<= _cl;
			mainWindow->viewport->view_height <<= _cl;
		}
	}
	mainWindow->saved_view_x -= mainWindow->viewport->view_width >> 1;
	mainWindow->saved_view_y -= mainWindow->viewport->view_height >> 1;
	window_invalidate(mainWindow);

	sub_69E9A7(); 
	RCT2_CALLPROC_EBPSAFE(0x006DFEE4);
	window_new_ride_init_vars();
	RCT2_GLOBAL(0x009DEB7C, uint16) = 0;
	if (RCT2_GLOBAL(0x0013587C4, uint32) == 0)		// this check is not in scenario play
		sub_69E869();

	load_palette();
	gfx_invalidate_screen();
	return 1;
}

/*
 *
 * rct2: 0x0069E9A7
 */
void sub_69E9A7()
{
	for (rct_sprite* spr = g_sprite_list; spr < (rct_sprite*)RCT2_ADDRESS_SPRITES_NEXT_INDEX; spr++)
		if (spr->unknown.sprite_identifier != 0xFF)
			sprite_move(spr->unknown.x, spr->unknown.y, spr->unknown.z, spr);
}

/**
 * 
 *  rct2: 0x0066DBB7
 */
static void load_game()
{
	window_loadsave_open(LOADSAVETYPE_LOAD | LOADSAVETYPE_GAME);
	return;

	if (open_load_game_dialog() == 0) {
		gfx_invalidate_screen();
	} else {
		// Set default filename
		char *esi = (char*)0x0141EF67;
		while (1) {
			esi++;
			if (*esi == '.')
				break;
			if (*esi != 0)
				continue;
			strcpy(esi, ".SV6");
			break;
		}
		strcpy((char*)RCT2_ADDRESS_SAVED_GAMES_PATH_2, (char*)0x0141EF68);

		if (game_load_save((char *)0x0141EF68)) {
			gfx_invalidate_screen();
			rct2_endupdate();
		} else {
			RCT2_GLOBAL(0x009DEA66, uint16) = 0;
			rct2_endupdate();
		}
	}
}

/**
 *
 *  rct2: 0x006750E9
 */
static int show_save_game_dialog(char *resultPath)
{
	rct_s6_info *s6Info = (rct_s6_info*)0x0141F570;

	int result;
	char title[256];
	char filename[MAX_PATH];
	char filterName[256];

	format_string(title, STR_SAVE_GAME_1040, NULL);
	strcpy(filename, RCT2_ADDRESS(RCT2_ADDRESS_SAVED_GAMES_PATH_2, char));
	format_string(filterName, STR_RCT2_SAVED_GAME, NULL);

	pause_sounds();
	result = platform_open_common_file_dialog(0, title, filename, "*.SV6", filterName);
	unpause_sounds();

	if (result)
		strcpy(resultPath, filename);
	return result;
}

char save_game()
{
	window_loadsave_open(LOADSAVETYPE_SAVE | LOADSAVETYPE_GAME);
	return 0;

	char path[256];

	if (!show_save_game_dialog(path)) {
		gfx_invalidate_screen();
		return 0;
	}

	// Ensure path has .SV6 extension
	path_set_extension(path, ".SV6");
	
	if (scenario_save(path, gGeneral_config.save_plugin_data ? 1 : 0)) {
		game_do_command(0, 1047, 0, -1, GAME_COMMAND_0, 0, 0);
		gfx_invalidate_screen();
		return 1;
	} else {
		return 0;
	}
}

/**
 * 
 *  rct2: 0x006E3879
 */
void rct2_exit()
{
	RCT2_CALLPROC_EBPSAFE(0x006E3879);
	//Post quit message does not work in 0x6e3879 as its windows only.
}

/**
 * 
 *  rct2: 0x0066DB79
 */
void game_load_or_quit_no_save_prompt()
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_SAVE_PROMPT_MODE, uint16) < 1) {
		game_do_command(0, 1, 0, 1, GAME_COMMAND_LOAD_OR_QUIT, 0, 0);
		tool_cancel();
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2)
			load_landscape();
		else
			load_game();
	} else if (RCT2_GLOBAL(RCT2_ADDRESS_SAVE_PROMPT_MODE, uint16) == 1) {
		game_do_command(0, 1, 0, 1, GAME_COMMAND_LOAD_OR_QUIT, 0, 0);
		if (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_5) {
			RCT2_CALLPROC_EBPSAFE(0x0040705E);
			RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) &= ~INPUT_FLAG_5;
		}
		title_load();
		rct2_endupdate();
	} else {
		rct2_exit();
	}
}

#pragma region Game command function table

static uint32 game_do_command_table[58] = {
	0x006B2FC5,
	0x0066397F,
	0,
	0x006C511D,
	0x006C5B69,
	0,
	0x006B3F0F,
	0x006B49D9,
	0x006B4EA6,
	0x006B52D4,
	0, // 10
	0x006B5559,
	0x006660A8,
	0x0066640B,
	0x006E0E01,
	0x006E08F4,
	0x006E650F,
	0,
	0x006A68AE,
	0,
	0, // use new_game_command_table, original: 0x00663CCD, // 20
	0x006B53E9,
	0x00698D6C, // text input
	0x0068C542,
	0x0068C6D1,
	0x0068BC01,
	0x006E66A0,
	0x006E6878,
	0x006C5AE9,
	0, // use new_game_command_table, original: 0x006BEFA1, 29
	0x006C09D1, // 30
	0x006C0B83,
	0x006C0BB5,
	0x00669C6D,
	0,
	0x006649BD,
	0x006666E7,
	0,
	0x006CD8CE,
	0,
	0, // 40
	0x006E519A,
	0x006E5597,
	0x006B893C,
	0x006B8E1B,
	0,
	0,
	0x006D13FE,
	0,
	0x006CDEE4,
	0x006B9E6D, // 50
	0x006BA058,
	0x006E0F26,
	0x006E56B5,
	0x006B909A,
	0x006BA16A,
	0x006648E3,
	0
};

void game_command_emptysub(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp) {}

static GAME_COMMAND_POINTER* new_game_command_table[58] = {
	game_command_emptysub,
	game_command_emptysub,
	game_pause_toggle,
	game_command_emptysub,
	game_command_emptysub,
	game_load_or_quit,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_set_ride_name, // 10
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_place_footpath,
	game_command_emptysub,
	game_command_remove_footpath,
	game_command_change_surface_style, // 20
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_hire_new_staff_member, //game_command_emptysub,
	game_command_emptysub, // 30
	game_command_emptysub,
	game_command_emptysub,
	game_command_set_park_name,
	game_command_set_park_open,
	game_command_emptysub,
	game_command_emptysub,
	game_command_remove_park_entrance,
	game_command_emptysub,
	game_command_set_park_entrance_fee,
	game_command_update_staff_colour, // 40
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_set_current_loan,
	game_command_set_research_funding,
	game_command_emptysub,
	game_command_start_campaign,
	game_command_emptysub,
	game_command_emptysub, // 50
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_emptysub,
	game_command_clear_scenery
};

#pragma endregion
