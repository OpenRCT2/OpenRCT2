/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "TileElement.h"

constexpr uint8_t BANNER_NULL = 255;
constexpr size_t MAX_BANNERS = 250;
constexpr BannerIndex BANNER_INDEX_NULL = (BannerIndex)-1;

#pragma pack(push, 1)
struct rct_banner
{
    uint8_t type;
    uint8_t flags;              // 0x01
    rct_string_id string_idx; // 0x02
    union
    {
        uint8_t colour;     // 0x04
        uint8_t ride_index; // 0x04
    };
    uint8_t text_colour; // 0x05
    uint8_t x;           // 0x06
    uint8_t y;           // 0x07
};
assert_struct_size(rct_banner, 8);
#pragma pack(pop)

enum BANNER_FLAGS
{
    BANNER_FLAG_NO_ENTRY = (1 << 0),
    BANNER_FLAG_IS_LARGE_SCENERY = (1 << 1),
    BANNER_FLAG_LINKED_TO_RIDE = (1 << 2),
    BANNER_FLAG_IS_WALL = (1 << 3)
};

extern rct_banner gBanners[MAX_BANNERS];

void banner_init();
BannerIndex create_new_banner(uint8_t flags);
rct_tile_element* banner_get_tile_element(BannerIndex bannerIndex);
uint8_t banner_get_closest_ride_index(int32_t x, int32_t y, int32_t z);
void banner_reset_broken_index();
void fix_duplicated_banners();
void game_command_callback_place_banner(int32_t eax, int32_t ebx, int32_t ecx, int32_t edx, int32_t esi, int32_t edi, int32_t ebp);
