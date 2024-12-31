/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PauseToggleAction.h"

using namespace OpenRCT2;

uint16_t PauseToggleAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused | GameActions::Flags::IgnoreForReplays;
}

GameActions::Result PauseToggleAction::Query() const
{
    return GameActions::Result();
}

GameActions::Result PauseToggleAction::Execute() const
{
    PauseToggle();
    return GameActions::Result();
}
