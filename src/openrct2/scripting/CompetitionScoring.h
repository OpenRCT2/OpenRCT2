/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "CompetitionState.h"

namespace OpenRCT2
{
    struct GameState_t;
}

namespace OpenRCT2::Scripting
{
    void UpdateCompetitionScores(GameState_t& gameState);
} // namespace OpenRCT2::Scripting

#endif
