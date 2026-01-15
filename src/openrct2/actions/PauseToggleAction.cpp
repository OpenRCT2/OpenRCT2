/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PauseToggleAction.h"

namespace OpenRCT2::GameActions
{
    uint16_t PauseToggleAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused | Flags::IgnoreForReplays;
    }

    Result PauseToggleAction::Query(GameState_t& gameState) const
    {
        return Result();
    }

    Result PauseToggleAction::Execute(GameState_t& gameState) const
    {
        PauseToggle();
        return Result();
    }
} // namespace OpenRCT2::GameActions
