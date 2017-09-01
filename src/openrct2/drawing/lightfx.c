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

#ifdef __ENABLE_LIGHTFX__

#include "../common.h"
#include "../game.h"
#include "../interface/viewport.h"
#include "../interface/window.h"
#include "../paint/map_element/map_element.h"
#include "../world/Climate.h"
#include "drawing.h"
#include "lightfx.h"

static uint8 _bakedLightTexture_lantern_0[32*32];
static uint8 _bakedLightTexture_lantern_1[64*64];
static uint8 _bakedLightTexture_lantern_2[128*128];
static uint8 _bakedLightTexture_lantern_3[256*256];
static uint8 _bakedLightTexture_spot_0[32 * 32];
static uint8 _bakedLightTexture_spot_1[64 * 64];
static uint8 _bakedLightTexture_spot_2[128 * 128];
static uint8 _bakedLightTexture_spot_3[256 * 256];
static rct_drawpixelinfo    _pixelInfo;

static void* _light_rendered_buffer_back = NULL;
static void* _light_rendered_buffer_front = NULL;

static uint32   _lightPolution_back = 0;
static uint32   _lightPolution_front = 0;

typedef struct lightlist_entry {
    sint16  x, y, z;
    uint8   lightType;
    uint8   lightIntensity;
    uint32  lightID;
    uint16  lightIDqualifier;
    uint8   lightLinger;
    uint8   pad[1];
} lightlist_entry;

static lightlist_entry  _LightListA[16000];
static lightlist_entry  _LightListB[16000];

static lightlist_entry  *_LightListBack;
static lightlist_entry  *_LightListFront;

static uint32           LightListCurrentCountBack;
static uint32           LightListCurrentCountFront;

static sint16           _current_view_x_front           = 0;
static sint16           _current_view_y_front           = 0;
static uint8            _current_view_rotation_front    = 0;
static uint8            _current_view_zoom_front        = 0;
static sint16           _current_view_x_back            = 0;
static sint16           _current_view_y_back            = 0;
static uint8            _current_view_rotation_back     = 0;
static uint8            _current_view_zoom_back         = 0;
static uint8            _current_view_zoom_back_delay   = 0;

static rct_palette gPalette_light;

static uint8 soft_light(uint8 a, uint8 b);
static uint8 lerp(uint8 a, uint8 b, float t);
static float flerp(float a, float b, float t);

static uint8 calc_light_intensity_lantern(sint32 x, sint32 y) {
    double distance = (double)(x * x + y * y);

    double light = 0.03 + pow(10.0 / (1.0 + distance / 100.0), 0.55);
    light *= min(1.0, max(0.0f, 2.0 - sqrt(distance) / 64));
    light *= 0.1f;

    return (uint8)(min(255.0, light * 255.0));
}

static uint8 calc_light_intensity_spot(sint32 x, sint32 y) {
    double distance = (double)(x * x + y * y);

    double light = 0.3 + pow(10.0 / (1.0 + distance / 100.0), 0.75);
    light *= min(1.0, max(0.0f, 2.0 - sqrt(distance) / 64));
    light *= 0.5f;

    return (uint8)(min(255.0, light * 255.0)) >> 4;
}

static void calc_rescale_light_half( uint8 *target, uint8 *source,uint32 targetWidth, uint32 targetHeight) {
    uint8 *parcerRead = source;
    uint8 *parcerWrite = target;

    for (uint32 y = 0; y < targetHeight; y++) {
        for (uint32 x = 0; x < targetWidth; x++) {
            *parcerWrite = (*parcerRead);
            parcerWrite++;
            parcerRead += 2;
        }
        parcerRead += targetWidth * 2;
    }
}

void lightfx_init()
{
    _LightListBack      = _LightListA;
    _LightListFront     = _LightListB;

    memset(_bakedLightTexture_lantern_0, 0xFF, 32 * 32);
    memset(_bakedLightTexture_lantern_1, 0xFF, 64 * 64);
    memset(_bakedLightTexture_lantern_2, 0xFF, 128 * 128);
    memset(_bakedLightTexture_lantern_3, 0xFF, 256 * 256);

    uint8 *parcer = _bakedLightTexture_lantern_3;

    for (sint32 y = 0; y < 256; y++) {
        for (sint32 x = 0; x < 256; x++) {
            *parcer = calc_light_intensity_lantern(x - 128, y - 128);
            parcer++;
        }
    }

    parcer = _bakedLightTexture_spot_3;

    for (sint32 y = 0; y < 256; y++) {
        for (sint32 x = 0; x < 256; x++) {
            *parcer = calc_light_intensity_spot(x - 128, y - 128);
            parcer++;
        }
    }

    calc_rescale_light_half(_bakedLightTexture_lantern_2, _bakedLightTexture_lantern_3, 128, 128);
    calc_rescale_light_half(_bakedLightTexture_lantern_1, _bakedLightTexture_lantern_2, 64, 64);
    calc_rescale_light_half(_bakedLightTexture_lantern_0, _bakedLightTexture_lantern_1, 32, 32);

    calc_rescale_light_half(_bakedLightTexture_spot_2, _bakedLightTexture_spot_3, 128, 128);
    calc_rescale_light_half(_bakedLightTexture_spot_1, _bakedLightTexture_spot_2, 64, 64);
    calc_rescale_light_half(_bakedLightTexture_spot_0, _bakedLightTexture_spot_1, 32, 32);
}

