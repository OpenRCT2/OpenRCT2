/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#include "Particle.h"

#include "../GameState.h"
#include "../SpriteIds.h"
#include "../audio/Audio.h"
#include "../core/DataSerialiser.h"
#include "../profiling/Profiling.h"
#include "../ride/VehicleColour.h"
#include "../scenario/Scenario.h"
#include "../world/Map.h"
#include "../world/tile_element/SurfaceElement.h"
#include "EntityRegistry.h"

#include <iterator>

namespace OpenRCT2
{
    template<>
    bool EntityBase::is<SteamParticle>() const
    {
        return type == EntityType::steamParticle;
    }

    template<>
    bool EntityBase::is<ExplosionFlare>() const
    {
        return type == EntityType::explosionFlare;
    }

    template<>
    bool EntityBase::is<ExplosionCloud>() const
    {
        return type == EntityType::explosionCloud;
    }

    template<>
    bool EntityBase::is<VehicleCrashParticle>() const
    {
        return type == EntityType::crashedVehicleParticle;
    }

    template<>
    bool EntityBase::is<CrashSplashParticle>() const
    {
        return type == EntityType::crashSplash;
    }

    void VehicleCrashParticle::SetSpriteData()
    {
        spriteData.width = 8;
        spriteData.heightMin = 8;
        spriteData.heightMax = 8;
    }

    void VehicleCrashParticle::Launch()
    {
        frame = (ScenarioRand() & 0xFF) * kCrashedVehicleParticleNumberSprites;
        time_to_live = (ScenarioRand() & 0x7F) + 140;
        crashed_sprite_base = ScenarioRandMax(kCrashedVehicleParticleNumberTypes);
        acceleration_x = (static_cast<int16_t>(ScenarioRand() & 0xFFFF)) * 4;
        acceleration_y = (static_cast<int16_t>(ScenarioRand() & 0xFFFF)) * 4;
        acceleration_z = (ScenarioRand() & 0xFFFF) * 4 + 0x10000;
        velocity_x = 0;
        velocity_y = 0;
        velocity_z = 0;
    }

    /**
     *
     *  rct2: 0x006735A1
     */
    void VehicleCrashParticle::Create(VehicleColour& colours, const CoordsXYZ& vehiclePos)
    {
        VehicleCrashParticle* sprite = getGameState().entities.CreateEntity<VehicleCrashParticle>();
        if (sprite != nullptr)
        {
            sprite->moveTo(vehiclePos);
            sprite->colour[0] = colours.Body;
            sprite->colour[1] = colours.Trim;
            sprite->SetSpriteData();
            sprite->Launch();
        }
    }

    /**
     *
     *  rct2: 0x00673298
     */
    void VehicleCrashParticle::Update()
    {
        invalidate();
        time_to_live--;
        if (time_to_live == 0)
        {
            getGameState().entities.EntityRemove(this);
            return;
        }

        // Apply gravity
        acceleration_z -= 5041;

        // Apply air resistance
        acceleration_x -= (acceleration_x / 256);
        acceleration_y -= (acceleration_y / 256);
        acceleration_z -= (acceleration_z / 256);

        // Update velocity and position
        int32_t vx = velocity_x + acceleration_x;
        int32_t vy = velocity_y + acceleration_y;
        int32_t vz = velocity_z + acceleration_z;

        CoordsXYZ newLoc = { x + (vx >> 16), y + (vy >> 16), z + (vz >> 16) };

        velocity_x = vx & 0xFFFF;
        velocity_y = vy & 0xFFFF;
        velocity_z = vz & 0xFFFF;

        // Check collision with land / water
        int16_t landZ = TileElementHeight(newLoc);
        int16_t waterZ = TileElementWaterHeight(newLoc);

        if (waterZ != 0 && z >= waterZ && newLoc.z <= waterZ)
        {
            // Splash
            Audio::Play3D(Audio::SoundId::water2, { x, y, waterZ });
            CrashSplashParticle::Create({ x, y, waterZ });
            getGameState().entities.EntityRemove(this);
            return;
        }

        if (z >= landZ && newLoc.z <= landZ)
        {
            // Bounce
            acceleration_z *= -1;
            newLoc.z = landZ;
        }
        moveTo(newLoc);

        frame += kCrashedVehicleParticleFrameIncrement;
        if (frame >= (kCrashedVehicleParticleNumberSprites * kCrashedVehicleParticleFrameToSprite))
        {
            frame = 0;
        }
    }

