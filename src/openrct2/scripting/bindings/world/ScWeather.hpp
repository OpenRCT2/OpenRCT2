/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "../../../Context.h"
    #include "../../../GameState.h"
    #include "../../../core/StringTypes.h"
    #include "../../../object/ClimateObject.h"
    #include "../../../object/ObjectManager.h"
    #include "../../../world/Weather.h"
    #include "../../Duktape.hpp"
    #include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    class ScWeatherState
    {
    private:
        std::string _weather;
        int8_t _temperature;

    public:
        ScWeatherState(std::string weather, int8_t temperature)
            : _weather(weather)
            , _temperature(temperature)
        {
        }

        std::string weather_get() const
        {
            return _weather;
        }

        int8_t temperature_get() const
        {
            return _temperature;
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScWeatherState::weather_get, nullptr, "weather");
            dukglue_register_property(ctx, &ScWeatherState::temperature_get, nullptr, "temperature");
        }
    };

    class ScWeather
    {
    public:
        static std::string WeatherTypeToString(Weather::Type token)
        {
            switch (token)
            {
                case Weather::Type::Sunny:
                    return "sunny";
                case Weather::Type::PartiallyCloudy:
                    return "partiallyCloudy";
                case Weather::Type::Cloudy:
                    return "cloudy";
                case Weather::Type::Rain:
                    return "rain";
                case Weather::Type::HeavyRain:
                    return "heavyRain";
                case Weather::Type::Thunder:
                    return "thunder";
                case Weather::Type::Snow:
                    return "snow";
                case Weather::Type::HeavySnow:
                    return "heavySnow";
                case Weather::Type::Blizzard:
                    return "blizzard";
                case Weather::Type::Count:
                    return {};
            }
            return {};
        }

        std::string type_get() const
        {
            auto& objManager = GetContext()->GetObjectManager();
            auto* climateObj = objManager.GetLoadedObject<ClimateObject>(0);
            if (climateObj == nullptr)
                return {};

            return climateObj->getScriptName();
        }

        std::shared_ptr<ScWeatherState> current_get() const
        {
            auto& gameState = getGameState();
            std::string weatherType = WeatherTypeToString(gameState.weatherCurrent.weatherType);
            return std::make_shared<ScWeatherState>(weatherType, gameState.weatherCurrent.temperature);
        }

        std::shared_ptr<ScWeatherState> future_get() const
        {
            auto& gameState = getGameState();
            std::string weatherType = WeatherTypeToString(gameState.weatherNext.weatherType);
            return std::make_shared<ScWeatherState>(weatherType, gameState.weatherNext.temperature);
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScWeather::type_get, nullptr, "type");
            dukglue_register_property(ctx, &ScWeather::current_get, nullptr, "current");
            dukglue_register_property(ctx, &ScWeather::future_get, nullptr, "future");
        }
    };

} // namespace OpenRCT2::Scripting

#endif