void lightfx_update_buffers(rct_drawpixelinfo *info)
{
    _light_rendered_buffer_front    = realloc(_light_rendered_buffer_front, info->width * info->height);
    _light_rendered_buffer_back     = realloc(_light_rendered_buffer_back,  info->width * info->height);

    memcpy(&_pixelInfo, info, sizeof(rct_drawpixelinfo));
}

extern void sub_68862C();
extern void viewport_paint_setup();

void lightfx_prepare_light_list()
{
    for (uint32 light = 0; light < LightListCurrentCountFront; light++) {
        lightlist_entry *entry = &_LightListFront[light];

        if (entry->z == 0x7FFF) {
            entry->lightIntensity = 0xFF;
            continue;
        }

        rct_xyz16 coord_3d = {
            .x = entry->x,
            .y = entry->y,
            .z = entry->z
        };

        rct_xy16 coord_2d = coordinate_3d_to_2d(&coord_3d, _current_view_rotation_front);

        entry->x = coord_2d.x;// - (_current_view_x_front);
        entry->y = coord_2d.y;// - (_current_view_y_front);

        sint32 posOnScreenX = entry->x - _current_view_x_front;
        sint32 posOnScreenY = entry->y - _current_view_y_front;

        posOnScreenX >>= _current_view_zoom_front;
        posOnScreenY >>= _current_view_zoom_front;

        if ((posOnScreenX < -128) ||
            (posOnScreenY < -128) ||
            (posOnScreenX > _pixelInfo.width + 128) ||
            (posOnScreenY > _pixelInfo.height + 128)) {
            entry->lightType = LIGHTFX_LIGHT_TYPE_NONE;
            continue;
        }


    //  entry->x >>= _current_view_zoom_front;
    //  entry->y >>= _current_view_zoom_front;

        uint32 lightIntensityOccluded = 0x0;

        sint32 dirVecX = 707;
        sint32 dirVecY = 707;

        switch (_current_view_rotation_front) {
        case 0:
            dirVecX = 707;
            dirVecY = 707;
            break;
        case 1:
            dirVecX = -707;
            dirVecY = 707;
            break;
        case 2:
            dirVecX = -707;
            dirVecY = -707;
            break;
        case 3:
            dirVecX = 707;
            dirVecY = -707;
            break;
        default:
            dirVecX = 0;
            dirVecY = 0;
            break;
        }

#ifdef LIGHTFX_UNKNOWN_PART_1
        sint32 tileOffsetX = 0;
        sint32 tileOffsetY = 0;
        switch (_current_view_rotation_front) {
        case 0:
            tileOffsetX = 0;
            tileOffsetY = 0;
            break;
        case 1:
            tileOffsetX = 16;
            tileOffsetY = 0;
            break;
        case 2:
            tileOffsetX = 32;
            tileOffsetY = 32;
            break;
        case 3:
            tileOffsetX = 0;
            tileOffsetY = 16;
            break;
        }

        sint32 mapFrontDiv = 1 << _current_view_zoom_front;
        static sint16 offsetPattern[26] = {     0, 0,       -4, 0,      0, -3,      4, 0,       0, 3,
                                                            -2, -1,     -1, -1,     2, 1,       1, 1,
                                                            -3, -2,     -3, 2,      3, -2,      3, 2    };
#endif //LIGHTFX_UNKNOWN_PART_1

        if (true) {
            sint32 totalSamplePoints = 5;
            sint32 startSamplePoint = 1;
            // sint32 lastSampleCount = 0;

            if ((entry->lightIDqualifier & 0xF) == LIGHTFX_LIGHT_QUALIFIER_MAP) {
                startSamplePoint = 0;
                totalSamplePoints = 1;
            }

            for (sint32 pat = startSamplePoint; pat < totalSamplePoints; pat++) {
                rct_xy16 mapCoord = { 0 };

                rct_map_element *mapElement = 0;

                sint32 interactionType = 0;

                rct_window *w = window_get_main();
                if (w != NULL) {
                //  get_map_coordinates_from_pos(entry->x + offsetPattern[pat*2] / mapFrontDiv, entry->y + offsetPattern[pat*2+1] / mapFrontDiv, VIEWPORT_INTERACTION_MASK_NONE, &mapCoord.x, &mapCoord.y, &interactionType, &mapElement, NULL);

#ifdef LIGHTFX_UNKNOWN_PART_1
                    _unk9AC154 = ~VIEWPORT_INTERACTION_MASK_SPRITE & 0xFFFF;
                    _viewportDpi1.zoom = _current_view_zoom_front;
                    _viewportDpi1.x = entry->x + offsetPattern[0 + pat * 2] / mapFrontDiv;
                    _viewportDpi1.y = entry->y + offsetPattern[1 + pat * 2] / mapFrontDiv;
                    rct_drawpixelinfo* dpi = &_viewportDpi2;
                    dpi->x = _viewportDpi1.x;
                    dpi->y = _viewportDpi1.y;
                    dpi->zoom_level = _viewportDpi1.zoom;
                    dpi->height = 1;
                    dpi->width = 1;
                    gPaintSession.EndOfPaintStructArray = 0xF1A4CC;
                    gPaintSession.Unk140E9A8 = dpi;
                    painter_setup();
                    viewport_paint_setup();
                    paint_arrange_structs();
                    sub_68862C();

                //  log_warning("[%i, %i]", dpi->x, dpi->y);

                    mapCoord.x = _interactionMapX + tileOffsetX;
                    mapCoord.y = _interactionMapY + tileOffsetY;
                    interactionType = _interactionSpriteType;
                    mapElement = RCT2_GLOBAL(0x9AC150, rct_map_element*);
#endif //LIGHTFX_UNKNOWN_PART_1

                    //RCT2_GLOBAL(0x9AC154, uint16_t) = VIEWPORT_INTERACTION_MASK_NONE;
                    //RCT2_GLOBAL(0x9AC148, uint8_t) = 0;
                    //RCT2_GLOBAL(0x9AC138 + 4, int16_t) = screenX;
                    //RCT2_GLOBAL(0x9AC138 + 6, int16_t) = screenY;
                    //if (screenX >= 0 && screenX < (sint32)myviewport->width && screenY >= 0 && screenY < (sint32)myviewport->height)
                    //{
                    //  screenX <<= myviewport->zoom;
                    //  screenY <<= myviewport->zoom;
                    //  screenX += (sint32)myviewport->view_x;
                    //  screenY += (sint32)myviewport->view_y;
                    //  RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_ZOOM, uint16_t) = myviewport->zoom;
                    //  screenX &= (0xFFFF << myviewport->zoom) & 0xFFFF;
                    //  screenY &= (0xFFFF << myviewport->zoom) & 0xFFFF;
                    //  RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_X, int16_t) = screenX;
                    //  RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_Y, int16_t) = screenY;
                    //  rct_drawpixelinfo* dpi = RCT2_ADDRESS(RCT2_ADDRESS_VIEWPORT_DPI, rct_drawpixelinfo);
                    //  dpi->y = RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_Y, int16_t);
                    //  dpi->height = 1;
                    //  dpi->zoom_level = RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_ZOOM, uint16_t);
                    //  dpi->x = RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_X, int16_t);
                    //  dpi->width = 1;
                    //  RCT2_GLOBAL(0xEE7880, uint32_t) = 0xF1A4CC;
                    //  RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*) = dpi;
                    //  painter_setup();
                    //  viewport_paint_setup();
                    //  paint_arrange_structs();
                    //  sub_68862C();
                    //}
                }

                sint32 minDist = 0;
                sint32 baseHeight = -999;

                if (interactionType != VIEWPORT_INTERACTION_ITEM_SPRITE && mapElement) {
                    baseHeight = mapElement->base_height;
                }

                minDist = ((baseHeight * 8) - coord_3d.z) / 2;

                sint32 deltaX = mapCoord.x - coord_3d.x;
                sint32 deltaY = mapCoord.y - coord_3d.y;

                sint32 projDot = (dirVecX * deltaX + dirVecY * deltaY) / 1000;

                projDot = max(minDist, projDot);

                if (projDot < 5) {
                    lightIntensityOccluded  += 100;
                }
                else {
                    lightIntensityOccluded  += max(0, 200 - (projDot * 20));
                }

            //  log_warning("light %i [%i, %i, %i], [%i, %i] minDist to %i: %i; projdot: %i", light, coord_3d.x, coord_3d.y, coord_3d.z, mapCoord.x, mapCoord.y, baseHeight, minDist, projDot);


                if (pat == 0) {
                    if (lightIntensityOccluded == 100)
                        break;
                    if (_current_view_zoom_front > 2)
                        break;
                    totalSamplePoints += 4;
                }
                else if (pat == 4) {
                    if (_current_view_zoom_front > 1)
                        break;
                    if (lightIntensityOccluded == 0 || lightIntensityOccluded == 500)
                        break;
                    // lastSampleCount = lightIntensityOccluded / 500;
                //  break;
                    totalSamplePoints += 4;
                }
                else if (pat == 8) {
                    break;
                    // if (_current_view_zoom_front > 0)
                    //  break;
                    // sint32 newSampleCount = lightIntensityOccluded / 900;
                    // if (abs(newSampleCount - lastSampleCount) < 10)
                    //  break;
                    // totalSamplePoints += 4;
                }
            }

            totalSamplePoints -= startSamplePoint;

        //  lightIntensityOccluded = totalSamplePoints * 100;

        //  log_warning("sample-count: %i, occlusion: %i", totalSamplePoints, lightIntensityOccluded);

            if (lightIntensityOccluded == 0) {
                entry->lightType = LIGHTFX_LIGHT_TYPE_NONE;
                continue;
            }

        //  log_warning("sample-count: %i, occlusion: %i", totalSamplePoints, lightIntensityOccluded / totalSamplePoints);

            entry->lightIntensity = min(0xFF, (entry->lightIntensity * lightIntensityOccluded) / (totalSamplePoints * 100));
            entry->lightIntensity = max(0x00, entry->lightIntensity - _current_view_zoom_front * 5);
        }

        if (_current_view_zoom_front > 0) {
            if ((entry->lightType & 0x3) < _current_view_zoom_front) {
                entry->lightType = LIGHTFX_LIGHT_TYPE_NONE;
                continue;
            }

            entry->lightType -= _current_view_zoom_front;
        }
    }
}

