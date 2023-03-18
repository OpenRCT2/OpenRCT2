/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Climate.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Game.h"
#include "../OpenRCT2.h"
#include "../audio/AudioChannel.h"
#include "../audio/AudioMixer.h"
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../drawing/Drawing.h"
#include "../interface/Window.h"
#include "../localisation/Date.h"
#include "../profiling/Profiling.h"
#include "../scenario/Scenario.h"
#include "../sprites.h"
#include "../util/Util.h"
#include "../windows/Intent.h"

#include <algorithm>
#include <iterator>
#include <memory>

using namespace OpenRCT2::Audio;

constexpr int32_t MAX_THUNDER_INSTANCES = 2;

enum class THUNDER_STATUS
{
    NONE,
    PLAYING,
};

struct WeatherTransition
{
    int8_t BaseTemperature;
    int8_t DistributionSize;
    WeatherType Distribution[24];
};

extern const WeatherTransition* ClimateTransitions[4];
extern const WeatherState ClimateWeatherData[EnumValue(WeatherType::Count)];
extern const FilterPaletteID ClimateWeatherGloomColours[4];

// Climate data
ClimateType gClimate;
ClimateState gClimateCurrent;
ClimateState gClimateNext;
uint16_t gClimateUpdateTimer;
uint16_t gClimateLightningFlash;

// Sound data
static int32_t _weatherVolume = 1;
static uint32_t _lightningTimer;
static uint32_t _thunderTimer;
static std::shared_ptr<IAudioChannel> _thunderSoundChannels[MAX_THUNDER_INSTANCES];
static THUNDER_STATUS _thunderStatus[MAX_THUNDER_INSTANCES] = {
    THUNDER_STATUS::NONE,
    THUNDER_STATUS::NONE,
};
static OpenRCT2::Audio::SoundId _thunderSoundId;
static int32_t _thunderVolume;
static int32_t _thunderStereoEcho = 0;
static std::shared_ptr<IAudioChannel> _weatherSoundChannel;

static int8_t ClimateStepWeatherLevel(int8_t currentWeatherLevel, int8_t nextWeatherLevel);
static void ClimateDetermineFutureWeather(int32_t randomDistribution);
static void ClimateUpdateWeatherSound();
static void ClimateUpdateThunderSound();
static void ClimateUpdateLightning();
static void ClimateUpdateThunder();
static void ClimatePlayThunder(int32_t instanceIndex, OpenRCT2::Audio::SoundId soundId, int32_t volume, int32_t pan);

int32_t ClimateCelsiusToFahrenheit(int32_t celsius)
{
    return (celsius * 29) / 16 + 32;
}

/**
 * Set climate and determine start weather.
 */
void ClimateReset(ClimateType climate)
{
    auto weather = WeatherType::PartiallyCloudy;
    int32_t month = gDate.GetMonth();
    const WeatherTransition* transition = &ClimateTransitions[static_cast<uint8_t>(climate)][month];
    const WeatherState* weatherState = &ClimateWeatherData[EnumValue(weather)];

    gClimate = climate;
    gClimateCurrent.Weather = weather;
    gClimateCurrent.Temperature = transition->BaseTemperature + weatherState->TemperatureDelta;
    gClimateCurrent.WeatherEffect = weatherState->EffectLevel;
    gClimateCurrent.WeatherGloom = weatherState->GloomLevel;
    gClimateCurrent.Level = weatherState->Level;

    _lightningTimer = 0;
    _thunderTimer = 0;
    if (_weatherVolume != 1)
    {
        ClimateStopWeatherSound();
        _weatherVolume = 1;
    }

    ClimateDetermineFutureWeather(ScenarioRand());
}

/**
 * Weather & climate update iteration.
 * Gradually changes the weather parameters towards their determined next values.
 */
