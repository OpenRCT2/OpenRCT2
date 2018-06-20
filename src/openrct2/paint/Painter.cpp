/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../config/Config.h"
#include "../drawing/IDrawingEngine.h"
#include "../OpenRCT2.h"
#include "../title/TitleScreen.h"
#include "../ui/UiContext.h"
#include "Painter.h"

#include "../drawing/Drawing.h"
#include "../Game.h"
#include "../interface/Chat.h"
#include "../interface/InteractiveConsole.h"
#include "../Intro.h"
#include "../localisation/Language.h"
#include "../localisation/FormatCodes.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Paint;
using namespace OpenRCT2::Ui;

Painter::Painter(const std::shared_ptr<IUiContext>& uiContext)
    : _uiContext(uiContext)
{
}

void Painter::Paint(IDrawingEngine& de)
{
    auto dpi = de.GetDrawingPixelInfo();
    if (gIntroState != INTRO_STATE_NONE)
    {
        intro_draw(dpi);
    }
    else
    {
        de.PaintWindows();

        update_palette_effects();
        _uiContext->Draw(dpi);

        if ((gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) && !title_should_hide_version_info())
        {
            DrawOpenRCT2(dpi, 0, _uiContext->GetHeight() - 20);
        }

        gfx_draw_pickedup_peep(dpi);
        gfx_invalidate_pickedup_peep();

        de.PaintRain();
    }

    if (gConfigGeneral.show_fps)
    {
        PaintFPS(dpi);
    }
    gCurrentDrawCount++;
}

void Painter::PaintFPS(rct_drawpixelinfo * dpi)
{
    int32_t x = _uiContext->GetWidth() / 2;
    int32_t y = 2;

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
    int32_t stringWidth = gfx_get_string_width(buffer);
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