void lightfx_swap_buffers()
{
    void *tmp = _light_rendered_buffer_back;
    _light_rendered_buffer_back = _light_rendered_buffer_front;
    _light_rendered_buffer_front = tmp;

    tmp = _light_rendered_buffer_back;
    _light_rendered_buffer_back = _light_rendered_buffer_front;
    _light_rendered_buffer_front = tmp;

    tmp = _LightListBack;
    _LightListBack = _LightListFront;
    _LightListFront = tmp;

    LightListCurrentCountFront = LightListCurrentCountBack;
    LightListCurrentCountBack = 0x0;

    uint32 uTmp = _lightPolution_back;
    _lightPolution_back = _lightPolution_front;
    _lightPolution_front = uTmp;

    _current_view_x_front = _current_view_x_back;
    _current_view_y_front = _current_view_y_back;
    _current_view_rotation_front = _current_view_rotation_back;
    _current_view_zoom_front = _current_view_zoom_back_delay;
    _current_view_zoom_back_delay = _current_view_zoom_back;
}

void lightfx_update_viewport_settings()
{
    rct_window *mainWindow = window_get_main();
    if (mainWindow) {
        _current_view_x_back = mainWindow->viewport->view_x;
        _current_view_y_back = mainWindow->viewport->view_y;
        _current_view_rotation_back = get_current_rotation();
        _current_view_zoom_back = mainWindow->viewport->zoom;
    }
}

