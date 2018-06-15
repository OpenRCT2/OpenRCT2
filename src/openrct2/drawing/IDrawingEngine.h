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
        virtual void Resize(uint32 width, uint32 height)            abstract;
        virtual void SetPalette(const rct_palette_entry * colours)  abstract;

        virtual void SetVSync(bool vsync) abstract;

        virtual void    Invalidate(sint32 left, sint32 top, sint32 right, sint32 bottom) abstract;
        virtual void    BeginDraw() abstract;
        virtual void    EndDraw() abstract;
        virtual void    PaintWindows() abstract;
        virtual void    PaintRain() abstract;
        virtual void    CopyRect(sint32 x, sint32 y, sint32 width, sint32 height, sint32 dx, sint32 dy) abstract;
        virtual sint32  Screenshot() abstract;

        virtual IDrawingContext *   GetDrawingContext(rct_drawpixelinfo * dpi) abstract;
        virtual rct_drawpixelinfo * GetDrawingPixelInfo() abstract;

        virtual DRAWING_ENGINE_FLAGS GetFlags() abstract;

        virtual void InvalidateImage(uint32 image) abstract;
    };

    interface IDrawingEngineFactory
    {
        virtual ~IDrawingEngineFactory() { }
        virtual std::unique_ptr<IDrawingEngine> Create(DRAWING_ENGINE_TYPE type, const std::shared_ptr<OpenRCT2::Ui::IUiContext>& uiContext) abstract;
    };

    interface IRainDrawer
    {
        virtual ~IRainDrawer() { }
        virtual void Draw(sint32 x,
            sint32 y,
            sint32 width,
            sint32 height,
            sint32 xStart,
            sint32 yStart) abstract;
    };
} // namespace OpenRCT2::Drawing