void ClimateUpdate()
{
    PROFILED_FUNCTION();

    // Only do climate logic if playing (not in scenario editor or title screen)
    if (gScreenFlags & (~SCREEN_FLAGS_PLAYING))
        return;

    if (!gCheatsFreezeWeather)
    {
        if (gClimateUpdateTimer)
        {
            if (gClimateUpdateTimer == 960)
            {
                auto intent = Intent(INTENT_ACTION_UPDATE_CLIMATE);
                ContextBroadcastIntent(&intent);
            }
            gClimateUpdateTimer--;
        }
        else if (!(gCurrentTicks & 0x7F))
        {
            if (gClimateCurrent.Temperature == gClimateNext.Temperature)
            {
                if (gClimateCurrent.WeatherGloom == gClimateNext.WeatherGloom)
                {
                    gClimateCurrent.WeatherEffect = gClimateNext.WeatherEffect;
                    _thunderTimer = 0;
                    _lightningTimer = 0;

                    if (gClimateCurrent.Level == gClimateNext.Level)
                    {
                        gClimateCurrent.Weather = gClimateNext.Weather;
                        ClimateDetermineFutureWeather(ScenarioRand());
                        auto intent = Intent(INTENT_ACTION_UPDATE_CLIMATE);
                        ContextBroadcastIntent(&intent);
                    }
                    else if (gClimateNext.Level <= WeatherLevel::Heavy)
                    {
                        gClimateCurrent.Level = static_cast<WeatherLevel>(ClimateStepWeatherLevel(
                            static_cast<int8_t>(gClimateCurrent.Level), static_cast<int8_t>(gClimateNext.Level)));
                    }
                }
                else
                {
                    gClimateCurrent.WeatherGloom = ClimateStepWeatherLevel(
                        gClimateCurrent.WeatherGloom, gClimateNext.WeatherGloom);
                    GfxInvalidateScreen();
                }
            }
            else
            {
                gClimateCurrent.Temperature = ClimateStepWeatherLevel(gClimateCurrent.Temperature, gClimateNext.Temperature);
                auto intent = Intent(INTENT_ACTION_UPDATE_CLIMATE);
                ContextBroadcastIntent(&intent);
            }
        }
    }

    if (_thunderTimer != 0)
    {
        ClimateUpdateLightning();
        ClimateUpdateThunder();
    }
    else if (
        gClimateCurrent.WeatherEffect == WeatherEffectType::Storm
        || gClimateCurrent.WeatherEffect == WeatherEffectType::Blizzard)
    {
        // Create new thunder and lightning
        uint32_t randomNumber = UtilRand();
        if ((randomNumber & 0xFFFF) <= 0x1B4)
        {
            randomNumber >>= 16;
            _thunderTimer = 43 + (randomNumber % 64);
            _lightningTimer = randomNumber % 32;
        }
    }
}

void ClimateForceWeather(WeatherType weather)
{
    int32_t month = gDate.GetMonth();
    const WeatherTransition* transition = &ClimateTransitions[static_cast<uint8_t>(gClimate)][month];
    const auto weatherState = &ClimateWeatherData[EnumValue(weather)];

    gClimateCurrent.Weather = weather;
    gClimateCurrent.WeatherGloom = weatherState->GloomLevel;
    gClimateCurrent.Level = weatherState->Level;
    gClimateCurrent.WeatherEffect = weatherState->EffectLevel;
    gClimateCurrent.Temperature = transition->BaseTemperature + weatherState->TemperatureDelta;
    gClimateUpdateTimer = 1920;

    ClimateUpdate();

    // In case of change in gloom level force a complete redraw
    GfxInvalidateScreen();
}

void ClimateUpdateSound()
{
    PROFILED_FUNCTION();

    if (!OpenRCT2::Audio::IsAvailable())
        return;

    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    ClimateUpdateWeatherSound();
    ClimateUpdateThunderSound();
}

bool ClimateIsRaining()
{
    if (gClimateCurrent.Weather == WeatherType::Rain || gClimateCurrent.Weather == WeatherType::HeavyRain
        || gClimateCurrent.Weather == WeatherType::Thunder)
    {
        return true;
    }

    return false;
}

bool ClimateIsSnowing()
{
    if (gClimateCurrent.Weather == WeatherType::Snow || gClimateCurrent.Weather == WeatherType::HeavySnow
        || gClimateCurrent.Weather == WeatherType::Blizzard)
    {
        return true;
    }

    return false;
}

bool WeatherIsDry(WeatherType weatherType)
{
    return weatherType == WeatherType::Sunny || weatherType == WeatherType::PartiallyCloudy
        || weatherType == WeatherType::Cloudy;
}

FilterPaletteID ClimateGetWeatherGloomPaletteId(const ClimateState& state)
{
    auto paletteId = FilterPaletteID::PaletteNull;
    auto gloom = state.WeatherGloom;
    if (gloom < std::size(ClimateWeatherGloomColours))
    {
        paletteId = ClimateWeatherGloomColours[gloom];
    }
    return paletteId;
}

