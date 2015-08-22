#include "../audio/audio.h"
#include "sprite.h"

/**
 *
 *  rct2: 0x006735A1
 */
void crashed_vehicle_particle_create(rct_vehicle_colour colours, int x, int y, int z)
{
	rct_crashed_vehicle_particle *sprite = (rct_crashed_vehicle_particle*)create_sprite(2);
	if (sprite != NULL) {
		sprite->colour[0] = colours.body_colour;
		sprite->colour[1] = colours.trim_colour;
		sprite->sprite_width = 8;
		sprite->sprite_height_negative = 8;
		sprite->sprite_height_positive = 8;
		sprite->sprite_identifier = SPRITE_IDENTIFIER_MISC;
		sprite_move(x, y, z + 4, (rct_sprite*)sprite);
		sprite->misc_identifier = SPRITE_MISC_CRASHED_VEHICLE_PARTICLE;

		sprite->var_26 = (rand() & 0xFF) * 12;
		sprite->var_24 = (rand() & 0x7F) + 140;
		sprite->var_2E = ((rand() & 0xFF) * 5) >> 8;
		sprite->acceleration_x = (rand() & 0xFFFF) * 4;
		sprite->acceleration_y = (rand() & 0xFFFF) * 4;
		sprite->acceleration_z = (rand() & 0xFFFF) * 4 + 0x10000;
		sprite->velocity_x = 0;
		sprite->velocity_y = 0;
		sprite->velocity_z = 0;
	}
}

/**
 *
 *  rct: 0x00673298
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
		sound_play_panned(SOUND_WATER_2, 0x8001, particle->x, particle->y, waterZ);
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

	particle->var_26 += 85;
	if (particle->var_26 >= 3072) {
		particle->var_26 = 0;
	}
}

/**
 *
 *  rct2: 0x00673699
 */
void crash_splash_create(int x, int y, int z)
{
	rct_unk_sprite *sprite = (rct_unk_sprite*)create_sprite(2);
	if (sprite != NULL) {
		sprite->sprite_width = 33;
		sprite->sprite_height_negative = 51;
		sprite->sprite_height_positive = 16;
		sprite->sprite_identifier = SPRITE_IDENTIFIER_MISC;
		sprite_move(x, y, z + 3, (rct_sprite*)sprite);
		sprite->misc_identifier = SPRITE_MISC_CRASH_SPLASH;
		sprite->var_26 = 0;
	}
}

/**
 *
 *  rct: 0x0067339D
 */
void crash_splash_update(rct_crash_splash *splash)
{
	invalidate_sprite_2((rct_sprite*)splash);
	splash->var_26 += 85;
	if (splash->var_26 >= 7168) {
		sprite_remove((rct_sprite*)splash);
	}
}
