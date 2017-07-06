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

#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "../common.h"
#include "../peep/peep.h"
#include "../ride/vehicle.h"

#define SPRITE_INDEX_NULL       0xFFFF
#define SPRITE_LOCATION_NULL    ((sint16)(uint16)0x8000)
#define MAX_SPRITES             10000
#define NUM_SPRITE_LISTS        6

enum SPRITE_IDENTIFIER {
    SPRITE_IDENTIFIER_VEHICLE = 0,
    SPRITE_IDENTIFIER_PEEP = 1,
    SPRITE_IDENTIFIER_MISC = 2,
    SPRITE_IDENTIFIER_LITTER = 3,
    SPRITE_IDENTIFIER_NULL = 255
};

enum SPRITE_LIST {
    SPRITE_LIST_NULL,
    SPRITE_LIST_TRAIN,
    SPRITE_LIST_PEEP,
    SPRITE_LIST_MISC,
    SPRITE_LIST_LITTER,
    SPRITE_LIST_UNKNOWN,
};

#pragma pack(push, 1)
typedef struct rct_unk_sprite {
    uint8 sprite_identifier;        // 0x00
    uint8 misc_identifier;          // 0x01
    uint16 next_in_quadrant;        // 0x02
    uint16 next;                    // 0x04
    uint16 previous;                // 0x06
    uint8 linked_list_type_offset;  // 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
    // Height from centre of sprite to bottom
    uint8 sprite_height_negative;   // 0x09
    uint16 sprite_index;            // 0x0A
    uint16 flags;           // 0x0C
    sint16 x;                       // 0x0E
    sint16 y;                       // 0x10
    sint16 z;                       // 0x12
    // Width from centre of sprite to edge
    uint8 sprite_width;             // 0x14
    // Height from centre of sprite to top
    uint8 sprite_height_positive;   // 0x15
    sint16 sprite_left;             // 0x16
    sint16 sprite_top;              // 0x18
    sint16 sprite_right;            // 0x1A
    sint16 sprite_bottom;           // 0x1C
    uint8  sprite_direction; //direction of sprite? 0x1e
    uint8 pad_1F[3]; // 0x1f
    rct_string_id name_string_idx;  // 0x22
    uint16 var_24;
    uint16 frame;                   // 0x26
    uint8 var_28[3];
    uint8 var_2B;
    uint8 pad_2C[0x45];
    uint8 var_71;
} rct_unk_sprite;
assert_struct_size(rct_unk_sprite, 0x72);

typedef struct rct_litter {
    uint8 sprite_identifier;        // 0x00
    uint8 type;                     // 0x01
    uint16 next_in_quadrant;        // 0x02
    uint16 next;                    // 0x04
    uint16 previous;                // 0x06
    uint8 linked_list_type_offset;  // 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
    uint8 sprite_height_negative;   // 0x09
    uint16 sprite_index;            // 0x0A
    uint16 flags;           // 0x0C
    sint16 x;                       // 0x0E
    sint16 y;                       // 0x10
    sint16 z;                       // 0x12
    uint8 sprite_width;             // 0x14
    uint8 sprite_height_positive;   // 0x15
    uint8 pad_16[8];
    uint8 sprite_direction;         // 0x1E
    uint8 pad_1F[5];
    uint32 creationTick;            // 0x24
} rct_litter;
assert_struct_size(rct_litter, 0x28);

typedef struct rct_balloon {
    uint8 sprite_identifier;        // 0x00
    uint8 misc_identifier;          // 0x01
    uint16 next_in_quadrant;        // 0x02
    uint16 next;                    // 0x04
    uint16 previous;                // 0x06
    uint8 linked_list_type_offset;  // 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
    uint8 var_09;                   // 0x09
    uint16 sprite_index;            // 0x0A
    uint16 flags;           // 0x0C
    sint16 x;                       // 0x0E
    sint16 y;                       // 0x10
    sint16 z;                       // 0x12
    uint8 var_14;                   // 0x14
    uint8 var_15;                   // 0x15
    uint8 pad_16[0xE];
    uint16 popped;                  // 0x24
    union {
        uint16 frame;               // 0x26
        struct {
            uint8 var_26a;
            uint8 var_26b;
        };
    };
    uint8 pad_28[4];
    uint8 colour;                   // 0x2C
    uint8 var_2D;

#ifdef __cplusplus
    void Update();
    void Pop();
    void Press();
#endif

} rct_balloon;
assert_struct_size(rct_balloon, 0x2e);

