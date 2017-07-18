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

extern "C"
{
    #include "paint.h"
}

/**
 * A context for painting a single map tile.
 */
interface IPainter
{
    ///////////////////////////////////////////////////////////////////////////
    // General painting
    ///////////////////////////////////////////////////////////////////////////
    virtual paint_struct * Paint6C(uint8 direction, sprite_bb spriteBB) abstract;
    virtual paint_struct * Paint7C(uint8 direction, sprite_bb spriteBB) abstract;
    virtual paint_struct * Paint8C(uint8 direction, sprite_bb spriteBB) abstract;
    virtual paint_struct * Paint9C(uint8 direction, sprite_bb spriteBB) abstract;

    ///////////////////////////////////////////////////////////////////////////
    // Tunnels
    ///////////////////////////////////////////////////////////////////////////
    /**
     * Adds a new tunnel of the given type to the tile at the given Z.
     */
    virtual void PushTunnel(uint8 type, uint8 direction, sint32 height) abstract;

    /**
     * Sets the Z at which the vertical is drawn. There can only be one for a tile.
     */
    virtual void SetVerticalTunnel(sint32 height) abstract;

    ///////////////////////////////////////////////////////////////////////////
    // Supports
    ///////////////////////////////////////////////////////////////////////////
    /**
     * Paints supports of the given type at the given height.
     */
    virtual uint32 PaintSupports(uint32 type, uint32 special, uint32 spriteFlags, sint32 height) abstract;

    /**
     * Sets the base Z for small supports at the given segments.
     */
    virtual void SetSegmentSupportHeight(uint32 segments, sint32 height, uint8 slope) abstract;

    /**
     * Sets the base Z for large supports.
     */
    virtual void SetGeneralSupportHeight(sint32 height, uint8 slope) abstract;

    ///////////////////////////////////////////////////////////////////////////
    // Extensions
    ///////////////////////////////////////////////////////////////////////////
    void SetSegmentSupportHeight(uint32 segments);
};
