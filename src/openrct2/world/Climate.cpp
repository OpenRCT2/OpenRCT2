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

#include "../config/Config.h"
#include "../core/Math.hpp"
#include "Climate.h"

extern "C"
{
    #include "../audio/audio.h"
    #include "../audio/AudioMixer.h"
    #include "../cheats.h"
    #include "../drawing/drawing.h"
    #include "../game.h"
    #include "../interface/window.h"
    #include "../localisation/date.h"
    #include "../rct2.h"
    #include "../scenario/scenario.h"
    #include "../sprites.h"
    #include "../util/util.h"
}

constexpr sint32 MAX_THUNDER_INSTANCES = 2;

enum class THUNDER_STATUS
{
    NONE,
    PLAYING,
};

struct WeatherTransition
{
    sint8 BaseTemperature;
    sint8 DistributionSize;
    sint8 Distribution[24];
};

extern const WeatherTransition * ClimateTransitions[4];

// Climate data
uint8   gClimate;
uint8   gClimateCurrentWeather;
sint8   gClimateCurrentTemperature;
uint8   gClimateCurrentWeatherEffect;
uint8   gClimateCurrentWeatherGloom;
uint8   gClimateCurrentRainLevel;
uint8   gClimateNextWeather;
sint8   gClimateNextTemperature;
uint8   gClimateNextWeatherEffect;
uint8   gClimateNextWeatherGloom;
uint8   gClimateNextRainLevel;
uint16  gClimateUpdateTimer;
uint16  gClimateLightningFlash;

// Sound data
static sint32           _rainVolume = 1;
static uint32           _lightningTimer;
static uint32           _thunderTimer;
static void *           _thunderSoundChannels[MAX_THUNDER_INSTANCES];
static THUNDER_STATUS   _thunderStatus[MAX_THUNDER_INSTANCES] = { THUNDER_STATUS::NONE, THUNDER_STATUS::NONE };
static uint32           _thunderSoundId;
static sint32           _thunderVolume;
static sint32           _thunderStereoEcho = 0;

static sint8 climate_step_weather_level(sint8 currentWeatherLevel, sint8 nextWeatherLevel);
static void climate_determine_future_weather(sint32 randomDistribution);
static void climate_update_rain_sound();
static void climate_update_thunder_sound();
static void climate_update_lightning();
static void climate_update_thunder();
static void climate_play_thunder(sint32 instanceIndex, sint32 soundId, sint32 volume, sint32 pan);

