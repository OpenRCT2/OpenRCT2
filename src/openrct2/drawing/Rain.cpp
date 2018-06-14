#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

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
