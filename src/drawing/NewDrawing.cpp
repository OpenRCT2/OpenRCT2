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
    #include "../config.h"
	#include "../localisation/string_ids.h"
    #include "../platform/platform.h"
}

static IDrawingEngine * _drawingEngine = nullptr;

extern "C" rct_string_id DrawingEngineStringIds[] =
{
    STR_DRAWING_ENGINE_SOFTWARE,
    STR_DRAWING_ENGINE_SOFTWARE_WITH_HARDWARE_DISPLAY,
    STR_DRAWING_ENGINE_OPENGL,
};

extern "C"
{
    void drawing_engine_init()
    {
        assert(_drawingEngine == nullptr);
        switch (gConfigGeneral.drawing_engine) {
        case DRAWING_ENGINE_SOFTWARE:
            _drawingEngine = DrawingEngineFactory::CreateSoftware();
            break;
        case DRAWING_ENGINE_SOFTWARE_WITH_HARDWARE_DISPLAY:
            _drawingEngine = DrawingEngineFactory::CreateSoftware();
            break;
        case DRAWING_ENGINE_OPENGL:
            _drawingEngine = DrawingEngineFactory::CreateOpenGL();
            break;
        }
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

    void drawing_engine_copy_rect(int x, int y, int width, int height, int dx, int dy)
    {
        if (_drawingEngine != nullptr)
        {
            _drawingEngine->CopyRect(x, y, width, height, dx, dy);
        }
    }

    void drawing_engine_dispose()
    {
        delete _drawingEngine;
        _drawingEngine = nullptr;
    }

    rct_drawpixelinfo * drawing_engine_get_dpi()
    {
        assert(_drawingEngine != nullptr);
        return _drawingEngine->GetDrawingPixelInfo();
    }

    bool drawing_engine_has_dirty_optimisations()
    {
        bool result = false;
        if (_drawingEngine != nullptr)
        {
            result = (_drawingEngine->GetFlags() & DEF_DIRTY_OPTIMISATIONS);
        }
        return result;
    }

    void drawing_engine_invalidate_image(uint32 image)
    {
        if (_drawingEngine != nullptr)
        {
            _drawingEngine->InvalidateImage(image);
        }
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

    void gfx_clear(rct_drawpixelinfo * dpi, int colour)
    {
        if (_drawingEngine != nullptr)
        {
            IDrawingContext * dc = _drawingEngine->GetDrawingContext(dpi);
            dc->Clear(colour);
        }
    }

    void gfx_fill_rect(rct_drawpixelinfo * dpi, int left, int top, int right, int bottom, int colour)
    {
        if (_drawingEngine != nullptr)
        {
            IDrawingContext * dc = _drawingEngine->GetDrawingContext(dpi);
            dc->FillRect(colour, left, top, right, bottom);
        }
    }

    void gfx_draw_line(rct_drawpixelinfo *dpi, int x1, int y1, int x2, int y2, int colour)
    {
        if (_drawingEngine != nullptr)
        {
            IDrawingContext * dc = _drawingEngine->GetDrawingContext(dpi);
            dc->DrawLine(colour, x1, y1, x2, y2);
        }
    }

    void FASTCALL gfx_draw_sprite(rct_drawpixelinfo * dpi, int image, int x, int y, uint32 tertiary_colour)
    {
        if (_drawingEngine != nullptr)
        {
            IDrawingContext * dc = _drawingEngine->GetDrawingContext(dpi);
            dc->DrawSprite(image, x, y, tertiary_colour);
        }
    }

    void FASTCALL gfx_draw_sprite_palette_set(rct_drawpixelinfo * dpi, int image, int x, int y, uint8 * palette, uint8 * unknown)
    {
        if (_drawingEngine != nullptr)
        {
            IDrawingContext * dc = _drawingEngine->GetDrawingContext(dpi);
            dc->DrawSpritePaletteSet(image, x, y, palette, unknown);
        }
    }

    void FASTCALL gfx_draw_sprite_raw_masked(rct_drawpixelinfo * dpi, int x, int y, int maskImage, int colourImage)
    {
        if (_drawingEngine != nullptr)
        {
            IDrawingContext * dc = _drawingEngine->GetDrawingContext(dpi);
            dc->DrawSpriteRawMasked(x, y, maskImage, colourImage);
        }
    }

    int screenshot_dump()
    {
        if (_drawingEngine != nullptr)
        {
            return _drawingEngine->Screenshot();
        }
        return false;
    }
}
