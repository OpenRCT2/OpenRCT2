/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LightFX.h"

#include "../Diagnostic.h"
#include "../Game.h"
#include "../GameState.h"
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
#include "../world/Map.h"
#include "../world/tile_element/TileElement.h"
#include "Drawing.h"

#include <cmath>
#include <cstring>

namespace OpenRCT2::Drawing::LightFx
{
    static uint8_t _bakedLightTexture_lantern_0[32 * 32];
    static uint8_t _bakedLightTexture_lantern_1[64 * 64];
    static uint8_t _bakedLightTexture_lantern_2[128 * 128];
    static uint8_t _bakedLightTexture_lantern_3[256 * 256];
    static uint8_t _bakedLightTexture_spot_0[32 * 32];
    static uint8_t _bakedLightTexture_spot_1[64 * 64];
    static uint8_t _bakedLightTexture_spot_2[128 * 128];
    static uint8_t _bakedLightTexture_spot_3[256 * 256];
    static DrawPixelInfo _pixelInfo;
    static bool _lightfxAvailable = false;

    static void* _light_rendered_buffer_back = nullptr;
    static void* _light_rendered_buffer_front = nullptr;

    static uint32_t _lightPolution_back = 0;
    static uint32_t _lightPolution_front = 0;

    enum class Qualifier : uint8_t
    {
        Entity,
        Map,
    };

    struct LightListEntry
    {
        CoordsXYZ position;
        ScreenCoordsXY viewCoords;
        LightType type;
        uint8_t lightIntensity;
        uint32_t lightHash;
        Qualifier qualifier;
        uint8_t lightID;
        uint8_t lightLinger;
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

    static uint8_t CalcLightIntensityLantern(int32_t x, int32_t y)
    {
        double distance = static_cast<double>(x * x + y * y);

        double light = 0.03 + std::pow(10.0 / (1.0 + distance / 100.0), 0.55);
        light *= std::min(1.0, std::max(0.0, 2.0 - std::sqrt(distance) / 64));
        light *= 0.1f;

        return static_cast<uint8_t>(std::min(255.0, light * 255.0));
    }

    static uint8_t CalcLightIntensitySpot(int32_t x, int32_t y)
    {
        double distance = static_cast<double>(x * x + y * y);

        double light = 0.3 + std::pow(10.0 / (1.0 + distance / 100.0), 0.75);
        light *= std::min(1.0, std::max(0.0, 2.0 - std::sqrt(distance) / 64));
        light *= 0.5f;

        return static_cast<uint8_t>(std::min(255.0, light * 255.0)) >> 4;
    }

    static void CalcRescaleLightHalf(uint8_t* target, uint8_t* source, uint32_t targetWidth, uint32_t targetHeight)
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

    void SetAvailable(bool available)
    {
        _lightfxAvailable = available;
    }

    bool IsAvailable()
    {
        return _lightfxAvailable && Config::Get().general.EnableLightFx;
    }

    bool ForVehiclesIsAvailable()
    {
        return IsAvailable() && Config::Get().general.EnableLightFxForVehicles;
    }

