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
#include "Scenery.h"

struct SmallSceneryEntry;

enum
{
    MAP_ELEM_SMALL_SCENERY_COLOUR_FLAG_NEEDS_SUPPORTS = (1 << 5),
};

SmallSceneryEntry* GetSmallSceneryEntry(ObjectEntryIndex entryIndex);
