/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
    struct RestartCommand
    {
        static constexpr const char* Name = "Restart Command";
        static constexpr const char* ScriptingName = "restart";

        int16_t operator()(int16_t timer);
    };
} // namespace OpenRCT2::Title
