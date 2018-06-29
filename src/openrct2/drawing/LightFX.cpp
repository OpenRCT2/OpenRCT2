/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef __ENABLE_LIGHTFX__

#include <algorithm>
#include <cmath>
#include <cstring>
#include "../common.h"
#include "../config/Config.h"
#include "../Game.h"
#include "../interface/Viewport.h"
#include "../interface/Window.h"
#include "../ride/Ride.h"
#include "../util/Util.h"
#include "../world/Climate.h"
#include "../world/Map.h"
#include "../world/Sprite.h"
#include "Drawing.h"
#include "LightFX.h"

static uint8_t _bakedLightTexture_lantern_0[32*32];
static uint8_t _bakedLightTexture_lantern_1[64*64];
static uint8_t _bakedLightTexture_lantern_2[128*128];
static uint8_t _bakedLightTexture_lantern_3[256*256];
static uint8_t _bakedLightTexture_spot_0[32 * 32];
static uint8_t _bakedLightTexture_spot_1[64 * 64];
static uint8_t _bakedLightTexture_spot_2[128 * 128];
static uint8_t _bakedLightTexture_spot_3[256 * 256];
static rct_drawpixelinfo    _pixelInfo;
static bool _lightfxAvailable = false;

static void* _light_rendered_buffer_back = nullptr;
static void* _light_rendered_buffer_front = nullptr;

static uint32_t   _lightPolution_back = 0;
static uint32_t   _lightPolution_front = 0;

struct lightlist_entry {
    int16_t  x, y, z;
    uint8_t   lightType;
    uint8_t   lightIntensity;
    uint32_t  lightID;
    uint16_t  lightIDqualifier;
    uint8_t   lightLinger;
    uint8_t   pad[1];
};

static lightlist_entry  _LightListA[16000];
static lightlist_entry  _LightListB[16000];

static lightlist_entry  *_LightListBack;
static lightlist_entry  *_LightListFront;

static uint32_t           LightListCurrentCountBack;
static uint32_t           LightListCurrentCountFront;

static int16_t           _current_view_x_front           = 0;
static int16_t           _current_view_y_front           = 0;
static uint8_t            _current_view_rotation_front    = 0;
static uint8_t            _current_view_zoom_front        = 0;
static int16_t           _current_view_x_back            = 0;
static int16_t           _current_view_y_back            = 0;
static uint8_t            _current_view_rotation_back     = 0;
static uint8_t            _current_view_zoom_back         = 0;
static uint8_t            _current_view_zoom_back_delay   = 0;

static rct_palette gPalette_light;

static uint8_t calc_light_intensity_lantern(int32_t x, int32_t y) {
    double distance = (double)(x * x + y * y);

    double light = 0.03 + std::pow(10.0 / (1.0 + distance / 100.0), 0.55);
    light *= std::min(1.0, std::max(0.0, 2.0 - std::sqrt(distance) / 64));
    light *= 0.1f;

    return (uint8_t)(std::min(255.0, light * 255.0));
}

static uint8_t calc_light_intensity_spot(int32_t x, int32_t y) {
    double distance = (double)(x * x + y * y);

    double light = 0.3 + std::pow(10.0 / (1.0 + distance / 100.0), 0.75);
    light *= std::min(1.0, std::max(0.0, 2.0 - std::sqrt(distance) / 64));
    light *= 0.5f;

    return (uint8_t)(std::min(255.0, light * 255.0)) >> 4;
}

static void calc_rescale_light_half( uint8_t *target, uint8_t *source,uint32_t targetWidth, uint32_t targetHeight) {
    uint8_t *parcerRead = source;
    uint8_t *parcerWrite = target;

    for (uint32_t y = 0; y < targetHeight; y++) {
        for (uint32_t x = 0; x < targetWidth; x++) {
            *parcerWrite = (*parcerRead);
            parcerWrite++;
            parcerRead += 2;
        }
        parcerRead += targetWidth * 2;
    }
}

void lightfx_set_available(bool available)
{
    _lightfxAvailable = available;
}