extern "C"
{
    sint32 climate_celsius_to_fahrenheit(sint32 celsius)
    {
        return (celsius * 29) / 16 + 32;
    }

    /**
     * Set climate and determine start weather.
     */
    void climate_reset(sint32 climate)
    {
        uint8 weather = WEATHER_PARTIALLY_CLOUDY;
        sint32 month = date_get_month(gDateMonthsElapsed);
        const WeatherTransition * transition = &ClimateTransitions[climate][month];
        const WeatherState * weatherState = &ClimateWeatherData[weather];

        gClimate = climate;
        gClimateCurrentWeather = weather;
        gClimateCurrentTemperature = transition->BaseTemperature + weatherState->TemperatureDelta;
        gClimateCurrentWeatherEffect = weatherState->EffectLevel;
        gClimateCurrentWeatherGloom = weatherState->GloomLevel;
        gClimateCurrentRainLevel = weatherState->RainLevel;

        _lightningTimer = 0;
        _thunderTimer = 0;
        if (_rainVolume != 1)
        {
            audio_stop_rain_sound();
            _rainVolume = 1;
        }

        climate_determine_future_weather(scenario_rand());
    }

    /**
     * Weather & climate update iteration.
     * Gradually changes the weather parameters towards their determined next values.
     */
    void climate_update()
    {
        // Only do climate logic if playing (not in scenario editor or title screen)
        if (gScreenFlags & (~SCREEN_FLAGS_PLAYING)) return;

        if (!gCheatsFreezeClimate)
        {
            if (gClimateUpdateTimer)
            {
                if (gClimateUpdateTimer == 960)
                {
                    gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_CLIMATE;
                }
                gClimateUpdateTimer--;
            }
            else if (!(gCurrentTicks & 0x7F))
            {
                if (gClimateCurrentTemperature == gClimateNextTemperature)
                {
                    if (gClimateCurrentWeatherGloom == gClimateNextWeatherGloom)
                    {
                        gClimateCurrentWeatherEffect = gClimateNextWeatherEffect;
                        _thunderTimer = 0;
                        _lightningTimer = 0;

                        if (gClimateCurrentRainLevel == gClimateNextRainLevel)
                        {
                            gClimateCurrentWeather = gClimateNextWeather;
                            climate_determine_future_weather(scenario_rand());
                            gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_CLIMATE;
                        }
                        else if (gClimateNextRainLevel <= 2)
                        {
                            gClimateCurrentRainLevel = climate_step_weather_level(gClimateCurrentRainLevel, gClimateNextRainLevel);
                        }
                    }
                    else
                    {
                        gClimateCurrentWeatherGloom = climate_step_weather_level(gClimateCurrentWeatherGloom, gClimateNextWeatherGloom);
                        gfx_invalidate_screen();
                    }
                }
                else
                {
                    gClimateCurrentTemperature = climate_step_weather_level(gClimateCurrentTemperature, gClimateNextTemperature);
                    gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_CLIMATE;
                }
            }

        }

        if (_thunderTimer != 0)
        {
            climate_update_lightning();
            climate_update_thunder();
        }
        else if (gClimateCurrentWeatherEffect == 2)
        {
            // Create new thunder and lightning
            uint32 randomNumber = util_rand();
            if ((randomNumber & 0xFFFF) <= 0x1B4)
            {
                randomNumber >>= 16;
                _thunderTimer = 43 + (randomNumber % 64);
                _lightningTimer = randomNumber % 32;
            }
        }
    }

    void climate_force_weather(uint8 weather)
    {
        const auto weatherState = &ClimateWeatherData[weather];
        gClimateCurrentWeather = weather;
        gClimateCurrentWeatherGloom = weatherState->GloomLevel;
        gClimateCurrentRainLevel = weatherState->RainLevel;
        gClimateCurrentWeatherEffect = weatherState->EffectLevel;
        gClimateUpdateTimer = 1920;

        climate_update();

        // In case of change in gloom level force a complete redraw
        gfx_invalidate_screen();
    }


    void climate_update_sound()
    {
        if (gAudioCurrentDevice == -1) return;
        if (gGameSoundsOff) return;
        if (!gConfigSound.sound_enabled) return;
        if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) return;

        climate_update_rain_sound();
        climate_update_thunder_sound();
    }
}

static sint8 climate_step_weather_level(sint8 currentWeatherLevel, sint8 nextWeatherLevel)
{
    if (nextWeatherLevel > currentWeatherLevel)
    {
        return currentWeatherLevel + 1;
    }
    else
    {
        return currentWeatherLevel - 1;
    }
}

/**
* Calculates future weather development.
* RCT2 implements this as discrete probability distributions dependant on month and climate
* for nextWeather. The other weather parameters are then looked up depending only on the
* next weather.
*/
static void climate_determine_future_weather(sint32 randomDistribution)
{
    sint8 month = date_get_month(gDateMonthsElapsed);

    // Generate a random variable with values 0 up to DistributionSize-1 and chose weather from the distribution table accordingly
    const WeatherTransition * transition = &ClimateTransitions[gClimate][month];
    sint8 nextWeather = transition->Distribution[((randomDistribution & 0xFF) * transition->DistributionSize) >> 8];
    gClimateNextWeather = nextWeather;

    const auto nextWeatherState = &ClimateWeatherData[nextWeather];
    gClimateNextTemperature = transition->BaseTemperature + nextWeatherState->TemperatureDelta;
    gClimateNextWeatherEffect = nextWeatherState->EffectLevel;
    gClimateNextWeatherGloom = nextWeatherState->GloomLevel;
    gClimateNextRainLevel = nextWeatherState->RainLevel;

    gClimateUpdateTimer = 1920;
}

