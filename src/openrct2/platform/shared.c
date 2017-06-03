#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include <stdlib.h>
#include <time.h>
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../Context.h"
#include "../drawing/drawing.h"
#include "../drawing/IDrawingEngine.h"
#include "../drawing/lightfx.h"
#include "../editor.h"
#include "../game.h"
#include "../input.h"
#include "../interface/console.h"
#include "../interface/Cursors.h"
#include "../interface/keyboard_shortcut.h"
#include "../interface/window.h"
#include "../localisation/currency.h"
#include "../localisation/localisation.h"
#include "../OpenRCT2.h"
#include "../rct2.h"
#include "../title/TitleScreen.h"
#include "../util/util.h"
#include "../Version.h"
#include "../world/Climate.h"
#include "platform.h"

#ifdef __APPLE__
	#include <mach/mach_time.h>
	#include <AvailabilityMacros.h>
	#ifndef __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
		#error Missing __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ define
	#endif
#endif

#if defined(__APPLE__) && (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 101200)
	mach_timebase_info_data_t _mach_base_info = { 0 };
#endif

typedef void(*update_palette_func)(const uint8*, sint32, sint32);

rct_palette_entry gPalette[256];

void platform_draw()
{
	if (!gOpenRCT2Headless) {
		drawing_engine_draw();
	}
}

static uint8 soft_light(uint8 a, uint8 b)
{
	float fa = a / 255.0f;
	float fb = b / 255.0f;
	float fr;
	if (fb < 0.5f) {
		fr = (2 * fa * fb) + ((fa * fa) * (1 - (2 * fb)));
	} else {
		fr = (2 * fa * (1 - fb)) + (sqrtf(fa) * ((2 * fb) - 1));
	}
	return (uint8)(clamp(0.0f, fr, 1.0f) * 255.0f);
}

static uint8 lerp(uint8 a, uint8 b, float t)
{
	if (t <= 0) return a;
	if (t >= 1) return b;

	sint32 range = b - a;
	sint32 amount = (sint32)(range * t);
	return (uint8)(a + amount);
}

void platform_update_palette(const uint8* colours, sint32 start_index, sint32 num_colours)
{
	colours += start_index * 4;

	for (sint32 i = start_index; i < num_colours + start_index; i++) {
		uint8 r = colours[2];
		uint8 g = colours[1];
		uint8 b = colours[0];

#ifdef __ENABLE_LIGHTFX__
		if (gConfigGeneral.enable_light_fx)
		{
			lightfx_apply_palette_filter(i, &r, &g, &b);
		}
		else
#endif
		{
			float night = gDayNightCycle;
			if (night >= 0 && gClimateLightningFlash == 0) {
				r = lerp(r, soft_light(r, 8), night);
				g = lerp(g, soft_light(g, 8), night);
				b = lerp(b, soft_light(b, 128), night);
			}
		}

		gPalette[i].red = r;
		gPalette[i].green = g;
		gPalette[i].blue = b;
		gPalette[i].alpha = 0;
		colours += 4;
	}

	if (!gOpenRCT2Headless) {
		drawing_engine_set_palette(gPalette);
	}
}

void platform_init()
{
	// gKeysPressed = malloc(sizeof(uint8) * 256);
	// memset(gKeysPressed, 0, sizeof(uint8) * 256);

	// Set the highest palette entry to white.
	// This fixes a bug with the TT:rainbow road due to the
	// image not using the correct white palette entry.
	gPalette[255].alpha = 0;
	gPalette[255].red = 255;
	gPalette[255].green = 255;
	gPalette[255].blue = 255;
}

sint32 platform_scancode_to_rct_keycode(sint32 sdl_key)
{
	char keycode = (char)SDL_GetKeyFromScancode((SDL_Scancode)sdl_key);

	// Until we reshuffle the text files to use the new positions
	// this will suffice to move the majority to the correct positions.
	// Note any special buttons PgUp PgDwn are mapped wrong.
	if (keycode >= 'a' && keycode <= 'z')
		keycode = toupper(keycode);

	return keycode;
}

void platform_free()
{
	// free(gKeysPressed);
}

void platform_toggle_windowed_mode()
{
	sint32 targetMode = gConfigGeneral.fullscreen_mode == 0 ? 2 : 0;
	context_set_fullscreen_mode(targetMode);
	gConfigGeneral.fullscreen_mode = targetMode;
	config_save_default();
}

void platform_refresh_video()
{
	SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, gConfigGeneral.minimize_fullscreen_focus_loss ? "1" : "0");

	drawing_engine_dispose();
	drawing_engine_init();
	drawing_engine_resize();
	drawing_engine_set_palette(gPalette);
	gfx_invalidate_screen();
}

uint32 platform_get_ticks()
{
#ifdef _WIN32
    return GetTickCount();
#elif defined(__APPLE__) && (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 101200)
	return (uint32)(((mach_absolute_time() * _mach_base_info.numer) / _mach_base_info.denom) / 1000000);
#else
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        log_fatal("clock_gettime failed");
        exit(-1);
    }
    return (uint32)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
}

void platform_sleep(uint32 ms)
{
    SDL_Delay(ms);
}

uint8 platform_get_currency_value(const char *currCode) {
	if (currCode == NULL || strlen(currCode) < 3) {
			return CURRENCY_POUNDS;
	}

	for (sint32 currency = 0; currency < CURRENCY_END; ++currency) {
		if (strncmp(currCode, CurrencyDescriptors[currency].isoCode, 3) == 0) {
			return currency;
		}
	}

	return CURRENCY_POUNDS;
}

void core_init()
{
	static bool initialised = false;
	if (!initialised)
	{
		initialised = true;

		bitcount_init();

#if defined(__APPLE__) && (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 101200)
		kern_return_t ret = mach_timebase_info(&_mach_base_info);
		if (ret != 0) {
			log_fatal("Unable to get mach_timebase_info.");
			exit(-1);
		}
#endif
	}
}
