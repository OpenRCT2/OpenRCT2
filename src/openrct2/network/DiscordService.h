/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef __ENABLE_DISCORD__

#    include "../common.h"
#    include "../core/Timer.hpp"

#    include <limits>

class DiscordService final
{
private:
    OpenRCT2::Timer _updateTimer;

public:
    DiscordService();
    ~DiscordService();

    void Tick();

private:
    void RefreshPresence() const;
};

#endif
