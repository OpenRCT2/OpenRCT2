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

#include "addresses.h"
#include "audio/audio.h"
#include "audio/mixer.h"
#include "drawing/drawing.h"
#include "intro.h"
#include "localisation/localisation.h"
#include "platform/platform.h"
#include "sprites.h"

#define BACKROUND_COLOUR_DARK		10
#define BACKROUND_COLOUR_LOGO		245
#define BORDER_COLOUR_PUBLISHER		129

#define PALETTE_G1_IDX_DEVELOPER	23217
#define PALETTE_G1_IDX_LOGO			23224

uint8 gIntroState;

// Used mainly for timing but also for Y coordinate and fading.
static int _introStateCounter;

static void *_soundChannel = NULL;
static bool _chainLiftFinished;

static void screen_intro_process_mouse_input();
static void screen_intro_process_keyboard_input();
static void screen_intro_skip_part();
static void screen_intro_draw_logo();

// rct2: 0x0068E966
void intro_update()
{
	rct_drawpixelinfo *screenDPI = &gScreenDPI;
	int screenWidth = gScreenWidth;

	screen_intro_process_mouse_input();
	screen_intro_process_keyboard_input();

	switch (gIntroState) {
	case INTRO_STATE_DISCLAIMER_1:
	case INTRO_STATE_DISCLAIMER_2:
		// Originally used for the disclaimer text
		gIntroState = INTRO_STATE_PUBLISHER_BEGIN;
	case INTRO_STATE_PUBLISHER_BEGIN:
		gfx_clear(screenDPI, BACKROUND_COLOUR_DARK);

		// Set the Y for the Infogrammes logo
		_introStateCounter = -580;

		// Play the chain lift sound
		_soundChannel = Mixer_Play_Effect(SOUND_LIFT_7, MIXER_LOOP_INFINITE, SDL_MIX_MAXVOLUME, 0.5f, 1, true);
		_chainLiftFinished = false;
		gIntroState++;
		break;
	case INTRO_STATE_PUBLISHER_SCROLL:
		// Move the Infogrammes logo down
		_introStateCounter += 5;

		gfx_clear(screenDPI, BACKROUND_COLOUR_DARK);

		// Draw a white rectangle for the logo background (gives a bit of white margin)
		gfx_fill_rect(screenDPI,
			(screenWidth / 2) - 320 + 50, _introStateCounter + 50,
			(screenWidth / 2) - 320 + 50 + 540, _introStateCounter + 50 + 425,
			BORDER_COLOUR_PUBLISHER);

		// Draw Infogrames logo
		gfx_draw_sprite(screenDPI, SPR_INTRO_INFOGRAMES_00, (screenWidth / 2) - 320 + 69, _introStateCounter + 69, 0);
		gfx_draw_sprite(screenDPI, SPR_INTRO_INFOGRAMES_10, (screenWidth / 2) - 320 + 319, _introStateCounter + 69, 0);
		gfx_draw_sprite(screenDPI, SPR_INTRO_INFOGRAMES_01, (screenWidth / 2) - 320 + 69, _introStateCounter + 319, 0);
		gfx_draw_sprite(screenDPI, SPR_INTRO_INFOGRAMES_11, (screenWidth / 2) - 320 + 319, _introStateCounter + 319, 0);

		// Check if logo is off the screen...ish
		if (_introStateCounter > gScreenHeight - 120) {
			gfx_clear(screenDPI, BACKROUND_COLOUR_DARK);
			_introStateCounter = -116;
			gIntroState++;
		}

		break;
	case INTRO_STATE_DEVELOPER_BEGIN:
		_introStateCounter += 5;
		gfx_clear(screenDPI, BACKROUND_COLOUR_DARK);
		gfx_transpose_palette(PALETTE_G1_IDX_DEVELOPER, 255);

		// Set the Y for the Chris Sawyer logo
		_introStateCounter = -116;

		gIntroState++;
		break;
	case INTRO_STATE_DEVELOPER_SCROLL:
		_introStateCounter += 5;
		gfx_clear(screenDPI, BACKROUND_COLOUR_DARK);

		// Draw Chris Sawyer logo
		gfx_draw_sprite(screenDPI, SPR_INTRO_CHRIS_SAWYER_00, (screenWidth / 2) - 320 + 70, _introStateCounter, 0);
		gfx_draw_sprite(screenDPI, SPR_INTRO_CHRIS_SAWYER_10, (screenWidth / 2) - 320 + 320, _introStateCounter, 0);

		// Check if logo is almost scrolled to the bottom
		if (!_chainLiftFinished && _introStateCounter >= gScreenHeight + 40 - 421) {
			_chainLiftFinished = true;

			// Stop the chain lift sound
			if (_soundChannel != NULL) {
				Mixer_Stop_Channel(_soundChannel);
				_soundChannel = NULL;
			}

			// Play the track friction sound
			_soundChannel = Mixer_Play_Effect(SOUND_TRACK_FRICTION_3, MIXER_LOOP_INFINITE, SDL_MIX_MAXVOLUME, 0.25f, 0.75, true);
		}

		// Check if logo is off the screen...ish
		if (_introStateCounter >= gScreenHeight + 40) {
			gfx_transpose_palette(PALETTE_G1_IDX_LOGO, 0);

			screen_intro_draw_logo();

			// Stop the track friction sound
			if (_soundChannel != NULL) {
				Mixer_Stop_Channel(_soundChannel);
				_soundChannel = NULL;
			}

			// Play long peep scream sound
			_soundChannel = Mixer_Play_Effect(SOUND_SCREAM_1, MIXER_LOOP_NONE, SDL_MIX_MAXVOLUME, 0.5f, 1, true);

			gIntroState++;
			_introStateCounter = 0;
		}
		break;
	case INTRO_STATE_LOGO_FADE_IN:
		// Fade in, add 4 / 256 to fading
		_introStateCounter += 0x400;
		if (_introStateCounter <= 0xFF00) {
			gfx_transpose_palette(PALETTE_G1_IDX_LOGO, (_introStateCounter >> 8) & 0xFF);
		} else {
			gfx_transpose_palette(PALETTE_G1_IDX_LOGO, 255);
			gIntroState++;
			_introStateCounter = 0;
		}
		screen_intro_draw_logo();
		break;
	case INTRO_STATE_LOGO_WAIT:
		// Wait 80 game ticks
		_introStateCounter++;
		if (_introStateCounter >= 80) {
			// Set fading to 256
			_introStateCounter = 0xFF00;

			gIntroState++;
		}
		screen_intro_draw_logo();
		break;
	case INTRO_STATE_LOGO_FADE_OUT:
		// Fade out, subtract 4 / 256 from fading
		_introStateCounter -= 0x400;
		if (_introStateCounter >= 0) {
			// Do palette thing
			gfx_transpose_palette(PALETTE_G1_IDX_LOGO, (_introStateCounter >> 8) & 0xFF);
		} else {
			gfx_transpose_palette(PALETTE_G1_IDX_LOGO, 0);
			gIntroState = INTRO_STATE_CLEAR;
		}
		screen_intro_draw_logo();
		break;
	case INTRO_STATE_CLEAR:
		gfx_clear(screenDPI, BACKROUND_COLOUR_DARK);

		// Stop any playing sound
		if (_soundChannel != NULL) {
			Mixer_Stop_Channel(_soundChannel);
			_soundChannel = NULL;
		}

		// Move to next part
		gIntroState++;
		_introStateCounter = 0;
		break;
	case INTRO_STATE_FINISH:
		gIntroState = INTRO_STATE_NONE;
		load_palette();
		gfx_invalidate_screen();
		break;
	}
}