void lightfx_render_lights_to_frontbuffer()
{
    if (_light_rendered_buffer_front == NULL) {
        return;
    }

    memset(_light_rendered_buffer_front, 0, _pixelInfo.width * _pixelInfo.height);

    _lightPolution_back = 0;

//  log_warning("%i lights", LightListCurrentCountFront);

    for (uint32 light = 0; light < LightListCurrentCountFront; light++) {
        const uint8 *bufReadBase    = 0;
        uint8       *bufWriteBase   = _light_rendered_buffer_front;
        uint32      bufReadWidth, bufReadHeight;
        sint32      bufWriteX, bufWriteY;
        sint32      bufWriteWidth, bufWriteHeight;
        uint32      bufReadSkip, bufWriteSkip;

        lightlist_entry * entry = &_LightListFront[light];

        sint32      inRectCentreX   = entry->x;
        sint32      inRectCentreY   = entry->y;

        if (entry->z != 0x7FFF) {
            inRectCentreX -= _current_view_x_front;
            inRectCentreY -= _current_view_y_front;
            inRectCentreX >>= _current_view_zoom_front;
            inRectCentreY >>= _current_view_zoom_front;
        }

        switch (entry->lightType) {
            case LIGHTFX_LIGHT_TYPE_LANTERN_0:
                bufReadWidth    = 32;
                bufReadHeight   = 32;
                bufReadBase     = _bakedLightTexture_lantern_0;
                break;
            case LIGHTFX_LIGHT_TYPE_LANTERN_1:
                bufReadWidth    = 64;
                bufReadHeight   = 64;
                bufReadBase     = _bakedLightTexture_lantern_1;
                break;
            case LIGHTFX_LIGHT_TYPE_LANTERN_2:
                bufReadWidth    = 128;
                bufReadHeight   = 128;
                bufReadBase     = _bakedLightTexture_lantern_2;
                break;
            case LIGHTFX_LIGHT_TYPE_LANTERN_3:
                bufReadWidth    = 256;
                bufReadHeight   = 256;
                bufReadBase     = _bakedLightTexture_lantern_3;
                break;
            case LIGHTFX_LIGHT_TYPE_SPOT_0:
                bufReadWidth = 32;
                bufReadHeight = 32;
                bufReadBase = _bakedLightTexture_spot_0;
                break;
            case LIGHTFX_LIGHT_TYPE_SPOT_1:
                bufReadWidth = 64;
                bufReadHeight = 64;
                bufReadBase = _bakedLightTexture_spot_1;
                break;
            case LIGHTFX_LIGHT_TYPE_SPOT_2:
                bufReadWidth = 128;
                bufReadHeight = 128;
                bufReadBase = _bakedLightTexture_spot_2;
                break;
            case LIGHTFX_LIGHT_TYPE_SPOT_3:
                bufReadWidth = 256;
                bufReadHeight = 256;
                bufReadBase = _bakedLightTexture_spot_3;
                break;
            default:
                continue;
        }

        bufWriteX   = inRectCentreX - bufReadWidth / 2;
        bufWriteY   = inRectCentreY - bufReadHeight / 2;

        bufWriteWidth   = bufReadWidth;
        bufWriteHeight  = bufReadHeight;

        if (bufWriteX < 0) {
            bufReadBase     += -bufWriteX;
            bufWriteWidth   += bufWriteX;
        }
        else {
            bufWriteBase    += bufWriteX;
        }

        if (bufWriteWidth <= 0)
            continue;

        if (bufWriteY < 0) {
            bufReadBase     += -bufWriteY * bufReadWidth;
            bufWriteHeight  += bufWriteY;
        }
        else {
            bufWriteBase    += bufWriteY * _pixelInfo.width;
        }


        if (bufWriteHeight <= 0)
            continue;

        sint32  rightEdge = bufWriteX + bufWriteWidth;
        sint32  bottomEdge = bufWriteY + bufWriteHeight;

        if (rightEdge > _pixelInfo.width) {
            bufWriteWidth -= rightEdge - _pixelInfo.width;
        }
        if (bottomEdge > _pixelInfo.height) {
            bufWriteHeight -= bottomEdge - _pixelInfo.height;
        }

        if (bufWriteWidth <= 0)
            continue;
        if (bufWriteHeight <= 0)
            continue;

        _lightPolution_back += (bufWriteWidth * bufWriteHeight) / 256;

        bufReadSkip     = bufReadWidth - bufWriteWidth;
        bufWriteSkip    = _pixelInfo.width - bufWriteWidth;

        if (entry->lightIntensity == 0xFF) {
            for (sint32 y = 0; y < bufWriteHeight; y++) {
                for (sint32 x = 0; x < bufWriteWidth; x++) {
                    *bufWriteBase = min(0xFF, *bufWriteBase + *bufReadBase);
                    bufWriteBase++;
                    bufReadBase++;
                }

                bufWriteBase    += bufWriteSkip;
                bufReadBase     += bufReadSkip;
            }
        }
        else {
            for (sint32 y = 0; y < bufWriteHeight; y++) {
                for (sint32 x = 0; x < bufWriteWidth; x++) {
                    *bufWriteBase = min(0xFF, *bufWriteBase + (((*bufReadBase) * (1 + entry->lightIntensity)) >> 8));
                    bufWriteBase++;
                    bufReadBase++;
                }

                bufWriteBase += bufWriteSkip;
                bufReadBase += bufReadSkip;
            }
        }
    }
}

