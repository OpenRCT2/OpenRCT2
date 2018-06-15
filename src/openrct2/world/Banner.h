/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _BANNER_H_
#define _BANNER_H_

#include "../common.h"
#include "TileElement.h"

#define BANNER_NULL 255
#define MAX_BANNERS 250
#define BANNER_INDEX_NULL -1

#pragma pack(push, 1)
struct rct_banner {
    uint8 type;
    uint8 flags; // 0x01 bit 0 is no entry
    rct_string_id string_idx; // 0x02
    uint8 colour; // 0x04
    uint8 text_colour; // 0x05
    uint8 x; // 0x06
    uint8 y; // 0x07
};
assert_struct_size(rct_banner, 8);
#pragma pack(pop)

enum{
    BANNER_FLAG_NO_ENTRY = (1 << 0),
    BANNER_FLAG_IS_LARGE_SCENERY = (1 << 1),
    BANNER_FLAG_LINKED_TO_RIDE = (1 << 2),
    BANNER_FLAG_IS_WALL = (1 << 3)
};

extern rct_banner gBanners[MAX_BANNERS];

void banner_init();
sint32 create_new_banner(uint8 flags);
rct_tile_element *banner_get_tile_element(sint32 bannerIndex);
sint32 banner_get_closest_ride_index(sint32 x, sint32 y, sint32 z);
void banner_reset_broken_index();
void fix_duplicated_banners();
void game_command_callback_place_banner(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp);

#endif