bool lightfx_is_available()
{
    return _lightfxAvailable && gConfigGeneral.enable_light_fx != 0;
}

void lightfx_init()
{
    _LightListBack      = _LightListA;
    _LightListFront     = _LightListB;

    memset(_bakedLightTexture_lantern_0, 0xFF, 32 * 32);
    memset(_bakedLightTexture_lantern_1, 0xFF, 64 * 64);
    memset(_bakedLightTexture_lantern_2, 0xFF, 128 * 128);
    memset(_bakedLightTexture_lantern_3, 0xFF, 256 * 256);

    uint8_t *parcer = _bakedLightTexture_lantern_3;

    for (int32_t y = 0; y < 256; y++) {
        for (int32_t x = 0; x < 256; x++) {
            *parcer = calc_light_intensity_lantern(x - 128, y - 128);
            parcer++;
        }
    }

    parcer = _bakedLightTexture_spot_3;

    for (int32_t y = 0; y < 256; y++) {
        for (int32_t x = 0; x < 256; x++) {
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

extern void viewport_paint_setup();

void lightfx_prepare_light_list()
{
    for (uint32_t light = 0; light < LightListCurrentCountFront; light++) {
        lightlist_entry *entry = &_LightListFront[light];

        if (entry->z == 0x7FFF) {
            entry->lightIntensity = 0xFF;
            continue;
        }

        LocationXYZ16 coord_3d = {
            /* .x = */ entry->x,
            /* .y = */ entry->y,
            /* .z = */ entry->z
        };

        LocationXY16 coord_2d = coordinate_3d_to_2d(&coord_3d, _current_view_rotation_front);

        entry->x = coord_2d.x;// - (_current_view_x_front);
        entry->y = coord_2d.y;// - (_current_view_y_front);

        int32_t posOnScreenX = entry->x - _current_view_x_front;
        int32_t posOnScreenY = entry->y - _current_view_y_front;

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

        uint32_t lightIntensityOccluded = 0x0;

        int32_t dirVecX = 707;
        int32_t dirVecY = 707;

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
        int32_t tileOffsetX = 0;
        int32_t tileOffsetY = 0;
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

        int32_t mapFrontDiv = 1 << _current_view_zoom_front;
        static int16_t offsetPattern[26] = {     0, 0,       -4, 0,      0, -3,      4, 0,       0, 3,
                                                            -2, -1,     -1, -1,     2, 1,       1, 1,
                                                            -3, -2,     -3, 2,      3, -2,      3, 2    };
#endif //LIGHTFX_UNKNOWN_PART_1

        if (true) {
            int32_t totalSamplePoints = 5;
            int32_t startSamplePoint = 1;
            // int32_t lastSampleCount = 0;

            if ((entry->lightIDqualifier & 0xF) == LIGHTFX_LIGHT_QUALIFIER_MAP) {
                startSamplePoint = 0;
                totalSamplePoints = 1;
            }

            for (int32_t pat = startSamplePoint; pat < totalSamplePoints; pat++) {
                LocationXY16 mapCoord = {};

                rct_tile_element *tileElement = nullptr;

                int32_t interactionType = 0;

                rct_window *w = window_get_main();
                if (w != nullptr) {
                //  get_map_coordinates_from_pos(entry->x + offsetPattern[pat*2] / mapFrontDiv, entry->y + offsetPattern[pat*2+1] / mapFrontDiv, VIEWPORT_INTERACTION_MASK_NONE, &mapCoord.x, &mapCoord.y, &interactionType, &tileElement, NULL);

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
                    gPaintSession.DPI = dpi;
                    painter_setup();
                    viewport_paint_setup();
                    paint_session_arrange(gPaintSession);
                    sub_68862C();

                //  log_warning("[%i, %i]", dpi->x, dpi->y);

                    mapCoord.x = _interactionMapX + tileOffsetX;
                    mapCoord.y = _interactionMapY + tileOffsetY;
                    interactionType = _interactionSpriteType;
                    tileElement = RCT2_GLOBAL(0x9AC150, rct_tile_element*);
#endif //LIGHTFX_UNKNOWN_PART_1

                    //RCT2_GLOBAL(0x9AC154, uint16_t) = VIEWPORT_INTERACTION_MASK_NONE;
                    //RCT2_GLOBAL(0x9AC148, uint8_t) = 0;
                    //RCT2_GLOBAL(0x9AC138 + 4, int16_t) = screenX;
                    //RCT2_GLOBAL(0x9AC138 + 6, int16_t) = screenY;
                    //if (screenX >= 0 && screenX < (int32_t)myviewport->width && screenY >= 0 && screenY < (int32_t)myviewport->height)
                    //{
                    //  screenX <<= myviewport->zoom;
                    //  screenY <<= myviewport->zoom;
                    //  screenX += (int32_t)myviewport->view_x;
                    //  screenY += (int32_t)myviewport->view_y;
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
                    //  paint_session_arrange(gPaintSession);
                    //  sub_68862C();
                    //}
                }

                int32_t minDist = 0;
                int32_t baseHeight = -999;

                if (interactionType != VIEWPORT_INTERACTION_ITEM_SPRITE && tileElement) {
                    baseHeight = tileElement->base_height;
                }

                minDist = ((baseHeight * 8) - coord_3d.z) / 2;

                int32_t deltaX = mapCoord.x - coord_3d.x;
                int32_t deltaY = mapCoord.y - coord_3d.y;

                int32_t projDot = (dirVecX * deltaX + dirVecY * deltaY) / 1000;

                projDot = std::max(minDist, projDot);

                if (projDot < 5) {
                    lightIntensityOccluded  += 100;
                }
                else {
                    lightIntensityOccluded  += std::max(0, 200 - (projDot * 20));
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
                    // int32_t newSampleCount = lightIntensityOccluded / 900;
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

            entry->lightIntensity = std::min<uint32_t>(0xFF, (entry->lightIntensity * lightIntensityOccluded) / (totalSamplePoints * 100));
            entry->lightIntensity = std::max<uint32_t>(0x00, entry->lightIntensity - _current_view_zoom_front * 5);
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
    _LightListFront = (lightlist_entry *)tmp;

    LightListCurrentCountFront = LightListCurrentCountBack;
    LightListCurrentCountBack = 0x0;

    uint32_t uTmp = _lightPolution_back;
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
        rct_viewport * viewport = window_get_viewport(mainWindow);
        _current_view_x_back = viewport->view_x;
        _current_view_y_back = viewport->view_y;
        _current_view_rotation_back = get_current_rotation();
        _current_view_zoom_back = viewport->zoom;
    }
}

void lightfx_render_lights_to_frontbuffer()
{
    if (_light_rendered_buffer_front == nullptr) {
        return;
    }

    memset(_light_rendered_buffer_front, 0, _pixelInfo.width * _pixelInfo.height);

    _lightPolution_back = 0;

//  log_warning("%i lights", LightListCurrentCountFront);

    for (uint32_t light = 0; light < LightListCurrentCountFront; light++) {
        const uint8_t *bufReadBase    = nullptr;
        uint8_t       *bufWriteBase   = (uint8_t *)_light_rendered_buffer_front;
        uint32_t      bufReadWidth, bufReadHeight;
        int32_t      bufWriteX, bufWriteY;
        int32_t      bufWriteWidth, bufWriteHeight;
        uint32_t      bufReadSkip, bufWriteSkip;

        lightlist_entry * entry = &_LightListFront[light];

        int32_t      inRectCentreX   = entry->x;
        int32_t      inRectCentreY   = entry->y;

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

        int32_t  rightEdge = bufWriteX + bufWriteWidth;
        int32_t  bottomEdge = bufWriteY + bufWriteHeight;

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
            for (int32_t y = 0; y < bufWriteHeight; y++) {
                for (int32_t x = 0; x < bufWriteWidth; x++) {
                    *bufWriteBase = std::min(0xFF, *bufWriteBase + *bufReadBase);
                    bufWriteBase++;
                    bufReadBase++;
                }

                bufWriteBase    += bufWriteSkip;
                bufReadBase     += bufReadSkip;
            }
        }
        else {
            for (int32_t y = 0; y < bufWriteHeight; y++) {
                for (int32_t x = 0; x < bufWriteWidth; x++) {
                    *bufWriteBase = std::min(0xFF, *bufWriteBase + (((*bufReadBase) * (1 + entry->lightIntensity)) >> 8));
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

void lightfx_add_3d_light(uint32_t lightID, uint16_t lightIDqualifier, int16_t x, int16_t y, uint16_t z, uint8_t lightType)
{
    if (LightListCurrentCountBack == 15999) {
        return;
    }

//  log_warning("%i lights in back", LightListCurrentCountBack);

    for (uint32_t i = 0; i < LightListCurrentCountBack; i++) {
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

void lightfx_add_3d_light_magic_from_drawing_tile(LocationXY16 mapPosition, int16_t offsetX, int16_t offsetY, int16_t offsetZ, uint8_t lightType)
{
    int16_t x = mapPosition.x + offsetX;
    int16_t y = mapPosition.y + offsetY;

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

uint32_t lightfx_get_light_polution()
{
    return _lightPolution_front;
}

void lightfx_add_lights_magic_vehicles()
{
    uint16_t spriteIndex = gSpriteListHead[SPRITE_LIST_TRAIN];
    while (spriteIndex != SPRITE_INDEX_NULL) {
        rct_vehicle * vehicle = &(get_sprite(spriteIndex)->vehicle);
        uint16_t vehicleID = spriteIndex;
        spriteIndex = vehicle->next;

        rct_vehicle *mother_vehicle = vehicle;

        if (mother_vehicle->ride_subtype == RIDE_ENTRY_INDEX_NULL) {
            continue;
        }

        for (uint16_t q = vehicleID; q != SPRITE_INDEX_NULL; ) {
            vehicle = GET_VEHICLE(q);

            vehicleID = q;
            if (vehicle->next_vehicle_on_train == q)
                break;
            q = vehicle->next_vehicle_on_train;

            int16_t place_x, place_y, place_z;

            place_x = vehicle->x;
            place_y = vehicle->y;
            place_z = vehicle->z;

            static constexpr const int16_t offsetLookup[32] = { 10, 10, 9, 8, 7, 6, 4, 2, 0, -2, -4, -6, -7, -8, -9, -10, -10, -10, -9, -8, -7, -6, -4, -2, 0, 2, 4, 6, 7, 8, 9, 10 };

            Ride *ride = get_ride(vehicle->ride);
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
            case RIDE_TYPE_BOAT_HIRE:
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

void lightfx_apply_palette_filter(uint8_t i, uint8_t *r, uint8_t *g, uint8_t *b)
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

    float sunLight = std::max(0.0f, std::min(1.0f, 2.0f - night * 3.0f));

    // Night version
    natLightR = flerp(natLightR * 4.0f, 0.635f, (float)(std::pow(night, 0.035f + sunLight * 10.50f)));
    natLightG = flerp(natLightG * 4.0f, 0.650f, (float)(std::pow(night, 0.100f + sunLight *  5.50f)));
    natLightB = flerp(natLightB * 4.0f, 0.850f, (float)(std::pow(night, 0.200f + sunLight *  1.5f)));

    float overExpose = 0.0f;
    float lightAvg = (natLightR + natLightG + natLightB) / 3.0f;
#ifdef LIGHTFX_UNKNOWN_PART_2
    float lightMax = (natLightR + natLightG + natLightB) / 3.0f;
#endif // LIGHTFX_UNKNOWN_PART_2

    //  overExpose += ((lightMax - lightAvg) / lightMax) * 0.01f;

    if (gClimateCurrent.Temperature > 20) {
        float offset = ((float)(gClimateCurrent.Temperature - 20)) * 0.04f;
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

    float targetFogginess = (float)(gClimateCurrent.RainLevel) / 8.0f;
    targetFogginess += (night * night) * 0.15f;

    if (gClimateCurrent.Temperature < 10) {
        targetFogginess += ((float)(10 - gClimateCurrent.Temperature)) * 0.01f;
    }

    fogginess -= (fogginess - targetFogginess) * 0.00001f;

    wetness *= 0.999995f;
    wetness += fogginess * 0.001f;
    wetness = std::min(wetness, 1.0f);

    float boost = 1.0f;
    float envFog = fogginess;
    float lightFog = envFog;

    float addLightNatR = 0.0f;
    float addLightNatG = 0.0f;
    float addLightNatB = 0.0f;

    float reduceColourNat = 1.0f;
    float reduceColourLit = 1.0f;

    reduceColourLit *= night / (float)std::pow(std::max(1.01f, 0.4f + lightAvg), 2.0);

    float   targetLightPollution = reduceColourLit * std::max(0.0f, 0.0f + 0.000001f * (float)lightfx_get_light_polution());
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

    reduceColourLit += (float)(gClimateCurrent.RainLevel) / 2.0f;

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
                float eff = (wetness * ((float)std::pow(iVal, 1.5) * 0.85f));
                reduceColourNat *= 1.0f - eff;
                addLightNatR += fogR * eff * 3.95f;
                addLightNatG += fogR * eff * 3.95f;
                addLightNatB += fogR * eff * 3.95f;
            }
        }

        addLightNatR *= 1.0f - envFog;
        addLightNatG *= 1.0f - envFog;
        addLightNatB *= 1.0f - envFog;

        *r = (uint8_t)(std::min(255.0f, std::max(0.0f, (-overExpose + (float)(*r) * reduceColourNat * natLightR + envFog * fogR + addLightNatR))));
        *g = (uint8_t)(std::min(255.0f, std::max(0.0f, (-overExpose + (float)(*g) * reduceColourNat * natLightG + envFog * fogG + addLightNatG))));
        *b = (uint8_t)(std::min(255.0f, std::max(0.0f, (-overExpose + (float)(*b) * reduceColourNat * natLightB + envFog * fogB + addLightNatB))));

        rct_palette_entry * dstEntry = &gPalette_light.entries[i];
        dstEntry->red = (uint8_t)(std::min<float>(0xFF, ((float)(*r) * reduceColourLit * boost + lightFog) * elecMultR));
        dstEntry->green = (uint8_t)(std::min<float>(0xFF, ((float)(*g) * reduceColourLit * boost + lightFog) * elecMultG));
        dstEntry->blue = (uint8_t)(std::min<float>(0xFF, ((float)(*b) * reduceColourLit * boost + lightFog) * elecMultB));
    }
}

static uint8_t mix_light(uint32_t a, uint32_t b, uint32_t intensity)
{
    intensity = intensity * 6;
    uint32_t bMul = (b * intensity) >> 8;
    uint32_t ab = a + bMul;
    uint8_t result = std::min<uint32_t>(255, ab);
    return result;
}

void lightfx_render_to_texture(
    void * dstPixels,
    uint32_t dstPitch,
    uint8_t * bits,
    uint32_t width,
    uint32_t height,
    const uint32_t * palette,
    const uint32_t * lightPalette)
{
    lightfx_update_viewport_settings();
    lightfx_swap_buffers();
    lightfx_prepare_light_list();
    lightfx_render_lights_to_frontbuffer();

    uint8_t * lightBits = (uint8_t *)lightfx_get_front_buffer();
    if (lightBits == nullptr) {
        return;
    }

    for (uint32_t y = 0; y < height; y++) {
        uintptr_t dstOffset = (uintptr_t)(y * dstPitch);
        uint32_t * dst = (uint32_t *)((uintptr_t)dstPixels + dstOffset);
        for (uint32_t x = 0; x < width; x++) {
            uint8_t * src = &bits[y * width + x];
            uint32_t darkColour = palette[*src];
            uint32_t lightColour = lightPalette[*src];
            uint8_t lightIntensity = lightBits[y * width + x];

            uint32_t colour = 0;
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