void* lightfx_get_front_buffer()
{
    return _light_rendered_buffer_front;
}

const rct_palette * lightfx_get_palette()
{
    return &gPalette_light;
}

void lightfx_add_3d_light(uint32 lightID, uint16 lightIDqualifier, sint16 x, sint16 y, uint16 z, uint8 lightType)
{
    if (LightListCurrentCountBack == 15999) {
        return;
    }

//  log_warning("%i lights in back", LightListCurrentCountBack);

    for (uint32 i = 0; i < LightListCurrentCountBack; i++) {
        lightlist_entry *entry = &_LightListBack[i];
        if (entry->lightID != lightID)
            continue;
        if (entry->lightIDqualifier != lightIDqualifier)
            continue;

        entry->x = x;
        entry->y = y;
        entry->z = z;
        entry->lightType = lightType;
        entry->lightIntensity = 0xFF;
        entry->lightID = lightID;
        entry->lightIDqualifier = lightIDqualifier;
        entry->lightLinger = 1;

        return;
    }

    lightlist_entry *entry = &_LightListBack[LightListCurrentCountBack++];

    entry->x                = x;
    entry->y                = y;
    entry->z                = z;
    entry->lightType        = lightType;
    entry->lightIntensity   = 0xFF;
    entry->lightID          = lightID;
    entry->lightIDqualifier = lightIDqualifier;
    entry->lightLinger      = 1;

//  log_warning("new 3d light");
}

void lightfx_add_3d_light_magic_from_drawing_tile(sint16 offsetX, sint16 offsetY, sint16 offsetZ, uint8 lightType)
{
    sint16 x = gPaintMapPosition.x + offsetX;
    sint16 y = gPaintMapPosition.y + offsetY;

    switch (get_current_rotation()) {
    case 0:
        x += 16;
        y += 16;
        break;
    case 1:
        x += 16;
        y += 16;
        break;
    case 2:
        x += 16;
        y -= 16;
        break;
    case 3:
        x += 16;
        y -= 16;
        break;
    default:
        return;
    }

    lightfx_add_3d_light((x << 16) | y, (offsetZ << 8) | LIGHTFX_LIGHT_QUALIFIER_MAP, x, y, offsetZ, lightType);
}

uint32 lightfx_get_light_polution()
{
    return _lightPolution_front;
}

