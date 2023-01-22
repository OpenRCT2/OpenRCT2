/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Weather.h"

#include "../Game.h"
#include "../config/Config.h"
#include "../interface/Viewport.h"
#include "../ride/TrackDesign.h"
#include "../scenario/Scenario.h"
#include "../ui/UiContext.h"
#include "../world/Climate.h"
#include "Drawing.h"
#include "IDrawingEngine.h"

namespace OpenRCT2
{
    using namespace Drawing;

    static void DrawLightRain(
        DrawPixelInfo* dpi, IWeatherDrawer* weatherDrawer, int32_t left, int32_t top, int32_t width, int32_t height);
    static void DrawHeavyRain(
        DrawPixelInfo* dpi, IWeatherDrawer* weatherDrawer, int32_t left, int32_t top, int32_t width, int32_t height);
    static void DrawLightSnow(
        DrawPixelInfo* dpi, IWeatherDrawer* weatherDrawer, int32_t left, int32_t top, int32_t width, int32_t height);
    static void DrawHeavySnow(
        DrawPixelInfo* dpi, IWeatherDrawer* weatherDrawer, int32_t left, int32_t top, int32_t width, int32_t height);

    /**
     *
     *  rct2: 0x009AC058
     */
    const DrawWeatherFunc DrawRainFunctions[] = {
        nullptr,
        &DrawLightRain,
        &DrawHeavyRain,
    };

    const DrawWeatherFunc DrawSnowFunctions[] = {
        nullptr,
        &DrawLightSnow,
        &DrawHeavySnow,
    };

    /**
     *
     *  rct2: 0x00684218
     */
    void DrawWeather(DrawPixelInfo* dpi, IWeatherDrawer* weatherDrawer)
    {
        if (gConfigGeneral.RenderWeatherEffects)
        {
            uint32_t viewFlags = 0;

            const auto* viewport = WindowGetViewport(WindowGetMain());
            if (viewport != nullptr)
                viewFlags = viewport->flags;

            // Get weather draw function and draw weather
            auto weatherLevel = gClimateCurrent.Level;
            if (weatherLevel != WeatherLevel::None && !gTrackDesignSaveMode
                && !(viewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES))
            {
                auto drawFunc = DrawRainFunctions[static_cast<int8_t>(weatherLevel)];
                if (ClimateIsSnowing())
                {
                    drawFunc = DrawSnowFunctions[static_cast<int8_t>(weatherLevel)];
                }
                auto uiContext = GetContext()->GetUiContext();
                uiContext->DrawWeatherAnimation(weatherDrawer, dpi, drawFunc);
            }
        }
    }

    /**
     *
     *  rct2: 0x00684114
     */
    static void DrawLightRain(
        DrawPixelInfo* dpi, IWeatherDrawer* weatherDrawer, int32_t left, int32_t top, int32_t width, int32_t height)
    {
        int32_t x_start = -static_cast<int32_t>(gCurrentTicks) + 8;
        int32_t y_start = (gCurrentTicks * 3) + 7;
        y_start = -y_start;
        x_start += left;
        y_start += top;
        weatherDrawer->Draw(dpi, left, top, width, height, x_start, y_start, RainPattern);

        x_start = -static_cast<int32_t>(gCurrentTicks) + 0x18;
        y_start = (gCurrentTicks * 4) + 0x0D;
        y_start = -y_start;
        x_start += left;
        y_start += top;
        weatherDrawer->Draw(dpi, left, top, width, height, x_start, y_start, RainPattern);
    }

    /**
     *
     *  rct2: 0x0068416D
     */
    static void DrawHeavyRain(
        DrawPixelInfo* dpi, IWeatherDrawer* weatherDrawer, int32_t left, int32_t top, int32_t width, int32_t height)
    {
        int32_t x_start = -static_cast<int32_t>(gCurrentTicks);
        int32_t y_start = gCurrentTicks * 5;
        y_start = -y_start;
        x_start += left;
        y_start += top;
        weatherDrawer->Draw(dpi, left, top, width, height, x_start, y_start, RainPattern);

        x_start = -static_cast<int32_t>(gCurrentTicks) + 0x10;
        y_start = (gCurrentTicks * 6) + 5;
        y_start = -y_start;
        x_start += left;
        y_start += top;
        weatherDrawer->Draw(dpi, left, top, width, height, x_start, y_start, RainPattern);

        x_start = -static_cast<int32_t>(gCurrentTicks) + 8;
        y_start = (gCurrentTicks * 3) + 7;
        y_start = -y_start;
        x_start += left;
        y_start += top;
        weatherDrawer->Draw(dpi, left, top, width, height, x_start, y_start, RainPattern);

        x_start = -static_cast<int32_t>(gCurrentTicks) + 0x18;
        y_start = (gCurrentTicks * 4) + 0x0D;
        y_start = -y_start;
        x_start += left;
        y_start += top;
        weatherDrawer->Draw(dpi, left, top, width, height, x_start, y_start, RainPattern);
    }

    static void DrawLightSnow(
        DrawPixelInfo* dpi, IWeatherDrawer* weatherDrawer, int32_t left, int32_t top, int32_t width, int32_t height)
    {
        const uint32_t t = gCurrentTicks / 2;
        const int32_t negT = -static_cast<int32_t>(t);
        const double cosTick = static_cast<double>(gCurrentTicks) * 0.05;

        int32_t x_start = negT + 1 + (cos(1.0 + cosTick) * 6);
        int32_t y_start = t + 1;
        y_start = -y_start;
        x_start += left;
        y_start += top;
        weatherDrawer->Draw(dpi, left, top, width, height, x_start, y_start, SnowPattern);

        x_start = negT + 16 + (cos(cosTick) * 6);
        y_start = t + 16;
        y_start = -y_start;
        x_start += left;
        y_start += top;
        weatherDrawer->Draw(dpi, left, top, width, height, x_start, y_start, SnowPattern);
    }

    static void DrawHeavySnow(
        DrawPixelInfo* dpi, IWeatherDrawer* weatherDrawer, int32_t left, int32_t top, int32_t width, int32_t height)
    {
        int32_t x_start = -static_cast<int32_t>(gCurrentTicks * 3) + 1;
        int32_t y_start = gCurrentTicks + 23;
        y_start = -y_start;
        x_start += left;
        y_start += top;
        weatherDrawer->Draw(dpi, left, top, width, height, x_start, y_start, SnowPattern);

        x_start = -static_cast<int32_t>(gCurrentTicks * 4) + 6;
        y_start = gCurrentTicks + 5;
        y_start = -y_start;
        x_start += left;
        y_start += top;
        weatherDrawer->Draw(dpi, left, top, width, height, x_start, y_start, SnowPattern);

        x_start = -static_cast<int32_t>(gCurrentTicks * 2) + 11;
        y_start = gCurrentTicks + 18;
        y_start = -y_start;
        x_start += left;
        y_start += top;
        weatherDrawer->Draw(dpi, left, top, width, height, x_start, y_start, SnowPattern);

        x_start = -static_cast<int32_t>(gCurrentTicks * 3) + 17;
        y_start = gCurrentTicks + 11;
        y_start = -y_start;
        x_start += left;
        y_start += top;
        weatherDrawer->Draw(dpi, left, top, width, height, x_start, y_start, SnowPattern);
    }
} // namespace OpenRCT2
