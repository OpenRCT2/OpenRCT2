/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LoadScenario.h"

namespace OpenRCT2::Title
{
    int16_t LoadScenarioCommand::operator()(int16_t timer)
    {
        // Scenario loading is currently handled by the title sequence player
        return 0;
    }
} // namespace OpenRCT2::Title
