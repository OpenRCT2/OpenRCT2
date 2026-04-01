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
            return JSFromStdString(ctx, gScWeatherState.GetOpaque<OpaqueWeatherStateData*>(thisVal)->weather);
        }

        static JSValue temperature_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewInt32(ctx, gScWeatherState.GetOpaque<OpaqueWeatherStateData*>(thisVal)->temperature);
        }

    public:
        JSValue New(JSContext* ctx, const std::string& weather, int8_t temperature)
        {
            return MakeWithOpaque(ctx, new OpaqueWeatherStateData{ weather, temperature });
        }

        void Register(JSContext* ctx)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("weather", ScWeatherState::weather_get, nullptr),
                JS_CGETSET_DEF("temperature", ScWeatherState::temperature_get, nullptr),
            };
            RegisterBase(ctx, "WeatherState", Finalize, funcs);
        }

        static void Finalize(JSRuntime*, JSValue thisVal)
        {
            OpaqueWeatherStateData* data = gScWeatherState.GetOpaque<OpaqueWeatherStateData*>(thisVal);
            if (data)
                delete data;
        }
    };

    class ScWeather;
    extern ScWeather gScWeather;
    class ScWeather final : public ScBase
    {
    private:
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

        static JSValue type_get(JSContext* ctx, JSValue)
        {
            auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
            auto* climateObj = objManager.GetLoadedObject<ClimateObject>(0);
            if (climateObj == nullptr)
                return JSFromStdString(ctx, {});

            return JSFromStdString(ctx, climateObj->getScriptName());
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
            return MakeWithOpaque(ctx, nullptr);
        }

        void Register(JSContext* ctx)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("type", ScWeather::type_get, nullptr),
                JS_CGETSET_DEF("current", ScWeather::current_get, nullptr),
                JS_CGETSET_DEF("future", ScWeather::future_get, nullptr),
            };
            RegisterBase(ctx, "Weather", nullptr, funcs);
        }
    };

} // namespace OpenRCT2::Scripting

#endif
