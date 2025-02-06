/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../audio/AudioChannel.h"
#include "../audio/AudioMixer.h"
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../core/EnumUtils.hpp"
#include "../drawing/Drawing.h"
#include "../localisation/Localisation.Date.h"
#include "../profiling/Profiling.h"
#include "../scenario/Scenario.h"
#include "../sprites.h"
#include "../util/Util.h"
#include "../windows/Intent.h"

#include <iterator>
#include <memory>

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;

constexpr int32_t kMaxThunderInstances = 2;

enum class ThunderStatus
{
    none,
    playing,
};

struct WeatherPattern
{
    int8_t baseTemperature;
    int8_t randomBias;
    WeatherType distribution[24];
};

struct WeatherTrait
{
    int8_t temperatureDelta;
    WeatherEffectType effectLevel;
    int8_t gloomLevel;
    WeatherLevel level;
    uint32_t spriteId;
};

// TODO: no need for these to be declared extern, just move the definitions up
extern const WeatherPattern* kClimatePatterns[4];
extern const WeatherTrait kClimateWeatherTraits[EnumValue(WeatherType::Count)];
extern const FilterPaletteID kClimateWeatherGloomColours[4];

// Climate data
uint16_t gClimateLightningFlash;

// Sound data
static int32_t _weatherVolume = 1;
static uint32_t _lightningTimer;
static uint32_t _thunderTimer;
static std::shared_ptr<IAudioChannel> _thunderSoundChannels[kMaxThunderInstances];
static ThunderStatus _thunderStatus[kMaxThunderInstances] = {
    ThunderStatus::none,
    ThunderStatus::none,
};
static OpenRCT2::Audio::SoundId _thunderSoundId;
static int32_t _thunderVolume;
static int32_t _thunderStereoEcho = 0;
static std::shared_ptr<IAudioChannel> _weatherSoundChannel;