static void screen_intro_process_mouse_input()
{
	if (gCursorState.any == CURSOR_PRESSED) {
		screen_intro_skip_part();
	}
}

/**
 *
 *  rct2: 0x006E3AEC
 */
static void screen_intro_process_keyboard_input()
{
	if (gLastKeyPressed != 0) {
		screen_intro_skip_part();
	}
}

static void screen_intro_skip_part()
{
	switch (gIntroState) {
	case INTRO_STATE_NONE:
		break;
	case INTRO_STATE_DISCLAIMER_2:
		gIntroState = INTRO_STATE_PUBLISHER_BEGIN;
		break;
	default:
		gIntroState = INTRO_STATE_CLEAR;
		break;
	}
}

static void screen_intro_draw_logo()
{
	rct_drawpixelinfo *screenDPI = &gScreenDPI;
	sint32 screenWidth = gScreenWidth;
	sint32 imageWidth = 640;
	sint32 imageX = (screenWidth - imageWidth) / 2;

	gfx_clear(screenDPI, BACKROUND_COLOUR_LOGO);
	gfx_draw_sprite(screenDPI, SPR_INTRO_LOGO_00, imageX + 0,     0, 0);
	gfx_draw_sprite(screenDPI, SPR_INTRO_LOGO_10, imageX + 220,   0, 0);
	gfx_draw_sprite(screenDPI, SPR_INTRO_LOGO_20, imageX + 440,   0, 0);
	gfx_draw_sprite(screenDPI, SPR_INTRO_LOGO_01, imageX + 0,   240, 0);
	gfx_draw_sprite(screenDPI, SPR_INTRO_LOGO_11, imageX + 220, 240, 0);
	gfx_draw_sprite(screenDPI, SPR_INTRO_LOGO_21, imageX + 440, 240, 0);
}
