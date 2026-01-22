/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "ColourPalette.h"

#include <cstdint>

struct CoordsXY;
struct Vehicle;
struct CoordsXYZ;
struct EntityBase;

namespace OpenRCT2
{
    struct Viewport;
}

namespace OpenRCT2::Drawing
{
    struct RenderTarget;
}

namespace OpenRCT2::Drawing::LightFx
{
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

    void SetAvailable(bool available);
    bool IsAvailable();
    bool ForVehiclesIsAvailable();

    void Init();

    void UpdateBuffers(RenderTarget&);
    const GamePalette& GetPalette();

    void Add3DLight(const EntityBase& entity, uint8_t id, const CoordsXYZ& loc, LightType lightType);

    void Add3DLightMagicFromDrawingTile(
        const CoordsXY& mapPosition, int16_t offsetX, int16_t offsetY, int16_t offsetZ, LightType lightType);

    void AddLightsMagicVehicle(const Vehicle* vehicle);
    void AddLightsMagicVehicle_ObservationTower(const Vehicle* vehicle);
    void AddLightsMagicVehicle_MineTrainCoaster(const Vehicle* vehicle);
    void AddLightsMagicVehicle_ChairLift(const Vehicle* vehicle);
    void AddLightsMagicVehicle_BoatHire(const Vehicle* vehicle);
    void AddLightsMagicVehicle_Monorail(const Vehicle* vehicle);
    void AddLightsMagicVehicle_MiniatureRailway(const Vehicle* vehicle);

    void AddKioskLights(const CoordsXY& mapPosition, int32_t height, uint8_t zOffset);
    void AddShopLights(const CoordsXY& mapPosition, uint8_t direction, int32_t height, uint8_t zOffset);

    void ApplyPaletteFilter(uint8_t i, uint8_t* r, uint8_t* g, uint8_t* b);
    void RenderToTexture(
        const Viewport& vp, void* dstPixels, uint32_t dstPitch, uint8_t* bits, uint32_t width, uint32_t height,
        const uint32_t* palette, const uint32_t* lightPalette);

} // namespace OpenRCT2::Drawing::LightFx
