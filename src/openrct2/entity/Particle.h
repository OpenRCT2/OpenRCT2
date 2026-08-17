/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "EntityBase.h"

struct CoordsXYZ;
struct PaintSession;
struct VehicleColour;

namespace OpenRCT2::Drawing
{
    enum class Colour : uint8_t;
}

namespace OpenRCT2
{
    class DataSerialiser;

    constexpr int32_t kCrashedVehicleParticleFrameToSprite = 256;
    constexpr int32_t kCrashedVehicleParticleNumberSprites = 12;
    constexpr int32_t kCrashedVehicleParticleNumberTypes = 5;
    constexpr int32_t kCrashedVehicleParticleFrameIncrement = 85; // 1/3 of 256, rounded up

    struct VehicleCrashParticle : EntityBase
    {
        static constexpr auto kEntityType = EntityType::crashedVehicleParticle;

        uint16_t frame;
        uint16_t timeToLive;
        Drawing::Colour colour[2];
        uint16_t crashedSpriteBase;
        int16_t velocityX;
        int16_t velocityY;
        int16_t velocityZ;
        int32_t accelerationX;
        int32_t accelerationY;
        int32_t accelerationZ;

        static void create(VehicleColour& colours, const CoordsXYZ& vehiclePos);
        void setSpriteData();
        void launch();
        void update();
        void serialise(DataSerialiser& stream);
    };

    struct CrashSplashParticle : EntityBase
    {
        static constexpr auto kEntityType = EntityType::crashSplash;

        uint16_t frame;

        static void create(const CoordsXYZ& splashPos);
        void update();
        void serialise(DataSerialiser& stream);
    };

    struct ExplosionFlare : EntityBase
    {
        static constexpr auto kEntityType = EntityType::explosionFlare;

        uint16_t frame;

        static void create(const CoordsXYZ& flarePos);
        void update();
        void serialise(DataSerialiser& stream);
    };

    struct ExplosionCloud : EntityBase
    {
        static constexpr auto kEntityType = EntityType::explosionCloud;

        uint16_t frame;

        static void create(const CoordsXYZ& cloudPos);
        void update();
        void serialise(DataSerialiser& stream);
    };

    struct SteamParticle : EntityBase
    {
        static constexpr auto kEntityType = EntityType::steamParticle;

        uint16_t frame;
        uint16_t timeToMove;

        static void create(const CoordsXYZ& coords);
        void update();
        void serialise(DataSerialiser& stream);
    };
} // namespace OpenRCT2
