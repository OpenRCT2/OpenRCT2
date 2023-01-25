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
#include "./Weather.h"

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
struct GamePalette;

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

        virtual void Initialise() abstract;
        virtual void Resize(uint32_t width, uint32_t height) abstract;
        virtual void SetPalette(const GamePalette& colours) abstract;

        virtual void SetVSync(bool vsync) abstract;

        virtual void Invalidate(int32_t left, int32_t top, int32_t right, int32_t bottom) abstract;
        virtual void BeginDraw() abstract;
        virtual void EndDraw() abstract;
        virtual void PaintWindows() abstract;
        virtual void PaintWeather() abstract;
        virtual void CopyRect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t dx, int32_t dy) abstract;
        virtual std::string Screenshot() abstract;

        virtual IDrawingContext* GetDrawingContext() abstract;
        virtual DrawPixelInfo* GetDrawingPixelInfo() abstract;

        virtual DRAWING_ENGINE_FLAGS GetFlags() abstract;

        virtual void InvalidateImage(uint32_t image) abstract;
    };

    struct IDrawingEngineFactory
    {
        virtual ~IDrawingEngineFactory()
        {
        }
        [[nodiscard]] virtual std::unique_ptr<IDrawingEngine> Create(
            DrawingEngine type, const std::shared_ptr<OpenRCT2::Ui::IUiContext>& uiContext) abstract;
    };

    struct IWeatherDrawer
    {
        virtual ~IWeatherDrawer() = default;
        virtual void Draw(
            DrawPixelInfo* dpi, int32_t x, int32_t y, int32_t width, int32_t height, int32_t xStart, int32_t yStart,
            const uint8_t* weatherpattern) abstract;
    };
} // namespace OpenRCT2::Drawing
