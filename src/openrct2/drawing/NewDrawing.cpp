/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Context.h"
#include "../ui/UiContext.h"
#include "../interface/Screenshot.h"
#include "../paint/Painter.h"
#include "IDrawingContext.h"
#include "IDrawingEngine.h"
#include "NewDrawing.h"

#include "../config/Config.h"
#include "../drawing/Drawing.h"
#include "../localisation/StringIds.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Paint;
using namespace OpenRCT2::Ui;

rct_string_id DrawingEngineStringIds[] =
{
    STR_DRAWING_ENGINE_SOFTWARE,
    STR_DRAWING_ENGINE_SOFTWARE_WITH_HARDWARE_DISPLAY,
    STR_DRAWING_ENGINE_OPENGL,
};

int32_t drawing_engine_get_type()
{
    auto context = GetContext();
    return context->GetDrawingEngineType();
}

static IDrawingEngine * GetDrawingEngine()
{
    IDrawingEngine * result = nullptr;
    auto context = GetContext();
    if (context != nullptr)
    {
        result = context->GetDrawingEngine();
    }
    return result;
}

bool drawing_engine_requires_new_window(int32_t srcEngine, int32_t dstEngine)
{
#ifdef _WIN32
    if (srcEngine != DRAWING_ENGINE_OPENGL && dstEngine != DRAWING_ENGINE_OPENGL)
    {
        // Windows is apparently able to switch to hardware rendering on the fly although
        // using the same window in an unaccelerated and accelerated context is unsupported by SDL2
        return false;
    }
#endif

    return true;
}

void drawing_engine_init()
{
    auto context = GetContext();
    if (context != nullptr)
    {
        context->InitialiseDrawingEngine();
    }
}

void drawing_engine_resize()
{
    auto context = GetContext();
    if (context != nullptr)
    {
        auto drawingEngine = context->GetDrawingEngine();
        if (drawingEngine != nullptr)
        {
            auto uiContext = context->GetUiContext();
            drawingEngine->Resize(uiContext->GetWidth(), uiContext->GetHeight());
        }
    }
}

void drawing_engine_set_palette(const rct_palette_entry * colours)
{
    auto context = GetContext();
    if (context != nullptr)
    {
        auto drawingEngine = context->GetDrawingEngine();
        if (drawingEngine != nullptr)
        {
            drawingEngine->SetPalette(colours);
        }
    }
}

void drawing_engine_copy_rect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t dx, int32_t dy)
{
    auto context = GetContext();
    if (context != nullptr)
    {
        auto drawingEngine = context->GetDrawingEngine();
        if (drawingEngine != nullptr)
        {
            drawingEngine->CopyRect(x, y, width, height, dx, dy);
        }
    }
}

void drawing_engine_dispose()
{
    auto context = GetContext();
    if (context != nullptr)
    {
        context->DisposeDrawingEngine();
    }
}

rct_drawpixelinfo * drawing_engine_get_dpi()
{
    auto context = GetContext();
    auto drawingEngine = context->GetDrawingEngine();
    return drawingEngine->GetDrawingPixelInfo();
}

bool drawing_engine_has_dirty_optimisations()
{
    bool result = false;
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        result = (drawingEngine->GetFlags() & DEF_DIRTY_OPTIMISATIONS);
    }
    return result;
}

void drawing_engine_invalidate_image(uint32_t image)
{
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        drawingEngine->InvalidateImage(image);
    }
}

void drawing_engine_set_vsync(bool vsync)
{
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        drawingEngine->SetVSync(vsync);
    }
}

void gfx_set_dirty_blocks(int16_t left, int16_t top, int16_t right, int16_t bottom)
{
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        drawingEngine->Invalidate(left, top, right, bottom);
    }
}

void gfx_draw_all_dirty_blocks()
{
}

void gfx_clear(rct_drawpixelinfo * dpi, uint8_t paletteIndex)
{
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        IDrawingContext * dc = drawingEngine->GetDrawingContext(dpi);
        dc->Clear(paletteIndex);
    }
}

void gfx_fill_rect(rct_drawpixelinfo * dpi, int32_t left, int32_t top, int32_t right, int32_t bottom, int32_t colour)
{
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        IDrawingContext * dc = drawingEngine->GetDrawingContext(dpi);
        dc->FillRect(colour, left, top, right, bottom);
    }
}

void gfx_filter_rect(rct_drawpixelinfo * dpi, int32_t left, int32_t top, int32_t right, int32_t bottom, FILTER_PALETTE_ID palette)
{
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        IDrawingContext * dc = drawingEngine->GetDrawingContext(dpi);
        dc->FilterRect(palette, left, top, right, bottom);
    }
}

void gfx_draw_line(rct_drawpixelinfo *dpi, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t colour)
{
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        IDrawingContext * dc = drawingEngine->GetDrawingContext(dpi);
        dc->DrawLine(colour, x1, y1, x2, y2);
    }
}

void FASTCALL gfx_draw_sprite(rct_drawpixelinfo * dpi, int32_t image, int32_t x, int32_t y, uint32_t tertiary_colour)
{
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        IDrawingContext * dc = drawingEngine->GetDrawingContext(dpi);
        dc->DrawSprite(image, x, y, tertiary_colour);
    }
}

void FASTCALL gfx_draw_glpyh(rct_drawpixelinfo * dpi, int32_t image, int32_t x, int32_t y, uint8_t * palette)
{
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        IDrawingContext * dc = drawingEngine->GetDrawingContext(dpi);
        dc->DrawGlyph(image, x, y, palette);
    }
}

void FASTCALL gfx_draw_sprite_raw_masked(rct_drawpixelinfo * dpi, int32_t x, int32_t y, int32_t maskImage, int32_t colourImage)
{
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        IDrawingContext * dc = drawingEngine->GetDrawingContext(dpi);
        dc->DrawSpriteRawMasked(x, y, maskImage, colourImage);
    }
}

void FASTCALL gfx_draw_sprite_solid(rct_drawpixelinfo * dpi, int32_t image, int32_t x, int32_t y, uint8_t colour)
{
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        IDrawingContext * dc = drawingEngine->GetDrawingContext(dpi);
        dc->DrawSpriteSolid(image, x, y, colour);
    }
}

int32_t screenshot_dump()
{
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        return drawingEngine->Screenshot();
    }
    return false;
}

