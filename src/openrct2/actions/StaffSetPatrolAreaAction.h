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
    CoordsXY _loc;
    StaffSetPatrolAreaMode _mode;

public:
    StaffSetPatrolAreaAction() = default;
    StaffSetPatrolAreaAction(EntityId spriteId, const CoordsXY& loc, const StaffSetPatrolAreaMode mode);

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;
};
