/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Money.hpp"
#include "../localisation/StringIdType.h"
#include "ObjectTypes.h"

enum
{
    BANNER_ENTRY_FLAG_HAS_PRIMARY_COLOUR = (1 << 0),
};

struct BannerSceneryEntry
{
    static constexpr auto kObjectType = ObjectType::Banners;

    StringId name;
    uint32_t image;
    uint8_t scrolling_mode;
    uint8_t flags;
    money64 price;
    ObjectEntryIndex scenery_tab_id;
};
