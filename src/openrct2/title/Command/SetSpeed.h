/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

namespace OpenRCT2::Title
{
    struct SetSpeedCommand
    {
        static constexpr const char* Name = "Set Speed Command";

        uint8_t Speed;

        int16_t operator()(int16_t timer);
    };
} // namespace OpenRCT2::Title