void lightfx_add_lights_magic_vehicles()
{
    uint16 spriteIndex = gSpriteListHead[SPRITE_LIST_TRAIN];
    while (spriteIndex != SPRITE_INDEX_NULL) {
        rct_vehicle * vehicle = &(get_sprite(spriteIndex)->vehicle);
        uint16 vehicleID = spriteIndex;
        spriteIndex = vehicle->next;

        rct_vehicle *mother_vehicle = vehicle;

        if (mother_vehicle->ride_subtype == RIDE_ENTRY_INDEX_NULL) {
            continue;
        }

        for (uint16 q = vehicleID; q != SPRITE_INDEX_NULL; ) {
            vehicle = GET_VEHICLE(q);

            vehicleID = q;
            if (vehicle->next_vehicle_on_train == q)
                break;
            q = vehicle->next_vehicle_on_train;

            sint16 place_x, place_y, place_z;

            place_x = vehicle->x;
            place_y = vehicle->y;
            place_z = vehicle->z;

            static const sint16 offsetLookup[32] = { 10, 10, 9, 8, 7, 6, 4, 2, 0, -2, -4, -6, -7, -8, -9, -10, -10, -10, -9, -8, -7, -6, -4, -2, 0, 2, 4, 6, 7, 8, 9, 10 };

            rct_ride *ride = get_ride(vehicle->ride);
            switch (ride->type) {
            case RIDE_TYPE_OBSERVATION_TOWER:
                lightfx_add_3d_light(vehicleID, 0x0000 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, vehicle->x, vehicle->y + 16, vehicle->z, LIGHTFX_LIGHT_TYPE_SPOT_3);
                lightfx_add_3d_light(vehicleID, 0x0100 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, vehicle->x + 16, vehicle->y, vehicle->z, LIGHTFX_LIGHT_TYPE_SPOT_3);
                lightfx_add_3d_light(vehicleID, 0x0200 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, vehicle->x - 16, vehicle->y, vehicle->z, LIGHTFX_LIGHT_TYPE_SPOT_3);
                lightfx_add_3d_light(vehicleID, 0x0300 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, vehicle->x, vehicle->y - 16, vehicle->z, LIGHTFX_LIGHT_TYPE_SPOT_3);
                break;
            case RIDE_TYPE_MINE_TRAIN_COASTER:
            case RIDE_TYPE_GHOST_TRAIN:
                if (vehicle == vehicle_get_head(vehicle)) {
                    place_x -= offsetLookup[(vehicle->sprite_direction + 0) % 32] * 2;
                    place_y -= offsetLookup[(vehicle->sprite_direction + 8) % 32] * 2;
                    lightfx_add_3d_light(vehicleID, 0x0000 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, place_x, place_y, place_z, LIGHTFX_LIGHT_TYPE_SPOT_3);
                }
                break;
            case RIDE_TYPE_CHAIRLIFT:
                lightfx_add_3d_light(vehicleID, 0x0000 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, place_x, place_y, place_z - 16, LIGHTFX_LIGHT_TYPE_LANTERN_2);
                break;
            case RIDE_TYPE_BOAT_RIDE:
            case RIDE_TYPE_CAR_RIDE:
            case RIDE_TYPE_GO_KARTS:
            case RIDE_TYPE_DODGEMS:
            case RIDE_TYPE_MINI_HELICOPTERS:
            case RIDE_TYPE_MONORAIL_CYCLES:
            case RIDE_TYPE_SUBMARINE_RIDE:
            case RIDE_TYPE_SPLASH_BOATS:
            case RIDE_TYPE_WATER_COASTER: {
                rct_vehicle *vehicle_draw = vehicle_get_head(vehicle);
                if (vehicle_draw->next_vehicle_on_train != SPRITE_INDEX_NULL) {
                    vehicle_draw = GET_VEHICLE(vehicle_draw->next_vehicle_on_train);
                }
                place_x = vehicle_draw->x;
                place_y = vehicle_draw->y;
                place_z = vehicle_draw->z;
                place_x -= offsetLookup[(vehicle_draw->sprite_direction + 0) % 32];
                place_y -= offsetLookup[(vehicle_draw->sprite_direction + 8) % 32];
                lightfx_add_3d_light(vehicleID, 0x0000 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, place_x, place_y, place_z, LIGHTFX_LIGHT_TYPE_SPOT_2);
                place_x -= offsetLookup[(vehicle_draw->sprite_direction + 0) % 32];
                place_y -= offsetLookup[(vehicle_draw->sprite_direction + 8) % 32];
                lightfx_add_3d_light(vehicleID, 0x0100 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, place_x, place_y, place_z, LIGHTFX_LIGHT_TYPE_SPOT_2);
                break;
            }
            case RIDE_TYPE_MONORAIL:
                lightfx_add_3d_light(vehicleID, 0x0000 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, vehicle->x, vehicle->y, vehicle->z + 12, LIGHTFX_LIGHT_TYPE_SPOT_2);
                if (vehicle == vehicle_get_head(vehicle)) {
                    place_x -= offsetLookup[(vehicle->sprite_direction + 0) % 32] * 2;
                    place_y -= offsetLookup[(vehicle->sprite_direction + 8) % 32] * 2;
                    lightfx_add_3d_light(vehicleID, 0x0100 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, place_x, place_y, place_z + 10, LIGHTFX_LIGHT_TYPE_LANTERN_3);
                    place_x -= offsetLookup[(vehicle->sprite_direction + 0) % 32] * 3;
                    place_y -= offsetLookup[(vehicle->sprite_direction + 8) % 32] * 3;
                    lightfx_add_3d_light(vehicleID, 0x0200 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, place_x, place_y, place_z + 2, LIGHTFX_LIGHT_TYPE_LANTERN_3);
                }
                if (vehicle == vehicle_get_tail(vehicle)) {
                    place_x += offsetLookup[(vehicle->sprite_direction + 0) % 32] * 2;
                    place_y += offsetLookup[(vehicle->sprite_direction + 8) % 32] * 2;
                    lightfx_add_3d_light(vehicleID, 0x0300 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, place_x, place_y, place_z + 10, LIGHTFX_LIGHT_TYPE_LANTERN_3);
                    place_x += offsetLookup[(vehicle->sprite_direction + 0) % 32] * 2;
                    place_y += offsetLookup[(vehicle->sprite_direction + 8) % 32] * 2;
                    lightfx_add_3d_light(vehicleID, 0x0400 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, place_x, place_y, place_z + 2, LIGHTFX_LIGHT_TYPE_LANTERN_3);
                }
                break;
            case RIDE_TYPE_MINIATURE_RAILWAY:
                if (vehicle == vehicle_get_head(vehicle)) {
                    place_x -= offsetLookup[(vehicle->sprite_direction + 0) % 32] * 2;
                    place_y -= offsetLookup[(vehicle->sprite_direction + 8) % 32] * 2;
                    lightfx_add_3d_light(vehicleID, 0x0100 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, place_x, place_y, place_z + 10, LIGHTFX_LIGHT_TYPE_LANTERN_3);
                    place_x -= offsetLookup[(vehicle->sprite_direction + 0) % 32] * 2;
                    place_y -= offsetLookup[(vehicle->sprite_direction + 8) % 32] * 2;
                    lightfx_add_3d_light(vehicleID, 0x0200 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, place_x, place_y, place_z + 2, LIGHTFX_LIGHT_TYPE_LANTERN_3);

                }
                else {
                    lightfx_add_3d_light(vehicleID, 0x0000 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, place_x, place_y, place_z + 10, LIGHTFX_LIGHT_TYPE_LANTERN_3);
                }
                break;
            default:
                break;
            };
        }
    }
}

