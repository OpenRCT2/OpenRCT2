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

#pragma once

#include "../common.h"

#include <SDL_video.h>

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

#ifdef __cplusplus

struct rct_drawpixelinfo;
interface IDrawingContext;

interface IDrawingEngine
{
    virtual ~IDrawingEngine() { }

    virtual void Initialise(SDL_Window * window)     abstract;
    virtual void Resize(uint32 width, uint32 height) abstract;
    virtual void SetPalette(SDL_Color * colours)     abstract;

    virtual void SetUncappedFrameRate(bool uncapped) abstract;

    virtual void    Invalidate(sint32 left, sint32 top, sint32 right, sint32 bottom) abstract;
    virtual void    Draw() abstract;
    virtual void    CopyRect(sint32 x, sint32 y, sint32 width, sint32 height, sint32 dx, sint32 dy) abstract;
    virtual sint32  Screenshot() abstract;

    virtual IDrawingContext *   GetDrawingContext(rct_drawpixelinfo * dpi) abstract;
    virtual rct_drawpixelinfo * GetDrawingPixelInfo() abstract;

    virtual DRAWING_ENGINE_FLAGS GetFlags() abstract;

    virtual void InvalidateImage(uint32 image) abstract;
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

namespace OpenRCT2
{
    interface IRegistration;

    namespace Drawing
    {
        interface IDrawingEngineFactory
        {
            virtual ~IDrawingEngineFactory() { }
            virtual IDrawingEngine * Create() abstract;
        };

        namespace DrawingEngineFactory
        {
            IRegistration * Register(DRAWING_ENGINE type, IDrawingEngineFactory * factory);
        }
    }
}

#endif
