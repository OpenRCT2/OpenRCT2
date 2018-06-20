/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
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

void lightfx_add_3d_light(uint32_t lightID, uint16_t lightIDqualifier, int16_t x, int16_t y, uint16_t z, uint8_t lightType);

void lightfx_add_3d_light_magic_from_drawing_tile(LocationXY16 mapPosition, int16_t offsetX, int16_t offsetY, int16_t offsetZ, uint8_t lightType);

void lightfx_add_lights_magic_vehicles();

uint32_t lightfx_get_light_polution();

void lightfx_apply_palette_filter(uint8_t i, uint8_t *r, uint8_t *g, uint8_t *b);
void lightfx_render_to_texture(
    void * dstPixels,
    uint32_t dstPitch,
    uint8_t * bits,
    uint32_t width,
    uint32_t height,
    const uint32_t * palette,
    const uint32_t * lightPalette);

#endif // __ENABLE_LIGHTFX__

#endif
