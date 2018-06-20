/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "IDrawingContext.h"
#include "IDrawingEngine.h"

namespace OpenRCT2
{
    namespace Ui
    {
        interface IUiContext;
    } // namespace Ui

    namespace Drawing
    {
        class X8DrawingContext;

        struct DirtyGrid
        {
            uint32_t  BlockShiftX;
            uint32_t  BlockShiftY;
            uint32_t  BlockWidth;
            uint32_t  BlockHeight;
            uint32_t  BlockColumns;
            uint32_t  BlockRows;
            uint8_t * Blocks;
        };

        class X8RainDrawer final : public IRainDrawer
        {
        private:
            struct RainPixel
            {
                uint32_t Position;
                uint8_t  Colour;
            };

            static constexpr uint32_t MaxRainPixels = 0xFFFE;

            size_t              _rainPixelsCapacity = MaxRainPixels;
            uint32_t              _rainPixelsCount    = 0;
            RainPixel *         _rainPixels         = nullptr;
            rct_drawpixelinfo * _screenDPI          = nullptr;

        public:
            X8RainDrawer();
            ~X8RainDrawer();
            void SetDPI(rct_drawpixelinfo * dpi);
            void Draw(int32_t x, int32_t y, int32_t width, int32_t height, int32_t xStart, int32_t yStart) override;
            void Restore();
        };

#ifdef __WARN_SUGGEST_FINAL_TYPES__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsuggest-final-types"
#endif
        class X8DrawingEngine : public IDrawingEngine
        {
        protected:
            uint32_t  _width      = 0;
            uint32_t  _height     = 0;
            uint32_t  _pitch      = 0;
            size_t  _bitsSize   = 0;
            uint8_t * _bits       = nullptr;

            DirtyGrid   _dirtyGrid  = {};

            rct_drawpixelinfo _bitsDPI  = {};

    #ifdef __ENABLE_LIGHTFX__
            bool _lastLightFXenabled = false;
    #endif

            X8RainDrawer        _rainDrawer;
            X8DrawingContext *  _drawingContext;

        public:
            explicit X8DrawingEngine(const std::shared_ptr<Ui::IUiContext>& uiContext);
            ~X8DrawingEngine() override;

            void Initialise() override;
            void Resize(uint32_t width, uint32_t height) override;
            void SetPalette(const rct_palette_entry * palette) override;
            void SetVSync(bool vsync) override;
            void Invalidate(int32_t left, int32_t top, int32_t right, int32_t bottom) override;
            void BeginDraw() override;
            void EndDraw() override;
            void PaintWindows() override;
            void PaintRain() override;
            void CopyRect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t dx, int32_t dy) override;
            int32_t Screenshot() override;
            IDrawingContext * GetDrawingContext(rct_drawpixelinfo * dpi) override;
            rct_drawpixelinfo * GetDrawingPixelInfo() override;
            DRAWING_ENGINE_FLAGS GetFlags() override;
            void InvalidateImage(uint32_t image) override;

            rct_drawpixelinfo * GetDPI();

        protected:
            void ConfigureBits(uint32_t width, uint32_t height, uint32_t pitch);
            virtual void OnDrawDirtyBlock(uint32_t x, uint32_t y, uint32_t columns, uint32_t rows);

        private:
            void ConfigureDirtyGrid();
            static void ResetWindowVisbilities();
            void DrawAllDirtyBlocks();
            void DrawDirtyBlocks(uint32_t x, uint32_t y, uint32_t columns, uint32_t rows);
        };
#ifdef __WARN_SUGGEST_FINAL_TYPES__
    #pragma GCC diagnostic pop
#endif

        class X8DrawingContext final : public IDrawingContext
        {
        private:
            X8DrawingEngine *   _engine = nullptr;
            rct_drawpixelinfo * _dpi    = nullptr;

        public:
            explicit X8DrawingContext(X8DrawingEngine * engine);

            IDrawingEngine * GetEngine() override;

            void Clear(uint8_t paletteIndex) override;
            void FillRect(uint32_t colour, int32_t x, int32_t y, int32_t w, int32_t h) override;
            void FilterRect(FILTER_PALETTE_ID palette, int32_t left, int32_t top, int32_t right, int32_t bottom) override;
            void DrawLine(uint32_t colour, int32_t x1, int32_t y1, int32_t x2, int32_t y2) override;
            void DrawSprite(uint32_t image, int32_t x, int32_t y, uint32_t tertiaryColour) override;
            void DrawSpriteRawMasked(int32_t x, int32_t y, uint32_t maskImage, uint32_t colourImage) override;
            void DrawSpriteSolid(uint32_t image, int32_t x, int32_t y, uint8_t colour) override;
            void DrawGlyph(uint32_t image, int32_t x, int32_t y, uint8_t * palette) override;

            void SetDPI(rct_drawpixelinfo * dpi);
        };
    } // namespace Drawing
} // namespace OpenRCT2