static void climate_update_rain_sound()
{
    if (gClimateCurrentWeatherEffect == 1 || gClimateCurrentWeatherEffect == 2)
    {
        // Start playing the rain sound
        if (gRainSoundChannel == nullptr)
        {
            gRainSoundChannel = Mixer_Play_Effect(SOUND_RAIN_1, MIXER_LOOP_INFINITE, DStoMixerVolume(-4000), 0.5f, 1, 0);
        }
        if (_rainVolume == 1)
        {
            _rainVolume = -4000;
        }
        else
        {
            // Increase rain sound
            _rainVolume = Math::Min(-1400, _rainVolume + 80);
            if (gRainSoundChannel != nullptr)
            {
                Mixer_Channel_Volume(gRainSoundChannel, DStoMixerVolume(_rainVolume));
            }
        }
    }
    else if (_rainVolume != 1)
    {
        // Decrease rain sound
        _rainVolume -= 80;
        if (_rainVolume > -4000)
        {
            if (gRainSoundChannel != nullptr)
            {
                Mixer_Channel_Volume(gRainSoundChannel, DStoMixerVolume(_rainVolume));
            }
        }
        else
        {
            audio_stop_rain_sound();
            _rainVolume = 1;
        }
    }
}

static void climate_update_thunder_sound()
{
    if (_thunderStereoEcho)
    {
        // Play thunder on right side
        _thunderStereoEcho = 0;
        climate_play_thunder(1, _thunderSoundId, _thunderVolume, 10000);
    }

    // Stop thunder sounds if they have finished
    for (sint32 i = 0; i < MAX_THUNDER_INSTANCES; i++)
    {
        if (_thunderStatus[i] != THUNDER_STATUS::NONE)
        {
            void * channel = _thunderSoundChannels[i];
            if (!Mixer_Channel_IsPlaying(channel))
            {
                Mixer_Stop_Channel(channel);
                _thunderStatus[i] = THUNDER_STATUS::NONE;
            }
        }
    }
}

static void climate_update_lightning()
{
    if (_lightningTimer == 0) return;
    if (gConfigGeneral.disable_lightning_effect) return;
    if (!gConfigGeneral.render_weather_effects && !gConfigGeneral.render_weather_gloom) return;

    _lightningTimer--;
    if (gClimateLightningFlash == 0)
    {
        if ((util_rand() & 0xFFFF) <= 0x2000)
        {
            gClimateLightningFlash = 1;
        }
    }
}

static void climate_update_thunder()
{
    _thunderTimer--;
    if (_thunderTimer == 0)
    {
        uint32 randomNumber = util_rand();
        if (randomNumber & 0x10000)
        {
            if (_thunderStatus[0] == THUNDER_STATUS::NONE &&
                _thunderStatus[1] == THUNDER_STATUS::NONE)
            {
                // Play thunder on left side
                _thunderSoundId = (randomNumber & 0x20000) ? SOUND_THUNDER_1 : SOUND_THUNDER_2;
                _thunderVolume = (-((sint32)((randomNumber >> 18) & 0xFF))) * 8;
                climate_play_thunder(0, _thunderSoundId, _thunderVolume, -10000);

                // Let thunder play on right side
                _thunderStereoEcho = 1;
            }
        }
        else
        {
            if (_thunderStatus[0] == THUNDER_STATUS::NONE)
            {
                _thunderSoundId = (randomNumber & 0x20000) ? SOUND_THUNDER_1 : SOUND_THUNDER_2;
                sint32 pan = (((randomNumber >> 18) & 0xFF) - 128) * 16;
                climate_play_thunder(0, _thunderSoundId, 0, pan);
            }
        }
    }
}

