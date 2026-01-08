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
    class StaffSetCostumeAction final : public GameActionBase<GameCommand::SetStaffCostume>
    {
    private:
        EntityId _spriteIndex{ EntityId::GetNull() };
        ObjectEntryIndex _costume = kObjectEntryIndexNull;

    public:
        StaffSetCostumeAction() = default;
        StaffSetCostumeAction(EntityId spriteIndex, ObjectEntryIndex costume);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;
    };
} // namespace OpenRCT2::GameActions
