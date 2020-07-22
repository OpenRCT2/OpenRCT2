/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../audio/audio.h"
#include "../paint/sprite/Paint.Sprite.h"
#include "../scenario/Scenario.h"
#include "Sprite.h"

#include <iterator>

template<> bool SpriteBase::Is<VehicleCrashParticle>() const
{
    return sprite_identifier == SPRITE_IDENTIFIER_MISC && type == SPRITE_MISC_CRASHED_VEHICLE_PARTICLE;
}

template<> bool SpriteBase::Is<CrashSplashParticle>() const
{
    return sprite_identifier == SPRITE_IDENTIFIER_MISC && type == SPRITE_MISC_CRASH_SPLASH;
}
/**
 *
 *  rct2: 0x006735A1
 */
void crashed_vehicle_particle_create(rct_vehicle_colour colours, const CoordsXYZ& vehiclePos)
{
    VehicleCrashParticle* sprite = &create_sprite(SPRITE_IDENTIFIER_MISC)->crashed_vehicle_particle;
    if (sprite != nullptr)
    {
        sprite->colour[0] = colours.body_colour;
        sprite->colour[1] = colours.trim_colour;
        sprite->sprite_width = 8;
        sprite->sprite_height_negative = 8;
        sprite->sprite_height_positive = 8;
        sprite->sprite_identifier = SPRITE_IDENTIFIER_MISC;
        sprite->MoveTo(vehiclePos);
        sprite->type = SPRITE_MISC_CRASHED_VEHICLE_PARTICLE;

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
    Invalidate0();
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
        audio_play_sound_at_location(SoundId::Water2, { x, y, waterZ });
        crash_splash_create({ x, y, waterZ });
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
    Invalidate0();

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
void crash_splash_create(const CoordsXYZ& splashPos)
{
    SpriteGeneric* sprite = &create_sprite(SPRITE_IDENTIFIER_MISC)->generic;
    if (sprite != nullptr)
    {
        sprite->sprite_width = 33;
        sprite->sprite_height_negative = 51;
        sprite->sprite_height_positive = 16;
        sprite->sprite_identifier = SPRITE_IDENTIFIER_MISC;
        sprite->MoveTo(splashPos + CoordsXYZ{ 0, 0, 3 });
        sprite->type = SPRITE_MISC_CRASH_SPLASH;
        sprite->frame = 0;
    }
}

/**
 *
 *  rct2: 0x0067339D
 */
void CrashSplashParticle::Update()
{
    Invalidate2();
    frame += 85;
    if (frame >= 7168)
    {
        sprite_remove(this);
    }
}
