/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../../../Context.h"
#    include "../../../common.h"
#    include "../../../core/String.hpp"
#    include "../../../world/Climate.h"
#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    class ScClimateState
    {
    private:
        std::string _weather;
        int8_t _temperature;

    public:
        ScClimateState(std::string weather, int8_t temperature)
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
            dukglue_register_property(ctx, &ScClimateState::weather_get, nullptr, "weather");
            dukglue_register_property(ctx, &ScClimateState::temperature_get, nullptr, "temperature");
        }
    };

    class ScClimate
    {
    public:
        static std::string ClimateTypeToString(ClimateType token)
        {
            switch (token)
            {
                case ClimateType::CoolAndWet:
                    return "coolAndWet";
                case ClimateType::Warm:
                    return "warm";
                case ClimateType::HotAndDry:
                    return "hotAndDry";
                case ClimateType::Cold:
                    return "cold";
                case ClimateType::Count:
                    return "";
            }
            return "";
        }

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
                    return "";
            }
            return "";
        }

        std::string type_get() const
        {
            return ClimateTypeToString(gClimate);
        }

        std::shared_ptr<ScClimateState> current_get() const
        {
            std::string weatherType = WeatherTypeToString(gClimateCurrent.Weather);
            return std::make_shared<ScClimateState>(weatherType, gClimateCurrent.Temperature);
        }

        std::shared_ptr<ScClimateState> future_get() const
        {
            std::string weatherType = WeatherTypeToString(gClimateNext.Weather);
            return std::make_shared<ScClimateState>(weatherType, gClimateNext.Temperature);
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
