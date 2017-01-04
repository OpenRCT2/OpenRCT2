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

#include "../audio/audio.h"
#include "../audio/AudioMixer.h"
#include "../cheats.h"
#include "../config.h"
#include "../drawing/drawing.h"
#include "../game.h"
#include "../interface/window.h"
#include "../localisation/date.h"
#include "../rct2.h"
#include "../scenario/scenario.h"
#include "../sprites.h"
#include "../util/util.h"
#include "climate.h"

enum {
	THUNDER_STATUS_NULL = 0,
	THUNDER_STATUS_PLAYING = 1,

	MAX_THUNDER_INSTANCES = 2
};

#pragma pack(push, 1)
typedef struct rct_weather_transition {
	sint8 base_temperature;
	sint8 distribution_size;
	sint8 distribution[24];
} rct_weather_transition;
assert_struct_size(rct_weather_transition, 26);
#pragma pack(pop)

uint8 gClimate;
uint8 gClimateCurrentWeather;
sint8 gClimateCurrentTemperature;
uint8 gClimateCurrentWeatherEffect;
uint8 gClimateCurrentWeatherGloom;
uint8 gClimateCurrentRainLevel;
uint8 gClimateNextWeather;
sint8 gClimateNextTemperature;
uint8 gClimateNextWeatherEffect;
uint8 gClimateNextWeatherGloom;
uint8 gClimateNextRainLevel;
uint16 gClimateUpdateTimer;

uint16 gClimateLightningFlash;

static const rct_weather_transition* climate_transitions[4];

// Sound data
static int _rainVolume = 1;
static unsigned int _lightningTimer, _thunderTimer;
static void* _thunderSoundChannels[MAX_THUNDER_INSTANCES];
static int _thunderStatus[MAX_THUNDER_INSTANCES] = { THUNDER_STATUS_NULL, THUNDER_STATUS_NULL };
static unsigned int _thunderSoundId;
static int _thunderVolume;
static int _thunderStereoEcho = 0;

static void climate_determine_future_weather(int randomDistribution);

static void climate_update_rain_sound();
static void climate_update_thunder_sound();
static void climate_update_lightning();
static void climate_update_thunder();
static int climate_play_thunder(int instanceIndex, int soundId, int volume, int pan);

int climate_celsius_to_fahrenheit(int celsius)
{
	return (celsius * 29) / 16 + 32;
}

/**
 * Set climate and determine start weather.
 *  rct2: 0x006C45ED
 */
void climate_reset(int climate)
{
	gClimate = climate;

	sint8 month = gDateMonthsElapsed & 7;
	const rct_weather_transition* climate_table = climate_transitions[climate];
	rct_weather_transition transition = climate_table[month];
	sint8 weather = WEATHER_PARTIALLY_CLOUDY;
	gClimateCurrentWeather = weather;
	gClimateCurrentTemperature = transition.base_temperature + climate_weather_data[weather].temp_delta;
	gClimateCurrentWeatherEffect = climate_weather_data[weather].effect_level;
	gClimateCurrentWeatherGloom = climate_weather_data[weather].gloom_level;
	gClimateCurrentRainLevel = climate_weather_data[weather].rain_level;

	_lightningTimer = 0;
	_thunderTimer = 0;
	if (_rainVolume != 1){
		audio_stop_rain_sound();
		_rainVolume = 1;
	}

	climate_determine_future_weather(scenario_rand());
}

static sint8 step_weather_level(sint8 cur_weather_level, sint8 next_weather_level) {
	if (next_weather_level > cur_weather_level) {
		return cur_weather_level + 1;
	} else {
		return cur_weather_level - 1;
	}
}

/**
 * Weather & climate update iteration.
 * Gradually changes the weather parameters towards their determined next values.
 *
 *  rct2: 0x006C46B1
 */
