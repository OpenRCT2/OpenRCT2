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
    #include "../../../world/Climate.h"
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

    class ScClimate
    {
    public:
        static std::string WeatherTypeToString(WeatherType token)
        {
            switch (token)
            {
                case WeatherType::Sunny:
                    return "sunny";
                case WeatherType::PartiallyCloudy:
                    return "partiallyCloudy";
                case WeatherType::Cloudy:
                    return "cloudy";
                case WeatherType::Rain:
                    return "rain";
                case WeatherType::HeavyRain:
                    return "heavyRain";
                case WeatherType::Thunder:
                    return "thunder";
                case WeatherType::Snow:
                    return "snow";
                case WeatherType::HeavySnow:
                    return "heavySnow";
                case WeatherType::Blizzard:
                    return "blizzard";
                case WeatherType::Count:
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
            dukglue_register_property(ctx, &ScClimate::type_get, nullptr, "type");
            dukglue_register_property(ctx, &ScClimate::current_get, nullptr, "current");
            dukglue_register_property(ctx, &ScClimate::future_get, nullptr, "future");
        }
    };

} // namespace OpenRCT2::Scripting

#endif
