/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SetSpeed.h"

#include "../../Game.h"

#include <algorithm>

namespace OpenRCT2::Title
{
    int16_t SetSpeedCommand::operator()(int16_t timer)
    {
        gGameSpeed = std::clamp<uint8_t>(Speed, 1, 4);

        return 0;
    }
} // namespace OpenRCT2::Title
