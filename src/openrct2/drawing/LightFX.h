/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _LIGHTFX_H
#define _LIGHTFX_H

#ifdef __ENABLE_LIGHTFX__

#    include "../common.h"

struct CoordsXY;
struct Vehicle;
struct rct_drawpixelinfo;
struct GamePalette;

enum class LightType : uint8_t
{
    None = 0,

    Lantern0 = 4,
    Lantern1 = 5,
    Lantern2 = 6,
    Lantern3 = 7,

    Spot0 = 8,
    Spot1 = 9,
    Spot2 = 10,
    Spot3 = 11,
};

enum LIGHTFX_LIGHT_QUALIFIER
{
    LIGHTFX_LIGHT_QUALIFIER_SPRITE = 0x1,
    LIGHTFX_LIGHT_QUALIFIER_MAP = 0x2
};

constexpr uint8_t GetLightTypeSize(LightType type)
{
    return static_cast<uint8_t>(type) & 0x3;
}
constexpr LightType SetLightTypeSize(LightType type, uint8_t size)
{
    return static_cast<LightType>(((static_cast<uint8_t>(type) & ~0x3) | size));
}

void lightfx_set_available(bool available);
bool lightfx_is_available();
bool lightfx_for_vehicles_is_available();

void lightfx_init();

void lightfx_update_buffers(rct_drawpixelinfo*);

void lightfx_prepare_light_list();
void lightfx_swap_buffers();
void lightfx_render_lights_to_frontbuffer();
void lightfx_update_viewport_settings();

void* lightfx_get_front_buffer();
const GamePalette& lightfx_get_palette();

void lightfx_add_3d_light(uint32_t lightID, uint16_t lightIDqualifier, int16_t x, int16_t y, uint16_t z, LightType lightType);

void lightfx_add_3d_light_magic_from_drawing_tile(
    const CoordsXY& mapPosition, int16_t offsetX, int16_t offsetY, int16_t offsetZ, LightType lightType);

void lightfx_add_lights_magic_vehicle(const Vehicle* vehicle);

uint32_t lightfx_get_light_polution();

void lightfx_apply_palette_filter(uint8_t i, uint8_t* r, uint8_t* g, uint8_t* b);
void lightfx_render_to_texture(
    void* dstPixels, uint32_t dstPitch, uint8_t* bits, uint32_t width, uint32_t height, const uint32_t* palette,
    const uint32_t* lightPalette);

#endif // __ENABLE_LIGHTFX__

#endif
