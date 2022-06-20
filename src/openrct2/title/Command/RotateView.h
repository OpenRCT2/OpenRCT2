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
    struct RotateViewCommand
    {
        static constexpr const char* Name = "Rotate View Command";
        static constexpr const char* ScriptingName = "rotate";

        uint8_t Rotations;

        int16_t operator()(int16_t timer);
    };
} // namespace OpenRCT2::Title
