/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

struct CoordsXY;
struct Vehicle;
struct rct_drawpixelinfo;
struct GamePalette;
struct CoordsXYZ;
struct EntityBase;

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

constexpr uint8_t GetLightTypeSize(LightType type)
{
    return static_cast<uint8_t>(type) & 0x3;
}
constexpr LightType SetLightTypeSize(LightType type, uint8_t size)
{
    return static_cast<LightType>(((static_cast<uint8_t>(type) & ~0x3) | size));
}

void LightFXSetAvailable(bool available);
bool LightFXIsAvailable();
bool LightFXForVehiclesIsAvailable();

void LightFXInit();

void LightFXUpdateBuffers(rct_drawpixelinfo*);

void LightFXPrepareLightList();
void LightFXSwapBuffers();
void LightFXRenderLightsToFrontBuffer();
void LightFXUpdateViewportSettings();

void* LightFXGetFrontBuffer();
const GamePalette& LightFXGetPalette();

void LightFXAdd3DLight(const EntityBase& entity, const uint8_t id, const CoordsXYZ& loc, const LightType lightType);

void LightFXAdd3DLightMagicFromDrawingTile(
    const CoordsXY& mapPosition, int16_t offsetX, int16_t offsetY, int16_t offsetZ, LightType lightType);

void LightFXAddLightsMagicVehicle(const Vehicle* vehicle);
void LightFxAddLightsMagicVehicle_ObservationTower(const Vehicle* vehicle);
void LightFxAddLightsMagicVehicle_MineTrainCoaster(const Vehicle* vehicle);
void LightFxAddLightsMagicVehicle_ChairLift(const Vehicle* vehicle);
void LightFxAddLightsMagicVehicle_BoatHire(const Vehicle* vehicle);
void LightFxAddLightsMagicVehicle_Monorail(const Vehicle* vehicle);
void LightFxAddLightsMagicVehicle_MiniatureRailway(const Vehicle* vehicle);

void LightFxAddKioskLights(const CoordsXY& mapPosition, const int32_t height, const uint8_t zOffset);
void LightFxAddShopLights(const CoordsXY& mapPosition, const uint8_t direction, const int32_t height, const uint8_t zOffset);

uint32_t LightFXGetLightPolution();

void LightFXApplyPaletteFilter(uint8_t i, uint8_t* r, uint8_t* g, uint8_t* b);
void LightFXRenderToTexture(
    void* dstPixels, uint32_t dstPitch, uint8_t* bits, uint32_t width, uint32_t height, const uint32_t* palette,
    const uint32_t* lightPalette);
