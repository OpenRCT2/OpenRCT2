/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

namespace OpenRCT2::GameActions
{
    enum class StaffSetPatrolAreaMode : uint8_t
    {
        Set,
        Unset,
        ClearAll
    };

    class StaffSetPatrolAreaAction final : public GameActionBase<GameCommand::SetStaffPatrol>
    {
    private:
        EntityId _spriteId{ EntityId::GetNull() };
        MapRange _range;
        StaffSetPatrolAreaMode _mode;

        Result QueryExecute(GameState_t& gameState, bool executing) const;

    public:
        StaffSetPatrolAreaAction() = default;
        StaffSetPatrolAreaAction(EntityId spriteId, const MapRange& range, StaffSetPatrolAreaMode mode);

        uint16_t GetActionFlags() const override;

        void AcceptParameters(GameActionParameterVisitor&) final;
        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;
    };
} // namespace OpenRCT2::GameActions
