/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Climate.h"

#include "../Context.h"
#include "../Game.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../SpriteIds.h"
#include "../audio/Audio.h"
#include "../audio/AudioChannel.h"
#include "../audio/AudioMixer.h"
#include "../config/Config.h"
#include "../core/EnumUtils.hpp"
#include "../drawing/Drawing.h"
#include "../object/ClimateObject.h"
#include "../object/ObjectManager.h"
#include "../profiling/Profiling.h"
#include "../scenario/Scenario.h"
#include "../util/Util.h"
#include "../windows/Intent.h"

#include <array>
#include <memory>

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Audio;

struct WeatherTrait
{
    int8_t temperatureDelta;
    WeatherEffectType effectLevel;
    int8_t gloomLevel;
    WeatherLevel level;
    uint32_t spriteId;
};

// clang-format off
constexpr std::array<WeatherTrait, EnumValue(WeatherType::Count)> kClimateWeatherTraits = { {
    {  10, WeatherEffectType::None,     0, WeatherLevel::None,  SPR_WEATHER_SUN          }, // Sunny
    {   5, WeatherEffectType::None,     0, WeatherLevel::None,  SPR_WEATHER_SUN_CLOUD    }, // Partially Cloudy
    {   0, WeatherEffectType::None,     0, WeatherLevel::None,  SPR_WEATHER_CLOUD        }, // Cloudy
    {  -2, WeatherEffectType::Rain,     1, WeatherLevel::Light, SPR_WEATHER_LIGHT_RAIN   }, // Rain
    {  -4, WeatherEffectType::Rain,     2, WeatherLevel::Heavy, SPR_WEATHER_HEAVY_RAIN   }, // Heavy Rain
    {   2, WeatherEffectType::Storm,    2, WeatherLevel::Heavy, SPR_WEATHER_STORM        }, // Thunderstorm
    { -10, WeatherEffectType::Snow,     1, WeatherLevel::Light, SPR_G2_WEATHER_SNOW      }, // Snow
    { -15, WeatherEffectType::Snow,     2, WeatherLevel::Heavy, SPR_G2_WEATHER_HEAVY_SNOW}, // Heavy Snow
    { -20, WeatherEffectType::Blizzard, 2, WeatherLevel::Heavy, SPR_G2_WEATHER_BLIZZARD  }, // Blizzard
} };
// clang-format on

constexpr int32_t kMaxThunderInstances = 2;

enum class ThunderStatus
{
    none,
    playing,
};

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
static SoundId _thunderSoundId;
static int32_t _thunderVolume;
static int32_t _thunderStereoEcho = 0;
static std::shared_ptr<IAudioChannel> _weatherSoundChannel;

constexpr FilterPaletteID kClimateWeatherGloomColours[4] = {
    FilterPaletteID::paletteNull,
    FilterPaletteID::paletteDarken1,
    FilterPaletteID::paletteDarken2,
    FilterPaletteID::paletteDarken3,
};

static int8_t ClimateStepWeatherLevel(int8_t currentWeatherLevel, int8_t nextWeatherLevel);
static void ClimateDetermineFutureWeather(uint32_t randomValue);
static void ClimateUpdateWeatherSound();
static void ClimateUpdateThunderSound();
static void ClimateUpdateLightning();
static void ClimateUpdateThunder();
static void ClimatePlayThunder(int32_t instanceIndex, SoundId soundId, int32_t volume, int32_t pan);

int32_t ClimateCelsiusToFahrenheit(int32_t celsius)
{
    return (celsius * 29) / 16 + 32;
}

/**
 * Set climate and determine start weather.
 */