void climate_update()
{
	uint8 screen_flags = gScreenFlags;
	sint8 temperature = gClimateCurrentTemperature,
		target_temperature = gClimateNextTemperature,
		cur_gloom = gClimateCurrentWeatherGloom,
		next_gloom = gClimateNextWeatherGloom,
		cur_rain = gClimateCurrentRainLevel,
		next_rain = gClimateNextRainLevel;

	if (screen_flags & (~SCREEN_FLAGS_PLAYING)) // only normal play mode gets climate
		return;

	if (!gCheatsFreezeClimate) {

		if (gClimateUpdateTimer) {
			if (gClimateUpdateTimer == 960) {
				gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_CLIMATE;
			}
			gClimateUpdateTimer--;
		} else if (!(gCurrentTicks & 0x7F)) {
			if (temperature == target_temperature) {
				if (cur_gloom == next_gloom) {
					gClimateCurrentWeatherEffect = gClimateNextWeatherEffect;
					_thunderTimer = 0;
					_lightningTimer = 0;

					if (cur_rain == next_rain) {
						gClimateCurrentWeather = gClimateNextWeather;
						climate_determine_future_weather(scenario_rand());
						gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_CLIMATE;
					} else if (next_rain <= 2) { // Safe-guard
						gClimateCurrentRainLevel = step_weather_level(cur_rain, next_rain);
					}
				} else {
					gClimateCurrentWeatherGloom = step_weather_level(cur_gloom, next_gloom);
					gfx_invalidate_screen();
				}

			} else {
				gClimateCurrentTemperature = step_weather_level(temperature, target_temperature);
				gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_CLIMATE;
			}
		}

	}

	if (_thunderTimer != 0) {
		climate_update_lightning();
		climate_update_thunder();
	} else if (gClimateCurrentWeatherEffect == 2) {
		// Create new thunder and lightning
		unsigned int randomNumber = util_rand();
		if ((randomNumber & 0xFFFF) <= 0x1B4) {
			randomNumber >>= 16;
			_thunderTimer = 43 + (randomNumber % 64);
			_lightningTimer = randomNumber % 32;
		}
	}
}

void climate_force_weather(uint8 weather){
	gClimateCurrentWeather = weather;
	gClimateCurrentWeatherGloom = climate_weather_data[weather].gloom_level;
	gClimateCurrentRainLevel = climate_weather_data[weather].rain_level;
	gClimateCurrentWeatherEffect = climate_weather_data[weather].effect_level;
	gClimateUpdateTimer = 1920;

	climate_update();

	// In case of change in gloom level force a complete redraw
	gfx_invalidate_screen();
}

/**
 * Calculates future weather development.
 * RCT2 implements this as discrete probability distributions dependant on month and climate
 * for next_weather. The other weather parameters are then looked up depending only on the
 * next weather.
 *
 *  rct2: 0x006C461C
 */
static void climate_determine_future_weather(int randomDistribution)
{
	sint8 climate = gClimate;
	const rct_weather_transition* climate_table = climate_transitions[climate];
	sint8 month = gDateMonthsElapsed & 7;
	rct_weather_transition transition = climate_table[month];

	// Generate a random variable with values 0 up to distribution_size-1 and chose weather from the distribution table accordingly
	sint8 next_weather = transition.distribution[ ((randomDistribution & 0xFF) * transition.distribution_size) >> 8 ];
	gClimateNextWeather = next_weather;

	gClimateNextTemperature = transition.base_temperature + climate_weather_data[next_weather].temp_delta;
	gClimateNextWeatherEffect = climate_weather_data[next_weather].effect_level;
	gClimateNextWeatherGloom = climate_weather_data[next_weather].gloom_level;
	gClimateNextRainLevel = climate_weather_data[next_weather].rain_level;

	gClimateUpdateTimer = 1920;
}

/**
 *
 *  rct2: 0x006BCB91
 */
void climate_update_sound()
{
	if (gAudioCurrentDevice == -1)
		return;
	if (gGameSoundsOff)
		return;
	if (!gConfigSound.sound_enabled)
		return;
	if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
		return;

	climate_update_rain_sound();
	climate_update_thunder_sound();
}