    void Init()
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
                *parcer = CalcLightIntensityLantern(x - 128, y - 128);
                parcer++;
            }
        }

        parcer = _bakedLightTexture_spot_3;

        for (int32_t y = 0; y < 256; y++)
        {
            for (int32_t x = 0; x < 256; x++)
            {
                *parcer = CalcLightIntensitySpot(x - 128, y - 128);
                parcer++;
            }
        }

        CalcRescaleLightHalf(_bakedLightTexture_lantern_2, _bakedLightTexture_lantern_3, 128, 128);
        CalcRescaleLightHalf(_bakedLightTexture_lantern_1, _bakedLightTexture_lantern_2, 64, 64);
        CalcRescaleLightHalf(_bakedLightTexture_lantern_0, _bakedLightTexture_lantern_1, 32, 32);

        CalcRescaleLightHalf(_bakedLightTexture_spot_2, _bakedLightTexture_spot_3, 128, 128);
        CalcRescaleLightHalf(_bakedLightTexture_spot_1, _bakedLightTexture_spot_2, 64, 64);
        CalcRescaleLightHalf(_bakedLightTexture_spot_0, _bakedLightTexture_spot_1, 32, 32);
    }

    void UpdateBuffers(DrawPixelInfo& info)
    {
        _light_rendered_buffer_front = realloc(_light_rendered_buffer_front, info.width * info.height);
        _light_rendered_buffer_back = realloc(_light_rendered_buffer_back, info.width * info.height);
        _pixelInfo = info;
    }

    void PrepareLightList()
    {
        for (uint32_t light = 0; light < LightListCurrentCountFront; light++)
        {
            LightListEntry& entry = _LightListFront[light];

            if (entry.position.z == 0x7FFF)
            {
                entry.lightIntensity = 0xFF;
                continue;
            }

            int32_t posOnScreenX = entry.viewCoords.x - _current_view_x_front;
            int32_t posOnScreenY = entry.viewCoords.y - _current_view_y_front;

            posOnScreenX = _current_view_zoom_front.ApplyInversedTo(posOnScreenX);
            posOnScreenY = _current_view_zoom_front.ApplyInversedTo(posOnScreenY);

            if ((posOnScreenX < -128) || (posOnScreenY < -128) || (posOnScreenX > _pixelInfo.width + 128)
                || (posOnScreenY > _pixelInfo.height + 128))
            {
                entry.type = LightType::None;
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

                if (entry.qualifier == Qualifier::Map)
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
                        DrawPixelInfo dpi;
                        dpi.zoom_level = _current_view_zoom_front;
                        dpi.x = _current_view_zoom_front.ApplyInversedTo(entry.viewCoords.x + offsetPattern[0 + pat * 2]);
                        dpi.y = _current_view_zoom_front.ApplyInversedTo(entry.viewCoords.y + offsetPattern[1 + pat * 2]);
                        dpi.height = 1;
                        dpi.width = 1;

                        PaintSession* session = PaintSessionAlloc(dpi, w->viewport->flags, w->viewport->rotation);
                        PaintSessionGenerate(*session);
                        PaintSessionArrange(*session);
                        auto info = SetInteractionInfoFromPaintSession(session, w->viewport->flags, ViewportInteractionItemAll);
                        PaintSessionFree(session);

                        //  LOG_WARNING("[%i, %i]", dpi->x, dpi->y);

                        mapCoord = info.Loc;
                        mapCoord.x += tileOffsetX;
                        mapCoord.y += tileOffsetY;
                        interactionType = info.interactionType;
                        tileElement = info.Element;
                    }

                    int32_t minDist = 0;
                    int32_t baseHeight = (-999) * kCoordsZStep;

                    if (interactionType != ViewportInteractionItem::Entity && tileElement != nullptr)
                    {
                        baseHeight = tileElement->GetBaseZ();
                    }

                    minDist = (baseHeight - entry.position.z) / 2;

                    int32_t deltaX = mapCoord.x - entry.position.x;
                    int32_t deltaY = mapCoord.y - entry.position.y;

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

                    //  LOG_WARNING("light %i [%i, %i, %i], [%i, %i] minDist to %i: %i; projdot: %i", light, coord_3d.x,
                    //  coord_3d.y, coord_3d.z, mapCoord.x, mapCoord.y, baseHeight, minDist, projDot);

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
                    entry.type = LightType::None;
                    continue;
                }

                entry.lightIntensity = static_cast<uint8_t>(
                    std::min<uint32_t>(0xFF, (entry.lightIntensity * lightIntensityOccluded) / (totalSamplePoints * 100)));
            }

            if (_current_view_zoom_front > ZoomLevel{ 0 })
            {
                const int8_t zoomNumber = static_cast<int8_t>(_current_view_zoom_front);
                entry.lightIntensity -= 5 * zoomNumber;
                if (GetLightTypeSize(entry.type) < zoomNumber)
                {
                    entry.type = LightType::None;
                    continue;
                }

                entry.type = SetLightTypeSize(entry.type, GetLightTypeSize(entry.type) - zoomNumber);
            }
        }
    }

    void SwapBuffers()
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

    void UpdateViewportSettings()
    {
        WindowBase* mainWindow = WindowGetMain();
        if (mainWindow != nullptr)
        {
            Viewport* viewport = WindowGetViewport(mainWindow);
            _current_view_x_back = viewport->viewPos.x;
            _current_view_y_back = viewport->viewPos.y;
            _current_view_rotation_back = viewport->rotation;
            _current_view_zoom_back = viewport->zoom;
        }
    }

    void RenderLightsToFrontBuffer()
    {
        if (_light_rendered_buffer_front == nullptr)
        {
            return;
        }

        std::memset(_light_rendered_buffer_front, 0, _pixelInfo.width * _pixelInfo.height);

        _lightPolution_back = 0;

        //  LOG_WARNING("%i lights", LightListCurrentCountFront);

        for (uint32_t light = 0; light < LightListCurrentCountFront; light++)
        {
            const uint8_t* bufReadBase = nullptr;
            uint8_t* bufWriteBase = static_cast<uint8_t*>(_light_rendered_buffer_front);
            uint32_t bufReadWidth, bufReadHeight;
            int32_t bufWriteX, bufWriteY;
            int32_t bufWriteWidth, bufWriteHeight;
            uint32_t bufReadSkip, bufWriteSkip;

            LightListEntry& entry = _LightListFront[light];

            int32_t inRectCentreX = entry.viewCoords.x;
            int32_t inRectCentreY = entry.viewCoords.y;

            if (entry.position.z != 0x7FFF)
            {
                inRectCentreX -= _current_view_x_front;
                inRectCentreY -= _current_view_y_front;
                inRectCentreX = _current_view_zoom_front.ApplyInversedTo(inRectCentreX);
                inRectCentreY = _current_view_zoom_front.ApplyInversedTo(inRectCentreY);
            }

            switch (entry.type)
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

            if (entry.lightIntensity == 0xFF)
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
                        *bufWriteBase = std::min(0xFF, *bufWriteBase + (((*bufReadBase) * (1 + entry.lightIntensity)) >> 8));
                        bufWriteBase++;
                        bufReadBase++;
                    }

                    bufWriteBase += bufWriteSkip;
                    bufReadBase += bufReadSkip;
                }
            }
        }
    }

    void* GetFrontBuffer()
    {
        return _light_rendered_buffer_front;
    }

    const GamePalette& GetPalette()
    {
        return gPalette_light;
    }

    static void Add3DLight(
        const uint32_t lightHash, const Qualifier qualifier, const uint8_t id, const CoordsXYZ& loc, const LightType lightType)
    {
        if (LightListCurrentCountBack == 15999)
        {
            return;
        }

        //  LOG_WARNING("%i lights in back", LightListCurrentCountBack);

        for (uint32_t i = 0; i < LightListCurrentCountBack; i++)
        {
            LightListEntry& entry = _LightListBack[i];
            if (entry.lightHash != lightHash)
                continue;
            if (entry.qualifier != qualifier)
                continue;
            if (entry.lightID != id)
                continue;

            entry.position = loc;
            entry.viewCoords = Translate3DTo2DWithZ(GetCurrentRotation(), loc);
            entry.type = lightType;
            entry.lightIntensity = 0xFF;
            entry.lightHash = lightHash;
            entry.qualifier = qualifier;
            entry.lightID = id;
            entry.lightLinger = 1;

            return;
        }

        LightListEntry* entry = &_LightListBack[LightListCurrentCountBack++];

        entry->position = loc;
        entry->viewCoords = Translate3DTo2DWithZ(GetCurrentRotation(), loc);
        entry->type = lightType;
        entry->lightIntensity = 0xFF;
        entry->lightHash = lightHash;
        entry->qualifier = qualifier;
        entry->lightID = id;
        entry->lightLinger = 1;

        //  LOG_WARNING("new 3d light");
    }

    static void Add3DLight(const CoordsXYZ& loc, const LightType lightType)
    {
        Add3DLight(((loc.x << 16) | loc.y), Qualifier::Map, loc.z, loc, lightType);
    }

    void Add3DLight(const EntityBase& entity, const uint8_t id, const CoordsXYZ& loc, const LightType lightType)
    {
        Add3DLight(entity.Id.ToUnderlying(), Qualifier::Entity, id, loc, lightType);
    }

    void Add3DLightMagicFromDrawingTile(
        const CoordsXY& mapPosition, int16_t offsetX, int16_t offsetY, int16_t offsetZ, LightType lightType)
    {
        int16_t x = mapPosition.x + offsetX + 16;
        int16_t y = mapPosition.y + offsetY + 16;

        Add3DLight({ x, y, offsetZ }, lightType);
    }

    uint32_t GetLightPolution()
    {
        return _lightPolution_front;
    }

    static constexpr int16_t offsetLookup[] = {
        10, 10, 9, 8, 7, 6, 4, 2, 0, -2, -4, -6, -7, -8, -9, -10, -10, -10, -9, -8, -7, -6, -4, -2, 0, 2, 4, 6, 7, 8, 9, 10,
    };
    void AddLightsMagicVehicle_ObservationTower(const Vehicle* vehicle)
    {
        Add3DLight(*vehicle, 0, { vehicle->x, vehicle->y + 16, vehicle->z }, LightType::Spot3);
        Add3DLight(*vehicle, 1, { vehicle->x + 16, vehicle->y, vehicle->z }, LightType::Spot3);
        Add3DLight(*vehicle, 2, { vehicle->x - 16, vehicle->y, vehicle->z }, LightType::Spot3);
        Add3DLight(*vehicle, 3, { vehicle->x, vehicle->y - 16, vehicle->z }, LightType::Spot3);
    }

    void AddLightsMagicVehicle_MineTrainCoaster(const Vehicle* vehicle)
    {
        if (vehicle == vehicle->TrainHead())
        {
            int16_t place_x = vehicle->x - offsetLookup[(vehicle->Orientation + 0) % 32] * 2;
            int16_t place_y = vehicle->y - offsetLookup[(vehicle->Orientation + 8) % 32] * 2;
            Add3DLight(*vehicle, 0, { place_x, place_y, vehicle->z }, LightType::Spot3);
        }
    }

    void AddLightsMagicVehicle_ChairLift(const Vehicle* vehicle)
    {
        Add3DLight(*vehicle, 0, { vehicle->x, vehicle->y, vehicle->z - 16 }, LightType::Lantern2);
    }
    void AddLightsMagicVehicle_BoatHire(const Vehicle* vehicle)
    {
        Vehicle* vehicle_draw = vehicle->TrainHead();
        auto* nextVeh = GetEntity<Vehicle>(vehicle_draw->next_vehicle_on_train);
        if (nextVeh != nullptr)
        {
            vehicle_draw = nextVeh;
        }
        int16_t place_x = vehicle_draw->x;
        int16_t place_y = vehicle_draw->y;
        place_x -= offsetLookup[(vehicle_draw->Orientation + 0) % 32];
        place_y -= offsetLookup[(vehicle_draw->Orientation + 8) % 32];
        Add3DLight(*vehicle, 0, { place_x, place_y, vehicle_draw->z }, LightType::Spot2);
        place_x -= offsetLookup[(vehicle_draw->Orientation + 0) % 32];
        place_y -= offsetLookup[(vehicle_draw->Orientation + 8) % 32];
        Add3DLight(*vehicle, 1, { place_x, place_y, vehicle_draw->z }, LightType::Spot2);
    }
    void AddLightsMagicVehicle_Monorail(const Vehicle* vehicle)
    {
        Add3DLight(*vehicle, 0, { vehicle->x, vehicle->y, vehicle->z + 12 }, LightType::Spot2);
        int16_t place_x = vehicle->x;
        int16_t place_y = vehicle->y;
        if (vehicle == vehicle->TrainHead())
        {
            place_x -= offsetLookup[(vehicle->Orientation + 0) % 32] * 2;
            place_y -= offsetLookup[(vehicle->Orientation + 8) % 32] * 2;
            Add3DLight(*vehicle, 1, { place_x, place_y, vehicle->z + 10 }, LightType::Lantern3);
            place_x -= offsetLookup[(vehicle->Orientation + 0) % 32] * 3;
            place_y -= offsetLookup[(vehicle->Orientation + 8) % 32] * 3;
            Add3DLight(*vehicle, 2, { place_x, place_y, vehicle->z + 2 }, LightType::Lantern3);
        }
        if (vehicle == vehicle->TrainTail())
        {
            place_x += offsetLookup[(vehicle->Orientation + 0) % 32] * 2;
            place_y += offsetLookup[(vehicle->Orientation + 8) % 32] * 2;
            Add3DLight(*vehicle, 3, { place_x, place_y, vehicle->z + 10 }, LightType::Lantern3);
            place_x += offsetLookup[(vehicle->Orientation + 0) % 32] * 2;
            place_y += offsetLookup[(vehicle->Orientation + 8) % 32] * 2;
            Add3DLight(*vehicle, 4, { place_x, place_y, vehicle->z + 2 }, LightType::Lantern3);
        }
    }
    void AddLightsMagicVehicle_MiniatureRailway(const Vehicle* vehicle)
    {
        if (vehicle == vehicle->TrainHead())
        {
            int16_t place_x = vehicle->x - offsetLookup[(vehicle->Orientation + 0) % 32] * 2;
            int16_t place_y = vehicle->y - offsetLookup[(vehicle->Orientation + 8) % 32] * 2;
            Add3DLight(*vehicle, 1, { place_x, place_y, vehicle->z + 10 }, LightType::Lantern3);
            place_x -= offsetLookup[(vehicle->Orientation + 0) % 32] * 2;
            place_y -= offsetLookup[(vehicle->Orientation + 8) % 32] * 2;
            Add3DLight(*vehicle, 2, { place_x, place_y, vehicle->z + 2 }, LightType::Lantern3);
        }
        else
        {
            Add3DLight(*vehicle, 0, { vehicle->x, vehicle->y, vehicle->z + 10 }, LightType::Lantern3);
        }
    }

    void AddLightsMagicVehicle(const Vehicle* vehicle)
    {
        auto ride = vehicle->GetRide();
        if (ride == nullptr)
            return;

        const auto& rtd = GetRideTypeDescriptor(ride->type);
        if (rtd.LightFXAddLightsMagicVehicle != nullptr)
            rtd.LightFXAddLightsMagicVehicle(vehicle);
    }

    void AddKioskLights(const CoordsXY& mapPosition, const int32_t height, const uint8_t zOffset)
    {
        uint8_t relativeRotation = (4 - GetCurrentRotation()) % 4;
        CoordsXY lanternOffset1 = CoordsXY(0, 16).Rotate(relativeRotation);
        CoordsXY lanternOffset2 = CoordsXY(16, 0).Rotate(relativeRotation);
        Add3DLightMagicFromDrawingTile(mapPosition, lanternOffset1.x, lanternOffset1.y, height + zOffset, LightType::Lantern3);
        Add3DLightMagicFromDrawingTile(mapPosition, lanternOffset2.x, lanternOffset2.y, height + zOffset, LightType::Lantern3);
        Add3DLightMagicFromDrawingTile(mapPosition, 8, 32, height, LightType::Spot1);
        Add3DLightMagicFromDrawingTile(mapPosition, 32, 8, height, LightType::Spot1);
        Add3DLightMagicFromDrawingTile(mapPosition, -32, 8, height, LightType::Spot1);
        Add3DLightMagicFromDrawingTile(mapPosition, 8, -32, height, LightType::Spot1);
        Add3DLightMagicFromDrawingTile(mapPosition, -8, 32, height, LightType::Spot1);
        Add3DLightMagicFromDrawingTile(mapPosition, 32, -8, height, LightType::Spot1);
        Add3DLightMagicFromDrawingTile(mapPosition, -32, -8, height, LightType::Spot1);
        Add3DLightMagicFromDrawingTile(mapPosition, -8, -32, height, LightType::Spot1);
    }

    void AddShopLights(const CoordsXY& mapPosition, const uint8_t direction, const int32_t height, const uint8_t zOffset)
    {
        if (direction == (4 - GetCurrentRotation()) % 4) // Back Right Facing Stall
        {
            CoordsXY spotOffset1 = CoordsXY(-32, 8).Rotate(direction);
            CoordsXY spotOffset2 = CoordsXY(-32, 4).Rotate(direction);
            Add3DLightMagicFromDrawingTile(mapPosition, spotOffset1.x, spotOffset1.y, height, LightType::Spot1);
            Add3DLightMagicFromDrawingTile(mapPosition, spotOffset2.x, spotOffset2.y, height, LightType::Spot2);
        }
        else if (direction == (7 - GetCurrentRotation()) % 4) // Back left Facing Stall
        {
            CoordsXY spotOffset1 = CoordsXY(-32, -8).Rotate(direction);
            CoordsXY spotOffset2 = CoordsXY(-32, -4).Rotate(direction);
            Add3DLightMagicFromDrawingTile(mapPosition, spotOffset1.x, spotOffset1.y, height, LightType::Spot1);
            Add3DLightMagicFromDrawingTile(mapPosition, spotOffset2.x, spotOffset2.y, height, LightType::Spot2);
        }
        else // Forward Facing Stall
        {
            CoordsXY spotOffset1 = CoordsXY(-32, 8).Rotate(direction);
            CoordsXY spotOffset2 = CoordsXY(-32, -8).Rotate(direction);
            CoordsXY lanternOffset = CoordsXY(-16, 0).Rotate(direction);
            Add3DLightMagicFromDrawingTile(
                mapPosition, lanternOffset.x, lanternOffset.y, height + zOffset, LightType::Lantern3);
            Add3DLightMagicFromDrawingTile(mapPosition, spotOffset1.x, spotOffset1.y, height, LightType::Spot1);
            Add3DLightMagicFromDrawingTile(mapPosition, spotOffset2.x, spotOffset2.y, height, LightType::Spot1);
        }
    }

    void ApplyPaletteFilter(uint8_t i, uint8_t* r, uint8_t* g, uint8_t* b)
    {
        auto& gameState = GetGameState();

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
        natLightR = FLerp(natLightR * 4.0f, 0.635f, (std::pow(night, 0.035f + sunLight * 10.50f)));
        natLightG = FLerp(natLightG * 4.0f, 0.650f, (std::pow(night, 0.100f + sunLight * 5.50f)));
        natLightB = FLerp(natLightB * 4.0f, 0.850f, (std::pow(night, 0.200f + sunLight * 1.5f)));

        float overExpose = 0.0f;
        float lightAvg = (natLightR + natLightG + natLightB) / 3.0f;
#ifdef LIGHTFX_UNKNOWN_PART_2
        float lightMax = (natLightR + natLightG + natLightB) / 3.0f;
#endif // LIGHTFX_UNKNOWN_PART_2

        //  overExpose += ((lightMax - lightAvg) / lightMax) * 0.01f;

        if (gameState.ClimateCurrent.Temperature > 20)
        {
            float offset = (static_cast<float>(gameState.ClimateCurrent.Temperature - 20)) * 0.04f;
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

        float targetFogginess = static_cast<float>(gameState.ClimateCurrent.Level) / 8.0f;
        targetFogginess += (night * night) * 0.15f;

        if (gameState.ClimateCurrent.Temperature < 10)
        {
            targetFogginess += (static_cast<float>(10 - gameState.ClimateCurrent.Temperature)) * 0.01f;
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
            * std::max(0.0f, 0.0f + 0.000001f * static_cast<float>(GetLightPolution()));
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

        reduceColourLit += static_cast<float>(gameState.ClimateCurrent.Level) / 2.0f;

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
            *r = Lerp(*r, SoftLight(*r, 8), night);
            *g = Lerp(*g, SoftLight(*g, 8), night);
            *b = Lerp(*b, SoftLight(*b, 128), night);

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
                    0.0f,
                    (-overExpose + static_cast<float>(*r) * reduceColourNat * natLightR + envFog * fogR + addLightNatR))));
            *g = static_cast<uint8_t>(std::min(
                255.0f,
                std::max(
                    0.0f,
                    (-overExpose + static_cast<float>(*g) * reduceColourNat * natLightG + envFog * fogG + addLightNatG))));
            *b = static_cast<uint8_t>(std::min(
                255.0f,
                std::max(
                    0.0f,
                    (-overExpose + static_cast<float>(*b) * reduceColourNat * natLightB + envFog * fogB + addLightNatB))));

            auto dstEntry = &gPalette_light[i];
            dstEntry->Red = static_cast<uint8_t>(
                std::min<float>(0xFF, (static_cast<float>(*r) * reduceColourLit * boost + lightFog) * elecMultR));
            dstEntry->Green = static_cast<uint8_t>(
                std::min<float>(0xFF, (static_cast<float>(*g) * reduceColourLit * boost + lightFog) * elecMultG));
            dstEntry->Blue = static_cast<uint8_t>(
                std::min<float>(0xFF, (static_cast<float>(*b) * reduceColourLit * boost + lightFog) * elecMultB));
        }
    }

    static uint8_t MixLight(uint32_t a, uint32_t b, uint32_t intensity)
    {
        intensity = intensity * 6;
        uint32_t bMul = (b * intensity) >> 8;
        uint32_t ab = a + bMul;
        uint8_t result = static_cast<uint8_t>(std::min<uint32_t>(255, ab));
        return result;
    }

    void RenderToTexture(
        void* dstPixels, uint32_t dstPitch, uint8_t* bits, uint32_t width, uint32_t height, const uint32_t* palette,
        const uint32_t* lightPalette)
    {
        UpdateViewportSettings();
        SwapBuffers();
        PrepareLightList();
        RenderLightsToFrontBuffer();

        uint8_t* lightBits = static_cast<uint8_t*>(GetFrontBuffer());
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
                    colour |= MixLight((darkColour >> 0) & 0xFF, (lightColour >> 0) & 0xFF, lightIntensity);
                    colour |= MixLight((darkColour >> 8) & 0xFF, (lightColour >> 8) & 0xFF, lightIntensity) << 8;
                    colour |= MixLight((darkColour >> 16) & 0xFF, (lightColour >> 16) & 0xFF, lightIntensity) << 16;
                    colour |= MixLight((darkColour >> 24) & 0xFF, (lightColour >> 24) & 0xFF, lightIntensity) << 24;
                }
                *dst++ = colour;
            }
        }
    }
} // namespace OpenRCT2::Drawing::LightFx
