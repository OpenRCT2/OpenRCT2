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
#include "Entity.h"
#include "Fountain.h"
#include "SpriteBase.h"

enum LitterType : uint8_t;

struct Litter : SpriteBase
{
    static constexpr auto cEntityType = EntityType::Litter;
    LitterType SubType;
    uint32_t creationTick;
};

struct Balloon : MiscEntity
{
    static constexpr auto cEntityType = EntityType::Balloon;
    uint16_t popped;
    uint8_t time_to_move;
    uint8_t colour;

    void Update();
    void Pop();
    void Press();
};

struct Duck : MiscEntity
{
    static constexpr auto cEntityType = EntityType::Duck;
    enum class DuckState : uint8_t
    {
        FlyToWater,
        Swim,
        Drink,
        DoubleDrink,
        FlyAway,
    };
    int16_t target_x;
    int16_t target_y;
    DuckState state;

    void Update();
    uint32_t GetFrameImage(int32_t direction) const;
    bool IsFlying();
    void Remove();

private:
    void UpdateFlyToWater();
    void UpdateSwim();
    void UpdateDrink();
    void UpdateDoubleDrink();
    void UpdateFlyAway();
};

struct MoneyEffect : MiscEntity
{
    static constexpr auto cEntityType = EntityType::MoneyEffect;
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

struct VehicleCrashParticle : MiscEntity
{
    static constexpr auto cEntityType = EntityType::CrashedVehicleParticle;
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

struct ExplosionFlare : MiscEntity
{
    static constexpr auto cEntityType = EntityType::ExplosionFlare;
    void Update();
};

struct ExplosionCloud : MiscEntity
{
    static constexpr auto cEntityType = EntityType::ExplosionCloud;
    void Update();
};

struct CrashSplashParticle : MiscEntity
{
    static constexpr auto cEntityType = EntityType::CrashSplash;
    void Update();
};

struct SteamParticle : MiscEntity
{
    static constexpr auto cEntityType = EntityType::SteamParticle;
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
    MiscEntity misc;
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
    SPRITE_FLAGS_IS_CRASHED_VEHICLE_SPRITE = 1 << 7,
    SPRITE_FLAGS_PEEP_VISIBLE = 1 << 8,  // Peep is eligible to show in summarized guest list window (is inside park?)
    SPRITE_FLAGS_PEEP_FLASHING = 1 << 9, // Deprecated: Use sprite_set_flashing/sprite_get_flashing instead.
};

enum LitterType : uint8_t
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

constexpr const uint32_t SPATIAL_INDEX_SIZE = (MAXIMUM_MAP_SIZE_TECHNICAL * MAXIMUM_MAP_SIZE_TECHNICAL) + 1;
constexpr const uint32_t SPATIAL_INDEX_LOCATION_NULL = SPATIAL_INDEX_SIZE - 1;

extern const rct_string_id litterNames[12];

rct_sprite* create_sprite(EntityType type);
template<typename T> T* CreateEntity()
{
    return reinterpret_cast<T*>(create_sprite(T::cEntityType));
}

// Use only with imports that must happen at a specified index
SpriteBase* CreateEntityAt(const uint16_t index, const EntityType type);
// Use only with imports that must happen at a specified index
template<typename T> T* CreateEntityAt(const uint16_t index)
{
    return static_cast<T*>(CreateEntityAt(index, T::cEntityType));
}
void reset_sprite_list();
void reset_sprite_spatial_index();
void sprite_clear_all_unused();
void sprite_misc_update_all();
void sprite_set_coordinates(const CoordsXYZ& spritePos, SpriteBase* sprite);
void sprite_remove(SpriteBase* sprite);
void litter_create(const CoordsXYZD& litterPos, LitterType type);
void litter_remove_at(const CoordsXYZ& litterPos);
uint16_t remove_floating_sprites();
void sprite_misc_explosion_cloud_create(const CoordsXYZ& cloudPos);
void sprite_misc_explosion_flare_create(const CoordsXYZ& flarePos);

///////////////////////////////////////////////////////////////
// Balloon
///////////////////////////////////////////////////////////////
void create_balloon(const CoordsXYZ& balloonPos, int32_t colour, bool isPopped);
void balloon_update(Balloon* balloon);

///////////////////////////////////////////////////////////////
// Duck
///////////////////////////////////////////////////////////////
void create_duck(const CoordsXY& pos);
void duck_press(Duck* duck);
void duck_remove_all();

///////////////////////////////////////////////////////////////
// Crash particles
///////////////////////////////////////////////////////////////
void crashed_vehicle_particle_create(rct_vehicle_colour colours, const CoordsXYZ& vehiclePos);
void crash_splash_create(const CoordsXYZ& splashPos);

rct_sprite_checksum sprite_checksum();

void sprite_set_flashing(SpriteBase* sprite, bool flashing);
bool sprite_get_flashing(SpriteBase* sprite);

class EntityTweener
{
    std::vector<SpriteBase*> Entities;
    std::vector<CoordsXYZ> PrePos;
    std::vector<CoordsXYZ> PostPos;

private:
    void PopulateEntities();

public:
    static EntityTweener& Get();

    void PreTick();
    void PostTick();
    void RemoveEntity(SpriteBase* entity);
    void Tween(float alpha);
    void Restore();
    void Reset();
};

#endif
