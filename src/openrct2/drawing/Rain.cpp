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

static void DrawLightRain(IRainDrawer * rainDrawer, sint32 left, sint32 top, sint32 width, sint32 height);
static void DrawHeavyRain(IRainDrawer * rainDrawer, sint32 left, sint32 top, sint32 width, sint32 height);

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
        uint32 rainType = gClimateCurrent.RainLevel;
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
static void DrawLightRain(IRainDrawer * rainDrawer, sint32 left, sint32 top, sint32 width, sint32 height)
{
    sint32 x_start = -(sint32)gScenarioTicks + 8;
    sint32 y_start = (gScenarioTicks * 3) + 7;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    rainDrawer->Draw(left, top, width, height, x_start, y_start);

    x_start = -(sint32)gScenarioTicks + 0x18;
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
static void DrawHeavyRain(IRainDrawer * rainDrawer, sint32 left, sint32 top, sint32 width, sint32 height)
{
    sint32 x_start = -(sint32)gScenarioTicks;
    sint32 y_start = gScenarioTicks * 5;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    rainDrawer->Draw(left, top, width, height, x_start, y_start);

    x_start = -(sint32)gScenarioTicks + 0x10;
    y_start = (gScenarioTicks * 6) + 5;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    rainDrawer->Draw(left, top, width, height, x_start, y_start);

    x_start = -(sint32)gScenarioTicks + 8;
    y_start = (gScenarioTicks * 3) + 7;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    rainDrawer->Draw(left, top, width, height, x_start, y_start);

    x_start = -(sint32)gScenarioTicks + 0x18;
    y_start = (gScenarioTicks * 4) + 0x0D;
    y_start = -y_start;
    x_start += left;
    y_start += top;
    rainDrawer->Draw(left, top, width, height, x_start, y_start);
}