uint32_t ClimateGetWeatherSpriteId(const ClimateState& state)
{
    uint32_t spriteId = SPR_WEATHER_SUN;
    if (EnumValue(state.Weather) < std::size(ClimateWeatherData))
    {
        spriteId = ClimateWeatherData[EnumValue(state.Weather)].SpriteId;
    }
    return spriteId;
}

static int8_t ClimateStepWeatherLevel(int8_t currentWeatherLevel, int8_t nextWeatherLevel)
{
    if (nextWeatherLevel > currentWeatherLevel)
    {
        return currentWeatherLevel + 1;
    }

    return currentWeatherLevel - 1;
}

/**
 * Calculates future weather development.
 * RCT2 implements this as discrete probability distributions dependent on month and climate
 * for nextWeather. The other weather parameters are then looked up depending only on the
 * next weather.
 */
static void ClimateDetermineFutureWeather(int32_t randomDistribution)
{
    int32_t month = gDate.GetMonth();

    // Generate a random variable with values 0 up to DistributionSize-1 and chose weather from the distribution table
    // accordingly
    const WeatherTransition* transition = &ClimateTransitions[static_cast<uint8_t>(gClimate)][month];
    WeatherType nextWeather = (transition->Distribution[((randomDistribution & 0xFF) * transition->DistributionSize) >> 8]);
    gClimateNext.Weather = nextWeather;

    const auto nextWeatherState = &ClimateWeatherData[EnumValue(nextWeather)];
    gClimateNext.Temperature = transition->BaseTemperature + nextWeatherState->TemperatureDelta;
    gClimateNext.WeatherEffect = nextWeatherState->EffectLevel;
    gClimateNext.WeatherGloom = nextWeatherState->GloomLevel;
    gClimateNext.Level = nextWeatherState->Level;

    gClimateUpdateTimer = 1920;
}

static void ClimateUpdateWeatherSound()
{
    if (gClimateCurrent.WeatherEffect == WeatherEffectType::Rain || gClimateCurrent.WeatherEffect == WeatherEffectType::Storm)
    {
        // Start playing the weather sound
        if (_weatherSoundChannel == nullptr || _weatherSoundChannel->IsDone())
        {
            _weatherSoundChannel = CreateAudioChannel(SoundId::Rain, true, DStoMixerVolume(-4000));
        }
        if (_weatherVolume == 1)
        {
            _weatherVolume = -4000;
        }
        else
        {
            // Increase weather sound
            _weatherVolume = std::min(-1400, _weatherVolume + 80);
            if (_weatherSoundChannel != nullptr)
            {
                _weatherSoundChannel->SetVolume(DStoMixerVolume(_weatherVolume));
            }
        }
    }
    else if (_weatherVolume != 1)
    {
        // Decrease weather sound
        _weatherVolume -= 80;
        if (_weatherVolume > -4000)
        {
            if (_weatherSoundChannel != nullptr)
            {
                _weatherSoundChannel->SetVolume(DStoMixerVolume(_weatherVolume));
            }
        }
        else
        {
            ClimateStopWeatherSound();
            _weatherVolume = 1;
        }
    }
}

void ClimateStopWeatherSound()
{
    if (_weatherSoundChannel != nullptr)
    {
        _weatherSoundChannel->Stop();
        _weatherSoundChannel = nullptr;
    }
}

static void ClimateUpdateThunderSound()
{
    if (_thunderStereoEcho)
    {
        // Play thunder on right side
        _thunderStereoEcho = 0;
        ClimatePlayThunder(1, _thunderSoundId, _thunderVolume, 10000);
    }

    // Stop thunder sounds if they have finished
    for (int32_t i = 0; i < MAX_THUNDER_INSTANCES; i++)
    {
        if (_thunderStatus[i] != THUNDER_STATUS::NONE)
        {
            auto& channel = _thunderSoundChannels[i];
            if (!channel->IsPlaying())
            {
                channel->Stop();
                _thunderStatus[i] = THUNDER_STATUS::NONE;
            }
        }
    }
}

static void ClimateUpdateLightning()
{
    if (_lightningTimer == 0)
        return;
    if (gConfigGeneral.DisableLightningEffect)
        return;
    if (!gConfigGeneral.RenderWeatherEffects && !gConfigGeneral.RenderWeatherGloom)
        return;

    _lightningTimer--;
    if (gClimateLightningFlash == 0)
    {
        if ((UtilRand() & 0xFFFF) <= 0x2000)
        {
            gClimateLightningFlash = 1;
        }
    }
}

