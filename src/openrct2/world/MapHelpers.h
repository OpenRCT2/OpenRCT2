/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3
 *****************************************************************************/

#ifndef _MAP_HELPERS_H_
#define _MAP_HELPERS_H_

#include "../common.h"

enum {
    SLOPE_S_THRESHOLD_FLAGS = (1 << 0),
    SLOPE_W_THRESHOLD_FLAGS = (1 << 1),
    SLOPE_N_THRESHOLD_FLAGS = (1 << 2),
    SLOPE_E_THRESHOLD_FLAGS = (1 << 3)
};

sint32 map_smooth(sint32 l, sint32 t, sint32 r, sint32 b);
sint32 tile_smooth(sint32 x, sint32 y);

#endif
