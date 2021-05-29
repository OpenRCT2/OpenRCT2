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
#include "SpriteBase.h"

#include <array>

class DataSerialiser;

struct ExplosionFlare : MiscEntity
{
    static constexpr auto cEntityType = EntityType::ExplosionFlare;
    void Update();
    void Serialise(DataSerialiser& stream);
};

struct ExplosionCloud : MiscEntity
{
    static constexpr auto cEntityType = EntityType::ExplosionCloud;
    void Update();
    void Serialise(DataSerialiser& stream);
};

struct SteamParticle : MiscEntity
{
    static constexpr auto cEntityType = EntityType::SteamParticle;
    uint16_t time_to_move;

    void Update();
    void Serialise(DataSerialiser& stream);
};

#pragma pack(push, 1)
/**
 * Sprite structure.
 * size: 0x0200
 */
union rct_sprite
{
    uint8_t pad_00[0x200];
    SpriteBase base;
};
assert_struct_size(rct_sprite, 0x200);

struct rct_sprite_checksum
{
    std::array<std::byte, 20> raw;

    std::string ToString() const;
};

#pragma pack(pop)

enum
{
    SPRITE_FLAGS_IS_CRASHED_VEHICLE_SPRITE = 1 << 7,
    SPRITE_FLAGS_PEEP_VISIBLE = 1 << 8,  // Peep is eligible to show in summarized guest list window (is inside park?)
    SPRITE_FLAGS_PEEP_FLASHING = 1 << 9, // Deprecated: Use sprite_set_flashing/sprite_get_flashing instead.
};

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
uint16_t remove_floating_sprites();
void sprite_misc_explosion_cloud_create(const CoordsXYZ& cloudPos);
void sprite_misc_explosion_flare_create(const CoordsXYZ& flarePos);

rct_sprite_checksum sprite_checksum();

void sprite_set_flashing(SpriteBase* sprite, bool flashing);
bool sprite_get_flashing(SpriteBase* sprite);

#endif
