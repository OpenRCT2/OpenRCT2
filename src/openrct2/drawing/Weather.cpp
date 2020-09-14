/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Weather.h"

#include "../config/Config.h"
#include "../interface/Viewport.h"
#include "../ride/TrackDesign.h"
#include "../scenario/Scenario.h"
#include "../ui/UiContext.h"
#include "../world/Climate.h"
#include "Drawing.h"
#include "IDrawingEngine.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;

static void DrawLightRain(IWeatherDrawer* weatherDrawer, int32_t left, int32_t top, int32_t width, int32_t height);
static void DrawHeavyRain(IWeatherDrawer* weatherDrawer, int32_t left, int32_t top, int32_t width, int32_t height);

/**
 *
 *  rct2: 0x009AC058
 */
const DrawWeatherFunc DrawRainFunctions[] = {
    nullptr,
    &DrawLightRain,
    &DrawHeavyRain,
};

/**
 *
 *  rct2: 0x00684218
 */
void DrawWeather(rct_drawpixelinfo* dpi, IWeatherDrawer* weatherDrawer)
{
    if (gConfigGeneral.render_weather_effects)
    {
        uint32_t viewFlags = 0;

        rct_viewport* viewport = window_get_viewport(window_get_main());
        if (viewport != nullptr)
            viewFlags = viewport->flags;

        // Get rain draw function and draw rain
        WeatherLevel rainType = gClimateCurrent.Level;
        if (rainType != WeatherLevel::None && !gTrackDesignSaveMode && !(viewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES))
        {
            auto drawFunc = DrawRainFunctions[static_cast<int8_t>(rainType)];
            auto uiContext = GetContext()->GetUiContext();
            uiContext->DrawWeatherAnimation(weatherDrawer, dpi, drawFunc);
        }
    }
}

/**
 *
 *  rct2: 0x00684114
 */
static void DrawLightRain(IWeatherDrawer* weatherDrawer, int32_t left, int32_t top, int32_t width, int32_t height)
{
    int32_t x_start = -static_cast<int32_t>(gScenarioTicks) + 8;
    int32_t y_start = (gScenarioTicks * 3) + 7;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    weatherDrawer->Draw(left, top, width, height, x_start, y_start);

    x_start = -static_cast<int32_t>(gScenarioTicks) + 0x18;
    y_start = (gScenarioTicks * 4) + 0x0D;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    weatherDrawer->Draw(left, top, width, height, x_start, y_start);
}

/**
 *
 *  rct2: 0x0068416D
 */
static void DrawHeavyRain(IWeatherDrawer* weatherDrawer, int32_t left, int32_t top, int32_t width, int32_t height)
{
    int32_t x_start = -static_cast<int32_t>(gScenarioTicks);
    int32_t y_start = gScenarioTicks * 5;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    weatherDrawer->Draw(left, top, width, height, x_start, y_start);

    x_start = -static_cast<int32_t>(gScenarioTicks) + 0x10;
    y_start = (gScenarioTicks * 6) + 5;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    weatherDrawer->Draw(left, top, width, height, x_start, y_start);

    x_start = -static_cast<int32_t>(gScenarioTicks) + 8;
    y_start = (gScenarioTicks * 3) + 7;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    weatherDrawer->Draw(left, top, width, height, x_start, y_start);

    x_start = -static_cast<int32_t>(gScenarioTicks) + 0x18;
    y_start = (gScenarioTicks * 4) + 0x0D;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    weatherDrawer->Draw(left, top, width, height, x_start, y_start);
}
