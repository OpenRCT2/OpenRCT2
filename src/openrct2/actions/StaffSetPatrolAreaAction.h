/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
    MapRange _range;
    StaffSetPatrolAreaMode _mode;

    OpenRCT2::GameActions::Result QueryExecute(bool executing) const;

public:
    StaffSetPatrolAreaAction() = default;
    StaffSetPatrolAreaAction(EntityId spriteId, const MapRange& range, const StaffSetPatrolAreaMode mode);

    uint16_t GetActionFlags() const override;

    void AcceptParameters(GameActionParameterVisitor& visitor) override;
    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;
};
