/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3
 *****************************************************************************/

#ifndef _LIGHTFX_H
#define _LIGHTFX_H

#ifdef __ENABLE_LIGHTFX__

#include "../common.h"

struct LocationXY16;
struct rct_drawpixelinfo;
struct rct_palette;

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

void lightfx_set_available(bool available);
bool lightfx_is_available();

void lightfx_init();

void lightfx_update_buffers(rct_drawpixelinfo*);

void lightfx_prepare_light_list();
void lightfx_swap_buffers();
void lightfx_render_lights_to_frontbuffer();
void lightfx_update_viewport_settings();

void* lightfx_get_front_buffer();
const rct_palette * lightfx_get_palette();

void lightfx_add_3d_light(uint32 lightID, uint16 lightIDqualifier, sint16 x, sint16 y, uint16 z, uint8 lightType);

void lightfx_add_3d_light_magic_from_drawing_tile(LocationXY16 mapPosition, sint16 offsetX, sint16 offsetY, sint16 offsetZ, uint8 lightType);

void lightfx_add_lights_magic_vehicles();

uint32 lightfx_get_light_polution();

void lightfx_apply_palette_filter(uint8 i, uint8 *r, uint8 *g, uint8 *b);
void lightfx_render_to_texture(
    void * dstPixels,
    uint32 dstPitch,
    uint8 * bits,
    uint32 width,
    uint32 height,
    const uint32 * palette,
    const uint32 * lightPalette);

#endif // __ENABLE_LIGHTFX__

#endif
