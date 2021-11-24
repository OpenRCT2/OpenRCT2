/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "EntityBase.h"

#include <array>

#pragma pack(push, 1)
/**
 * Sprite structure.
 * size: 0x0200
 */
union rct_sprite
{
    uint8_t pad_00[0x200];
    EntityBase base;
    // Provide a constructor as EntityBase is not trivialy constructable
    rct_sprite()
        : pad_00()
    {
    }
};
assert_struct_size(rct_sprite, 0x200);

struct rct_sprite_checksum
{
    std::array<std::byte, 20> raw;

    std::string ToString() const;
};

#pragma pack(pop)

void ResetAllEntities();
void reset_sprite_spatial_index();
void sprite_misc_update_all();
void sprite_set_coordinates(const CoordsXYZ& spritePos, EntityBase* sprite);
void sprite_remove(EntityBase* sprite);
uint16_t remove_floating_sprites();

rct_sprite_checksum sprite_checksum();

void sprite_set_flashing(EntityBase* sprite, bool flashing);
bool sprite_get_flashing(EntityBase* sprite);
