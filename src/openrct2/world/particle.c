#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../audio/audio.h"
#include "../util/util.h"
#include "../scenario/scenario.h"
#include "sprite.h"

/**
 *
 *  rct2: 0x006735A1
 */
void crashed_vehicle_particle_create(rct_vehicle_colour colours, sint32 x, sint32 y, sint32 z)
{
    rct_crashed_vehicle_particle *sprite = (rct_crashed_vehicle_particle*)create_sprite(2);
    if (sprite != NULL) {
        sprite->colour[0] = colours.body_colour;
        sprite->colour[1] = colours.trim_colour;
        sprite->sprite_width = 8;
        sprite->sprite_height_negative = 8;
        sprite->sprite_height_positive = 8;
        sprite->sprite_identifier = SPRITE_IDENTIFIER_MISC;
        sprite_move(x, y, z, (rct_sprite*)sprite);
        sprite->misc_identifier = SPRITE_MISC_CRASHED_VEHICLE_PARTICLE;

        sprite->frame = (scenario_rand() & 0xFF) * 12;
        sprite->var_24 = (scenario_rand() & 0x7F) + 140;
        sprite->var_2E = ((scenario_rand() & 0xFF) * 5) >> 8;
        sprite->acceleration_x = ((sint16)(scenario_rand() & 0xFFFF)) * 4;
        sprite->acceleration_y = ((sint16)(scenario_rand() & 0xFFFF)) * 4;
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
void crashed_vehicle_particle_update(rct_crashed_vehicle_particle *particle)
{
    invalidate_sprite_0((rct_sprite*)particle);
    particle->var_24--;
    if (particle->var_24 == 0) {
        sprite_remove((rct_sprite*)particle);
        return;
    }

    // Apply gravity
    particle->acceleration_z -= 5041;

    // Apply air resistance
    particle->acceleration_x -= (particle->acceleration_x / 256);
    particle->acceleration_y -= (particle->acceleration_y / 256);
    particle->acceleration_z -= (particle->acceleration_z / 256);

    // Update velocity and position
    sint32 vx = particle->velocity_x + particle->acceleration_x;
    sint32 vy = particle->velocity_y + particle->acceleration_y;
    sint32 vz = particle->velocity_z + particle->acceleration_z;

    sint16 x = particle->x + (vx >> 16);
    sint16 y = particle->y + (vy >> 16);
    sint16 z = particle->z + (vz >> 16);

    particle->velocity_x = vx & 0xFFFF;
    particle->velocity_y = vy & 0xFFFF;
    particle->velocity_z = vz & 0xFFFF;

    // Check collision with land / water
    uint32 waterLand = map_element_height(x, y);
    sint16 landZ = (waterLand & 0xFFFF);
    sint16 waterZ = (waterLand >> 16);

    if (waterZ != 0 && particle->z >= waterZ && z <= waterZ) {
        // Splash
        audio_play_sound_at_location(SOUND_WATER_2, particle->x, particle->y, waterZ);
        crash_splash_create(particle->x, particle->y, waterZ);
        sprite_remove((rct_sprite*)particle);
        return;
    }

    if (particle->z >= landZ && z <= landZ) {
        // Bounce
        particle->acceleration_z *= -1;
        z = landZ;
    }
    sprite_move(x, y, z, (rct_sprite*)particle);
    invalidate_sprite_0((rct_sprite*)particle);

    particle->frame += 85;
    if (particle->frame >= 3072) {
        particle->frame = 0;
    }
}

/**
 *
 *  rct2: 0x00673699
 */
void crash_splash_create(sint32 x, sint32 y, sint32 z)
{
    rct_unk_sprite *sprite = (rct_unk_sprite*)create_sprite(2);
    if (sprite != NULL) {
        sprite->sprite_width = 33;
        sprite->sprite_height_negative = 51;
        sprite->sprite_height_positive = 16;
        sprite->sprite_identifier = SPRITE_IDENTIFIER_MISC;
        sprite_move(x, y, z + 3, (rct_sprite*)sprite);
        sprite->misc_identifier = SPRITE_MISC_CRASH_SPLASH;
        sprite->frame = 0;
    }
}

/**
 *
 *  rct2: 0x0067339D
 */
void crash_splash_update(rct_crash_splash *splash)
{
    invalidate_sprite_2((rct_sprite*)splash);
    splash->frame += 85;
    if (splash->frame >= 7168) {
        sprite_remove((rct_sprite*)splash);
    }
}
