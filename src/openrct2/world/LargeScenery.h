/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "Map.h"
#include "TileElement.h"

LargeSceneryEntry* GetLargeSceneryEntry(ObjectEntryIndex entryIndex);

enum
{
    LARGE_SCENERY_ELEMENT_FLAGS2_ACCOUNTED = 1 << 0,
};
