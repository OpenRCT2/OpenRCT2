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

void reset_sprite_list();
void reset_sprite_spatial_index();
void sprite_clear_all_unused();
void sprite_misc_update_all();
void sprite_set_coordinates(const CoordsXYZ& spritePos, SpriteBase* sprite);
void sprite_remove(SpriteBase* sprite);
uint16_t remove_floating_sprites();

rct_sprite_checksum sprite_checksum();

void sprite_set_flashing(SpriteBase* sprite, bool flashing);
bool sprite_get_flashing(SpriteBase* sprite);

#endif