static int8_t ClimateStepWeatherLevel(int8_t currentWeatherLevel, int8_t nextWeatherLevel);
static void ClimateDetermineFutureWeather(uint32_t randomValue);
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
    int32_t month = GetDate().GetMonth();

    const WeatherPattern& pattern = kClimatePatterns[EnumValue(climate)][month];
    const WeatherTrait& trait = kClimateWeatherTraits[EnumValue(weather)];

    auto& gameState = GetGameState();
    gameState.Climate = climate;
    gameState.ClimateCurrent.Weather = weather;
    gameState.ClimateCurrent.Temperature = pattern.baseTemperature + trait.temperatureDelta;
    gameState.ClimateCurrent.WeatherEffect = trait.effectLevel;
    gameState.ClimateCurrent.WeatherGloom = trait.gloomLevel;
    gameState.ClimateCurrent.Level = trait.level;

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

    auto& gameState = GetGameState();

    // Only do climate logic if playing (not in scenario editor or title screen)
    if (gScreenFlags & (~SCREEN_FLAGS_PLAYING))
        return;

    if (!GetGameState().Cheats.freezeWeather)
    {
        if (gameState.ClimateUpdateTimer)
        {
            if (gameState.ClimateUpdateTimer == 960)
            {
                auto intent = Intent(INTENT_ACTION_UPDATE_CLIMATE);
                ContextBroadcastIntent(&intent);
            }
            gameState.ClimateUpdateTimer--;
        }
        else if (!(gameState.CurrentTicks & 0x7F))
        {
            if (gameState.ClimateCurrent.Temperature == gameState.ClimateNext.Temperature)
            {
                if (gameState.ClimateCurrent.WeatherGloom == gameState.ClimateNext.WeatherGloom)
                {
                    gameState.ClimateCurrent.WeatherEffect = gameState.ClimateNext.WeatherEffect;
                    _thunderTimer = 0;
                    _lightningTimer = 0;

                    if (gameState.ClimateCurrent.Level == gameState.ClimateNext.Level)
                    {
                        gameState.ClimateCurrent.Weather = gameState.ClimateNext.Weather;
                        ClimateDetermineFutureWeather(ScenarioRand());
                        auto intent = Intent(INTENT_ACTION_UPDATE_CLIMATE);
                        ContextBroadcastIntent(&intent);
                    }
                    else if (gameState.ClimateNext.Level <= WeatherLevel::Heavy)
                    {
                        gameState.ClimateCurrent.Level = static_cast<WeatherLevel>(ClimateStepWeatherLevel(
                            static_cast<int8_t>(gameState.ClimateCurrent.Level),
                            static_cast<int8_t>(gameState.ClimateNext.Level)));
                    }
                }
                else
                {
                    gameState.ClimateCurrent.WeatherGloom = ClimateStepWeatherLevel(
                        gameState.ClimateCurrent.WeatherGloom, gameState.ClimateNext.WeatherGloom);
                    GfxInvalidateScreen();
                }
            }
            else
            {
                gameState.ClimateCurrent.Temperature = ClimateStepWeatherLevel(
                    gameState.ClimateCurrent.Temperature, gameState.ClimateNext.Temperature);
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
        gameState.ClimateCurrent.WeatherEffect == WeatherEffectType::Storm
        || gameState.ClimateCurrent.WeatherEffect == WeatherEffectType::Blizzard)
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
    auto& gameState = GetGameState();
    int32_t month = GetDate().GetMonth();

    const auto& pattern = kClimatePatterns[EnumValue(gameState.Climate)][month];
    const auto& trait = kClimateWeatherTraits[EnumValue(weather)];

    gameState.ClimateCurrent.Weather = weather;
    gameState.ClimateCurrent.WeatherGloom = trait.gloomLevel;
    gameState.ClimateCurrent.Level = trait.level;
    gameState.ClimateCurrent.WeatherEffect = trait.effectLevel;
    gameState.ClimateCurrent.Temperature = pattern.baseTemperature + trait.temperatureDelta;
    gameState.ClimateUpdateTimer = 1920;

    ClimateDetermineFutureWeather(ScenarioRand());

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
    auto& weather = GetGameState().ClimateCurrent.Weather;
    return weather == WeatherType::Rain || weather == WeatherType::HeavyRain || weather == WeatherType::Thunder;
}

bool ClimateIsSnowing()
{
    auto& weather = GetGameState().ClimateCurrent.Weather;
    return weather == WeatherType::Snow || weather == WeatherType::HeavySnow || weather == WeatherType::Blizzard;
}

bool ClimateIsSnowingHeavily()
{
    auto& weather = GetGameState().ClimateCurrent.Weather;
    return weather == WeatherType::HeavySnow || weather == WeatherType::Blizzard;
}

bool WeatherIsDry(WeatherType weather)
{
    return weather == WeatherType::Sunny || weather == WeatherType::PartiallyCloudy || weather == WeatherType::Cloudy;
}

FilterPaletteID ClimateGetWeatherGloomPaletteId(const ClimateState& state)
{
    auto paletteId = FilterPaletteID::PaletteNull;
    auto gloom = state.WeatherGloom;
    if (gloom < std::size(kClimateWeatherGloomColours))
    {
        paletteId = kClimateWeatherGloomColours[gloom];
    }
    return paletteId;
}

uint32_t ClimateGetWeatherSpriteId(const ClimateState& state)
{
    uint32_t spriteId = SPR_WEATHER_SUN;
    if (EnumValue(state.Weather) < std::size(kClimateWeatherTraits))
    {
        spriteId = kClimateWeatherTraits[EnumValue(state.Weather)].spriteId;
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
static void ClimateDetermineFutureWeather(uint32_t randomValue)
{
    int32_t month = GetDate().GetMonth();
    auto& gameState = GetGameState();

    // Generate a random index with values 0 up to randomBias-1
    // and choose weather from the distribution table accordingly
    const auto& pattern = kClimatePatterns[EnumValue(gameState.Climate)][month];
    const auto randomIndex = ((randomValue % 256) * pattern.randomBias) / 256;
    const auto nextWeather = pattern.distribution[randomIndex];
    gameState.ClimateNext.Weather = nextWeather;

    const auto& nextWeatherTrait = kClimateWeatherTraits[EnumValue(nextWeather)];
    gameState.ClimateNext.Temperature = pattern.baseTemperature + nextWeatherTrait.temperatureDelta;
    gameState.ClimateNext.WeatherEffect = nextWeatherTrait.effectLevel;
    gameState.ClimateNext.WeatherGloom = nextWeatherTrait.gloomLevel;
    gameState.ClimateNext.Level = nextWeatherTrait.level;

    gameState.ClimateUpdateTimer = 1920;
}

static void ClimateUpdateWeatherSound()
{
    if (GetGameState().ClimateCurrent.WeatherEffect == WeatherEffectType::Rain
        || GetGameState().ClimateCurrent.WeatherEffect == WeatherEffectType::Storm)
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
    for (int32_t i = 0; i < kMaxThunderInstances; i++)
    {
        if (_thunderStatus[i] != ThunderStatus::none)
        {
            auto& channel = _thunderSoundChannels[i];
            if (!channel->IsPlaying())
            {
                channel->Stop();
                _thunderStatus[i] = ThunderStatus::none;
            }
        }
    }
}

static void ClimateUpdateLightning()
{
    if (_lightningTimer == 0)
        return;
    if (Config::Get().general.DisableLightningEffect)
        return;
    if (!Config::Get().general.RenderWeatherEffects && !Config::Get().general.RenderWeatherGloom)
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
            if (_thunderStatus[0] == ThunderStatus::none && _thunderStatus[1] == ThunderStatus::none)
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
            if (_thunderStatus[0] == ThunderStatus::none)
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
        _thunderStatus[instanceIndex] = ThunderStatus::playing;
    }
}

#pragma region Climate / Weather data tables

const FilterPaletteID kClimateWeatherGloomColours[4] = {
    FilterPaletteID::PaletteNull,
    FilterPaletteID::PaletteDarken1,
    FilterPaletteID::PaletteDarken2,
    FilterPaletteID::PaletteDarken3,
};

// clang-format off
const WeatherTrait kClimateWeatherTraits[EnumValue(WeatherType::Count)] = {
    {  10, WeatherEffectType::None,     0, WeatherLevel::None,  SPR_WEATHER_SUN        }, // Sunny
    {   5, WeatherEffectType::None,     0, WeatherLevel::None,  SPR_WEATHER_SUN_CLOUD  }, // Partially Cloudy
    {   0, WeatherEffectType::None,     0, WeatherLevel::None,  SPR_WEATHER_CLOUD      }, // Cloudy
    {  -2, WeatherEffectType::Rain,     1, WeatherLevel::Light, SPR_WEATHER_LIGHT_RAIN }, // Rain
    {  -4, WeatherEffectType::Rain,     2, WeatherLevel::Heavy, SPR_WEATHER_HEAVY_RAIN }, // Heavy Rain
    {   2, WeatherEffectType::Storm,    2, WeatherLevel::Heavy, SPR_WEATHER_STORM      }, // Thunderstorm
    { -10, WeatherEffectType::Snow,     1, WeatherLevel::Light, SPR_WEATHER_SNOW       }, // Snow
    { -15, WeatherEffectType::Snow,     2, WeatherLevel::Heavy, SPR_WEATHER_SNOW       }, // Heavy Snow
    { -20, WeatherEffectType::Blizzard, 2, WeatherLevel::Heavy, SPR_WEATHER_SNOW       }, // Blizzard
};
// clang-format on

constexpr auto S = WeatherType::Sunny;
constexpr auto P = WeatherType::PartiallyCloudy;
constexpr auto C = WeatherType::Cloudy;
constexpr auto R = WeatherType::Rain;
constexpr auto H = WeatherType::HeavyRain;
constexpr auto T = WeatherType::Thunder;

static constexpr WeatherPattern kClimatePatternsCoolAndWet[] = {
    { 8, 18, { S, P, P, P, P, P, C, C, C, C, C, C, C, R, R, R, H, H, S, S, S, S, S } },
    { 10, 21, { P, P, P, P, P, C, C, C, C, C, C, C, C, C, R, R, R, H, H, H, T, S, S } },
    { 14, 17, { S, S, S, P, P, P, P, P, P, C, C, C, C, R, R, R, H, S, S, S, S, S, S } },
    { 17, 17, { S, S, S, S, P, P, P, P, P, P, P, C, C, C, C, R, R, S, S, S, S, S, S } },
    { 19, 23, { S, S, S, S, S, S, S, S, S, S, P, P, P, P, P, P, C, C, C, C, C, R, H } },
    { 20, 23, { S, S, S, S, S, S, P, P, P, P, P, P, P, P, C, C, C, C, R, H, H, H, T } },
    { 16, 19, { S, S, S, P, P, P, P, P, C, C, C, C, C, C, R, R, H, H, T, S, S, S, S } },
    { 13, 16, { S, S, P, P, P, P, C, C, C, C, C, C, R, R, H, T, S, S, S, S, S, S, S } },
};
static constexpr WeatherPattern kClimatePatternsWarm[] = {
    { 12, 21, { S, S, S, S, S, P, P, P, P, P, P, P, P, C, C, C, C, C, C, C, H, S, S } },
    { 13, 22, { S, S, S, S, S, P, P, P, P, P, P, C, C, C, C, C, C, C, C, C, R, T, S } },
    { 16, 17, { S, S, S, S, S, S, P, P, P, P, P, P, C, C, C, C, R, S, S, S, S, S, S } },
    { 19, 18, { S, S, S, S, S, S, P, P, P, P, P, P, P, C, C, C, C, R, S, S, S, S, S } },
    { 21, 22, { S, S, S, S, S, S, S, S, S, S, P, P, P, P, P, P, P, P, P, C, C, C, S } },
    { 22, 17, { S, S, S, S, S, S, S, S, S, P, P, P, P, P, C, C, T, S, S, S, S, S, S } },
    { 19, 17, { S, S, S, S, S, P, P, P, P, P, C, C, C, C, C, C, R, S, S, S, S, S, S } },
    { 16, 17, { S, S, P, P, P, P, P, C, C, C, C, C, C, C, C, C, H, S, S, S, S, S, S } },
};
static constexpr WeatherPattern kClimatePatternsHotAndDry[] = {
    { 12, 15, { S, S, S, S, P, P, P, P, P, P, P, P, C, C, R, S, S, S, S, S, S, S, S } },
    { 14, 12, { S, S, S, S, S, P, P, P, P, P, C, C, S, S, S, S, S, S, S, S, S, S, S } },
    { 16, 11, { S, S, S, S, S, S, P, P, P, P, C, S, S, S, S, S, S, S, S, S, S, S, S } },
    { 19, 9, { S, S, S, S, S, S, P, P, P, S, S, S, S, S, S, S, S, S, S, S, S, S, S } },
    { 21, 13, { S, S, S, S, S, S, S, S, S, S, P, P, P, S, S, S, S, S, S, S, S, S, S } },
    { 22, 11, { S, S, S, S, S, S, S, S, S, P, P, S, S, S, S, S, S, S, S, S, S, S, S } },
    { 21, 12, { S, S, S, S, S, S, S, P, P, P, C, T, S, S, S, S, S, S, S, S, S, S, S } },
    { 16, 13, { S, S, S, S, S, S, S, S, P, P, P, C, R, S, S, S, S, S, S, S, S, S, S } },
};
static constexpr WeatherPattern kClimatePatternsCold[] = {
    { 4, 18, { S, S, S, S, P, P, P, P, P, C, C, C, C, C, C, C, R, H, S, S, S, S, S } },
    { 5, 21, { S, S, S, S, P, P, P, P, P, C, C, C, C, C, C, C, C, C, R, H, T, S, S } },
    { 7, 17, { S, S, S, S, P, P, P, P, P, P, P, C, C, C, C, R, H, S, S, S, S, S, S } },
    { 9, 17, { S, S, S, S, P, P, P, P, P, P, P, C, C, C, C, R, R, S, S, S, S, S, S } },
    { 10, 23, { S, S, S, S, S, S, S, S, S, S, P, P, P, P, P, P, C, C, C, C, C, R, H } },
    { 11, 23, { S, S, S, S, S, S, P, P, P, P, P, P, P, P, P, P, C, C, C, C, R, H, T } },
    { 9, 19, { S, S, S, S, S, P, P, P, P, P, C, C, C, C, C, C, R, H, T, S, S, S, S } },
    { 6, 16, { S, S, P, P, P, P, C, C, C, C, C, C, R, R, H, T, S, S, S, S, S, S, S } },
};

const WeatherPattern* kClimatePatterns[] = {
    kClimatePatternsCoolAndWet,
    kClimatePatternsWarm,
    kClimatePatternsHotAndDry,
    kClimatePatternsCold,
};

#pragma endregion
