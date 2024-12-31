/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Wait.h"

#include "../../../Context.h"

namespace OpenRCT2::Title
{
    int16_t WaitCommand::operator()(int16_t timer)
    {
        // Return number of game ticks this wait command lasts
        return std::max<int16_t>(1, kGameUpdateFPS * Milliseconds / 1000);
    }
} // namespace OpenRCT2::Title