    void VehicleCrashParticle::Serialise(DataSerialiser& stream)
    {
        EntityBase::serialise(stream);
        stream << frame;
        stream << time_to_live;
        stream << colour;
        stream << crashed_sprite_base;
        stream << velocity_x;
        stream << velocity_y;
        stream << velocity_z;
        stream << acceleration_x;
        stream << acceleration_y;
        stream << acceleration_z;
    }

    /**
     *
     *  rct2: 0x00673699
     */
    void CrashSplashParticle::Create(const CoordsXYZ& splashPos)
    {
        auto* sprite = getGameState().entities.CreateEntity<CrashSplashParticle>();
        if (sprite != nullptr)
        {
            sprite->spriteData.width = 33;
            sprite->spriteData.heightMin = 51;
            sprite->spriteData.heightMax = 16;
            sprite->moveTo(splashPos + CoordsXYZ{ 0, 0, 3 });
            sprite->frame = 0;
        }
    }

    /**
     *
     *  rct2: 0x0067339D
     */
    void CrashSplashParticle::Update()
    {
        invalidate();
        frame += 85;
        if (frame >= 7168)
        {
            getGameState().entities.EntityRemove(this);
        }
    }

    void CrashSplashParticle::Serialise(DataSerialiser& stream)
    {
        EntityBase::serialise(stream);
        stream << frame;
    }

    /**
     *
     *  rct2: 0x006734B2
     */
    void SteamParticle::Create(const CoordsXYZ& coords)
    {
        auto surfaceElement = MapGetSurfaceElementAt(coords);
        if (surfaceElement != nullptr && coords.z > surfaceElement->getBaseZ())
        {
            SteamParticle* steam = getGameState().entities.CreateEntity<SteamParticle>();
            if (steam == nullptr)
                return;

            steam->spriteData.width = 20;
            steam->spriteData.heightMin = 18;
            steam->spriteData.heightMax = 16;
            steam->frame = 256;
            steam->time_to_move = 0;
            steam->moveTo(coords);
        }
    }

    /**
     *
     *  rct2: 0x00673200
     */
    void SteamParticle::Update()
    {
        // Move up 1 z every 3 ticks (Starts after 4 ticks)
        invalidate();
        time_to_move++;
        if (time_to_move >= 4)
        {
            time_to_move = 1;
            moveTo({ x, y, z + 1 });
        }
        frame += 64;
        if (frame >= (56 * 64))
        {
            getGameState().entities.EntityRemove(this);
        }
    }

    void SteamParticle::Serialise(DataSerialiser& stream)
    {
        EntityBase::serialise(stream);
        stream << frame;
        stream << time_to_move;
    }

    /**
     *
     *  rct2: 0x0067363D
     */
    void ExplosionCloud::Create(const CoordsXYZ& cloudPos)
    {
        auto* entity = getGameState().entities.CreateEntity<ExplosionCloud>();
        if (entity != nullptr)
        {
            entity->spriteData.width = 44;
            entity->spriteData.heightMin = 32;
            entity->spriteData.heightMax = 34;
            entity->moveTo(cloudPos + CoordsXYZ{ 0, 0, 4 });
            entity->frame = 0;
        }
    }

    /**
     *
     *  rct2: 0x00673385
     */
    void ExplosionCloud::Update()
    {
        invalidate();
        frame += 128;
        if (frame >= (36 * 128))
        {
            getGameState().entities.EntityRemove(this);
        }
    }

    void ExplosionCloud::Serialise(DataSerialiser& stream)
    {
        EntityBase::serialise(stream);
        stream << frame;
    }

    /**
     *
     *  rct2: 0x0067366B
     */
    void ExplosionFlare::Create(const CoordsXYZ& flarePos)
    {
        auto* entity = getGameState().entities.CreateEntity<ExplosionFlare>();
        if (entity != nullptr)
        {
            entity->spriteData.width = 25;
            entity->spriteData.heightMin = 85;
            entity->spriteData.heightMax = 8;
            entity->moveTo(flarePos + CoordsXYZ{ 0, 0, 4 });
            entity->frame = 0;
        }
    }

    /**
     *
     *  rct2: 0x006733B4
     */
    void ExplosionFlare::Update()
    {
        invalidate();
        frame += 64;
        if (frame >= (124 * 64))
        {
            getGameState().entities.EntityRemove(this);
        }
    }

    void ExplosionFlare::Serialise(DataSerialiser& stream)
    {
        EntityBase::serialise(stream);
        stream << frame;
    }
} // namespace OpenRCT2
