/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
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

void DrawWeather(rct_drawpixelinfo* dpi, IWeatherDrawer* weatherDrawer);

static void DrawLightRain(IWeatherDrawer* weatherDrawer, int32_t left, int32_t top, int32_t width, int32_t height);
static void DrawHeavyRain(IWeatherDrawer* weatherDrawer, int32_t left, int32_t top, int32_t width, int32_t height);
static void DrawLightSnow(IWeatherDrawer* weatherDrawer, int32_t left, int32_t top, int32_t width, int32_t height);
static void DrawHeavySnow(IWeatherDrawer* weatherDrawer, int32_t left, int32_t top, int32_t width, int32_t height);

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
void DrawWeather(rct_drawpixelinfo* dpi, IWeatherDrawer* weatherDrawer)
{
    if (gConfigGeneral.render_weather_effects)
    {
        uint32_t viewFlags = 0;

        rct_viewport* viewport = window_get_viewport(window_get_main());
        if (viewport != nullptr)
            viewFlags = viewport->flags;

        // Get weather draw function and draw weather
        uint32_t weatherType = gClimateCurrent.WeatherLevel;
        if (weatherType != WEATHER_LEVEL_NONE && !gTrackDesignSaveMode && !(viewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES))
        {
            auto drawFunc = DrawRainFunctions[weatherType];
            if (climate_is_snowing())
            {
                drawFunc = DrawSnowFunctions[weatherType];
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
static void DrawLightRain(IWeatherDrawer* weatherDrawer, int32_t left, int32_t top, int32_t width, int32_t height)
{
    int32_t x_start = -(int32_t)gScenarioTicks + 8;
    int32_t y_start = (gScenarioTicks * 3) + 7;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    weatherDrawer->Draw(left, top, width, height, x_start, y_start, RainPattern);

    x_start = -(int32_t)gScenarioTicks + 0x18;
    y_start = (gScenarioTicks * 4) + 0x0D;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    weatherDrawer->Draw(left, top, width, height, x_start, y_start, RainPattern);
}

/**
 *
 *  rct2: 0x0068416D
 */
static void DrawHeavyRain(IWeatherDrawer* weatherDrawer, int32_t left, int32_t top, int32_t width, int32_t height)
{
    int32_t x_start = -(int32_t)gScenarioTicks;
    int32_t y_start = gScenarioTicks * 5;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    weatherDrawer->Draw(left, top, width, height, x_start, y_start, RainPattern);

    x_start = -(int32_t)gScenarioTicks + 0x10;
    y_start = (gScenarioTicks * 6) + 5;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    weatherDrawer->Draw(left, top, width, height, x_start, y_start, RainPattern);

    x_start = -(int32_t)gScenarioTicks + 8;
    y_start = (gScenarioTicks * 3) + 7;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    weatherDrawer->Draw(left, top, width, height, x_start, y_start, RainPattern);

    x_start = -(int32_t)gScenarioTicks + 0x18;
    y_start = (gScenarioTicks * 4) + 0x0D;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    weatherDrawer->Draw(left, top, width, height, x_start, y_start, RainPattern);
}

static void DrawLightSnow(IWeatherDrawer* weatherDrawer, int32_t left, int32_t top, int32_t width, int32_t height)
{
    const uint32_t t = gScenarioTicks / 2;
    const double t2 = (double)gScenarioTicks / 2.0f;

    int32_t x_start = -(int32_t)t + 1 + (cos(1.0 + left + t2 / 10.0) * 3);
    int32_t y_start = t + 1;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    weatherDrawer->Draw(left, top, width, height, x_start, y_start, SnowPattern);

    x_start = -(int32_t)t + 16 + (cos(left + t2 / 10.0) * 3);
    y_start = t + 16;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    weatherDrawer->Draw(left, top, width, height, x_start, y_start, SnowPattern);
}

static void DrawHeavySnow(IWeatherDrawer* weatherDrawer, int32_t left, int32_t top, int32_t width, int32_t height)
{
    int32_t x_start = -(int32_t)(gScenarioTicks * 3) + 1;
    int32_t y_start = gScenarioTicks + 23;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    weatherDrawer->Draw(left, top, width, height, x_start, y_start, SnowPattern);

    x_start = -(int32_t)(gScenarioTicks * 4) + 6;
    y_start = gScenarioTicks + 5;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    weatherDrawer->Draw(left, top, width, height, x_start, y_start, SnowPattern);

    x_start = -(int32_t)(gScenarioTicks * 2) + 11;
    y_start = gScenarioTicks + 18;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    weatherDrawer->Draw(left, top, width, height, x_start, y_start, SnowPattern);

    x_start = -(int32_t)(gScenarioTicks * 3) + 17;
    y_start = gScenarioTicks + 11;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    weatherDrawer->Draw(left, top, width, height, x_start, y_start, SnowPattern);
}
