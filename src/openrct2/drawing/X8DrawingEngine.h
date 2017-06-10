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

#pragma once

#include "../common.h"
#include "IDrawingContext.h"
#include "IDrawingEngine.h"

namespace OpenRCT2 { namespace Drawing
{
    class X8DrawingContext;

    struct DirtyGrid
    {
        uint32  BlockShiftX;
        uint32  BlockShiftY;
        uint32  BlockWidth;
        uint32  BlockHeight;
        uint32  BlockColumns;
        uint32  BlockRows;
        uint8 * Blocks;
    };

    class X8RainDrawer final : public IRainDrawer
    {
    private:
        struct RainPixel
        {
            uint32 Position;
            uint8  Colour;
        };

        static constexpr uint32 MaxRainPixels = 0xFFFE;

        size_t              _rainPixelsCapacity = MaxRainPixels;
        uint32              _rainPixelsCount    = 0;
        RainPixel *         _rainPixels         = nullptr;
        rct_drawpixelinfo * _screenDPI          = nullptr;

    public:
        X8RainDrawer();
        ~X8RainDrawer();
        void SetDPI(rct_drawpixelinfo * dpi);
        void Draw(sint32 x, sint32 y, sint32 width, sint32 height, sint32 xStart, sint32 yStart) override;
        void Restore();
    };

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-final-types"
    class X8DrawingEngine : public IDrawingEngine
    {
    protected:
        uint32  _width      = 0;
        uint32  _height     = 0;
        uint32  _pitch      = 0;
        size_t  _bitsSize   = 0;
        uint8 * _bits       = nullptr;

        DirtyGrid   _dirtyGrid  = { 0 };

        rct_drawpixelinfo _bitsDPI  = { 0 };

#ifdef __ENABLE_LIGHTFX__
        bool _lastLightFXenabled = false;
#endif

        X8RainDrawer        _rainDrawer;
        X8DrawingContext *  _drawingContext;

    public:
        explicit X8DrawingEngine();
        ~X8DrawingEngine() override;

        void Initialise(SDL_Window * window) override;
        void Resize(uint32 width, uint32 height) override;
        void SetPalette(const rct_palette_entry * palette) override;
        void SetUncappedFrameRate(bool uncapped) override;
        void Invalidate(sint32 left, sint32 top, sint32 right, sint32 bottom) override;
        void Draw() override;
        void CopyRect(sint32 x, sint32 y, sint32 width, sint32 height, sint32 dx, sint32 dy) override;
        sint32 Screenshot() override;
        IDrawingContext * GetDrawingContext(rct_drawpixelinfo * dpi) override;
        rct_drawpixelinfo * GetDrawingPixelInfo() override;
        DRAWING_ENGINE_FLAGS GetFlags() override;
        void InvalidateImage(uint32 image) override;

        rct_drawpixelinfo * GetDPI();

    protected:
        void ConfigureBits(uint32 width, uint32 height, uint32 pitch);

    private:
        void ConfigureDirtyGrid();
        static void ResetWindowVisbilities();
        void DrawAllDirtyBlocks();
        void DrawDirtyBlocks(uint32 x, uint32 y, uint32 columns, uint32 rows);
    };
#pragma GCC diagnostic pop

    class X8DrawingContext final : public IDrawingContext
    {
    private:
        X8DrawingEngine *   _engine = nullptr;
        rct_drawpixelinfo * _dpi    = nullptr;

    public:
        explicit X8DrawingContext(X8DrawingEngine * engine);
        ~X8DrawingContext() override;

        IDrawingEngine * GetEngine() override;

        void Clear(uint8 paletteIndex) override;
        void FillRect(uint32 colour, sint32 x, sint32 y, sint32 w, sint32 h) override;
        void FilterRect(FILTER_PALETTE_ID palette, sint32 left, sint32 top, sint32 right, sint32 bottom) override;
        void DrawLine(uint32 colour, sint32 x1, sint32 y1, sint32 x2, sint32 y2) override;
        void DrawSprite(uint32 image, sint32 x, sint32 y, uint32 tertiaryColour) override;
        void DrawSpriteRawMasked(sint32 x, sint32 y, uint32 maskImage, uint32 colourImage) override;
        void DrawSpriteSolid(uint32 image, sint32 x, sint32 y, uint8 colour) override;
        void DrawGlyph(uint32 image, sint32 x, sint32 y, uint8 * palette) override;

        void SetDPI(rct_drawpixelinfo * dpi);
    };
} }