void lightfx_apply_palette_filter(uint8 i, uint8 *r, uint8 *g, uint8 *b)
{
    float night = (float)(pow(gDayNightCycle, 1.5));

    float natLightR = 1.0f;
    float natLightG = 1.0f;
    float natLightB = 1.0f;

    float elecMultR = 1.0f;
    float elecMultG = 0.95f;
    float elecMultB = 0.45f;

    static float wetness = 0.0f;
    static float fogginess = 0.0f;
    static float lightPolution = 0.0f;

    float sunLight = max(0.0f, min(1.0f, 2.0f - night * 3.0f));

    // Night version
    natLightR = flerp(natLightR * 4.0f, 0.635f, (float)(pow(night, 0.035f + sunLight * 10.50f)));
    natLightG = flerp(natLightG * 4.0f, 0.650f, (float)(pow(night, 0.100f + sunLight *  5.50f)));
    natLightB = flerp(natLightB * 4.0f, 0.850f, (float)(pow(night, 0.200f + sunLight *  1.5f)));

    float overExpose = 0.0f;
    float lightAvg = (natLightR + natLightG + natLightB) / 3.0f;
#ifdef LIGHTFX_UNKNOWN_PART_2
    float lightMax = (natLightR + natLightG + natLightB) / 3.0f;
#endif // LIGHTFX_UNKNOWN_PART_2

    //  overExpose += ((lightMax - lightAvg) / lightMax) * 0.01f;

    if (gClimateCurrentTemperature > 20) {
        float offset = ((float)(gClimateCurrentTemperature - 20)) * 0.04f;
        offset *= 1.0f - night;
        lightAvg /= 1.0f + offset;
        //      overExpose += offset * 0.1f;
    }

#ifdef LIGHTFX_UNKNOWN_PART_2
    lightAvg += (lightMax - lightAvg) * 0.6f;
#endif // LIGHTFX_UNKNOWN_PART_2

    if (lightAvg > 1.0f) {
        natLightR /= lightAvg;
        natLightG /= lightAvg;
        natLightB /= lightAvg;
    }

    natLightR *= 1.0f + overExpose;
    natLightG *= 1.0f + overExpose;
    natLightB *= 1.0f + overExpose;
    overExpose *= 255.0f;

    float targetFogginess = (float)(gClimateCurrentRainLevel) / 8.0f;
    targetFogginess += (night * night) * 0.15f;

    if (gClimateCurrentTemperature < 10) {
        targetFogginess += ((float)(10 - gClimateCurrentTemperature)) * 0.01f;
    }

    fogginess -= (fogginess - targetFogginess) * 0.00001f;

    wetness *= 0.999995f;
    wetness += fogginess * 0.001f;
    wetness = min(wetness, 1.0f);

    float boost = 1.0f;
    float envFog = fogginess;
    float lightFog = envFog;

    float addLightNatR = 0.0f;
    float addLightNatG = 0.0f;
    float addLightNatB = 0.0f;

    float reduceColourNat = 1.0f;
    float reduceColourLit = 1.0f;

    reduceColourLit *= night / (float)pow(max(1.01f, 0.4f + lightAvg), 2.0);

    float   targetLightPollution = reduceColourLit * max(0.0f, 0.0f + 0.000001f * (float)lightfx_get_light_polution());
    lightPolution -= (lightPolution - targetLightPollution) * 0.001f;

    //  lightPollution /= 1.0f + fogginess * 1.0f;

    natLightR /= 1.0f + lightPolution * 20.0f;
    natLightG /= 1.0f + lightPolution * 20.0f;
    natLightB /= 1.0f + lightPolution * 20.0f;
    natLightR += elecMultR * 0.6f * lightPolution;
    natLightG += elecMultG * 0.6f * lightPolution;
    natLightB += elecMultB * 0.6f * lightPolution;
    natLightR /= 1.0f + lightPolution;
    natLightG /= 1.0f + lightPolution;
    natLightB /= 1.0f + lightPolution;

    reduceColourLit += (float)(gClimateCurrentRainLevel) / 2.0f;

    reduceColourNat /= 1.0f + fogginess;
    reduceColourLit /= 1.0f + fogginess;

    lightFog *= reduceColourLit;

    reduceColourNat *= 1.0f - envFog;
    reduceColourLit *= 1.0f - lightFog;

    float fogR = 35.5f * natLightR * 1.3f;
    float fogG = 45.0f * natLightG * 1.3f;
    float fogB = 50.0f * natLightB * 1.3f;
    lightFog *= 10.0f;

    float wetnessBoost = 1.0f;//1.0f + wetness * wetness * 0.1f;

    if (night >= 0 && gClimateLightningFlash != 1) {
        *r = lerp(*r, soft_light(*r, 8), night);
        *g = lerp(*g, soft_light(*g, 8), night);
        *b = lerp(*b, soft_light(*b, 128), night);

        //  if (i == 32)
        //      boost = 300000.0f;
        if ((i % 32) == 0)
            boost = 1.01f * wetnessBoost;
        else if ((i % 16) < 7)
            boost = 1.001f * wetnessBoost;
        if (i > 230 && i < 232)
            boost = ((float)(*b)) / 64.0f;

        if (false) {
            // This experiment shifts the colour of pixels as-if they are wet, but it is not a pretty solution at all
            if ((i % 16)) {
                float iVal = ((float)((i + 12) % 16)) / 16.0f;
                float eff = (wetness * ((float)pow(iVal, 1.5) * 0.85f));
                reduceColourNat *= 1.0f - eff;
                addLightNatR += fogR * eff * 3.95f;
                addLightNatG += fogR * eff * 3.95f;
                addLightNatB += fogR * eff * 3.95f;
            }
        }

        addLightNatR *= 1.0f - envFog;
        addLightNatG *= 1.0f - envFog;
        addLightNatB *= 1.0f - envFog;

        *r = (uint8)(min(255.0f, max(0.0f, (-overExpose + (float)(*r) * reduceColourNat * natLightR + envFog * fogR + addLightNatR))));
        *g = (uint8)(min(255.0f, max(0.0f, (-overExpose + (float)(*g) * reduceColourNat * natLightG + envFog * fogG + addLightNatG))));
        *b = (uint8)(min(255.0f, max(0.0f, (-overExpose + (float)(*b) * reduceColourNat * natLightB + envFog * fogB + addLightNatB))));

        rct_palette_entry * dstEntry = &gPalette_light.entries[i];
        dstEntry->red = (uint8)(min(0xFF, ((float)(*r) * reduceColourLit * boost + lightFog) * elecMultR));
        dstEntry->green = (uint8)(min(0xFF, ((float)(*g) * reduceColourLit * boost + lightFog) * elecMultG));
        dstEntry->blue = (uint8)(min(0xFF, ((float)(*b) * reduceColourLit * boost + lightFog) * elecMultB));
    }
}

