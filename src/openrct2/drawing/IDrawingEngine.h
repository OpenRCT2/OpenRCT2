/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include "../common.h"

enum DRAWING_ENGINE
{
    DRAWING_ENGINE_NONE = -1,
    DRAWING_ENGINE_SOFTWARE,
    DRAWING_ENGINE_SOFTWARE_WITH_HARDWARE_DISPLAY,
    DRAWING_ENGINE_OPENGL,
    DRAWING_ENGINE_COUNT,
};

enum DRAWING_ENGINE_FLAGS
{
    DEF_NONE = 0,

    /**
     * Whether or not the engine will only draw changed blocks of the screen each frame.
     */
    DEF_DIRTY_OPTIMISATIONS = 1 << 0,
};

struct rct_drawpixelinfo;
struct rct_palette_entry;

namespace OpenRCT2::Ui
{
    interface IUiContext;
} // namespace OpenRCT2::Ui

namespace OpenRCT2::Drawing
{
    enum class DRAWING_ENGINE_TYPE;
    interface IDrawingContext;

    interface IDrawingEngine
    {
        virtual ~IDrawingEngine() { }

        virtual void Initialise()                                   abstract;
        virtual void Resize(uint32_t width, uint32_t height)            abstract;
        virtual void SetPalette(const rct_palette_entry * colours)  abstract;

        virtual void SetVSync(bool vsync) abstract;

        virtual void    Invalidate(int32_t left, int32_t top, int32_t right, int32_t bottom) abstract;
        virtual void    BeginDraw() abstract;
        virtual void    EndDraw() abstract;
        virtual void    PaintWindows() abstract;
        virtual void    PaintRain() abstract;
        virtual void    CopyRect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t dx, int32_t dy) abstract;
        virtual int32_t  Screenshot() abstract;

        virtual IDrawingContext *   GetDrawingContext(rct_drawpixelinfo * dpi) abstract;
        virtual rct_drawpixelinfo * GetDrawingPixelInfo() abstract;

        virtual DRAWING_ENGINE_FLAGS GetFlags() abstract;

        virtual void InvalidateImage(uint32_t image) abstract;
    };

    interface IDrawingEngineFactory
    {
        virtual ~IDrawingEngineFactory() { }
        virtual std::unique_ptr<IDrawingEngine> Create(DRAWING_ENGINE_TYPE type, const std::shared_ptr<OpenRCT2::Ui::IUiContext>& uiContext) abstract;
    };

    interface IRainDrawer
    {
        virtual ~IRainDrawer() { }
        virtual void Draw(int32_t x,
            int32_t y,
            int32_t width,
            int32_t height,
            int32_t xStart,
            int32_t yStart) abstract;
    };
} // namespace OpenRCT2::Drawing
