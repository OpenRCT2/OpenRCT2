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
#include "../audio/Audio.h"
#include "../core/DataSerialiser.h"
#include "../ride/VehicleColour.h"
#include "../scenario/Scenario.h"
#include "../world/Map.h"
#include "../world/tile_element/SurfaceElement.h"
#include "EntityRegistry.h"

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

    void VehicleCrashParticle::setSpriteData()
    {
        spriteData.width = 8;
        spriteData.heightMin = 8;
        spriteData.heightMax = 8;
    }

    void VehicleCrashParticle::launch()
    {
        frame = (ScenarioRand() & 0xFF) * kCrashedVehicleParticleNumberSprites;
        timeToLive = (ScenarioRand() & 0x7F) + 140;
        crashedSpriteBase = ScenarioRandMax(kCrashedVehicleParticleNumberTypes);
        accelerationX = (static_cast<int16_t>(ScenarioRand() & 0xFFFF)) * 4;
        accelerationY = (static_cast<int16_t>(ScenarioRand() & 0xFFFF)) * 4;
        accelerationZ = (ScenarioRand() & 0xFFFF) * 4 + 0x10000;
        velocityX = 0;
        velocityY = 0;
        velocityZ = 0;
    }

    /**
     *
     *  rct2: 0x006735A1
     */
    void VehicleCrashParticle::create(VehicleColour& colours, const CoordsXYZ& vehiclePos)
    {
        VehicleCrashParticle* sprite = getGameState().entities.CreateEntity<VehicleCrashParticle>();
        if (sprite != nullptr)
        {
            sprite->moveTo(vehiclePos);
            sprite->colour[0] = colours.Body;
            sprite->colour[1] = colours.Trim;
            sprite->setSpriteData();
            sprite->launch();
        }
    }

    /**
     *
     *  rct2: 0x00673298
     */
    void VehicleCrashParticle::update()
    {
        invalidate();
        timeToLive--;
        if (timeToLive == 0)
        {
            getGameState().entities.EntityRemove(this);
            return;
        }

        // Apply gravity
        accelerationZ -= 5041;

        // Apply air resistance
        accelerationX -= (accelerationX / 256);
        accelerationY -= (accelerationY / 256);
        accelerationZ -= (accelerationZ / 256);

        // Update velocity and position
        int32_t vx = velocityX + accelerationX;
        int32_t vy = velocityY + accelerationY;
        int32_t vz = velocityZ + accelerationZ;

        CoordsXYZ newLoc = { x + (vx >> 16), y + (vy >> 16), z + (vz >> 16) };

        velocityX = vx & 0xFFFF;
        velocityY = vy & 0xFFFF;
        velocityZ = vz & 0xFFFF;

        // Check collision with land / water
        int16_t landZ = TileElementHeight(newLoc);
        int16_t waterZ = TileElementWaterHeight(newLoc);

        if (waterZ != 0 && z >= waterZ && newLoc.z <= waterZ)
        {
            // Splash
            Audio::Play3D(Audio::SoundId::water2, { x, y, waterZ });
            CrashSplashParticle::create({ x, y, waterZ });
            getGameState().entities.EntityRemove(this);
            return;
        }

        if (z >= landZ && newLoc.z <= landZ)
        {
            // Bounce
            accelerationZ *= -1;
            newLoc.z = landZ;
        }
        moveTo(newLoc);

        frame += kCrashedVehicleParticleFrameIncrement;
        if (frame >= (kCrashedVehicleParticleNumberSprites * kCrashedVehicleParticleFrameToSprite))
        {
            frame = 0;
        }
    }

    void VehicleCrashParticle::serialise(DataSerialiser& stream)
    {
        EntityBase::serialise(stream);
        stream << frame;
        stream << timeToLive;
        stream << colour;
        stream << crashedSpriteBase;
        stream << velocityX;
        stream << velocityY;
        stream << velocityZ;
        stream << accelerationX;
        stream << accelerationY;
        stream << accelerationZ;
    }

    /**
     *
     *  rct2: 0x00673699
     */
    void CrashSplashParticle::create(const CoordsXYZ& splashPos)
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
    void CrashSplashParticle::update()
    {
        invalidate();
        frame += 85;
        if (frame >= 7168)
        {
            getGameState().entities.EntityRemove(this);
        }
    }

    void CrashSplashParticle::serialise(DataSerialiser& stream)
    {
        EntityBase::serialise(stream);
        stream << frame;
    }

    /**
     *
     *  rct2: 0x006734B2
     */
    void SteamParticle::create(const CoordsXYZ& coords)
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
            steam->timeToMove = 0;
            steam->moveTo(coords);
        }
    }

    /**
     *
     *  rct2: 0x00673200
     */
    void SteamParticle::update()
    {
        // Move up 1 z every 3 ticks (Starts after 4 ticks)
        invalidate();
        timeToMove++;
        if (timeToMove >= 4)
        {
            timeToMove = 1;
            moveTo({ x, y, z + 1 });
        }
        frame += 64;
        if (frame >= (56 * 64))
        {
            getGameState().entities.EntityRemove(this);
        }
    }

    void SteamParticle::serialise(DataSerialiser& stream)
    {
        EntityBase::serialise(stream);
        stream << frame;
        stream << timeToMove;
    }

    /**
     *
     *  rct2: 0x0067363D
     */
    void ExplosionCloud::create(const CoordsXYZ& cloudPos)
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
    void ExplosionCloud::update()
    {
        invalidate();
        frame += 128;
        if (frame >= (36 * 128))
        {
            getGameState().entities.EntityRemove(this);
        }
    }

    void ExplosionCloud::serialise(DataSerialiser& stream)
    {
        EntityBase::serialise(stream);
        stream << frame;
    }

    /**
     *
     *  rct2: 0x0067366B
     */
    void ExplosionFlare::create(const CoordsXYZ& flarePos)
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
    void ExplosionFlare::update()
    {
        invalidate();
        frame += 64;
        if (frame >= (124 * 64))
        {
            getGameState().entities.EntityRemove(this);
        }
    }

    void ExplosionFlare::serialise(DataSerialiser& stream)
    {
        EntityBase::serialise(stream);
        stream << frame;
    }
} // namespace OpenRCT2
