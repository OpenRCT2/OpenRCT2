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

#ifndef _LIGHTFX_H
#define _LIGHTFX_H

#ifdef __ENABLE_LIGHTFX__

#include <SDL.h>
#include "../common.h"
#include "drawing.h"

enum LIGHTFX_LIGHT_TYPE {
    LIGHTFX_LIGHT_TYPE_NONE             = 0,
    LIGHTFX_LIGHT_TYPE_RESERVED_01      = 1,

    LIGHTFX_LIGHT_TYPE_LANTERN_0        = 4,
    LIGHTFX_LIGHT_TYPE_LANTERN_1        = 5,
    LIGHTFX_LIGHT_TYPE_LANTERN_2        = 6,
    LIGHTFX_LIGHT_TYPE_LANTERN_3        = 7,

    LIGHTFX_LIGHT_TYPE_SPOT_0           = 8,
    LIGHTFX_LIGHT_TYPE_SPOT_1           = 9,
    LIGHTFX_LIGHT_TYPE_SPOT_2           = 10,
    LIGHTFX_LIGHT_TYPE_SPOT_3           = 11,

    LIGHTFX_LIGHT_TYPE_RESERVED_FF      = 0xFF
};

enum LIGHTFX_LIGHT_QUALIFIER {
    LIGHTFX_LIGHT_QUALIFIER_SPRITE      = 0x1,
    LIGHTFX_LIGHT_QUALIFIER_MAP         = 0x2
};

void lightfx_init();

void lightfx_update_buffers(rct_drawpixelinfo*);

void lightfx_prepare_light_list();
void lightfx_swap_buffers();
void lightfx_render_lights_to_frontbuffer();
void lightfx_update_viewport_settings();

void* lightfx_get_front_buffer();
const rct_palette * lightfx_get_palette();

void lightfx_add_3d_light(uint32 lightID, uint16 lightIDqualifier, sint16 x, sint16 y, uint16 z, uint8 lightType);

void lightfx_add_3d_light_magic_from_drawing_tile(sint16 offsetX, sint16 offsetY, sint16 offsetZ, uint8 lightType);

void lightfx_add_lights_magic_vehicles();

uint32 lightfx_get_light_polution();

void lightfx_apply_palette_filter(uint8 i, uint8 *r, uint8 *g, uint8 *b);
void lightfx_render_to_texture(
    struct SDL_Texture * texture,
    uint8 * bits,
    uint32 width,
    uint32 height,
    uint32 * palette,
    uint32 * lightPalette);

#endif // __ENABLE_LIGHTFX__

#endif
