/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../ride/VehicleColour.h"
#include "SpriteBase.h"

class DataSerialiser;
struct CoordsXYZ;

struct VehicleCrashParticle : MiscEntity
{
    static constexpr auto cEntityType = EntityType::CrashedVehicleParticle;
    uint16_t time_to_live;
    uint8_t colour[2];
    uint16_t crashed_sprite_base;
    int16_t velocity_x;
    int16_t velocity_y;
    int16_t velocity_z;
    int32_t acceleration_x;
    int32_t acceleration_y;
    int32_t acceleration_z;

    void Update();
    void Serialise(DataSerialiser& stream);
};

struct CrashSplashParticle : MiscEntity
{
    static constexpr auto cEntityType = EntityType::CrashSplash;
    void Update();
    void Serialise(DataSerialiser& stream);
};

void crashed_vehicle_particle_create(rct_vehicle_colour colours, const CoordsXYZ& vehiclePos);
void crash_splash_create(const CoordsXYZ& splashPos);