static void climate_update_rain_sound()
{
	if (gClimateCurrentWeatherEffect == 1 || gClimateCurrentWeatherEffect == 2) {
		// Start playing the rain sound
		if (!gRainSoundChannel) {
			gRainSoundChannel = Mixer_Play_Effect(SOUND_RAIN_1, MIXER_LOOP_INFINITE, DStoMixerVolume(-4000), 0.5f, 1, 0);
		}
		if (_rainVolume == 1) {
			_rainVolume = -4000;
		} else {
			// Increase rain sound
			_rainVolume = min(-1400, _rainVolume + 80);
			if (gRainSoundChannel) {
				Mixer_Channel_Volume(gRainSoundChannel, DStoMixerVolume(_rainVolume));
			}
		}
	} else if (_rainVolume != 1) {
		// Decrease rain sound
		_rainVolume -= 80;
		if (_rainVolume > -4000) {
			if (gRainSoundChannel) {
				Mixer_Channel_Volume(gRainSoundChannel, DStoMixerVolume(_rainVolume));
			}
		} else {
			audio_stop_rain_sound();
			_rainVolume = 1;
		}
	}
}

static void climate_update_thunder_sound()
{
	if (_thunderStereoEcho) {
		// Play thunder on right side
		_thunderStereoEcho = 0;
		climate_play_thunder(1, _thunderSoundId, _thunderVolume, 10000);
	}

	// Stop thunder sounds if they have finished
	for (int i = 0; i < MAX_THUNDER_INSTANCES; i++) {
		if (_thunderStatus[i] == THUNDER_STATUS_NULL)
			continue;

		if (!Mixer_Channel_IsPlaying(_thunderSoundChannels[i])) {
			Mixer_Stop_Channel(_thunderSoundChannels[i]);
			_thunderStatus[i] = THUNDER_STATUS_NULL;
		}
	}
}

static void climate_update_lightning()
{
	if (_lightningTimer == 0 || gConfigGeneral.disable_lightning_effect ||
		(!gConfigGeneral.render_weather_effects && !gConfigGeneral.render_weather_gloom))
		return;

	_lightningTimer--;
	if (gClimateLightningFlash == 0)
		if ((util_rand() & 0xFFFF) <= 0x2000)
			gClimateLightningFlash = 1;
}

static void climate_update_thunder()
{
	_thunderTimer--;
	if (_thunderTimer != 0)
		return;

	unsigned int randomNumber = util_rand();
	if (randomNumber & 0x10000) {
		if (_thunderStatus[0] == THUNDER_STATUS_NULL && _thunderStatus[1] == THUNDER_STATUS_NULL) {
			// Play thunder on left side
			_thunderSoundId = (randomNumber & 0x20000) ? SOUND_THUNDER_1 : SOUND_THUNDER_2;
			_thunderVolume = (-((int)((randomNumber >> 18) & 0xFF))) * 8;
			climate_play_thunder(0, _thunderSoundId, _thunderVolume, -10000);

			// Let thunder play on right side
			_thunderStereoEcho = 1;
		}
	} else {
		if (_thunderStatus[0] == THUNDER_STATUS_NULL){
			_thunderSoundId = (randomNumber & 0x20000) ? SOUND_THUNDER_1 : SOUND_THUNDER_2;
			int pan = (((randomNumber >> 18) & 0xFF) - 128) * 16;
			climate_play_thunder(0, _thunderSoundId, 0, pan);
		}
	}
}

static int climate_play_thunder(int instanceIndex, int soundId, int volume, int pan)
{
	_thunderSoundChannels[instanceIndex] = Mixer_Play_Effect(soundId, MIXER_LOOP_NONE, DStoMixerVolume(volume), DStoMixerPan(pan), 1, 0);
	if (_thunderSoundChannels[instanceIndex]) {
		_thunderStatus[instanceIndex] = THUNDER_STATUS_PLAYING;
		return 1;
	}

	return 0;
}

#pragma region Climate / Weather data tables

/** rct2: 0x0098195C */
const FILTER_PALETTE_ID ClimateWeatherGloomColours[4] = {
	0,
	PALETTE_DARKEN_1,
	PALETTE_DARKEN_2,
	PALETTE_DARKEN_3,
};

