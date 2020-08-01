/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "../common.h"
#include "../peep/Peep.h"
#include "../ride/Vehicle.h"
#include "Fountain.h"
#include "SpriteBase.h"

#define SPRITE_INDEX_NULL 0xFFFF
#define MAX_SPRITES 10000

enum SPRITE_IDENTIFIER
{
    SPRITE_IDENTIFIER_VEHICLE = 0,
    SPRITE_IDENTIFIER_PEEP = 1,
    SPRITE_IDENTIFIER_MISC = 2,
    SPRITE_IDENTIFIER_LITTER = 3,
    SPRITE_IDENTIFIER_NULL = 255
};

enum class EntityListId : uint8_t
{
    Free,
    TrainHead,
    Peep,
    Misc,
    Litter,
    Vehicle,
    Count,
};

struct Litter : SpriteBase
{
    uint32_t creationTick;
};

struct Balloon : SpriteGeneric
{
    uint16_t popped;
    uint8_t time_to_move;
    uint8_t colour;

    void Update();
    void Pop();
    void Press();
};

struct Duck : SpriteGeneric
{
    int16_t target_x;
    int16_t target_y;
    uint8_t state;

    void Update();
    uint32_t GetFrameImage(int32_t direction) const;
    void Invalidate();
    bool IsFlying();
    void Remove();

private:
    void UpdateFlyToWater();
    void UpdateSwim();
    void UpdateDrink();
    void UpdateDoubleDrink();
    void UpdateFlyAway();
};

struct MoneyEffect : SpriteBase
{
    uint16_t MoveDelay;
    uint8_t NumMovements;
    uint8_t Vertical;
    money32 Value;
    int16_t OffsetX;
    uint16_t Wiggle;

    static void CreateAt(money32 value, const CoordsXYZ& effectPos, bool vertical);
    static void Create(money32 value, const CoordsXYZ& loc);
    void Update();
    std::pair<rct_string_id, money32> GetStringId() const;
};

struct VehicleCrashParticle : SpriteGeneric
{
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
};

struct ExplosionFlare : SpriteGeneric
{
    void Update();
};

struct ExplosionCloud : SpriteGeneric
{
    void Update();
};

struct CrashSplashParticle : SpriteGeneric
{
    void Update();
};

struct SteamParticle : SpriteGeneric
{
    uint16_t time_to_move;

    void Update();
};

#pragma pack(push, 1)
/**
 * Sprite structure.
 * size: 0x0200
 */
union rct_sprite
{
    uint8_t pad_00[0x200];
    SpriteGeneric generic;
    Peep peep;
    Litter litter;
    Vehicle vehicle;
    Balloon balloon;
    Duck duck;
    JumpingFountain jumping_fountain;
    MoneyEffect money_effect;
    VehicleCrashParticle crashed_vehicle_particle;
    CrashSplashParticle crash_splash;
    SteamParticle steam_particle;

    // Default constructor to prevent non trivial construction issues
    rct_sprite()
        : pad_00()
    {
    }
};
assert_struct_size(rct_sprite, 0x200);

struct rct_sprite_checksum
{
    std::array<uint8_t, 20> raw;

    std::string ToString() const;
};

#pragma pack(pop)