typedef struct rct_duck {
    uint8 sprite_identifier;        // 0x00
    uint8 misc_identifier;          // 0x01
    uint16 next_in_quadrant;        // 0x02
    uint16 next;                    // 0x04
    uint16 previous;                // 0x06
    uint8 linked_list_type_offset;  // 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
    uint8 var_09;                   // 0x09
    uint16 sprite_index;            // 0x0A
    uint16 flags;           // 0x0C
    sint16 x;                       // 0x0E
    sint16 y;                       // 0x10
    sint16 z;                       // 0x12
    uint8 var_14;                   // 0x14
    uint8 var_15;                   // 0x15
    uint8 pad_16[0x8];
    uint8 sprite_direction;         // 0x1E
    uint8 pad_1F[0x7];
    uint16 frame;
    uint8 pad_28[0x8];
    sint16 target_x;                // 0x30
    sint16 target_y;                // 0x32
    uint8 pad_34[0x14];
    uint8 state;                    // 0x48

#ifdef __cplusplus
    void UpdateFlyToWater();
    void UpdateSwim();
    void UpdateDrink();
    void UpdateDoubleDrink();
    void UpdateFlyAway();
    uint32 GetFrameImage(sint32 direction) const;
    void Invalidate();
    void Remove();
    void MoveTo(sint16 x, sint16 y, sint16 z);
#endif

} rct_duck;
assert_struct_size(rct_duck, 0x49);

typedef struct rct_jumping_fountain {
    uint8 sprite_identifier;        // 0x00
    uint8 misc_identifier;          // 0x01
    uint16 next_in_quadrant;        // 0x02
    uint16 next;                    // 0x04
    uint16 previous;                // 0x06
    uint8 linked_list_type_offset;  // 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
    uint8 var_09;
    uint16 sprite_index;            // 0x0A
    uint16 flags;           // 0x0C
    sint16 x;                       // 0x0E
    sint16 y;                       // 0x10
    sint16 z;                       // 0x12
    uint8 var_14;
    uint8 var_15;
    uint8 pad_16[0x8];
    uint8 sprite_direction;         // 0x1E
    uint8 pad_1F[0x7];
    union {
        uint16 var_26;
        struct {
            uint8 var_26a;
            uint8 var_26b;
        };
    };
    uint8 pad_28[0x6];
    uint8 var_2E;
    uint8 fountain_flags;           // 0x2F
    sint16 target_x;                // 0x30
    sint16 target_y;                // 0x32
    uint8 pad_34[0x12];
    uint16 iteration;               // 0x46
} rct_jumping_fountain;
assert_struct_size(rct_jumping_fountain, 0x48);

typedef struct rct_money_effect {
    uint8 sprite_identifier;        // 0x00
    uint8 misc_identifier;          // 0x01
    uint16 var_02;                  // 0x02
    uint16 next;                    // 0x04
    uint16 previous;                // 0x06
    uint8 linked_list_type_offset;  // 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
    uint8 var_09;
    uint16 sprite_index;            // 0x0A
    uint16 flags;           // 0x0C
    sint16 x;                       // 0x0E
    sint16 y;                       // 0x10
    sint16 z;                       // 0x12
    uint8 var_14;
    uint8 var_15;
    uint8 pad_16[0xE];
    uint16 move_delay;              // 0x24
    uint8 num_movements;            // 0x26
    uint8 vertical;
    money32 value;                  // 0x28
    uint8 pad_2C[0x18];
    sint16 offset_x;                // 0x44
    uint16 wiggle;                  // 0x46
} rct_money_effect;
assert_struct_size(rct_money_effect, 0x48);

typedef struct rct_crashed_vehicle_particle {
    uint8 sprite_identifier;        // 0x00
    uint8 misc_identifier;          // 0x01
    uint16 next_in_quadrant;        // 0x02
    uint16 next;                    // 0x04
    uint16 previous;                // 0x06
    uint8 linked_list_type_offset;  // 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
    // Height from centre of sprite to bottom
    uint8 sprite_height_negative;   // 0x09
    uint16 sprite_index;            // 0x0A
    uint16 flags;           // 0x0C
    sint16 x;                       // 0x0E
    sint16 y;                       // 0x10
    sint16 z;                       // 0x12
    // Width from centre of sprite to edge
    uint8 sprite_width;             // 0x14
    // Height from centre of sprite to top
    uint8 sprite_height_positive;   // 0x15
    sint16 sprite_left;             // 0x16
    sint16 sprite_top;              // 0x18
    sint16 sprite_right;            // 0x1A
    sint16 sprite_bottom;           // 0x1C
    uint8  sprite_direction; //direction of sprite? 0x1e
    uint8 pad_1F[3]; // 0x1f
    uint16 name_string_idx;         // 0x22
    uint16 var_24;
    uint16 frame;                   // 0x26
    uint8 var_28[3];
    uint8 var_2B;
    uint8 colour[2];
    uint16 var_2E;
    sint16 velocity_x;              // 0x30
    sint16 velocity_y;              // 0x32
    sint16 velocity_z;              // 0x34
    uint16 pad_36;
    sint32 acceleration_x;          // 0x38
    sint32 acceleration_y;          // 0x3C
    sint32 acceleration_z;          // 0x40
    uint8 pad_44[0x2D];
    uint8 var_71;
} rct_crashed_vehicle_particle;
assert_struct_size(rct_crashed_vehicle_particle, 0x45 + 0x2D);

