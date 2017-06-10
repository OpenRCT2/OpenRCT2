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

#include <stdexcept>
#include "../Context.h"
#include "../ui/UiContext.h"
#include "../core/Exception.hpp"
#include "../core/Registration.hpp"
#include "../interface/Screenshot.h"
#include "IDrawingContext.h"
#include "IDrawingEngine.h"
#include "NewDrawing.h"

extern "C"
{
    #include "../config/Config.h"
    #include "../drawing/drawing.h"
    #include "../localisation/string_ids.h"
    #include "../platform/platform.h"
    #include "../rct2.h"
}

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Ui;

static sint32                   _drawingEngineType  = DRAWING_ENGINE_SOFTWARE;
static IDrawingEngine *         _drawingEngine      = nullptr;

extern "C"
{
    rct_string_id DrawingEngineStringIds[] =
    {
        STR_DRAWING_ENGINE_SOFTWARE,
        STR_DRAWING_ENGINE_SOFTWARE_WITH_HARDWARE_DISPLAY,
        STR_DRAWING_ENGINE_OPENGL,
    };
}

extern "C"
{
    sint32 drawing_engine_get_type()
    {
        return _drawingEngineType;
    }

    bool drawing_engine_requires_restart(sint32 srcEngine, sint32 dstEngine)
    {
        // Linux requires a restart. This could be improved in the future by recreating the window,
        // https://github.com/OpenRCT2/OpenRCT2/issues/2015
        bool requiresRestart = true;
#ifdef __WINDOWS__
        if (dstEngine != DRAWING_ENGINE_OPENGL)
        {
            // Windows is apparently able to switch to hardware rendering on the fly although
            // using the same window in an unaccelerated and accelerated context is unsupported by SDL2
            requiresRestart = false;
        }
#endif
        return requiresRestart;
    }

    void drawing_engine_init()
    {
        assert(_drawingEngine == nullptr);

        _drawingEngineType = gConfigGeneral.drawing_engine;

        IContext * context = GetContext();
        IUiContext * uiContext = context->GetUiContext();
        IDrawingEngine * drawingEngine = uiContext->CreateDrawingEngine((DRAWING_ENGINE_TYPE)_drawingEngineType);
        if (drawingEngine == nullptr)
        {
            if (_drawingEngineType == DRAWING_ENGINE_SOFTWARE)
            {
                _drawingEngineType = DRAWING_ENGINE_NONE;
                log_fatal("Unable to create a drawing engine.");
                exit(-1);
            }
            else
            {
                log_error("Unable to create drawing engine. Falling back to software.");

                // Fallback to software
                gConfigGeneral.drawing_engine = DRAWING_ENGINE_SOFTWARE;
                config_save_default();
                drawing_engine_init();
            }
        }
        else
        {
            try
            {
                drawingEngine->Initialise();
                drawingEngine->SetUncappedFrameRate(gConfigGeneral.uncap_fps == 1);
                _drawingEngine = drawingEngine;
            }
            catch (const Exception &ex)
            {
                delete drawingEngine;
                drawingEngine = nullptr;
                if (_drawingEngineType == DRAWING_ENGINE_SOFTWARE)
                {
                    _drawingEngineType = DRAWING_ENGINE_NONE;
                    log_error(ex.GetMessage());
                    log_fatal("Unable to initialise a drawing engine.");
                    exit(-1);
                }
                else
                {
                    log_error(ex.GetMessage());
                    log_error("Unable to initialise drawing engine. Falling back to software.");

                    // Fallback to software
                    gConfigGeneral.drawing_engine = DRAWING_ENGINE_SOFTWARE;
                    config_save_default();
                    drawing_engine_init();
                }
            }
        }
    }

    void drawing_engine_resize()
    {
        if (_drawingEngine == nullptr)
        {
            drawing_engine_init();
        }

        IUiContext * uiContext = GetContext()->GetUiContext();
        _drawingEngine->Resize(uiContext->GetWidth(), uiContext->GetHeight());
    }

    void drawing_engine_set_palette(const rct_palette_entry * colours)
    {
        if (_drawingEngine != nullptr)
        {
            _drawingEngine->SetPalette(colours);
        }
    }

    void drawing_engine_draw()
    {
        if (_drawingEngine != nullptr)
        {
            _drawingEngine->Draw();
        }
    }

    void drawing_engine_copy_rect(sint32 x, sint32 y, sint32 width, sint32 height, sint32 dx, sint32 dy)
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

    void drawing_engine_set_fps_uncapped(bool uncapped)
    {
        if (_drawingEngine != nullptr)
        {
            _drawingEngine->SetUncappedFrameRate(uncapped);
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

    void gfx_clear(rct_drawpixelinfo * dpi, uint8 paletteIndex)
    {
        if (_drawingEngine != nullptr)
        {
            IDrawingContext * dc = _drawingEngine->GetDrawingContext(dpi);
            dc->Clear(paletteIndex);
        }
    }

    void gfx_fill_rect(rct_drawpixelinfo * dpi, sint32 left, sint32 top, sint32 right, sint32 bottom, sint32 colour)
    {
        if (_drawingEngine != nullptr)
        {
            IDrawingContext * dc = _drawingEngine->GetDrawingContext(dpi);
            dc->FillRect(colour, left, top, right, bottom);
        }
    }

    void gfx_filter_rect(rct_drawpixelinfo * dpi, sint32 left, sint32 top, sint32 right, sint32 bottom, FILTER_PALETTE_ID palette)
    {
        if (_drawingEngine != nullptr)
        {
            IDrawingContext * dc = _drawingEngine->GetDrawingContext(dpi);
            dc->FilterRect(palette, left, top, right, bottom);
        }
    }

    void gfx_draw_line(rct_drawpixelinfo *dpi, sint32 x1, sint32 y1, sint32 x2, sint32 y2, sint32 colour)
    {
        if (_drawingEngine != nullptr)
        {
            IDrawingContext * dc = _drawingEngine->GetDrawingContext(dpi);
            dc->DrawLine(colour, x1, y1, x2, y2);
        }
    }

    void FASTCALL gfx_draw_sprite(rct_drawpixelinfo * dpi, sint32 image, sint32 x, sint32 y, uint32 tertiary_colour)
    {
        if (_drawingEngine != nullptr)
        {
            IDrawingContext * dc = _drawingEngine->GetDrawingContext(dpi);
            dc->DrawSprite(image, x, y, tertiary_colour);
        }
    }

    void FASTCALL gfx_draw_glpyh(rct_drawpixelinfo * dpi, sint32 image, sint32 x, sint32 y, uint8 * palette)
    {
        if (_drawingEngine != nullptr)
        {
            IDrawingContext * dc = _drawingEngine->GetDrawingContext(dpi);
            dc->DrawGlyph(image, x, y, palette);
        }
    }

    void FASTCALL gfx_draw_sprite_raw_masked(rct_drawpixelinfo * dpi, sint32 x, sint32 y, sint32 maskImage, sint32 colourImage)
    {
        if (_drawingEngine != nullptr)
        {
            IDrawingContext * dc = _drawingEngine->GetDrawingContext(dpi);
            dc->DrawSpriteRawMasked(x, y, maskImage, colourImage);
        }
    }

    void FASTCALL gfx_draw_sprite_solid(rct_drawpixelinfo * dpi, sint32 image, sint32 x, sint32 y, uint8 colour)
    {
        if (_drawingEngine != nullptr)
        {
            IDrawingContext * dc = _drawingEngine->GetDrawingContext(dpi);
            dc->DrawSpriteSolid(image, x, y, colour);
        }
    }

    sint32 screenshot_dump()
    {
        if (_drawingEngine != nullptr)
        {
            return _drawingEngine->Screenshot();
        }
        return false;
    }
}
