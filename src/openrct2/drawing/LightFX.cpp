/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LightFX.h"

#include "../Game.h"
#include "../common.h"
#include "../config/Config.h"
#include "../entity/EntityRegistry.h"
#include "../interface/Viewport.h"
#include "../interface/Window.h"
#include "../interface/Window_internal.h"
#include "../paint/Paint.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/Vehicle.h"
#include "../util/Util.h"
#include "../world/Climate.h"
#include "../world/Map.h"
#include "Drawing.h"

#include <algorithm>
#include <cmath>
#include <cstring>

static uint8_t _bakedLightTexture_lantern_0[32 * 32];
static uint8_t _bakedLightTexture_lantern_1[64 * 64];
static uint8_t _bakedLightTexture_lantern_2[128 * 128];
static uint8_t _bakedLightTexture_lantern_3[256 * 256];
static uint8_t _bakedLightTexture_spot_0[32 * 32];
static uint8_t _bakedLightTexture_spot_1[64 * 64];
static uint8_t _bakedLightTexture_spot_2[128 * 128];
static uint8_t _bakedLightTexture_spot_3[256 * 256];
static rct_drawpixelinfo _pixelInfo;
static bool _lightfxAvailable = false;

static void* _light_rendered_buffer_back = nullptr;
static void* _light_rendered_buffer_front = nullptr;

static uint32_t _lightPolution_back = 0;
static uint32_t _lightPolution_front = 0;

enum class LightFXQualifier : uint8_t
{
    Entity,
    Map,
};

struct LightListEntry
{
    CoordsXYZ Position;
    ScreenCoordsXY ViewCoords;
    LightType Type;
    uint8_t LightIntensity;
    uint32_t LightHash;
    LightFXQualifier Qualifier;
    uint8_t LightID;
    uint8_t LightLinger;
};

static LightListEntry _LightListA[16000];
static LightListEntry _LightListB[16000];

static LightListEntry* _LightListBack;
static LightListEntry* _LightListFront;

static uint32_t LightListCurrentCountBack;
static uint32_t LightListCurrentCountFront;

static int16_t _current_view_x_front = 0;
static int16_t _current_view_y_front = 0;
static uint8_t _current_view_rotation_front = 0;
static ZoomLevel _current_view_zoom_front{ 0 };
static int16_t _current_view_x_back = 0;
static int16_t _current_view_y_back = 0;
static uint8_t _current_view_rotation_back = 0;
static ZoomLevel _current_view_zoom_back{ 0 };
static ZoomLevel _current_view_zoom_back_delay{ 0 };

static GamePalette gPalette_light;

static uint8_t calc_light_intensity_lantern(int32_t x, int32_t y)
{
    double distance = static_cast<double>(x * x + y * y);

    double light = 0.03 + std::pow(10.0 / (1.0 + distance / 100.0), 0.55);
    light *= std::min(1.0, std::max(0.0, 2.0 - std::sqrt(distance) / 64));
    light *= 0.1f;

    return static_cast<uint8_t>(std::min(255.0, light * 255.0));
}

static uint8_t calc_light_intensity_spot(int32_t x, int32_t y)
{
    double distance = static_cast<double>(x * x + y * y);

    double light = 0.3 + std::pow(10.0 / (1.0 + distance / 100.0), 0.75);
    light *= std::min(1.0, std::max(0.0, 2.0 - std::sqrt(distance) / 64));
    light *= 0.5f;

    return static_cast<uint8_t>(std::min(255.0, light * 255.0)) >> 4;
}

