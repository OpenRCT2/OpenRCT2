#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include "IDrawingContext.h"
#include "IDrawingEngine.h"

extern "C"
{
    #include "../platform/platform.h"
}

static IDrawingEngine * _drawingEngine = nullptr;

extern "C"
{
    void drawing_engine_init()
    {
        assert(_drawingEngine == nullptr);
        _drawingEngine = DrawingEngineFactory::CreateSoftware();
        _drawingEngine->Initialise(gWindow);
    }

    void drawing_engine_resize()
    {
        if (_drawingEngine == nullptr)
        {
            drawing_engine_init();
        }
        _drawingEngine->Resize(gScreenWidth, gScreenHeight);
    }

    void drawing_engine_set_palette(SDL_Color * colours)
    {
        _drawingEngine->SetPalette(colours);
    }

    void drawing_engine_draw()
    {
        _drawingEngine->Draw();
    }

    void drawing_engine_dispose()
    {
        delete _drawingEngine;
        _drawingEngine = nullptr;
    }

    void gfx_set_dirty_blocks(sint16 left, sint16 top, sint16 right, sint16 bottom)
    {
        if (_drawingEngine != nullptr)
        {
            _drawingEngine->Invalidate(left, top, right, bottom);
        }
    }

    void gfx_draw_all_dirty_blocks()
    {
    }

    /**
     * Clears the screen with the specified colour.
     *  rct2: 0x00678A9F
     */
    void gfx_clear(rct_drawpixelinfo * dpi, int colour)
    {
        if (_drawingEngine != nullptr)
        {
            IDrawingContext * dc = _drawingEngine->GetDrawingContext(dpi);
            dc->Clear(colour);
        }
    }

    /**
     *
     *  rct2: 0x00678AD4
     * dpi (edi)
     * left (ax)
     * top (cx)
     * right (bx)
     * bottom (dx)
     * colour (ebp)
     */
    void gfx_fill_rect(rct_drawpixelinfo *dpi, int left, int top, int right, int bottom, int colour)
    {
        if (_drawingEngine != nullptr)
        {
            IDrawingContext * dc = _drawingEngine->GetDrawingContext(dpi);
            dc->FillRect(colour, left, top, right, bottom);
        }
    }
}
