/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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

#include <memory>

namespace OpenRCT2
{
    namespace Ui
    {
        struct IUiContext;
    } // namespace Ui

    namespace Drawing
    {
        class X8DrawingContext;

        struct DirtyGrid
        {
            uint32_t BlockShiftX;
            uint32_t BlockShiftY;
            uint32_t BlockWidth;
            uint32_t BlockHeight;
            uint32_t BlockColumns;
            uint32_t BlockRows;
            uint8_t* Blocks;
        };

        class X8WeatherDrawer final : public IWeatherDrawer
        {
        private:
            struct WeatherPixel
            {
                uint32_t Position;
                uint8_t Colour;
            };

            static constexpr uint32_t MaxWeatherPixels = 0xFFFE;

            size_t _weatherPixelsCapacity = MaxWeatherPixels;
            uint32_t _weatherPixelsCount = 0;
            WeatherPixel* _weatherPixels = nullptr;

        public:
            X8WeatherDrawer();
            ~X8WeatherDrawer();
            void Draw(
                rct_drawpixelinfo* dpi, int32_t x, int32_t y, int32_t width, int32_t height, int32_t xStart, int32_t yStart,
                const uint8_t* weatherpattern) override;
            void Restore(rct_drawpixelinfo* dpi);
        };

#ifdef __WARN_SUGGEST_FINAL_TYPES__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wsuggest-final-types"
#endif
        class X8DrawingEngine : public IDrawingEngine
        {
        protected:
            uint32_t _width = 0;
            uint32_t _height = 0;
            uint32_t _pitch = 0;
            size_t _bitsSize = 0;
            uint8_t* _bits = nullptr;

            DirtyGrid _dirtyGrid = {};

            rct_drawpixelinfo _bitsDPI = {};

            bool _lastLightFXenabled = false;

            X8WeatherDrawer _weatherDrawer;
            X8DrawingContext* _drawingContext;

        public:
            explicit X8DrawingEngine(const std::shared_ptr<Ui::IUiContext>& uiContext);

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wsuggest-final-methods"
#    pragma GCC diagnostic ignored "-Wsuggest-final-types"
#endif
            ~X8DrawingEngine() override;
#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic pop
#endif

            void Initialise() override;
            void Resize(uint32_t width, uint32_t height) override;
            void SetPalette(const GamePalette& palette) override;
            void SetVSync(bool vsync) override;
            void Invalidate(int32_t left, int32_t top, int32_t right, int32_t bottom) override;
            void BeginDraw() override;
            void EndDraw() override;
            void PaintWindows() override;
            void PaintWeather() override;
            void CopyRect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t dx, int32_t dy) override;
            std::string Screenshot() override;
            IDrawingContext* GetDrawingContext() override;
            rct_drawpixelinfo* GetDrawingPixelInfo() override;
            DRAWING_ENGINE_FLAGS GetFlags() override;
            void InvalidateImage(uint32_t image) override;

            rct_drawpixelinfo* GetDPI();

        protected:
            void ConfigureBits(uint32_t width, uint32_t height, uint32_t pitch);
            virtual void OnDrawDirtyBlock(uint32_t x, uint32_t y, uint32_t columns, uint32_t rows);

        private:
            void ConfigureDirtyGrid();
            void DrawAllDirtyBlocks();
            uint32_t GetNumDirtyRows(const uint32_t x, const uint32_t y, const uint32_t columns);
            void DrawDirtyBlocks(uint32_t x, uint32_t y, uint32_t columns, uint32_t rows);
        };
#ifdef __WARN_SUGGEST_FINAL_TYPES__
#    pragma GCC diagnostic pop
#endif

        class X8DrawingContext final : public IDrawingContext
        {
        private:
            X8DrawingEngine* _engine = nullptr;

        public:
            explicit X8DrawingContext(X8DrawingEngine* engine);

            void Clear(rct_drawpixelinfo* dpi, uint8_t paletteIndex) override;
            void FillRect(rct_drawpixelinfo* dpi, uint32_t colour, int32_t x, int32_t y, int32_t w, int32_t h) override;
            void FilterRect(
                rct_drawpixelinfo* dpi, FilterPaletteID palette, int32_t left, int32_t top, int32_t right,
                int32_t bottom) override;
            void DrawLine(rct_drawpixelinfo* dpi, uint32_t colour, const ScreenLine& line) override;
            void DrawSprite(rct_drawpixelinfo* dpi, const ImageId imageId, int32_t x, int32_t y) override;
            void DrawSpriteRawMasked(
                rct_drawpixelinfo* dpi, int32_t x, int32_t y, const ImageId maskImage, const ImageId colourImage) override;
            void DrawSpriteSolid(rct_drawpixelinfo* dpi, const ImageId image, int32_t x, int32_t y, uint8_t colour) override;
            void DrawGlyph(
                rct_drawpixelinfo* dpi, const ImageId image, int32_t x, int32_t y, const PaletteMap& paletteMap) override;
            void DrawBitmap(
                rct_drawpixelinfo* dpi, uint32_t image, const void* pixels, int32_t width, int32_t height, int32_t x,
                int32_t y) override
            {
            }
        };
    } // namespace Drawing
} // namespace OpenRCT2