typedef struct rct_crash_splash {
    uint8 sprite_identifier;        // 0x00
    uint8 misc_identifier;          // 0x01
    uint16 next_in_quadrant;        // 0x02
    uint16 next;                    // 0x04
    uint16 previous;                // 0x06
    uint8 linked_list_type_offset;  // 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
    // Height from centre of sprite to bottom
    uint8 sprite_height_negative;   // 0x09
    uint16 sprite_index;            // 0x0A
    uint16 flags;           // 0x0C
    sint16 x;                       // 0x0E
    sint16 y;                       // 0x10
    sint16 z;                       // 0x12
    // Width from centre of sprite to edge
    uint8 sprite_width;             // 0x14
    // Height from centre of sprite to top
    uint8 sprite_height_positive;   // 0x15
    sint16 sprite_left;             // 0x16
    sint16 sprite_top;              // 0x18
    sint16 sprite_right;            // 0x1A
    sint16 sprite_bottom;           // 0x1C
    uint8  sprite_direction; //direction of sprite? 0x1e
    uint8 pad_1F[3]; // 0x1f
    uint16 name_string_idx;         // 0x22
    uint16 var_24;
    uint16 frame;                   // 0x26
} rct_crash_splash;
assert_struct_size(rct_crash_splash, 0x28);

typedef struct rct_steam_particle {
    uint8 sprite_identifier;        // 0x00
    uint8 misc_identifier;          // 0x01
    uint16 next_in_quadrant;        // 0x02
    uint16 next;                    // 0x04
    uint16 previous;                // 0x06
    uint8 linked_list_type_offset;  // 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
    // Height from centre of sprite to bottom
    uint8 sprite_height_negative;   // 0x09
    uint16 sprite_index;            // 0x0A
    uint16 flags;           // 0x0C
    sint16 x;                       // 0x0E
    sint16 y;                       // 0x10
    sint16 z;                       // 0x12
    // Width from centre of sprite to edge
    uint8 sprite_width;             // 0x14
    // Height from centre of sprite to top
    uint8 sprite_height_positive;   // 0x15
    sint16 sprite_left;             // 0x16
    sint16 sprite_top;              // 0x18
    sint16 sprite_right;            // 0x1A
    sint16 sprite_bottom;           // 0x1C
    uint8  sprite_direction;        // 0x1E
    uint8 pad_1F[3];                // 0x1F
    uint16 name_string_idx;         // 0x22
    uint16 var_24;
    uint16 frame;                   // 0x26
} rct_steam_particle;
assert_struct_size(rct_steam_particle, 0x28);

/**
 * Sprite structure.
 * size: 0x0100
 */
typedef union {
    uint8 pad_00[0x100];
    rct_unk_sprite unknown;
    rct_peep peep;
    rct_litter litter;
    rct_vehicle vehicle;
    rct_balloon balloon;
    rct_duck duck;
    rct_jumping_fountain jumping_fountain;
    rct_money_effect money_effect;
    rct_crashed_vehicle_particle crashed_vehicle_particle;
    rct_crash_splash crash_splash;
    rct_steam_particle steam_particle;

#ifdef __cplusplus
    bool IsBalloon();
    bool IsDuck();
    rct_balloon * AsBalloon();
    rct_duck * AsDuck();
#endif

} rct_sprite;
assert_struct_size(rct_sprite, 0x100);

#pragma pack(pop)

enum {
    SPRITE_MISC_STEAM_PARTICLE,
    SPRITE_MISC_MONEY_EFFECT,
    SPRITE_MISC_CRASHED_VEHICLE_PARTICLE,
    SPRITE_MISC_EXPLOSION_CLOUD,
    SPRITE_MISC_CRASH_SPLASH,
    SPRITE_MISC_EXPLOSION_FLARE,
    SPRITE_MISC_JUMPING_FOUNTAIN_WATER,
    SPRITE_MISC_BALLOON,
    SPRITE_MISC_DUCK,
    SPRITE_MISC_JUMPING_FOUNTAIN_SNOW
};

