/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "GameAction.h"

namespace OpenRCT2::GameActions
{
    class BalloonPressAction final : public GameActionBase<GameCommand::BalloonPress>
    {
        EntityId _spriteIndex{ EntityId::GetNull() };

    public:
        BalloonPressAction() = default;
        BalloonPressAction(EntityId spriteIndex);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query() const override;
        Result Execute() const override;
    };
} // namespace OpenRCT2::GameActions
