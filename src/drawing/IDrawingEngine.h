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

struct rct_drawpixelinfo;
interface IDrawingContext;

interface IDrawingEngine
{
    virtual ~IDrawingEngine() { }

    virtual void Initialise(SDL_Window * window)     abstract;
    virtual void Resize(uint32 width, uint32 height) abstract;
    virtual void SetPalette(SDL_Color * colours)     abstract;

    virtual void    Invalidate(sint32 left, sint32 top, sint32 right, sint32 bottom) abstract;
    virtual void    Draw() abstract;
    virtual sint32  Screenshot() abstract;

    virtual IDrawingContext * GetDrawingContext(rct_drawpixelinfo * dpi) abstract;
};

namespace DrawingEngineFactory
{
    IDrawingEngine * CreateSoftware();
    IDrawingEngine * CreateOpenGL();
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
