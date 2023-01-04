/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
    struct SetZoomCommand
    {
        static constexpr const char* Name = "Set Zoom Command";
        static constexpr const char* ScriptingName = "zoom";

        // TODO: Use ZoomLevel instead
        uint8_t Zoom{};

        int16_t operator()(int16_t timer);
    };
} // namespace OpenRCT2::Title
