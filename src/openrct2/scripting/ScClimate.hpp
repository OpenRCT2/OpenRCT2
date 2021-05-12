/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../Context.h"
#    include "../common.h"
#    include "../core/String.hpp"
#    include "../world/Climate.h"
#    include "Duktape.hpp"
#    include "ScriptEngine.h"

namespace OpenRCT2::Scripting
{
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

        std::string climate_get() const
        {
            return ClimateTypeToString(gClimate);
        }

        std::string currentWeather_get() const
        {
            return WeatherTypeToString(gClimateCurrent.Weather);
        }

        int8_t currentTemperature_get() const
        {
            return gClimateCurrent.Temperature;
        }

        std::string futureWeather_get() const
        {
            return WeatherTypeToString(gClimateNext.Weather);
        }

        int8_t futureTemperature_get() const
        {
            return gClimateNext.Temperature;
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScClimate::climate_get, nullptr, "climate");
            dukglue_register_property(ctx, &ScClimate::currentWeather_get, nullptr, "currentWeather");
            dukglue_register_property(ctx, &ScClimate::currentTemperature_get, nullptr, "currentTemperature");
            dukglue_register_property(ctx, &ScClimate::futureWeather_get, nullptr, "futureWeather");
            dukglue_register_property(ctx, &ScClimate::futureTemperature_get, nullptr, "futureTemperature");
        }
    };

} // namespace OpenRCT2::Scripting

#endif