static uint8 soft_light(uint8 a, uint8 b)
{
    float fa = a / 255.0f;
    float fb = b / 255.0f;
    float fr;
    if (fb < 0.5f) {
        fr = (2 * fa * fb) + ((fa * fa) * (1 - (2 * fb)));
    } else {
        fr = (2 * fa * (1 - fb)) + (sqrtf(fa) * ((2 * fb) - 1));
    }
    return (uint8)(clamp(0.0f, fr, 1.0f) * 255.0f);
}

static uint8 lerp(uint8 a, uint8 b, float t)
{
    if (t <= 0) return a;
    if (t >= 1) return b;

    sint32 range = b - a;
    sint32 amount = (sint32)(range * t);
    return (uint8)(a + amount);
}

static float flerp(float a, float b, float t)
{
    if (t <= 0) return a;
    if (t >= 1) return b;

    float range = b - a;
    float amount = range * t;
    return a + amount;
}

static uint8 mix_light(uint32 a, uint32 b, uint32 intensity)
{
    intensity = intensity * 6;
    uint32 bMul = (b * intensity) >> 8;
    uint32 ab = a + bMul;
    uint8 result = min(255, ab);
    return result;
}

void lightfx_render_to_texture(
    void * dstPixels,
    uint32 dstPitch,
    uint8 * bits,
    uint32 width,
    uint32 height,
    uint32 * palette,
    uint32 * lightPalette)
{
    lightfx_update_viewport_settings();
    lightfx_swap_buffers();
    lightfx_prepare_light_list();
    lightfx_render_lights_to_frontbuffer();

    uint8 * lightBits = (uint8 *)lightfx_get_front_buffer();
    if (lightBits == NULL) {
        return;
    }

    for (uint32 y = 0; y < height; y++) {
        uintptr_t dstOffset = (uintptr_t)(y * dstPitch);
        uint32 * dst = (uint32 *)((uintptr_t)dstPixels + dstOffset);
        for (uint32 x = 0; x < width; x++) {
            uint8 * src = &bits[y * width + x];
            uint32 darkColour = palette[*src];
            uint32 lightColour = lightPalette[*src];
            uint8 lightIntensity = lightBits[y * width + x];

            uint32 colour = 0;
            if (lightIntensity == 0) {
                colour = darkColour;
            } else {
                colour |= mix_light((darkColour >> 0) & 0xFF, (lightColour >> 0) & 0xFF, lightIntensity);
                colour |= mix_light((darkColour >> 8) & 0xFF, (lightColour >> 8) & 0xFF, lightIntensity) << 8;
                colour |= mix_light((darkColour >> 16) & 0xFF, (lightColour >> 16) & 0xFF, lightIntensity) << 16;
                colour |= mix_light((darkColour >> 24) & 0xFF, (lightColour >> 24) & 0xFF, lightIntensity) << 24;
            }
            *dst++ = colour;
        }
    }
}

#endif // __ENABLE_LIGHTFX__
