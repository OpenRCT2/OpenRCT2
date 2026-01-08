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
    struct StaffHireNewActionResult
    {
        EntityId StaffEntityId = EntityId::GetNull();
    };

    class StaffHireNewAction final : public GameActionBase<GameCommand::HireNewStaffMember>
    {
    private:
        bool _autoPosition{};
        uint8_t _staffType{ EnumValue(StaffType::count) };
        ObjectEntryIndex _costumeIndex;
        uint32_t _staffOrders{};

    public:
        StaffHireNewAction() = default;
        StaffHireNewAction(bool autoPosition, StaffType staffType, ObjectEntryIndex costumeIndex, uint32_t staffOrders);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;

    private:
        Result QueryExecute(GameState_t& gameState, bool execute) const;
        void AutoPositionNewStaff(GameState_t& gameState, Peep* newPeep) const;
    };
} // namespace OpenRCT2::GameActions
