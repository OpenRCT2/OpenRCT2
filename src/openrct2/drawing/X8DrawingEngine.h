/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "IDrawingContext.h"
#include "IDrawingEngine.h"
#include "InvalidationGrid.h"
#include "RenderTarget.h"

#include <memory>

struct TextDrawInfo;

namespace OpenRCT2
{
    namespace Ui
    {
        struct IUiContext;
    } // namespace Ui

    namespace Drawing
    {
        class X8DrawingContext;

        class X8WeatherDrawer final : public IWeatherDrawer
        {
        private:
            struct WeatherPixel
            {
                uint32_t Position;
                uint8_t Colour;
            };

            static constexpr uint32_t kMaxWeatherPixels = 0xFFFE;

            size_t _weatherPixelsCapacity = kMaxWeatherPixels;
            uint32_t _weatherPixelsCount = 0;
            WeatherPixel* _weatherPixels = nullptr;

        public:
            X8WeatherDrawer();
            ~X8WeatherDrawer();
            void Draw(
                RenderTarget& rt, int32_t x, int32_t y, int32_t width, int32_t height, int32_t xStart, int32_t yStart,
                const uint8_t* weatherpattern) override;
            void Restore(RenderTarget& rt);
        };

#ifdef __WARN_SUGGEST_FINAL_TYPES__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsuggest-final-types"
#endif
        class X8DrawingEngine : public IDrawingEngine
        {
        protected:
            uint32_t _width = 0;
            uint32_t _height = 0;
            uint32_t _pitch = 0;
            size_t _bitsSize = 0;
            uint8_t* _bits = nullptr;

            RenderTarget _mainRT = {};

            bool _lastLightFXenabled = false;

            X8WeatherDrawer _weatherDrawer;
            X8DrawingContext* _drawingContext;
            InvalidationGrid _invalidationGrid;

        public:
            explicit X8DrawingEngine(Ui::IUiContext& uiContext);

#ifdef __WARN_SUGGEST_FINAL_METHODS__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsuggest-final-methods"
    #pragma GCC diagnostic ignored "-Wsuggest-final-types"
#endif
            ~X8DrawingEngine() override;
#ifdef __WARN_SUGGEST_FINAL_METHODS__
    #pragma GCC diagnostic pop
#endif

            void Initialise() override;
            void Resize(uint32_t width, uint32_t height) override;
            void SetPalette(const GamePalette& palette) override;
            void SetVSync(bool vsync) override;
            void Invalidate(int32_t left, int32_t top, int32_t right, int32_t bottom) override;
#ifdef __WARN_SUGGEST_FINAL_METHODS__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsuggest-final-methods"
    #pragma GCC diagnostic ignored "-Wsuggest-final-types"
#endif
            void BeginDraw() override;
            void EndDraw() override;
#ifdef __WARN_SUGGEST_FINAL_METHODS__
    #pragma GCC diagnostic pop
#endif
            void PaintWindows() override;
            void PaintWeather() override;
            void CopyRect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t dx, int32_t dy) override;
            std::string Screenshot() override;
            IDrawingContext* GetDrawingContext() override;
            RenderTarget* getRT() override;
            DrawingEngineFlags GetFlags() override;
            void InvalidateImage(uint32_t image) override;

        protected:
            void ConfigureBits(uint32_t width, uint32_t height, uint32_t pitch);
            virtual void OnDrawDirtyBlock(int32_t left, int32_t top, int32_t right, int32_t bottom);

        private:
            void ConfigureDirtyGrid();
            void DrawAllDirtyBlocks();
            void DrawDirtyBlocks(int32_t left, int32_t top, int32_t right, int32_t bottom);
        };
#ifdef __WARN_SUGGEST_FINAL_TYPES__
    #pragma GCC diagnostic pop
#endif

        class X8DrawingContext final : public IDrawingContext
        {
        private:
            X8DrawingEngine* _engine = nullptr;
            bool _isDrawing = false;

        public:
            explicit X8DrawingContext(X8DrawingEngine* engine);

            void BeginDraw();
            void EndDraw();
            void Clear(RenderTarget& rt, PaletteIndex paletteIndex) override;
            void FillRect(
                RenderTarget& rt, PaletteIndex paletteIndex, int32_t x, int32_t y, int32_t w, int32_t h,
                bool crossHatch) override;
            void FilterRect(
                RenderTarget& rt, FilterPaletteID palette, int32_t left, int32_t top, int32_t right, int32_t bottom) override;
            void DrawLine(RenderTarget& rt, PaletteIndex colour, const ScreenLine& line) override;
            void DrawSprite(RenderTarget& rt, ImageId imageId, int32_t x, int32_t y) override;
            void DrawSpriteRawMasked(RenderTarget& rt, int32_t x, int32_t y, ImageId maskImage, ImageId colourImage) override;
            void DrawSpriteSolid(RenderTarget& rt, ImageId image, int32_t x, int32_t y, PaletteIndex colour) override;
            void DrawGlyph(RenderTarget& rt, ImageId image, int32_t x, int32_t y, const PaletteMap& paletteMap) override;
            void DrawTTFBitmap(
                RenderTarget& rt, TextDrawInfo* info, TTFSurface* surface, int32_t x, int32_t y,
                uint8_t hintingThreshold) override;

            bool IsActive() const noexcept
            {
                return _isDrawing;
            }
        };
    } // namespace Drawing
} // namespace OpenRCT2
