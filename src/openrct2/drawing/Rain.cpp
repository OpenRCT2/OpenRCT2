/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../config/Config.h"
#include "../interface/Viewport.h"
#include "../ride/TrackDesign.h"
#include "../scenario/Scenario.h"
#include "../ui/UiContext.h"
#include "../world/Climate.h"
#include "Drawing.h"
#include "IDrawingEngine.h"
#include "Rain.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;

static void DrawLightRain(IRainDrawer * rainDrawer, int32_t left, int32_t top, int32_t width, int32_t height);
static void DrawHeavyRain(IRainDrawer * rainDrawer, int32_t left, int32_t top, int32_t width, int32_t height);

/**
 *
 *  rct2: 0x009AC058
 */
const DrawRainFunc DrawRainFunctions[] =
{
    nullptr,
    &DrawLightRain,
    &DrawHeavyRain
};

/**
 *
 *  rct2: 0x00684218
 */
void DrawRain(rct_drawpixelinfo * dpi, IRainDrawer * rainDrawer)
{
    if (gConfigGeneral.render_weather_effects)
    {
        // Get rain draw function and draw rain
        uint32_t rainType = gClimateCurrent.RainLevel;
        if (rainType != RAIN_LEVEL_NONE && !gTrackDesignSaveMode && !(gCurrentViewportFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES))
        {
            auto drawFunc = DrawRainFunctions[rainType];
            auto uiContext = GetContext()->GetUiContext();
            uiContext->DrawRainAnimation(rainDrawer, dpi, drawFunc);
        }
    }
}

/**
 *
 *  rct2: 0x00684114
 */
static void DrawLightRain(IRainDrawer * rainDrawer, int32_t left, int32_t top, int32_t width, int32_t height)
{
    int32_t x_start = -(int32_t)gScenarioTicks + 8;
    int32_t y_start = (gScenarioTicks * 3) + 7;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    rainDrawer->Draw(left, top, width, height, x_start, y_start);

    x_start = -(int32_t)gScenarioTicks + 0x18;
    y_start = (gScenarioTicks * 4) + 0x0D;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    rainDrawer->Draw(left, top, width, height, x_start, y_start);
}

/**
 *
 *  rct2: 0x0068416D
 */
static void DrawHeavyRain(IRainDrawer * rainDrawer, int32_t left, int32_t top, int32_t width, int32_t height)
{
    int32_t x_start = -(int32_t)gScenarioTicks;
    int32_t y_start = gScenarioTicks * 5;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    rainDrawer->Draw(left, top, width, height, x_start, y_start);

    x_start = -(int32_t)gScenarioTicks + 0x10;
    y_start = (gScenarioTicks * 6) + 5;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    rainDrawer->Draw(left, top, width, height, x_start, y_start);

    x_start = -(int32_t)gScenarioTicks + 8;
    y_start = (gScenarioTicks * 3) + 7;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    rainDrawer->Draw(left, top, width, height, x_start, y_start);

    x_start = -(int32_t)gScenarioTicks + 0x18;
    y_start = (gScenarioTicks * 4) + 0x0D;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    rainDrawer->Draw(left, top, width, height, x_start, y_start);
}