// rct2: 0x00993C94
// There is actually a sprite at 0x5A9C for snow but only these weather types seem to be fully implemented
const rct_weather climate_weather_data[6] = {
	{ .temp_delta = 10, .effect_level = 0, .gloom_level = 0, .rain_level = 0, .sprite_id = SPR_WEATHER_SUN }, // Sunny
	{ .temp_delta = 5, .effect_level = 0, .gloom_level = 0, .rain_level = 0, .sprite_id = SPR_WEATHER_SUN_CLOUD }, // Partially Cloudy
	{ .temp_delta = 0, .effect_level = 0, .gloom_level = 0, .rain_level = 0, .sprite_id = SPR_WEATHER_CLOUD }, // Cloudy
	{ .temp_delta = -2, .effect_level = 1, .gloom_level = 1, .rain_level = 1, .sprite_id = SPR_WEATHER_LIGHT_RAIN }, // Rain
	{ .temp_delta = -4, .effect_level = 1, .gloom_level = 2, .rain_level = 2, .sprite_id = SPR_WEATHER_HEAVY_RAIN }, // Heavy Rain
	{ .temp_delta = 2, .effect_level = 2, .gloom_level = 2, .rain_level = 2, .sprite_id = SPR_WEATHER_STORM }, // Thunderstorm
};


// rct2: 00993998
static const rct_weather_transition climate_cool_and_wet_transitions[] = {
	{ .base_temperature = 8, .distribution_size = 18,
	.distribution = { 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 10, .distribution_size = 21,
	.distribution = { 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 0, 0 } },
	{ .base_temperature = 14, .distribution_size = 17,
	.distribution = { 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 17, .distribution_size = 17,
	.distribution = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 19, .distribution_size = 23,
	.distribution = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 4 } },
	{ .base_temperature = 20, .distribution_size = 23,
	.distribution = { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 4, 4, 4, 5 } },
	{ .base_temperature = 16, .distribution_size = 19,
	.distribution = { 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 4, 4, 5, 0, 0, 0, 0 } },
	{ .base_temperature = 13, .distribution_size = 16,
	.distribution = { 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 4, 5, 0, 0, 0, 0, 0, 0, 0 } }
};
static const rct_weather_transition climate_warm_transitions[] = {
	{ .base_temperature = 12, .distribution_size = 21,
	.distribution = { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 4, 0, 0 } },
	{ .base_temperature = 13, .distribution_size = 22,
	.distribution = { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 5, 0 } },
	{ .base_temperature = 16, .distribution_size = 17,
	.distribution = { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 19, .distribution_size = 18,
	.distribution = { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 21, .distribution_size = 22,
	.distribution = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 0 } },
	{ .base_temperature = 22, .distribution_size = 17,
	.distribution = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 5, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 19, .distribution_size = 17,
	.distribution = { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 16, .distribution_size = 17,
	.distribution = { 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 0, 0, 0, 0, 0, 0 } }
};
static const rct_weather_transition climate_hot_and_dry_transitions[] = {
	{ .base_temperature = 12, .distribution_size = 15,
	.distribution = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 14, .distribution_size = 12,
	.distribution = { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 16, .distribution_size = 11,
	.distribution = { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 19, .distribution_size = 9,
	.distribution = { 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 21, .distribution_size = 13,
	.distribution = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 22, .distribution_size = 11,
	.distribution = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 21, .distribution_size = 12,
	.distribution = { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 16, .distribution_size = 13,
	.distribution = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
};
static const rct_weather_transition climate_cold_transitions[] = {
	{ .base_temperature = 4, .distribution_size = 18,
	.distribution = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 4, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 5, .distribution_size = 21,
	.distribution = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 4, 5, 0, 0 } },
	{ .base_temperature = 7, .distribution_size = 17,
	.distribution = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 4, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 9, .distribution_size = 17,
	.distribution = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 10, .distribution_size = 23,
	.distribution = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 4 } },
	{ .base_temperature = 11, .distribution_size = 23,
	.distribution = { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 4, 5 } },
	{ .base_temperature = 9, .distribution_size = 19,
	.distribution = { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 4, 5, 0, 0, 0, 0 } },
	{ .base_temperature = 6, .distribution_size = 16,
	.distribution = { 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 4, 5, 0, 0, 0, 0, 0, 0, 0 } }
};

static const rct_weather_transition* climate_transitions[] = {
	climate_cool_and_wet_transitions,
	climate_warm_transitions,
	climate_hot_and_dry_transitions,
	climate_cold_transitions
};

#pragma endregion