static void calc_rescale_light_half(uint8_t* target, uint8_t* source, uint32_t targetWidth, uint32_t targetHeight)
{
    uint8_t* parcerRead = source;
    uint8_t* parcerWrite = target;

    for (uint32_t y = 0; y < targetHeight; y++)
    {
        for (uint32_t x = 0; x < targetWidth; x++)
        {
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
    return _lightfxAvailable && gConfigGeneral.EnableLightFx != 0;
}

bool lightfx_for_vehicles_is_available()
{
    return lightfx_is_available() && gConfigGeneral.EnableLightFxForVehicles != 0;
}

void lightfx_init()
{
    _LightListBack = _LightListA;
    _LightListFront = _LightListB;

    std::fill_n(_bakedLightTexture_lantern_0, 32 * 32, 0xFF);
    std::fill_n(_bakedLightTexture_lantern_1, 64 * 64, 0xFF);
    std::fill_n(_bakedLightTexture_lantern_2, 128 * 128, 0xFF);
    std::fill_n(_bakedLightTexture_lantern_3, 256 * 256, 0xFF);

    uint8_t* parcer = _bakedLightTexture_lantern_3;

    for (int32_t y = 0; y < 256; y++)
    {
        for (int32_t x = 0; x < 256; x++)
        {
            *parcer = calc_light_intensity_lantern(x - 128, y - 128);
            parcer++;
        }
    }

    parcer = _bakedLightTexture_spot_3;

    for (int32_t y = 0; y < 256; y++)
    {
        for (int32_t x = 0; x < 256; x++)
        {
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

void lightfx_update_buffers(rct_drawpixelinfo* info)
{
    _light_rendered_buffer_front = realloc(_light_rendered_buffer_front, info->width * info->height);
    _light_rendered_buffer_back = realloc(_light_rendered_buffer_back, info->width * info->height);
    _pixelInfo = *info;
}

void lightfx_prepare_light_list()
{
    for (uint32_t light = 0; light < LightListCurrentCountFront; light++)
    {
        LightListEntry* entry = &_LightListFront[light];

        if (entry->Position.z == 0x7FFF)
        {
            entry->LightIntensity = 0xFF;
            continue;
        }

        int32_t posOnScreenX = entry->ViewCoords.x - _current_view_x_front;
        int32_t posOnScreenY = entry->ViewCoords.y - _current_view_y_front;

        posOnScreenX = _current_view_zoom_front.ApplyInversedTo(posOnScreenX);
        posOnScreenY = _current_view_zoom_front.ApplyInversedTo(posOnScreenY);

        if ((posOnScreenX < -128) || (posOnScreenY < -128) || (posOnScreenX > _pixelInfo.width + 128)
            || (posOnScreenY > _pixelInfo.height + 128))
        {
            entry->Type = LightType::None;
            continue;
        }

        uint32_t lightIntensityOccluded = 0x0;

        int32_t dirVecX = 707;
        int32_t dirVecY = 707;

        switch (_current_view_rotation_front)
        {
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

        int32_t tileOffsetX = 0;
        int32_t tileOffsetY = 0;
        switch (_current_view_rotation_front)
        {
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

        int32_t mapFrontDiv = _current_view_zoom_front.ApplyTo(1);

        // clang-format off
        static int16_t offsetPattern[26] = {
            0, 0,
            -4, 0, 0, -3, 4, 0, 0, 3,
            -2, -1, -1, -1, 2, 1, 1, 1,
            -3, -2, -3, 2, 3, -2, 3, 2,
        };
        // clang-format on

        // Light occlusion code
        if (true)
        {
            int32_t totalSamplePoints = 5;
            int32_t startSamplePoint = 1;

            if (entry->Qualifier == LightFXQualifier::Map)
            {
                startSamplePoint = 0;
                totalSamplePoints = 1;
            }

            for (int32_t pat = startSamplePoint; pat < totalSamplePoints; pat++)
            {
                CoordsXY mapCoord{};

                TileElement* tileElement = nullptr;

                ViewportInteractionItem interactionType = ViewportInteractionItem::None;

                auto* w = WindowGetMain();
                if (w != nullptr)
                {
                    // based on GetMapCoordinatesFromPosWindow
                    rct_drawpixelinfo dpi;
                    dpi.x = entry->ViewCoords.x + offsetPattern[0 + pat * 2] / mapFrontDiv;
                    dpi.y = entry->ViewCoords.y + offsetPattern[1 + pat * 2] / mapFrontDiv;
                    dpi.height = 1;
                    dpi.zoom_level = _current_view_zoom_front;
                    dpi.width = 1;

                    PaintSession* session = PaintSessionAlloc(&dpi, w->viewport->flags);
                    PaintSessionGenerate(*session);
                    PaintSessionArrange(*session);
                    auto info = SetInteractionInfoFromPaintSession(session, w->viewport->flags, ViewportInteractionItemAll);
                    PaintSessionFree(session);

                    //  log_warning("[%i, %i]", dpi->x, dpi->y);

                    mapCoord = info.Loc;
                    mapCoord.x += tileOffsetX;
                    mapCoord.y += tileOffsetY;
                    interactionType = info.SpriteType;
                    tileElement = info.Element;
                }

                int32_t minDist = 0;
                int32_t baseHeight = (-999) * COORDS_Z_STEP;

                if (interactionType != ViewportInteractionItem::Entity && tileElement != nullptr)
                {
                    baseHeight = tileElement->GetBaseZ();
                }

                minDist = (baseHeight - entry->Position.z) / 2;

                int32_t deltaX = mapCoord.x - entry->Position.x;
                int32_t deltaY = mapCoord.y - entry->Position.y;

                int32_t projDot = (dirVecX * deltaX + dirVecY * deltaY) / 1000;

                projDot = std::max(minDist, projDot);

                if (projDot < 5)
                {
                    lightIntensityOccluded += 100;
                }
                else
                {
                    lightIntensityOccluded += std::max(0, 200 - (projDot * 20));
                }

                //  log_warning("light %i [%i, %i, %i], [%i, %i] minDist to %i: %i; projdot: %i", light, coord_3d.x, coord_3d.y,
                //  coord_3d.z, mapCoord.x, mapCoord.y, baseHeight, minDist, projDot);

                if (pat == 0)
                {
                    if (lightIntensityOccluded == 100)
                        break;
                    if (_current_view_zoom_front > ZoomLevel{ 2 })
                        break;
                    totalSamplePoints += 4;
                }
                else if (pat == 4)
                {
                    if (_current_view_zoom_front > ZoomLevel{ 1 })
                        break;
                    if (lightIntensityOccluded == 0 || lightIntensityOccluded == 500)
                        break;
                    // lastSampleCount = lightIntensityOccluded / 500;
                    //  break;
                    totalSamplePoints += 4;
                }
                else if (pat == 8)
                {
                    break;
                }
            }

            totalSamplePoints -= startSamplePoint;

            if (lightIntensityOccluded == 0)
            {
                entry->Type = LightType::None;
                continue;
            }

            entry->LightIntensity = std::min<uint32_t>(
                0xFF, (entry->LightIntensity * lightIntensityOccluded) / (totalSamplePoints * 100));
        }
        entry->LightIntensity = std::max<uint32_t>(
            0x00, entry->LightIntensity - static_cast<int8_t>(_current_view_zoom_front) * 5);

        if (_current_view_zoom_front > ZoomLevel{ 0 })
        {
            if (GetLightTypeSize(entry->Type) < static_cast<int8_t>(_current_view_zoom_front))
            {
                entry->Type = LightType::None;
                continue;
            }

            entry->Type = SetLightTypeSize(
                entry->Type, GetLightTypeSize(entry->Type) - static_cast<int8_t>(_current_view_zoom_front));
        }
    }
}

void lightfx_swap_buffers()
{
    void* tmp = _light_rendered_buffer_back;
    _light_rendered_buffer_back = _light_rendered_buffer_front;
    _light_rendered_buffer_front = tmp;

    tmp = _light_rendered_buffer_back;
    _light_rendered_buffer_back = _light_rendered_buffer_front;
    _light_rendered_buffer_front = tmp;

    tmp = _LightListBack;
    _LightListBack = _LightListFront;
    _LightListFront = static_cast<LightListEntry*>(tmp);

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
    rct_window* mainWindow = WindowGetMain();
    if (mainWindow != nullptr)
    {
        rct_viewport* viewport = WindowGetViewport(mainWindow);
        _current_view_x_back = viewport->viewPos.x;
        _current_view_y_back = viewport->viewPos.y;
        _current_view_rotation_back = GetCurrentRotation();
        _current_view_zoom_back = viewport->zoom;
    }
}

void lightfx_render_lights_to_frontbuffer()
{
    if (_light_rendered_buffer_front == nullptr)
    {
        return;
    }

    std::memset(_light_rendered_buffer_front, 0, _pixelInfo.width * _pixelInfo.height);

    _lightPolution_back = 0;

    //  log_warning("%i lights", LightListCurrentCountFront);

    for (uint32_t light = 0; light < LightListCurrentCountFront; light++)
    {
        const uint8_t* bufReadBase = nullptr;
        uint8_t* bufWriteBase = static_cast<uint8_t*>(_light_rendered_buffer_front);
        uint32_t bufReadWidth, bufReadHeight;
        int32_t bufWriteX, bufWriteY;
        int32_t bufWriteWidth, bufWriteHeight;
        uint32_t bufReadSkip, bufWriteSkip;

        LightListEntry* entry = &_LightListFront[light];

        int32_t inRectCentreX = entry->ViewCoords.x;
        int32_t inRectCentreY = entry->ViewCoords.y;

        if (entry->Position.z != 0x7FFF)
        {
            inRectCentreX -= _current_view_x_front;
            inRectCentreY -= _current_view_y_front;
            inRectCentreX = _current_view_zoom_front.ApplyInversedTo(inRectCentreX);
            inRectCentreY = _current_view_zoom_front.ApplyInversedTo(inRectCentreY);
        }

        switch (entry->Type)
        {
            case LightType::Lantern0:
                bufReadWidth = 32;
                bufReadHeight = 32;
                bufReadBase = _bakedLightTexture_lantern_0;
                break;
            case LightType::Lantern1:
                bufReadWidth = 64;
                bufReadHeight = 64;
                bufReadBase = _bakedLightTexture_lantern_1;
                break;
            case LightType::Lantern2:
                bufReadWidth = 128;
                bufReadHeight = 128;
                bufReadBase = _bakedLightTexture_lantern_2;
                break;
            case LightType::Lantern3:
                bufReadWidth = 256;
                bufReadHeight = 256;
                bufReadBase = _bakedLightTexture_lantern_3;
                break;
            case LightType::Spot0:
                bufReadWidth = 32;
                bufReadHeight = 32;
                bufReadBase = _bakedLightTexture_spot_0;
                break;
            case LightType::Spot1:
                bufReadWidth = 64;
                bufReadHeight = 64;
                bufReadBase = _bakedLightTexture_spot_1;
                break;
            case LightType::Spot2:
                bufReadWidth = 128;
                bufReadHeight = 128;
                bufReadBase = _bakedLightTexture_spot_2;
                break;
            case LightType::Spot3:
                bufReadWidth = 256;
                bufReadHeight = 256;
                bufReadBase = _bakedLightTexture_spot_3;
                break;
            default:
                continue;
        }

        // Clamp the reads to be no larger than the buffer size
        bufReadHeight = std::min<uint32_t>(_pixelInfo.height, bufReadHeight);
        bufReadWidth = std::min<uint32_t>(_pixelInfo.width, bufReadWidth);

        bufWriteX = inRectCentreX - bufReadWidth / 2;
        bufWriteY = inRectCentreY - bufReadHeight / 2;

        bufWriteWidth = bufReadWidth;
        bufWriteHeight = bufReadHeight;

        if (bufWriteX < 0)
        {
            bufReadBase += -bufWriteX;
            bufWriteWidth += bufWriteX;
        }
        else
        {
            bufWriteBase += bufWriteX;
        }

        if (bufWriteWidth <= 0)
            continue;

        if (bufWriteY < 0)
        {
            bufReadBase += -bufWriteY * bufReadWidth;
            bufWriteHeight += bufWriteY;
        }
        else
        {
            bufWriteBase += bufWriteY * _pixelInfo.width;
        }

        if (bufWriteHeight <= 0)
            continue;

        int32_t rightEdge = bufWriteX + bufWriteWidth;
        int32_t bottomEdge = bufWriteY + bufWriteHeight;

        if (rightEdge > _pixelInfo.width)
        {
            bufWriteWidth -= rightEdge - _pixelInfo.width;
        }
        if (bottomEdge > _pixelInfo.height)
        {
            bufWriteHeight -= bottomEdge - _pixelInfo.height;
        }

        if (bufWriteWidth <= 0)
            continue;
        if (bufWriteHeight <= 0)
            continue;

        _lightPolution_back += (bufWriteWidth * bufWriteHeight) / 256;

        bufReadSkip = bufReadWidth - bufWriteWidth;
        bufWriteSkip = _pixelInfo.width - bufWriteWidth;

        if (entry->LightIntensity == 0xFF)
        {
            for (int32_t y = 0; y < bufWriteHeight; y++)
            {
                for (int32_t x = 0; x < bufWriteWidth; x++)
                {
                    *bufWriteBase = std::min(0xFF, *bufWriteBase + *bufReadBase);
                    bufWriteBase++;
                    bufReadBase++;
                }

                bufWriteBase += bufWriteSkip;
                bufReadBase += bufReadSkip;
            }
        }
        else
        {
            for (int32_t y = 0; y < bufWriteHeight; y++)
            {
                for (int32_t x = 0; x < bufWriteWidth; x++)
                {
                    *bufWriteBase = std::min(0xFF, *bufWriteBase + (((*bufReadBase) * (1 + entry->LightIntensity)) >> 8));
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

const GamePalette& lightfx_get_palette()
{
    return gPalette_light;
}

static void LightfxAdd3DLight(
    const uint32_t lightHash, const LightFXQualifier qualifier, const uint8_t id, const CoordsXYZ& loc,
    const LightType lightType)
{
    if (LightListCurrentCountBack == 15999)
    {
        return;
    }

    //  log_warning("%i lights in back", LightListCurrentCountBack);

    for (uint32_t i = 0; i < LightListCurrentCountBack; i++)
    {
        LightListEntry* entry = &_LightListBack[i];
        if (entry->LightHash != lightHash)
            continue;
        if (entry->Qualifier != qualifier)
            continue;
        if (entry->LightID != id)
            continue;

        entry->Position = loc;
        entry->ViewCoords = Translate3DTo2DWithZ(GetCurrentRotation(), loc);
        entry->Type = lightType;
        entry->LightIntensity = 0xFF;
        entry->LightHash = lightHash;
        entry->Qualifier = qualifier;
        entry->LightID = id;
        entry->LightLinger = 1;

        return;
    }

    LightListEntry* entry = &_LightListBack[LightListCurrentCountBack++];

    entry->Position = loc;
    entry->ViewCoords = Translate3DTo2DWithZ(GetCurrentRotation(), loc);
    entry->Type = lightType;
    entry->LightIntensity = 0xFF;
    entry->LightHash = lightHash;
    entry->Qualifier = qualifier;
    entry->LightID = id;
    entry->LightLinger = 1;

    //  log_warning("new 3d light");
}

static void LightfxAdd3DLight(const CoordsXYZ& loc, const LightType lightType)
{
    LightfxAdd3DLight(((loc.x << 16) | loc.y), LightFXQualifier::Map, loc.z, loc, lightType);
}

void LightfxAdd3DLight(const EntityBase& entity, const uint8_t id, const CoordsXYZ& loc, const LightType lightType)
{
    LightfxAdd3DLight(entity.sprite_index.ToUnderlying(), LightFXQualifier::Entity, id, loc, lightType);
}

void lightfx_add_3d_light_magic_from_drawing_tile(
    const CoordsXY& mapPosition, int16_t offsetX, int16_t offsetY, int16_t offsetZ, LightType lightType)
{
    int16_t x = mapPosition.x + offsetX + 16;
    int16_t y = mapPosition.y + offsetY + 16;

    LightfxAdd3DLight({ x, y, offsetZ }, lightType);
}

uint32_t lightfx_get_light_polution()
{
    return _lightPolution_front;
}

static constexpr const int16_t offsetLookup[] = {
    10, 10, 9, 8, 7, 6, 4, 2, 0, -2, -4, -6, -7, -8, -9, -10, -10, -10, -9, -8, -7, -6, -4, -2, 0, 2, 4, 6, 7, 8, 9, 10,
};
void LightFxAddLightsMagicVehicle_ObservationTower(const Vehicle* vehicle)
{
    LightfxAdd3DLight(*vehicle, 0, { vehicle->x, vehicle->y + 16, vehicle->z }, LightType::Spot3);
    LightfxAdd3DLight(*vehicle, 1, { vehicle->x + 16, vehicle->y, vehicle->z }, LightType::Spot3);
    LightfxAdd3DLight(*vehicle, 2, { vehicle->x - 16, vehicle->y, vehicle->z }, LightType::Spot3);
    LightfxAdd3DLight(*vehicle, 3, { vehicle->x, vehicle->y - 16, vehicle->z }, LightType::Spot3);
}

void LightFxAddLightsMagicVehicle_MineTrainCoaster(const Vehicle* vehicle)
{
    if (vehicle == vehicle->TrainHead())
    {
        int16_t place_x = vehicle->x - offsetLookup[(vehicle->sprite_direction + 0) % 32] * 2;
        int16_t place_y = vehicle->y - offsetLookup[(vehicle->sprite_direction + 8) % 32] * 2;
        LightfxAdd3DLight(*vehicle, 0, { place_x, place_y, vehicle->z }, LightType::Spot3);
    }
}

void LightFxAddLightsMagicVehicle_ChairLift(const Vehicle* vehicle)
{
    LightfxAdd3DLight(*vehicle, 0, { vehicle->x, vehicle->y, vehicle->z - 16 }, LightType::Lantern2);
}
void LightFxAddLightsMagicVehicle_BoatHire(const Vehicle* vehicle)
{
    Vehicle* vehicle_draw = vehicle->TrainHead();
    auto* nextVeh = GetEntity<Vehicle>(vehicle_draw->next_vehicle_on_train);
    if (nextVeh != nullptr)
    {
        vehicle_draw = nextVeh;
    }
    int16_t place_x = vehicle_draw->x;
    int16_t place_y = vehicle_draw->y;
    place_x -= offsetLookup[(vehicle_draw->sprite_direction + 0) % 32];
    place_y -= offsetLookup[(vehicle_draw->sprite_direction + 8) % 32];
    LightfxAdd3DLight(*vehicle, 0, { place_x, place_y, vehicle_draw->z }, LightType::Spot2);
    place_x -= offsetLookup[(vehicle_draw->sprite_direction + 0) % 32];
    place_y -= offsetLookup[(vehicle_draw->sprite_direction + 8) % 32];
    LightfxAdd3DLight(*vehicle, 1, { place_x, place_y, vehicle_draw->z }, LightType::Spot2);
}
void LightFxAddLightsMagicVehicle_Monorail(const Vehicle* vehicle)
{
    LightfxAdd3DLight(*vehicle, 0, { vehicle->x, vehicle->y, vehicle->z + 12 }, LightType::Spot2);
    int16_t place_x = vehicle->x;
    int16_t place_y = vehicle->y;
    if (vehicle == vehicle->TrainHead())
    {
        place_x -= offsetLookup[(vehicle->sprite_direction + 0) % 32] * 2;
        place_y -= offsetLookup[(vehicle->sprite_direction + 8) % 32] * 2;
        LightfxAdd3DLight(*vehicle, 1, { place_x, place_y, vehicle->z + 10 }, LightType::Lantern3);
        place_x -= offsetLookup[(vehicle->sprite_direction + 0) % 32] * 3;
        place_y -= offsetLookup[(vehicle->sprite_direction + 8) % 32] * 3;
        LightfxAdd3DLight(*vehicle, 2, { place_x, place_y, vehicle->z + 2 }, LightType::Lantern3);
    }
    if (vehicle == vehicle->TrainTail())
    {
        place_x += offsetLookup[(vehicle->sprite_direction + 0) % 32] * 2;
        place_y += offsetLookup[(vehicle->sprite_direction + 8) % 32] * 2;
        LightfxAdd3DLight(*vehicle, 3, { place_x, place_y, vehicle->z + 10 }, LightType::Lantern3);
        place_x += offsetLookup[(vehicle->sprite_direction + 0) % 32] * 2;
        place_y += offsetLookup[(vehicle->sprite_direction + 8) % 32] * 2;
        LightfxAdd3DLight(*vehicle, 4, { place_x, place_y, vehicle->z + 2 }, LightType::Lantern3);
    }
}
void LightFxAddLightsMagicVehicle_MiniatureRailway(const Vehicle* vehicle)
{
    if (vehicle == vehicle->TrainHead())
    {
        int16_t place_x = vehicle->x - offsetLookup[(vehicle->sprite_direction + 0) % 32] * 2;
        int16_t place_y = vehicle->y - offsetLookup[(vehicle->sprite_direction + 8) % 32] * 2;
        LightfxAdd3DLight(*vehicle, 1, { place_x, place_y, vehicle->z + 10 }, LightType::Lantern3);
        place_x -= offsetLookup[(vehicle->sprite_direction + 0) % 32] * 2;
        place_y -= offsetLookup[(vehicle->sprite_direction + 8) % 32] * 2;
        LightfxAdd3DLight(*vehicle, 2, { place_x, place_y, vehicle->z + 2 }, LightType::Lantern3);
    }
    else
    {
        LightfxAdd3DLight(*vehicle, 0, { vehicle->x, vehicle->y, vehicle->z + 10 }, LightType::Lantern3);
    }
}

void LightfxAddLightsMagicVehicle(const Vehicle* vehicle)
{
    auto ride = vehicle->GetRide();
    if (ride == nullptr)
        return;

    const auto& rtd = GetRideTypeDescriptor(ride->type);
    if (rtd.LightFXAddLightsMagicVehicle != nullptr)
        rtd.LightFXAddLightsMagicVehicle(vehicle);
}

void LightFxAddKioskLights(const CoordsXY& mapPosition, const int32_t height, const uint8_t zOffset)
{
    uint8_t relativeRotation = (4 - GetCurrentRotation()) % 4;
    CoordsXY lanternOffset1 = CoordsXY(0, 16).Rotate(relativeRotation);
    CoordsXY lanternOffset2 = CoordsXY(16, 0).Rotate(relativeRotation);
    lightfx_add_3d_light_magic_from_drawing_tile(
        mapPosition, lanternOffset1.x, lanternOffset1.y, height + zOffset, LightType::Lantern3);
    lightfx_add_3d_light_magic_from_drawing_tile(
        mapPosition, lanternOffset2.x, lanternOffset2.y, height + zOffset, LightType::Lantern3);
    lightfx_add_3d_light_magic_from_drawing_tile(mapPosition, 8, 32, height, LightType::Spot1);
    lightfx_add_3d_light_magic_from_drawing_tile(mapPosition, 32, 8, height, LightType::Spot1);
    lightfx_add_3d_light_magic_from_drawing_tile(mapPosition, -32, 8, height, LightType::Spot1);
    lightfx_add_3d_light_magic_from_drawing_tile(mapPosition, 8, -32, height, LightType::Spot1);
    lightfx_add_3d_light_magic_from_drawing_tile(mapPosition, -8, 32, height, LightType::Spot1);
    lightfx_add_3d_light_magic_from_drawing_tile(mapPosition, 32, -8, height, LightType::Spot1);
    lightfx_add_3d_light_magic_from_drawing_tile(mapPosition, -32, -8, height, LightType::Spot1);
    lightfx_add_3d_light_magic_from_drawing_tile(mapPosition, -8, -32, height, LightType::Spot1);
}

void LightFxAddShopLights(const CoordsXY& mapPosition, const uint8_t direction, const int32_t height, const uint8_t zOffset)
{
    if (direction == (4 - GetCurrentRotation()) % 4) // Back Right Facing Stall
    {
        CoordsXY spotOffset1 = CoordsXY(-32, 8).Rotate(direction);
        CoordsXY spotOffset2 = CoordsXY(-32, 4).Rotate(direction);
        lightfx_add_3d_light_magic_from_drawing_tile(mapPosition, spotOffset1.x, spotOffset1.y, height, LightType::Spot1);
        lightfx_add_3d_light_magic_from_drawing_tile(mapPosition, spotOffset2.x, spotOffset2.y, height, LightType::Spot2);
    }
    else if (direction == (7 - GetCurrentRotation()) % 4) // Back left Facing Stall
    {
        CoordsXY spotOffset1 = CoordsXY(-32, -8).Rotate(direction);
        CoordsXY spotOffset2 = CoordsXY(-32, -4).Rotate(direction);
        lightfx_add_3d_light_magic_from_drawing_tile(mapPosition, spotOffset1.x, spotOffset1.y, height, LightType::Spot1);
        lightfx_add_3d_light_magic_from_drawing_tile(mapPosition, spotOffset2.x, spotOffset2.y, height, LightType::Spot2);
    }
    else // Forward Facing Stall
    {
        CoordsXY spotOffset1 = CoordsXY(-32, 8).Rotate(direction);
        CoordsXY spotOffset2 = CoordsXY(-32, -8).Rotate(direction);
        CoordsXY lanternOffset = CoordsXY(-16, 0).Rotate(direction);
        lightfx_add_3d_light_magic_from_drawing_tile(
            mapPosition, lanternOffset.x, lanternOffset.y, height + zOffset, LightType::Lantern3);
        lightfx_add_3d_light_magic_from_drawing_tile(mapPosition, spotOffset1.x, spotOffset1.y, height, LightType::Spot1);
        lightfx_add_3d_light_magic_from_drawing_tile(mapPosition, spotOffset2.x, spotOffset2.y, height, LightType::Spot1);
    }
}

void lightfx_apply_palette_filter(uint8_t i, uint8_t* r, uint8_t* g, uint8_t* b)
{
    float night = static_cast<float>(pow(gDayNightCycle, 1.5));

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
    natLightR = flerp(natLightR * 4.0f, 0.635f, (std::pow(night, 0.035f + sunLight * 10.50f)));
    natLightG = flerp(natLightG * 4.0f, 0.650f, (std::pow(night, 0.100f + sunLight * 5.50f)));
    natLightB = flerp(natLightB * 4.0f, 0.850f, (std::pow(night, 0.200f + sunLight * 1.5f)));

    float overExpose = 0.0f;
    float lightAvg = (natLightR + natLightG + natLightB) / 3.0f;
#ifdef LIGHTFX_UNKNOWN_PART_2
    float lightMax = (natLightR + natLightG + natLightB) / 3.0f;
#endif // LIGHTFX_UNKNOWN_PART_2

    //  overExpose += ((lightMax - lightAvg) / lightMax) * 0.01f;

    if (gClimateCurrent.Temperature > 20)
    {
        float offset = (static_cast<float>(gClimateCurrent.Temperature - 20)) * 0.04f;
        offset *= 1.0f - night;
        lightAvg /= 1.0f + offset;
        //      overExpose += offset * 0.1f;
    }

#ifdef LIGHTFX_UNKNOWN_PART_2
    lightAvg += (lightMax - lightAvg) * 0.6f;
#endif // LIGHTFX_UNKNOWN_PART_2

    if (lightAvg > 1.0f)
    {
        natLightR /= lightAvg;
        natLightG /= lightAvg;
        natLightB /= lightAvg;
    }

    natLightR *= 1.0f + overExpose;
    natLightG *= 1.0f + overExpose;
    natLightB *= 1.0f + overExpose;
    overExpose *= 255.0f;

    float targetFogginess = static_cast<float>(gClimateCurrent.Level) / 8.0f;
    targetFogginess += (night * night) * 0.15f;

    if (gClimateCurrent.Temperature < 10)
    {
        targetFogginess += (static_cast<float>(10 - gClimateCurrent.Temperature)) * 0.01f;
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

    reduceColourLit *= night / static_cast<float>(std::pow(std::max(1.01f, 0.4f + lightAvg), 2.0));

    float targetLightPollution = reduceColourLit
        * std::max(0.0f, 0.0f + 0.000001f * static_cast<float>(lightfx_get_light_polution()));
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

    reduceColourLit += static_cast<float>(gClimateCurrent.Level) / 2.0f;

    reduceColourNat /= 1.0f + fogginess;
    reduceColourLit /= 1.0f + fogginess;

    lightFog *= reduceColourLit;

    reduceColourNat *= 1.0f - envFog;
    reduceColourLit *= 1.0f - lightFog;

    float fogR = 35.5f * natLightR * 1.3f;
    float fogG = 45.0f * natLightG * 1.3f;
    float fogB = 50.0f * natLightB * 1.3f;
    lightFog *= 10.0f;

    float wetnessBoost = 1.0f; // 1.0f + wetness * wetness * 0.1f;

    if (night >= 0 && gClimateLightningFlash != 1)
    {
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
            boost = (static_cast<float>(*b)) / 64.0f;

        if (false)
        {
            // This experiment shifts the colour of pixels as-if they are wet, but it is not a pretty solution at all
            if ((i % 16))
            {
                float iVal = (static_cast<float>((i + 12) % 16)) / 16.0f;
                float eff = (wetness * (static_cast<float>(std::pow(iVal, 1.5)) * 0.85f));
                reduceColourNat *= 1.0f - eff;
                addLightNatR += fogR * eff * 3.95f;
                addLightNatG += fogR * eff * 3.95f;
                addLightNatB += fogR * eff * 3.95f;
            }
        }

        addLightNatR *= 1.0f - envFog;
        addLightNatG *= 1.0f - envFog;
        addLightNatB *= 1.0f - envFog;

        *r = static_cast<uint8_t>(std::min(
            255.0f,
            std::max(
                0.0f, (-overExpose + static_cast<float>(*r) * reduceColourNat * natLightR + envFog * fogR + addLightNatR))));
        *g = static_cast<uint8_t>(std::min(
            255.0f,
            std::max(
                0.0f, (-overExpose + static_cast<float>(*g) * reduceColourNat * natLightG + envFog * fogG + addLightNatG))));
        *b = static_cast<uint8_t>(std::min(
            255.0f,
            std::max(
                0.0f, (-overExpose + static_cast<float>(*b) * reduceColourNat * natLightB + envFog * fogB + addLightNatB))));

        auto dstEntry = &gPalette_light[i];
        dstEntry->Red = static_cast<uint8_t>(
            std::min<float>(0xFF, (static_cast<float>(*r) * reduceColourLit * boost + lightFog) * elecMultR));
        dstEntry->Green = static_cast<uint8_t>(
            std::min<float>(0xFF, (static_cast<float>(*g) * reduceColourLit * boost + lightFog) * elecMultG));
        dstEntry->Blue = static_cast<uint8_t>(
            std::min<float>(0xFF, (static_cast<float>(*b) * reduceColourLit * boost + lightFog) * elecMultB));
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
    void* dstPixels, uint32_t dstPitch, uint8_t* bits, uint32_t width, uint32_t height, const uint32_t* palette,
    const uint32_t* lightPalette)
{
    lightfx_update_viewport_settings();
    lightfx_swap_buffers();
    lightfx_prepare_light_list();
    lightfx_render_lights_to_frontbuffer();

    uint8_t* lightBits = static_cast<uint8_t*>(lightfx_get_front_buffer());
    if (lightBits == nullptr)
    {
        return;
    }

    for (uint32_t y = 0; y < height; y++)
    {
        uintptr_t dstOffset = static_cast<uintptr_t>(y * dstPitch);
        uint32_t* dst = reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(dstPixels) + dstOffset);
        for (uint32_t x = 0; x < width; x++)
        {
            uint8_t* src = &bits[y * width + x];
            uint32_t darkColour = palette[*src];
            uint32_t lightColour = lightPalette[*src];
            uint8_t lightIntensity = lightBits[y * width + x];

            uint32_t colour = 0;
            if (lightIntensity == 0)
            {
                colour = darkColour;
            }
            else
            {
                colour |= mix_light((darkColour >> 0) & 0xFF, (lightColour >> 0) & 0xFF, lightIntensity);
                colour |= mix_light((darkColour >> 8) & 0xFF, (lightColour >> 8) & 0xFF, lightIntensity) << 8;
                colour |= mix_light((darkColour >> 16) & 0xFF, (lightColour >> 16) & 0xFF, lightIntensity) << 16;
                colour |= mix_light((darkColour >> 24) & 0xFF, (lightColour >> 24) & 0xFF, lightIntensity) << 24;
            }
            *dst++ = colour;
        }
    }
}