enum
{
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

enum
{
    SPRITE_FLAGS_IS_CRASHED_VEHICLE_SPRITE = 1 << 7,
    SPRITE_FLAGS_PEEP_VISIBLE = 1 << 8,  // Peep is eligible to show in summarized guest list window (is inside park?)
    SPRITE_FLAGS_PEEP_FLASHING = 1 << 9, // Deprecated: Use sprite_set_flashing/sprite_get_flashing instead.
};

enum
{
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

SpriteBase* try_get_sprite(size_t spriteIndex);
SpriteBase* get_sprite(size_t sprite_idx);
template<typename T = SpriteBase> T* GetEntity(size_t sprite_idx)
{
    auto spr = get_sprite(sprite_idx);
    return spr != nullptr ? spr->As<T>() : nullptr;
}

template<typename T = SpriteBase> T* TryGetEntity(size_t sprite_idx)
{
    auto spr = try_get_sprite(sprite_idx);
    return spr != nullptr ? spr->As<T>() : nullptr;
}

uint16_t GetEntityListCount(EntityListId list);
extern uint16_t gSpriteListHead[static_cast<uint8_t>(EntityListId::Count)];
extern uint16_t gSpriteListCount[static_cast<uint8_t>(EntityListId::Count)];

constexpr const uint32_t SPATIAL_INDEX_SIZE = (MAXIMUM_MAP_SIZE_TECHNICAL * MAXIMUM_MAP_SIZE_TECHNICAL) + 1;
constexpr const uint32_t SPATIAL_INDEX_LOCATION_NULL = SPATIAL_INDEX_SIZE - 1;
extern uint16_t gSpriteSpatialIndex[SPATIAL_INDEX_SIZE];

extern const rct_string_id litterNames[12];

rct_sprite* create_sprite(SPRITE_IDENTIFIER spriteIdentifier);
rct_sprite* create_sprite(SPRITE_IDENTIFIER spriteIdentifier, EntityListId linkedListIndex);
void reset_sprite_list();
void reset_sprite_spatial_index();
void sprite_clear_all_unused();
void sprite_misc_update_all();
void sprite_set_coordinates(const CoordsXYZ& spritePos, SpriteBase* sprite);
void sprite_remove(SpriteBase* sprite);
void litter_create(const CoordsXYZD& litterPos, int32_t type);
void litter_remove_at(const CoordsXYZ& litterPos);
uint16_t remove_floating_sprites();
void sprite_misc_explosion_cloud_create(const CoordsXYZ& cloudPos);
void sprite_misc_explosion_flare_create(const CoordsXYZ& flarePos);
uint16_t sprite_get_first_in_quadrant(const CoordsXY& spritePos);
void sprite_position_tween_store_a();
void sprite_position_tween_store_b();
void sprite_position_tween_all(float nudge);
void sprite_position_tween_restore();
void sprite_position_tween_reset();

///////////////////////////////////////////////////////////////
// Balloon
///////////////////////////////////////////////////////////////
void create_balloon(const CoordsXYZ& balloonPos, int32_t colour, bool isPopped);
void balloon_update(Balloon* balloon);

///////////////////////////////////////////////////////////////
// Duck
///////////////////////////////////////////////////////////////
void create_duck(const CoordsXY& pos);
void duck_update(Duck* duck);
void duck_press(Duck* duck);
void duck_remove_all();

///////////////////////////////////////////////////////////////
// Crash particles
///////////////////////////////////////////////////////////////
void crashed_vehicle_particle_create(rct_vehicle_colour colours, const CoordsXYZ& vehiclePos);
void crashed_vehicle_particle_update(VehicleCrashParticle* particle);
void crash_splash_create(const CoordsXYZ& splashPos);
void crash_splash_update(CrashSplashParticle* splash);

rct_sprite_checksum sprite_checksum();

void sprite_set_flashing(SpriteBase* sprite, bool flashing);
bool sprite_get_flashing(SpriteBase* sprite);
int32_t check_for_sprite_list_cycles(bool fix);
int32_t fix_disjoint_sprites();

template<typename T, uint16_t SpriteBase::*NextList> class EntityIterator
{
private:
    T* Entity = nullptr;
    uint16_t NextEntityId = SPRITE_INDEX_NULL;

public:
    EntityIterator(const uint16_t _EntityId)
        : NextEntityId(_EntityId)
    {
        ++(*this);
    }
    EntityIterator& operator++()
    {
        Entity = nullptr;

        while (NextEntityId != SPRITE_INDEX_NULL && Entity == nullptr)
        {
            auto baseEntity = GetEntity(NextEntityId);
            if (!baseEntity)
            {
                NextEntityId = SPRITE_INDEX_NULL;
                continue;
            }
            NextEntityId = baseEntity->*NextList;
            Entity = baseEntity->template As<T>();
        }
        return *this;
    }

    EntityIterator operator++(int)
    {
        EntityIterator retval = *this;
        ++(*this);
        return retval;
    }
    bool operator==(EntityIterator other) const
    {
        return Entity == other.Entity;
    }
    bool operator!=(EntityIterator other) const
    {
        return !(*this == other);
    }
    T* operator*()
    {
        return Entity;
    }
    // iterator traits
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = const T*;
    using reference = const T&;
    using iterator_category = std::forward_iterator_tag;
};

template<typename T = SpriteBase> class EntityTileList
{
private:
    uint16_t FirstEntity = SPRITE_INDEX_NULL;
    using EntityTileIterator = EntityIterator<T, &SpriteBase::next_in_quadrant>;

public:
    EntityTileList(const CoordsXY& loc)
        : FirstEntity(sprite_get_first_in_quadrant(loc))
    {
    }

    EntityTileIterator begin()
    {
        return EntityTileIterator(FirstEntity);
    }
    EntityTileIterator end()
    {
        return EntityTileIterator(SPRITE_INDEX_NULL);
    }
};

template<typename T = SpriteBase> class EntityList
{
private:
    uint16_t FirstEntity = SPRITE_INDEX_NULL;
    using EntityListIterator = EntityIterator<T, &SpriteBase::next>;

public:
    EntityList(EntityListId type)
        : FirstEntity(gSpriteListHead[static_cast<uint8_t>(type)])
    {
    }

    EntityListIterator begin()
    {
        return EntityListIterator(FirstEntity);
    }
    EntityListIterator end()
    {
        return EntityListIterator(SPRITE_INDEX_NULL);
    }
};

#endif
