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

extern "C"
{
    #include "../interface/window.h"
    #include "../world/Climate.h"
    #include "drawing.h"
    #include "../config/Config.h"
}

#include "IDrawingEngine.h"
#include "Rain.h"
#include "../core/Math.hpp"

using namespace OpenRCT2::Drawing;

typedef void (* DrawRainFunc)(IRainDrawer * rainDrawer, sint32 left, sint32 top, sint32 width, sint32 height);

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
 *  rct2: 0x00684383
 */
static void CallDrawRainFunc(IRainDrawer * rainDrawer,
                             rct_window * w,
                             sint16 left,
                             sint16 right,
                             sint16 top,
                             sint16 bottom,
                             uint32 rainType)
{
    rct_viewport * vp = w->viewport;
    if (vp == nullptr)
    {
        return;
    }

    left = Math::Max(left, vp->x);
    right = Math::Min(right, vp->width);
    top = Math::Max(top, vp->y);
    bottom = Math::Min(bottom, vp->height);
    if (left >= right || top >= bottom)
    {
        return;
    }

    sint32 width = right - left;
    sint32 height = bottom - top;
    DrawRainFunctions[rainType](rainDrawer, left, top, width, height);
}

/**
 *
 *  rct2: 0x006842AF
 *  From 0x00684383 on: split into call_draw_rain_func
 */
static void DrawRainWindow(IRainDrawer * rainDrawer,
                           rct_window * original_w,
                           sint16 left,
                           sint16 right,
                           sint16 top,
                           sint16 bottom,
                           uint32 rainType)
{
    if (!gConfigGeneral.render_weather_effects)
        return;

    rct_window * newWindow = gWindowNextSlot;
    rct_window * w = original_w + 1; // Start from second window
    for (; ; w++)
    {
        if (w >= newWindow)
        {
            // Loop ended, draw rain for original_w
            CallDrawRainFunc(rainDrawer, original_w, left, right, top, bottom, rainType);
            return;
        }

        if (right <= w->x || bottom <= w->y)
        {
            continue;
        }

        if (RCT_WINDOW_RIGHT(w) <= left || RCT_WINDOW_BOTTOM(w) <= top)
        {
            continue;
        }

        if (left >= w->x)
        {
            break;
        }

        DrawRainWindow(rainDrawer, original_w, left, w->x, top, bottom, rainType);

        left = w->x;
        DrawRainWindow(rainDrawer, original_w, left, right, top, bottom, rainType);
        return;
    }

    sint16 w_right = RCT_WINDOW_RIGHT(w);
    if (right > w_right) {
        DrawRainWindow(rainDrawer, original_w, left, w_right, top, bottom, rainType);

        left = w_right;
        DrawRainWindow(rainDrawer, original_w, left, right, top, bottom, rainType);
        return;
    }

    if (top < w->y) {
        DrawRainWindow(rainDrawer, original_w, left, right, top, w->y, rainType);

        top = w->y;
        DrawRainWindow(rainDrawer, original_w, left, right, top, bottom, rainType);
        return;
    }

    sint16 w_bottom = RCT_WINDOW_BOTTOM(w);
    if (bottom > w_bottom)
    {
        DrawRainWindow(rainDrawer, original_w, left, right, top, w_bottom, rainType);

        top = w_bottom;
        DrawRainWindow(rainDrawer, original_w, left, right, top, bottom, rainType);
        return;
    }
}

/**
 *
 *  rct2: 0x00684266
 */
static void DrawRainAnimation(rct_drawpixelinfo * dpi, IRainDrawer * rainDrawer, uint32 rainType)
{
    sint32 left = dpi->x;
    sint32 right = left + dpi->width;
    sint32 top = dpi->y;
    sint32 bottom = top + dpi->height;

    rct_window * newWindow = gWindowNextSlot;
    for (rct_window * w = g_window_list; w < newWindow; w++)
    {
        DrawRainWindow(rainDrawer, w, left, right, top, bottom, rainType);
    }
}

/**
 *
 *  rct2: 0x00684218
 */
void DrawRain(rct_drawpixelinfo * dpi, IRainDrawer * rainDrawer)
{
    // Get rain draw function and draw rain
    uint32 rainType = gClimateCurrentRainLevel;
    if (rainType > 0 && !gTrackDesignSaveMode)
    {
        DrawRainAnimation(dpi, rainDrawer, rainType);
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
