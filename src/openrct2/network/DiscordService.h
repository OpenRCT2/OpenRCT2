/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef __ENABLE_DISCORD__

#include "../common.h"
#include <limits>

class DiscordService final
{
private:
    uint32_t _ticksSinceLastRefresh = std::numeric_limits<uint32_t>::max();

public:
    DiscordService();
    ~DiscordService();

    void Update();

private:
    void RefreshPresence();
};

#endif
