/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#include "Particle.h"

#include "../audio/audio.h"
#include "../paint/sprite/Paint.Sprite.h"
#include "../scenario/Scenario.h"
#include "Sprite.h"

#include <iterator>

template<> bool SpriteBase::Is<VehicleCrashParticle>() const
{
    return Type == EntityType::CrashedVehicleParticle;
}

template<> bool SpriteBase::Is<CrashSplashParticle>() const
{
    return Type == EntityType::CrashSplash;
}
/**
 *
 *  rct2: 0x006735A1
 */
void VehicleCrashParticle::Create(rct_vehicle_colour colours, const CoordsXYZ& vehiclePos)
{
    VehicleCrashParticle* sprite = CreateEntity<VehicleCrashParticle>();
    if (sprite != nullptr)
    {
        sprite->colour[0] = colours.body_colour;
        sprite->colour[1] = colours.trim_colour;
        sprite->sprite_width = 8;
        sprite->sprite_height_negative = 8;
        sprite->sprite_height_positive = 8;
        sprite->MoveTo(vehiclePos);

        sprite->frame = (scenario_rand() & 0xFF) * 12;
        sprite->time_to_live = (scenario_rand() & 0x7F) + 140;
        sprite->crashed_sprite_base = scenario_rand_max(static_cast<uint32_t>(std::size(vehicle_particle_base_sprites)));
        sprite->acceleration_x = (static_cast<int16_t>(scenario_rand() & 0xFFFF)) * 4;
        sprite->acceleration_y = (static_cast<int16_t>(scenario_rand() & 0xFFFF)) * 4;
        sprite->acceleration_z = (scenario_rand() & 0xFFFF) * 4 + 0x10000;
        sprite->velocity_x = 0;
        sprite->velocity_y = 0;
        sprite->velocity_z = 0;
    }
}

/**
 *
 *  rct2: 0x00673298
 */
void VehicleCrashParticle::Update()
{
    Invalidate();
    time_to_live--;
    if (time_to_live == 0)
    {
        sprite_remove(this);
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
    int16_t landZ = tile_element_height(newLoc);
    int16_t waterZ = tile_element_water_height(newLoc);

    if (waterZ != 0 && z >= waterZ && newLoc.z <= waterZ)
    {
        // Splash
        OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::Water2, { x, y, waterZ });
        CrashSplashParticle::Create({ x, y, waterZ });
        sprite_remove(this);
        return;
    }

    if (z >= landZ && newLoc.z <= landZ)
    {
        // Bounce
        acceleration_z *= -1;
        newLoc.z = landZ;
    }
    MoveTo(newLoc);

    frame += 85;
    if (frame >= 3072)
    {
        frame = 0;
    }
}

/**
 *
 *  rct2: 0x00673699
 */
void CrashSplashParticle::Create(const CoordsXYZ& splashPos)
{
    auto* sprite = CreateEntity<CrashSplashParticle>();
    if (sprite != nullptr)
    {
        sprite->sprite_width = 33;
        sprite->sprite_height_negative = 51;
        sprite->sprite_height_positive = 16;
        sprite->MoveTo(splashPos + CoordsXYZ{ 0, 0, 3 });
        sprite->frame = 0;
    }
}

/**
 *
 *  rct2: 0x0067339D
 */
void CrashSplashParticle::Update()
{
    Invalidate();
    frame += 85;
    if (frame >= 7168)
    {
        sprite_remove(this);
    }
}

/**
 *
 *  rct2: 0x006734B2
 */
void SteamParticle::Create(const CoordsXYZ& coords)
{
    auto surfaceElement = map_get_surface_element_at(coords);
    if (surfaceElement != nullptr && coords.z > surfaceElement->GetBaseZ())
    {
        SteamParticle* steam = CreateEntity<SteamParticle>();
        if (steam == nullptr)
            return;

        steam->sprite_width = 20;
        steam->sprite_height_negative = 18;
        steam->sprite_height_positive = 16;
        steam->frame = 256;
        steam->time_to_move = 0;
        steam->MoveTo(coords);
    }
}

/**
 *
 *  rct2: 0x00673200
 */
void SteamParticle::Update()
{
    // Move up 1 z every 3 ticks (Starts after 4 ticks)
    Invalidate();
    time_to_move++;
    if (time_to_move >= 4)
    {
        time_to_move = 1;
        MoveTo({ x, y, z + 1 });
    }
    frame += 64;
    if (frame >= (56 * 64))
    {
        sprite_remove(this);
    }
}

/**
 *
 *  rct2: 0x0067363D
 */
void ExplosionCloud::Create(const CoordsXYZ& cloudPos)
{
    auto* entity = CreateEntity<ExplosionCloud>();
    if (entity != nullptr)
    {
        entity->sprite_width = 44;
        entity->sprite_height_negative = 32;
        entity->sprite_height_positive = 34;
        entity->MoveTo(cloudPos + CoordsXYZ{ 0, 0, 4 });
        entity->frame = 0;
    }
}

/**
 *
 *  rct2: 0x00673385
 */
void ExplosionCloud::Update()
{
    Invalidate();
    frame += 128;
    if (frame >= (36 * 128))
    {
        sprite_remove(this);
    }
}

/**
 *
 *  rct2: 0x0067366B
 */
void ExplosionFlare::Create(const CoordsXYZ& flarePos)
{
    auto* entity = CreateEntity<ExplosionFlare>();
    if (entity != nullptr)
    {
        entity->sprite_width = 25;
        entity->sprite_height_negative = 85;
        entity->sprite_height_positive = 8;
        entity->MoveTo(flarePos + CoordsXYZ{ 0, 0, 4 });
        entity->frame = 0;
    }
}

/**
 *
 *  rct2: 0x006733B4
 */
void ExplosionFlare::Update()
{
    Invalidate();
    frame += 64;
    if (frame >= (124 * 64))
    {
        sprite_remove(this);
    }
}