enum {
    SPRITE_FLAGS_IS_CRASHED_VEHICLE_SPRITE = 1 << 7,
    SPRITE_FLAGS_PEEP_VISIBLE = 1 << 8, // Peep is eligible to show in summarized guest list window (is inside park?)
    SPRITE_FLAGS_PEEP_FLASHING = 1 << 9, // Deprecated: Use sprite_set_flashing/sprite_get_flashing instead.
};

enum {
    LITTER_TYPE_SICK,
    LITTER_TYPE_SICK_ALT,
    LITTER_TYPE_EMPTY_CAN,
    LITTER_TYPE_RUBBISH,
    LITTER_TYPE_EMPTY_BURGER_BOX,
    LITTER_TYPE_EMPTY_CUP,
    LITTER_TYPE_EMPTY_BOX,
    LITTER_TYPE_EMPTY_BOTTLE,
    LITTER_TYPE_EMPTY_BOWL_RED,
    LITTER_TYPE_EMPTY_DRINK_CARTON,
    LITTER_TYPE_EMPTY_JUICE_CUP,
    LITTER_TYPE_EMPTY_BOWL_BLUE,
};

rct_sprite *try_get_sprite(size_t spriteIndex);
rct_sprite *get_sprite(size_t sprite_idx);

#ifdef NO_RCT2
extern uint16 gSpriteListHead[6];
extern uint16 gSpriteListCount[6];
#else
extern uint16 *gSpriteListHead;
extern uint16 *gSpriteListCount;
#endif

extern uint16 gSpriteSpatialIndex[0x10001];

rct_sprite *create_sprite(uint8 bl);
void reset_empty_sprites();
void reset_sprite_list();
void reset_sprite_spatial_index();
void sprite_clear_all_unused();
void move_sprite_to_list(rct_sprite *sprite, uint8 cl);
void sprite_misc_update_all();
void sprite_move(sint16 x, sint16 y, sint16 z, rct_sprite* sprite);
void sprite_set_coordinates(sint16 x, sint16 y, sint16 z, rct_sprite *sprite);
void invalidate_sprite_0(rct_sprite* sprite);
void invalidate_sprite_1(rct_sprite *sprite);
void invalidate_sprite_2(rct_sprite *sprite);
void sprite_remove(rct_sprite *sprite);
void litter_create(sint32 x, sint32 y, sint32 z, sint32 direction, sint32 type);
void litter_remove_at(sint32 x, sint32 y, sint32 z);
void sprite_misc_explosion_cloud_create(sint32 x, sint32 y, sint32 z);
void sprite_misc_explosion_flare_create(sint32 x, sint32 y, sint32 z);
uint16 sprite_get_first_in_quadrant(sint32 x, sint32 y);
void sprite_position_tween_store_a();
void sprite_position_tween_store_b();
void sprite_position_tween_all(float nudge);
void sprite_position_tween_restore();
void sprite_position_tween_reset();

///////////////////////////////////////////////////////////////
// Balloon
///////////////////////////////////////////////////////////////
void create_balloon(sint32 x, sint32 y, sint32 z, sint32 colour, bool isPopped);
void balloon_update(rct_balloon *balloon);
void game_command_balloon_press(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);

///////////////////////////////////////////////////////////////
// Duck
///////////////////////////////////////////////////////////////
void create_duck(sint32 targetX, sint32 targetY);
void duck_update(rct_duck *duck);
void duck_press(rct_duck *duck);
void duck_remove_all();
uint32 duck_get_frame_image(const rct_duck * duck, sint32 direction);

///////////////////////////////////////////////////////////////
// Money effect
///////////////////////////////////////////////////////////////
void money_effect_create(money32 value);
void money_effect_create_at(money32 value, sint32 x, sint32 y, sint32 z, bool vertical);
void money_effect_update(rct_money_effect *moneyEffect);
rct_string_id money_effect_get_string_id(const rct_money_effect * sprite, money32 * outValue);

///////////////////////////////////////////////////////////////
// Crash particles
///////////////////////////////////////////////////////////////
void crashed_vehicle_particle_create(rct_vehicle_colour colours, sint32 x, sint32 y, sint32 z);
void crashed_vehicle_particle_update(rct_crashed_vehicle_particle *particle);
void crash_splash_create(sint32 x, sint32 y, sint32 z);
void crash_splash_update(rct_crash_splash *splash);

const char *sprite_checksum();

void sprite_set_flashing(rct_sprite *sprite, bool flashing);
bool sprite_get_flashing(rct_sprite *sprite);

#endif