void ClimateReset()
{
    auto& objManager = GetContext()->GetObjectManager();
    auto* climateObj = objManager.GetLoadedObject<ClimateObject>(0);
    if (climateObj == nullptr)
        return;

    int32_t month = GetDate().GetMonth();
    const WeatherPattern& pattern = climateObj->getPatternForMonth(month);

    auto weather = WeatherType::PartiallyCloudy;
    const WeatherTrait& trait = kClimateWeatherTraits[EnumValue(weather)];

    auto& gameState = getGameState();
    gameState.weatherCurrent.weatherType = weather;
    gameState.weatherCurrent.temperature = pattern.baseTemperature + trait.temperatureDelta;
    gameState.weatherCurrent.weatherEffect = trait.effectLevel;
    gameState.weatherCurrent.weatherGloom = trait.gloomLevel;
    gameState.weatherCurrent.level = trait.level;

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

    auto& gameState = getGameState();

    // Only do climate logic if playing (not in scenario editor or title screen)
    if (gLegacyScene != LegacyScene::playing)
        return;

    if (!getGameState().cheats.freezeWeather)
    {
        if (gameState.weatherUpdateTimer)
        {
            if (gameState.weatherUpdateTimer == 960)
            {
                auto intent = Intent(INTENT_ACTION_UPDATE_CLIMATE);
                ContextBroadcastIntent(&intent);
            }
            gameState.weatherUpdateTimer--;
        }
        else if (!(gameState.currentTicks & 0x7F))
        {
            if (gameState.weatherCurrent.temperature == gameState.weatherNext.temperature)
            {
                if (gameState.weatherCurrent.weatherGloom == gameState.weatherNext.weatherGloom)
                {
                    gameState.weatherCurrent.weatherEffect = gameState.weatherNext.weatherEffect;
                    _thunderTimer = 0;
                    _lightningTimer = 0;

                    if (gameState.weatherCurrent.level == gameState.weatherNext.level)
                    {
                        gameState.weatherCurrent.weatherType = gameState.weatherNext.weatherType;
                        ClimateDetermineFutureWeather(ScenarioRand());
                        auto intent = Intent(INTENT_ACTION_UPDATE_CLIMATE);
                        ContextBroadcastIntent(&intent);
                    }
                    else if (gameState.weatherNext.level <= WeatherLevel::Heavy)
                    {
                        gameState.weatherCurrent.level = static_cast<WeatherLevel>(ClimateStepWeatherLevel(
                            static_cast<int8_t>(gameState.weatherCurrent.level),
                            static_cast<int8_t>(gameState.weatherNext.level)));
                    }
                }
                else
                {
                    gameState.weatherCurrent.weatherGloom = ClimateStepWeatherLevel(
                        gameState.weatherCurrent.weatherGloom, gameState.weatherNext.weatherGloom);
                    GfxInvalidateScreen();
                }
            }
            else
            {
                gameState.weatherCurrent.temperature = ClimateStepWeatherLevel(
                    gameState.weatherCurrent.temperature, gameState.weatherNext.temperature);
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
    else
    {
        uint32_t thunderChance;
        if (gameState.weatherCurrent.weatherEffect == WeatherEffectType::Storm)
            thunderChance = 0x1B4;
        else if (gameState.weatherCurrent.weatherEffect == WeatherEffectType::Blizzard)
            thunderChance = 0x6D;
        else
            return;
        // Create new thunder and lightning. Their amount is scaled inversely proportional
        // to the game speed, otherwise they become annoying at very high speeds
        if (uint32_t randomNumber = UtilRand(); (randomNumber & 0xFFFF) <= (thunderChance >> (gGameSpeed - 1)))
        {
            randomNumber >>= 16;
            _thunderTimer = 43 + (randomNumber % 64);
            _lightningTimer = randomNumber % 32;
        }
    }
}

void ClimateForceWeather(WeatherType weather)
{
    auto& objManager = GetContext()->GetObjectManager();
    auto* climateObj = objManager.GetLoadedObject<ClimateObject>(0);
    if (climateObj == nullptr)
        return;

    int32_t month = GetDate().GetMonth();
    const WeatherPattern& pattern = climateObj->getPatternForMonth(month);

    const auto& trait = kClimateWeatherTraits[EnumValue(weather)];

    auto& gameState = getGameState();
    gameState.weatherCurrent.weatherType = weather;
    gameState.weatherCurrent.weatherGloom = trait.gloomLevel;
    gameState.weatherCurrent.level = trait.level;
    gameState.weatherCurrent.weatherEffect = trait.effectLevel;
    gameState.weatherCurrent.temperature = pattern.baseTemperature + trait.temperatureDelta;
    gameState.weatherUpdateTimer = 1920;

    ClimateDetermineFutureWeather(ScenarioRand());

    // In case of change in gloom level force a complete redraw
    GfxInvalidateScreen();
}

void ClimateUpdateSound()
{
    PROFILED_FUNCTION();

    if (!IsAvailable())
        return;

    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    ClimateUpdateWeatherSound();
    ClimateUpdateThunderSound();
}

bool ClimateIsRaining()
{
    auto& weather = getGameState().weatherCurrent.weatherType;
    return weather == WeatherType::Rain || weather == WeatherType::HeavyRain || weather == WeatherType::Thunder;
}

bool ClimateIsSnowing()
{
    auto& weather = getGameState().weatherCurrent.weatherType;
    return weather == WeatherType::Snow || weather == WeatherType::HeavySnow || weather == WeatherType::Blizzard;
}

bool ClimateTransitioningToSnow()
{
    auto& weather = getGameState().weatherNext.weatherType;
    return weather == WeatherType::Snow || weather == WeatherType::HeavySnow || weather == WeatherType::Blizzard;
}

bool ClimateIsSnowingHeavily()
{
    auto& weather = getGameState().weatherCurrent.weatherType;
    return weather == WeatherType::HeavySnow || weather == WeatherType::Blizzard;
}

bool ClimateIsPrecipitating()
{
    return ClimateIsRaining() || ClimateIsSnowingHeavily();
}

bool WeatherIsDry(WeatherType weather)
{
    return weather == WeatherType::Sunny || weather == WeatherType::PartiallyCloudy || weather == WeatherType::Cloudy;
}

bool ClimateHasWeatherEffect()
{
    const auto& weatherCurrent = getGameState().weatherCurrent;
    // The game starts drawing rain whenever this level is not none.
    return weatherCurrent.level != WeatherLevel::None;
}

FilterPaletteID ClimateGetWeatherGloomPaletteId(const WeatherState& state)
{
    auto paletteId = FilterPaletteID::paletteNull;
    auto gloom = state.weatherGloom;
    if (gloom < std::size(kClimateWeatherGloomColours))
    {
        paletteId = kClimateWeatherGloomColours[gloom];
    }
    return paletteId;
}

uint32_t ClimateGetWeatherSpriteId(const WeatherType weatherType)
{
    return kClimateWeatherTraits[EnumValue(weatherType)].spriteId;
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
    auto& objManager = GetContext()->GetObjectManager();
    auto* climateObj = objManager.GetLoadedObject<ClimateObject>(0);
    if (climateObj == nullptr)
        return;

    int32_t month = GetDate().GetMonth();
    const WeatherPattern& pattern = climateObj->getPatternForMonth(month);

    // Generate a random index with values 0 up to randomBias-1
    // and choose weather from the distribution table accordingly
    const auto randomIndex = ((randomValue % 256) * pattern.randomBias) / 256;
    const auto nextWeather = pattern.distribution[randomIndex];

    auto& gameState = getGameState();
    gameState.weatherNext.weatherType = nextWeather;

    const auto& nextWeatherTrait = kClimateWeatherTraits[EnumValue(nextWeather)];
    gameState.weatherNext.temperature = pattern.baseTemperature + nextWeatherTrait.temperatureDelta;
    gameState.weatherNext.weatherEffect = nextWeatherTrait.effectLevel;
    gameState.weatherNext.weatherGloom = nextWeatherTrait.gloomLevel;
    gameState.weatherNext.level = nextWeatherTrait.level;

    gameState.weatherUpdateTimer = 1920;
}

static void ClimateUpdateWeatherSound()
{
    if (getGameState().weatherCurrent.weatherEffect == WeatherEffectType::Rain
        || getGameState().weatherCurrent.weatherEffect == WeatherEffectType::Storm)
    {
        // Start playing the weather sound
        if (_weatherSoundChannel == nullptr || _weatherSoundChannel->IsDone())
        {
            _weatherSoundChannel = CreateAudioChannel(SoundId::rain, true, DStoMixerVolume(-4000));
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
    if (Config::Get().general.disableLightningEffect)
        return;
    if (!Config::Get().general.renderWeatherEffects && !Config::Get().general.renderWeatherGloom)
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
                _thunderSoundId = (randomNumber & 0x20000) ? SoundId::thunder1 : SoundId::thunder2;
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
                _thunderSoundId = (randomNumber & 0x20000) ? SoundId::thunder1 : SoundId::thunder2;
                int32_t pan = (((randomNumber >> 18) & 0xFF) - 128) * 16;
                ClimatePlayThunder(0, _thunderSoundId, 0, pan);
            }
        }
    }
}

static void ClimatePlayThunder(int32_t instanceIndex, SoundId soundId, int32_t volume, int32_t pan)
{
    _thunderSoundChannels[instanceIndex] = CreateAudioChannel(soundId, false, DStoMixerVolume(volume), DStoMixerPan(pan));
    if (_thunderSoundChannels[instanceIndex] != nullptr)
    {
        _thunderStatus[instanceIndex] = ThunderStatus::playing;
    }
}
