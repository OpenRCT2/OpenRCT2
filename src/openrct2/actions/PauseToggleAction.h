/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

// Clang format is broken for small game actions
// clang-format off
class PauseToggleAction final : public GameActionBase<GameCommand::TogglePause>
{
public:
    PauseToggleAction() = default;

    uint16_t GetActionFlags() const override;

    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;
};
// clang-format on
