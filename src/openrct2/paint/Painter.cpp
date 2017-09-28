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
#include "../drawing/IDrawingEngine.h"
#include "../OpenRCT2.h"
#include "../title/TitleScreen.h"
#include "../ui/UiContext.h"
#include "Painter.h"

#include "../drawing/drawing.h"
#include "../game.h"
#include "../interface/chat.h"
#include "../interface/console.h"
#include "../intro.h"
#include "../localisation/localisation.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Paint;
using namespace OpenRCT2::Ui;

Painter::Painter(IUiContext * uiContext)
    : _uiContext(uiContext)
{
}

void Painter::Paint(IDrawingEngine * de)
{
    auto dpi = de->GetDrawingPixelInfo();
    if (gIntroState != INTRO_STATE_NONE)
    {
        intro_draw(dpi);
    }
    else
    {
        de->PaintWindows();

        update_palette_effects();
        chat_draw(dpi);
        console_draw(dpi);

        if ((gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) && !title_should_hide_version_info())
        {
            DrawOpenRCT2(dpi, 0, _uiContext->GetHeight() - 20);
        }

        gfx_draw_pickedup_peep(dpi);
        gfx_invalidate_pickedup_peep();

        de->PaintRain();
    }

    if (gConfigGeneral.show_fps)
    {
        PaintFPS(dpi);
    }
    gCurrentDrawCount++;
}

void Painter::PaintFPS(rct_drawpixelinfo * dpi)
{
    sint32 x = _uiContext->GetWidth() / 2;
    sint32 y = 2;

    // Measure FPS
    MeasureFPS();

    // Format string
    utf8 buffer[64] = { 0 };
    utf8 * ch = buffer;
    ch = utf8_write_codepoint(ch, FORMAT_MEDIUMFONT);
    ch = utf8_write_codepoint(ch, FORMAT_OUTLINE);
    ch = utf8_write_codepoint(ch, FORMAT_WHITE);

    snprintf(ch, 64 - (ch - buffer), "%d", _currentFPS);

    // Draw Text
    sint32 stringWidth = gfx_get_string_width(buffer);
    x = x - (stringWidth / 2);
    gfx_draw_string(dpi, buffer, 0, x, y);

    // Make area dirty so the text doesn't get drawn over the last
    gfx_set_dirty_blocks(x - 16, y - 4, gLastDrawStringX + 16, 16);
}

void Painter::MeasureFPS()
{
    _frames++;

    auto currentTime = time(nullptr);
    if (currentTime != _lastSecond)
    {
        _currentFPS = _frames;
        _frames = 0;
    }
    _lastSecond = currentTime;
}
