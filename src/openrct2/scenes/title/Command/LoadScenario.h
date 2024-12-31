/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../core/StringTypes.h"

#include <cstdint>

constexpr uint8_t kTitleCommandScenarioLength = 64;

namespace OpenRCT2::Title
{
    struct LoadScenarioCommand
    {
        static constexpr const char* Name = "Load Scenario Command";
        static constexpr const char* ScriptingName = "loadsc";

        utf8 Scenario[kTitleCommandScenarioLength]{};

        int16_t operator()(int16_t timer);
    };
} // namespace OpenRCT2::Title
