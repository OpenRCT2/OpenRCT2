/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../entity/Staff.h"
#include "GameAction.h"

namespace OpenRCT2::GameActions
{
    class StaffSetColourAction final : public GameActionBase<GameCommand::SetStaffColour>
    {
    private:
        uint8_t _staffType{};
        Drawing::Colour _colour{};

    public:
        StaffSetColourAction() = default;
        StaffSetColourAction(StaffType staffType, Drawing::Colour colour);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;
    };
} // namespace OpenRCT2::GameActions
