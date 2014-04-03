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
#include "audio.h"
#include "gfx.h"
#include "intro.h"
#include "rct2.h"
#include "osinterface.h"
#include "sprites.h"
#include "strings.h"

static void screen_intro_process_mouse_input();
static void screen_intro_process_keyboard_input();
static void screen_intro_skip_part();

static int _sound_playing_flag = 0;		///< Used to test if a sound is currently playing.
static rct_sound _prepared_sound;		///< A prepared sound for playing.
static int _tick_counter;				///< Used mainly for timing but also for Y coordinate and fading.

// rct2: 0x0068E966
void intro_update()
{
	rct_drawpixelinfo *screenDPI = RCT2_ADDRESS(RCT2_ADDRESS_SCREEN_DPI, rct_drawpixelinfo);
	int screenWidth = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16);
	int screenHeight = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16);
	uint8 (*part) = RCT2_ADDRESS(RCT2_ADDRESS_RUN_INTRO_TICK_PART, uint8);

	screen_intro_process_mouse_input();
	screen_intro_process_keyboard_input();

	RCT2_GLOBAL(0x009E2C78, int) = 1;
	switch ((*part)) {
	case 8:
		// Clear the screen
		gfx_clear(screenDPI, 10);

		// Draw the disclaimer text
		gfx_draw_string_centred(screenDPI, STR_LICENCE_AGREEMENT_NOTICE_1, screenWidth / 2, 180, 13, 0);
		gfx_draw_string_centred(screenDPI, STR_LICENCE_AGREEMENT_NOTICE_2, screenWidth / 2, 195, 13, 0);

		// Set palette thing
		gfx_transpose_palette(1532, 255);

		// Reset wait counter
		_tick_counter = 0;

		// Move to next part
		(*part)++;
		break;
	case 9:
		// Wait 320 game ticks, then move to part 1
		_tick_counter++;
		if (_tick_counter >= 320)
			(*part) = 1;
		break;
	case 1:
		// Clear the screen
		gfx_clear(screenDPI, 10);

		// Set the Y for the Infogrammes logo
		_tick_counter = -580;

		// Chain lift sound
		_sound_playing_flag = 0;
		if (RCT2_GLOBAL(0x009AF280, sint32) != -1) {
			// Prepare and play the sound
			if (sound_prepare(RCT2_SOUND_CHAINLIFT, &_prepared_sound, 0, 1))
				if (sound_play(&_prepared_sound, 1, 0, 0, 0))
					_sound_playing_flag = 1;
		}

		// Move to next part
		(*part)++;
		break;
	case 2:
		// Move the Infogrammes logo down by 5 pixels
		_tick_counter += 5;

		// Clear the screen
		gfx_clear(screenDPI, 10);

		// Draw a white rectangle for the logo background (gives a bit of white margin)
		gfx_fill_rect(screenDPI,
			(screenWidth / 2) - 320 + 50, _tick_counter + 50,
			(screenWidth / 2) - 320 + 50 + 540, _tick_counter + 50 + 425,
			129);

		// Draw the logo
		gfx_draw_sprite(screenDPI, SPR_INTRO_INFOGRAMES_00, (screenWidth / 2) - 320 + 69, _tick_counter + 69);
		gfx_draw_sprite(screenDPI, SPR_INTRO_INFOGRAMES_10, (screenWidth / 2) - 320 + 319, _tick_counter + 69);
		gfx_draw_sprite(screenDPI, SPR_INTRO_INFOGRAMES_01, (screenWidth / 2) - 320 + 69, _tick_counter + 319);
		gfx_draw_sprite(screenDPI, SPR_INTRO_INFOGRAMES_11, (screenWidth / 2) - 320 + 319, _tick_counter + 319);

		// Check if logo is off the screen .ish
		if (_tick_counter > 520) {
			// Clear the screen
			gfx_clear(screenDPI, 10);

			_tick_counter = -116;

			// Move to the next part
			(*part)++;
		}

		break;
	case 3:
		// 
		_tick_counter += 5;

		// Clear the screen
		gfx_clear(screenDPI, 10);

		// Set some palette thing
		gfx_transpose_palette(23217, 255);

		// Set the Y for the Chris Sawyer logo
		_tick_counter = -116;

		// Move to the next part
		(*part)++;
		break;
	case 4:
		// Chris Sawyer logo
		_tick_counter += 5;

		// Clear the screen
		gfx_clear(screenDPI, 10);

		// Draw Chris Sawyer logo
		gfx_draw_sprite(screenDPI, SPR_INTRO_CHRIS_SAWYER_00, (screenWidth / 2) - 320 + 70, _tick_counter);
		gfx_draw_sprite(screenDPI, SPR_INTRO_CHRIS_SAWYER_10, (screenWidth / 2) - 320 + 320, _tick_counter);

		// Check if logo is at 259 pixels
		if (_tick_counter == 259) {
			// Stop the chain lift sound
			if (_sound_playing_flag == 1) {
				sound_stop(&_prepared_sound);
				_sound_playing_flag = 0;
			}

			// Play the track friction sound
			if (RCT2_GLOBAL(0x009AF280, sint32) != -1) {
				// Prepare and play the sound
				if (sound_prepare(RCT2_SOUND_TRACKFRICTION, &_prepared_sound, 1, 1))
					if (sound_play(&_prepared_sound, 1, -800, 0, 0x3A98))
						_sound_playing_flag = 1;
			}
		}

		// Check if logo is off the screen .ish
		if (_tick_counter >= 680) {
			// Clear the screen
			gfx_clear(screenDPI, 245);

			// Draw RollerCoaster Tycoon 2 logo
			gfx_draw_sprite(screenDPI, SPR_INTRO_LOGO_00, (screenWidth / 2) - 320 + 0, 0);
			gfx_draw_sprite(screenDPI, SPR_INTRO_LOGO_10, (screenWidth / 2) - 320 + 220, 0);
			gfx_draw_sprite(screenDPI, SPR_INTRO_LOGO_20, (screenWidth / 2) - 320 + 440, 0);
			gfx_draw_sprite(screenDPI, SPR_INTRO_LOGO_01, (screenWidth / 2) - 320 + 0, 240);
			gfx_draw_sprite(screenDPI, SPR_INTRO_LOGO_11, (screenWidth / 2) - 320 + 220, 240);
			gfx_draw_sprite(screenDPI, SPR_INTRO_LOGO_21, (screenWidth / 2) - 320 + 440, 240);

			// Set palette thing
			gfx_transpose_palette(23224, 0);

			// Stop the track friction sound
			if (_sound_playing_flag == 1) {
				sound_stop(&_prepared_sound);
				_sound_playing_flag = 0;
			}

			// Play long peep scream sound
			if (RCT2_GLOBAL(0x009AF280, sint32) != -1)
				if (sound_prepare(RCT2_SOUND_SCREAM, &_prepared_sound, 0, 1))
					if (sound_play(&_prepared_sound, 0, 0, 0, 0))
						_sound_playing_flag = 1;

			// Move to the next part
			(*part)++;

			// Set the current fade to 0
			_tick_counter = 0;
		}
		break;
	case 5:
		// Fade in, add 4 / 256 to fading
		_tick_counter += 0x000400;
		if (_tick_counter <= 0x00FF00) {
			// Set palette thing
			gfx_transpose_palette(23224, (_tick_counter >> 8) & 0xFF);
		}
		else {
			// Set palette thing
			gfx_transpose_palette(23224, 255);

			// Move to next part
			(*part)++;

			// Set the fade to 0
			_tick_counter = 0;
		}
		break;
	case 6:
		// Wait 80 game ticks
		_tick_counter++;
		if (_tick_counter >= 80) {
			// Set fading to 256
			_tick_counter = 0x00FF00;

			// Move to next part
			(*part)++;
		}
		break;
	case 7:
		// Fade out, subtract 4 / 256 from fading
		_tick_counter -= 0x000400;
		if (_tick_counter >= 0) {
			// Do palette thing
			gfx_transpose_palette(23224, (_tick_counter >> 8) & 0xFF);
		}
		else {
			// Do palette thing
			gfx_transpose_palette(23224, 0);

			// Finish the intro sequence
			(*part) = 254;
		}
		break;
	case 254:
		// Clear the screen
		gfx_clear(screenDPI, 10);

		// Stop any playing sound
		if (_sound_playing_flag == 1) {
			sound_stop(&_prepared_sound);
			_sound_playing_flag = 0;
		}

		// Move to next part
		(*part)++;
		_tick_counter = 0;
		break;
	case 255:
		// Finish the intro sequence
		(*part) = 0;

		// Change palette
		RCT2_CALLPROC_EBPSAFE(0x006837E3);

		RCT2_GLOBAL(0x009E2C78, sint32) = 0;
		gfx_invalidate_screen();
		break;
	}
}

static void screen_intro_process_mouse_input()
{
	if (gCursorState.any == CURSOR_PRESSED)
		screen_intro_skip_part();
}

/**
 *
 *  rct2: 0x006E3AEC
 */
static void screen_intro_process_keyboard_input()
{
	if (gLastKeyPressed != 0)
		screen_intro_skip_part();
}

static void screen_intro_skip_part()
{
	uint8 (*part) = RCT2_ADDRESS(RCT2_ADDRESS_RUN_INTRO_TICK_PART, uint8);

	switch ((*part)) {
	case 0:
		break;
	case 9:
		(*part) = 1;
		break;
	default:
		(*part) = 254;
		break;
	}
}