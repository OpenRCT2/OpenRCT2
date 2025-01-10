/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "./Weather.h"
#include "ColourPalette.h"

#include <memory>
#include <string>

enum class DrawingEngine : int32_t
{
    None = -1,
    Software,
    SoftwareWithHardwareDisplay,
    OpenGL,
    Count,
};

enum DRAWING_ENGINE_FLAGS
{
    DEF_NONE = 0,

    /**
     * Whether or not the engine will only draw changed blocks of the screen each frame.
     */
    DEF_DIRTY_OPTIMISATIONS = 1 << 0,

    /**
     * The drawing engine is capable of processing the drawing in parallel.
     */
    DEF_PARALLEL_DRAWING = 1 << 1,
};

struct DrawPixelInfo;

namespace OpenRCT2::Ui
{
    struct IUiContext;
} // namespace OpenRCT2::Ui

namespace OpenRCT2::Drawing
{
    struct IDrawingContext;

    struct IDrawingEngine
    {
        virtual ~IDrawingEngine()
        {
        }

        virtual void Initialise() = 0;
        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual void SetPalette(const GamePalette& colours) = 0;

        virtual void SetVSync(bool vsync) = 0;

        virtual void Invalidate(int32_t left, int32_t top, int32_t right, int32_t bottom) = 0;
        virtual void BeginDraw() = 0;
        virtual void EndDraw() = 0;
        virtual void PaintWindows() = 0;
        virtual void PaintWeather() = 0;
        virtual void CopyRect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t dx, int32_t dy) = 0;
        virtual std::string Screenshot() = 0;

        virtual IDrawingContext* GetDrawingContext() = 0;
        virtual DrawPixelInfo* GetDrawingPixelInfo() = 0;

        virtual DRAWING_ENGINE_FLAGS GetFlags() = 0;

        virtual void InvalidateImage(uint32_t image) = 0;
    };

    struct IDrawingEngineFactory
    {
        virtual ~IDrawingEngineFactory()
        {
        }
        [[nodiscard]] virtual std::unique_ptr<IDrawingEngine>
            Create(DrawingEngine type, const std::shared_ptr<OpenRCT2::Ui::IUiContext>& uiContext) = 0;
    };

    struct IWeatherDrawer
    {
        virtual ~IWeatherDrawer() = default;
        virtual void Draw(
            DrawPixelInfo& dpi, int32_t x, int32_t y, int32_t width, int32_t height, int32_t xStart, int32_t yStart,
            const uint8_t* weatherpattern)
            = 0;
    };
} // namespace OpenRCT2::Drawing