static void ClimateUpdateThunder()
{
    _thunderTimer--;
    if (_thunderTimer == 0)
    {
        uint32_t randomNumber = UtilRand();
        if (randomNumber & 0x10000)
        {
            if (_thunderStatus[0] == THUNDER_STATUS::NONE && _thunderStatus[1] == THUNDER_STATUS::NONE)
            {
                // Play thunder on left side
                _thunderSoundId = (randomNumber & 0x20000) ? OpenRCT2::Audio::SoundId::Thunder1
                                                           : OpenRCT2::Audio::SoundId::Thunder2;
                _thunderVolume = (-(static_cast<int32_t>((randomNumber >> 18) & 0xFF))) * 8;
                ClimatePlayThunder(0, _thunderSoundId, _thunderVolume, -10000);

                // Let thunder play on right side
                _thunderStereoEcho = 1;
            }
        }
        else
        {
            if (_thunderStatus[0] == THUNDER_STATUS::NONE)
            {
                _thunderSoundId = (randomNumber & 0x20000) ? OpenRCT2::Audio::SoundId::Thunder1
                                                           : OpenRCT2::Audio::SoundId::Thunder2;
                int32_t pan = (((randomNumber >> 18) & 0xFF) - 128) * 16;
                ClimatePlayThunder(0, _thunderSoundId, 0, pan);
            }
        }
    }
}

static void ClimatePlayThunder(int32_t instanceIndex, OpenRCT2::Audio::SoundId soundId, int32_t volume, int32_t pan)
{
    _thunderSoundChannels[instanceIndex] = CreateAudioChannel(soundId, false, DStoMixerVolume(volume), DStoMixerPan(pan));
    if (_thunderSoundChannels[instanceIndex] != nullptr)
    {
        _thunderStatus[instanceIndex] = THUNDER_STATUS::PLAYING;
    }
}

#pragma region Climate / Weather data tables

const FilterPaletteID ClimateWeatherGloomColours[4] = {
    FilterPaletteID::PaletteNull,
    FilterPaletteID::PaletteDarken1,
    FilterPaletteID::PaletteDarken2,
    FilterPaletteID::PaletteDarken3,
};

// There is actually a sprite at 0x5A9C for snow but only these weather types seem to be fully implemented
const WeatherState ClimateWeatherData[EnumValue(WeatherType::Count)] = {
    { 10, WeatherEffectType::None, 0, WeatherLevel::None, SPR_WEATHER_SUN },         // Sunny
    { 5, WeatherEffectType::None, 0, WeatherLevel::None, SPR_WEATHER_SUN_CLOUD },    // Partially Cloudy
    { 0, WeatherEffectType::None, 0, WeatherLevel::None, SPR_WEATHER_CLOUD },        // Cloudy
    { -2, WeatherEffectType::Rain, 1, WeatherLevel::Light, SPR_WEATHER_LIGHT_RAIN }, // Rain
    { -4, WeatherEffectType::Rain, 2, WeatherLevel::Heavy, SPR_WEATHER_HEAVY_RAIN }, // Heavy Rain
    { 2, WeatherEffectType::Storm, 2, WeatherLevel::Heavy, SPR_WEATHER_STORM },      // Thunderstorm
    { -10, WeatherEffectType::Snow, 1, WeatherLevel::Light, SPR_WEATHER_SNOW },      // Snow
    { -15, WeatherEffectType::Snow, 2, WeatherLevel::Heavy, SPR_WEATHER_SNOW },      // Heavy Snow
    { -20, WeatherEffectType::Blizzard, 2, WeatherLevel::Heavy, SPR_WEATHER_SNOW },  // Blizzard
};

constexpr auto S = WeatherType::Sunny;
constexpr auto P = WeatherType::PartiallyCloudy;
constexpr auto C = WeatherType::Cloudy;
constexpr auto R = WeatherType::Rain;
constexpr auto H = WeatherType::HeavyRain;
constexpr auto T = WeatherType::Thunder;

