/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../core/String.hpp"

#include <cstdint>

#define TITLE_COMMAND_SCENARIO_LENGTH 64

namespace OpenRCT2::Title
{
    struct LoadScenarioCommand
    {
        static constexpr const char* Name = "Load Scenario Command";

        utf8 Scenario[TITLE_COMMAND_SCENARIO_LENGTH];

        int16_t operator()(int16_t timer);
    };
} // namespace OpenRCT2::Title
