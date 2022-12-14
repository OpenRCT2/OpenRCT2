/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "EntityBase.h"

class DataSerialiser;
struct CoordsXYZ;
struct PaintSession;
struct VehicleColour;

struct VehicleCrashParticle : EntityBase
{
    static constexpr auto cEntityType = EntityType::CrashedVehicleParticle;
    uint16_t frame;
    uint16_t time_to_live;
    uint8_t colour[2];
    uint16_t crashed_sprite_base;
    int16_t velocity_x;
    int16_t velocity_y;
    int16_t velocity_z;
    int32_t acceleration_x;
    int32_t acceleration_y;
    int32_t acceleration_z;
    static void Create(VehicleColour& colours, const CoordsXYZ& vehiclePos);
    void Update();
    void Serialise(DataSerialiser& stream);
    void Paint(PaintSession& session, int32_t imageDirection) const;
};

struct CrashSplashParticle : EntityBase
{
    static constexpr auto cEntityType = EntityType::CrashSplash;
    uint16_t frame;
    static void Create(const CoordsXYZ& splashPos);
    void Update();
    void Serialise(DataSerialiser& stream);
    void Paint(PaintSession& session, int32_t imageDirection) const;
};

struct ExplosionFlare : EntityBase
{
    static constexpr auto cEntityType = EntityType::ExplosionFlare;
    uint16_t frame;
    static void Create(const CoordsXYZ& flarePos);
    void Update();
    void Serialise(DataSerialiser& stream);
    void Paint(PaintSession& session, int32_t imageDirection) const;
};

struct ExplosionCloud : EntityBase
{
    static constexpr auto cEntityType = EntityType::ExplosionCloud;
    uint16_t frame;
    static void Create(const CoordsXYZ& cloudPos);
    void Update();
    void Serialise(DataSerialiser& stream);
    void Paint(PaintSession& session, int32_t imageDirection) const;
};

struct SteamParticle : EntityBase
{
    static constexpr auto cEntityType = EntityType::SteamParticle;
    uint16_t frame;
    uint16_t time_to_move;

    static void Create(const CoordsXYZ& coords);
    void Update();
    void Serialise(DataSerialiser& stream);
    void Paint(PaintSession& session, int32_t imageDirection) const;
};