static constexpr const WeatherTransition ClimateTransitionsCoolAndWet[] = {
    { 8, 18, { S, P, P, P, P, P, C, C, C, C, C, C, C, R, R, R, H, H, S, S, S, S, S } },
    { 10, 21, { P, P, P, P, P, C, C, C, C, C, C, C, C, C, R, R, R, H, H, H, T, S, S } },
    { 14, 17, { S, S, S, P, P, P, P, P, P, C, C, C, C, R, R, R, H, S, S, S, S, S, S } },
    { 17, 17, { S, S, S, S, P, P, P, P, P, P, P, C, C, C, C, R, R, S, S, S, S, S, S } },
    { 19, 23, { S, S, S, S, S, S, S, S, S, S, P, P, P, P, P, P, C, C, C, C, C, R, H } },
    { 20, 23, { S, S, S, S, S, S, P, P, P, P, P, P, P, P, C, C, C, C, R, H, H, H, T } },
    { 16, 19, { S, S, S, P, P, P, P, P, C, C, C, C, C, C, R, R, H, H, T, S, S, S, S } },
    { 13, 16, { S, S, P, P, P, P, C, C, C, C, C, C, R, R, H, T, S, S, S, S, S, S, S } },
};
static constexpr const WeatherTransition ClimateTransitionsWarm[] = {
    { 12, 21, { S, S, S, S, S, P, P, P, P, P, P, P, P, C, C, C, C, C, C, C, H, S, S } },
    { 13, 22, { S, S, S, S, S, P, P, P, P, P, P, C, C, C, C, C, C, C, C, C, R, T, S } },
    { 16, 17, { S, S, S, S, S, S, P, P, P, P, P, P, C, C, C, C, R, S, S, S, S, S, S } },
    { 19, 18, { S, S, S, S, S, S, P, P, P, P, P, P, P, C, C, C, C, R, S, S, S, S, S } },
    { 21, 22, { S, S, S, S, S, S, S, S, S, S, P, P, P, P, P, P, P, P, P, C, C, C, S } },
    { 22, 17, { S, S, S, S, S, S, S, S, S, P, P, P, P, P, C, C, T, S, S, S, S, S, S } },
    { 19, 17, { S, S, S, S, S, P, P, P, P, P, C, C, C, C, C, C, R, S, S, S, S, S, S } },
    { 16, 17, { S, S, P, P, P, P, P, C, C, C, C, C, C, C, C, C, H, S, S, S, S, S, S } },
};
static constexpr const WeatherTransition ClimateTransitionsHotAndDry[] = {
    { 12, 15, { S, S, S, S, P, P, P, P, P, P, P, P, C, C, R, S, S, S, S, S, S, S, S } },
    { 14, 12, { S, S, S, S, S, P, P, P, P, P, C, C, S, S, S, S, S, S, S, S, S, S, S } },
    { 16, 11, { S, S, S, S, S, S, P, P, P, P, C, S, S, S, S, S, S, S, S, S, S, S, S } },
    { 19, 9, { S, S, S, S, S, S, P, P, P, S, S, S, S, S, S, S, S, S, S, S, S, S, S } },
    { 21, 13, { S, S, S, S, S, S, S, S, S, S, P, P, P, S, S, S, S, S, S, S, S, S, S } },
    { 22, 11, { S, S, S, S, S, S, S, S, S, P, P, S, S, S, S, S, S, S, S, S, S, S, S } },
    { 21, 12, { S, S, S, S, S, S, S, P, P, P, C, T, S, S, S, S, S, S, S, S, S, S, S } },
    { 16, 13, { S, S, S, S, S, S, S, S, P, P, P, C, R, S, S, S, S, S, S, S, S, S, S } },
};
static constexpr const WeatherTransition ClimateTransitionsCold[] = {
    { 4, 18, { S, S, S, S, P, P, P, P, P, C, C, C, C, C, C, C, R, H, S, S, S, S, S } },
    { 5, 21, { S, S, S, S, P, P, P, P, P, C, C, C, C, C, C, C, C, C, R, H, T, S, S } },
    { 7, 17, { S, S, S, S, P, P, P, P, P, P, P, C, C, C, C, R, H, S, S, S, S, S, S } },
    { 9, 17, { S, S, S, S, P, P, P, P, P, P, P, C, C, C, C, R, R, S, S, S, S, S, S } },
    { 10, 23, { S, S, S, S, S, S, S, S, S, S, P, P, P, P, P, P, C, C, C, C, C, R, H } },
    { 11, 23, { S, S, S, S, S, S, P, P, P, P, P, P, P, P, P, P, C, C, C, C, R, H, T } },
    { 9, 19, { S, S, S, S, S, P, P, P, P, P, C, C, C, C, C, C, R, H, T, S, S, S, S } },
    { 6, 16, { S, S, P, P, P, P, C, C, C, C, C, C, R, R, H, T, S, S, S, S, S, S, S } },
};

const WeatherTransition* ClimateTransitions[] = {
    ClimateTransitionsCoolAndWet,
    ClimateTransitionsWarm,
    ClimateTransitionsHotAndDry,
    ClimateTransitionsCold,
};

#pragma endregion
