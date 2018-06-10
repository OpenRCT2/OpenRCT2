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

sint32 drawing_engine_get_type()
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

bool drawing_engine_requires_new_window(sint32 srcEngine, sint32 dstEngine)
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

void drawing_engine_copy_rect(sint32 x, sint32 y, sint32 width, sint32 height, sint32 dx, sint32 dy)
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

void drawing_engine_invalidate_image(uint32 image)
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

void gfx_set_dirty_blocks(sint16 left, sint16 top, sint16 right, sint16 bottom)
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

void gfx_clear(rct_drawpixelinfo * dpi, uint8 paletteIndex)
{
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        IDrawingContext * dc = drawingEngine->GetDrawingContext(dpi);
        dc->Clear(paletteIndex);
    }
}

void gfx_fill_rect(rct_drawpixelinfo * dpi, sint32 left, sint32 top, sint32 right, sint32 bottom, sint32 colour)
{
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        IDrawingContext * dc = drawingEngine->GetDrawingContext(dpi);
        dc->FillRect(colour, left, top, right, bottom);
    }
}

void gfx_filter_rect(rct_drawpixelinfo * dpi, sint32 left, sint32 top, sint32 right, sint32 bottom, FILTER_PALETTE_ID palette)
{
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        IDrawingContext * dc = drawingEngine->GetDrawingContext(dpi);
        dc->FilterRect(palette, left, top, right, bottom);
    }
}

void gfx_draw_line(rct_drawpixelinfo *dpi, sint32 x1, sint32 y1, sint32 x2, sint32 y2, sint32 colour)
{
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        IDrawingContext * dc = drawingEngine->GetDrawingContext(dpi);
        dc->DrawLine(colour, x1, y1, x2, y2);
    }
}

void FASTCALL gfx_draw_sprite(rct_drawpixelinfo * dpi, sint32 image, sint32 x, sint32 y, uint32 tertiary_colour)
{
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        IDrawingContext * dc = drawingEngine->GetDrawingContext(dpi);
        dc->DrawSprite(image, x, y, tertiary_colour);
    }
}

void FASTCALL gfx_draw_glpyh(rct_drawpixelinfo * dpi, sint32 image, sint32 x, sint32 y, uint8 * palette)
{
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        IDrawingContext * dc = drawingEngine->GetDrawingContext(dpi);
        dc->DrawGlyph(image, x, y, palette);
    }
}

void FASTCALL gfx_draw_sprite_raw_masked(rct_drawpixelinfo * dpi, sint32 x, sint32 y, sint32 maskImage, sint32 colourImage)
{
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        IDrawingContext * dc = drawingEngine->GetDrawingContext(dpi);
        dc->DrawSpriteRawMasked(x, y, maskImage, colourImage);
    }
}

void FASTCALL gfx_draw_sprite_solid(rct_drawpixelinfo * dpi, sint32 image, sint32 x, sint32 y, uint8 colour)
{
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        IDrawingContext * dc = drawingEngine->GetDrawingContext(dpi);
        dc->DrawSpriteSolid(image, x, y, colour);
    }
}

sint32 screenshot_dump()
{
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        return drawingEngine->Screenshot();
    }
    return false;
}

