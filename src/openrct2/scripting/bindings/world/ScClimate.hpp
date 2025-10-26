/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "../../../Context.h"
    #include "../../../GameState.h"
    #include "../../../core/StringTypes.h"
    #include "../../../object/ClimateObject.h"
    #include "../../../object/ObjectManager.h"
    #include "../../../world/Climate.h"
    #include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    class ScWeatherState;
    extern ScWeatherState gScWeatherState;
    class ScWeatherState final : public ScBase
    {
    private:
        using OpaqueWeatherStateData = struct
        {
            std::string weather;
            int8_t temperature;
        };

        static JSValue weather_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewString(ctx, gScWeatherState.GetOpaque<OpaqueWeatherStateData*>(thisVal)->weather.c_str());
        }

        static JSValue temperature_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewInt32(ctx, gScWeatherState.GetOpaque<OpaqueWeatherStateData*>(thisVal)->temperature);
        }

    public:
        JSValue New(JSContext* ctx, const std::string& weather, int8_t temperature)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("weather", ScWeatherState::weather_get, nullptr),
                JS_CGETSET_DEF("temperature", ScWeatherState::temperature_get, nullptr),
            };

            return MakeWithOpaque(ctx, funcs, new OpaqueWeatherStateData{ weather, temperature });
        }

        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "WeatherState");
        }

        void Finalize(JSRuntime*, JSValue thisVal)
        {
            OpaqueWeatherStateData* data = gScWeatherState.GetOpaque<OpaqueWeatherStateData*>(thisVal);
            if (data)
                delete data;
        }
    };

    class ScClimate;
    extern ScClimate gScClimate;
    class ScClimate final : public ScBase
    {
    private:
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

        static JSValue type_get(JSContext* ctx, JSValue)
        {
            auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
            auto* climateObj = objManager.GetLoadedObject<ClimateObject>(0);
            if (climateObj == nullptr)
                return JS_NewString(ctx, {});

            return JS_NewString(ctx, climateObj->getScriptName().c_str());
        }

        static JSValue current_get(JSContext* ctx, JSValue)
        {
            auto& gameState = getGameState();
            std::string weatherType = WeatherTypeToString(gameState.weatherCurrent.weatherType);
            return gScWeatherState.New(ctx, weatherType, gameState.weatherCurrent.temperature);
        }

        static JSValue future_get(JSContext* ctx, JSValue)
        {
            auto& gameState = getGameState();
            std::string weatherType = WeatherTypeToString(gameState.weatherNext.weatherType);
            return gScWeatherState.New(ctx, weatherType, gameState.weatherCurrent.temperature);
        }

    public:
        JSValue New(JSContext* ctx)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("type", ScClimate::type_get, nullptr),
                JS_CGETSET_DEF("current", ScClimate::current_get, nullptr),
                JS_CGETSET_DEF("future", ScClimate::future_get, nullptr),
            };

            return MakeWithOpaque(ctx, funcs, nullptr);
        }

        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "Climate");
        }
    };

} // namespace OpenRCT2::Scripting

#endif