static void climate_play_thunder(sint32 instanceIndex, sint32 soundId, sint32 volume, sint32 pan)
{
    _thunderSoundChannels[instanceIndex] = Mixer_Play_Effect(soundId, MIXER_LOOP_NONE, DStoMixerVolume(volume), DStoMixerPan(pan), 1, 0);
    if (_thunderSoundChannels[instanceIndex] != nullptr)
    {
        _thunderStatus[instanceIndex] = THUNDER_STATUS::PLAYING;
    }
}

#pragma region Climate / Weather data tables

const FILTER_PALETTE_ID ClimateWeatherGloomColours[4] =
{
    (FILTER_PALETTE_ID)0,
    PALETTE_DARKEN_1,
    PALETTE_DARKEN_2,
    PALETTE_DARKEN_3,
};

// There is actually a sprite at 0x5A9C for snow but only these weather types seem to be fully implemented
const WeatherState ClimateWeatherData[6] =
{
    { 10, 0, 0, 0, SPR_WEATHER_SUN          }, // Sunny
    {  5, 0, 0, 0, SPR_WEATHER_SUN_CLOUD    }, // Partially Cloudy
    {  0, 0, 0, 0, SPR_WEATHER_CLOUD        }, // Cloudy
    { -2, 1, 1, 1, SPR_WEATHER_LIGHT_RAIN   }, // Rain
    { -4, 1, 2, 2, SPR_WEATHER_HEAVY_RAIN   }, // Heavy Rain
    {  2, 2, 2, 2, SPR_WEATHER_STORM        }, // Thunderstorm
};

static const WeatherTransition ClimateTransitionsCoolAndWet[] =
{
    {  8, 18, { 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 0, 0, 0, 0, 0 } },
    { 10, 21, { 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 0, 0 } },
    { 14, 17, { 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 0, 0, 0, 0, 0, 0 } },
    { 17, 17, { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 0, 0, 0, 0, 0, 0 } },
    { 19, 23, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 4 } },
    { 20, 23, { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 4, 4, 4, 5 } },
    { 16, 19, { 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 4, 4, 5, 0, 0, 0, 0 } },
    { 13, 16, { 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 4, 5, 0, 0, 0, 0, 0, 0, 0 } },
};
static const WeatherTransition ClimateTransitionsWarm[] = {
    { 12, 21, { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 4, 0, 0 } },
    { 13, 22, { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 5, 0 } },
    { 16, 17, { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 0, 0, 0, 0, 0, 0 } },
    { 19, 18, { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 0, 0, 0, 0, 0 } },
    { 21, 22, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 0 } },
    { 22, 17, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 5, 0, 0, 0, 0, 0, 0 } },
    { 19, 17, { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 0, 0, 0, 0, 0, 0 } },
    { 16, 17, { 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 0, 0, 0, 0, 0, 0 } },
};
static const WeatherTransition ClimateTransitionsHotAndDry[] = {
    { 12, 15, { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 14, 12, { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 16, 11, { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 19,  9, { 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 21, 13, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 22, 11, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 21, 12, { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 16, 13, { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
};
static const WeatherTransition ClimateTransitionsCold[] = {
    {  4, 18, { 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 4, 0, 0, 0, 0, 0 } },
    {  5, 21, { 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 4, 5, 0, 0 } },
    {  7, 17, { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 4, 0, 0, 0, 0, 0, 0 } },
    {  9, 17, { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 0, 0, 0, 0, 0, 0 } },
    { 10, 23, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 4 } },
    { 11, 23, { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 4, 5 } },
    {  9, 19, { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 4, 5, 0, 0, 0, 0 } },
    {  6, 16, { 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 4, 5, 0, 0, 0, 0, 0, 0, 0 } },
};

const WeatherTransition * ClimateTransitions[] =
{
    ClimateTransitionsCoolAndWet,
    ClimateTransitionsWarm,
    ClimateTransitionsHotAndDry,
    ClimateTransitionsCold,
};

#pragma endregion
