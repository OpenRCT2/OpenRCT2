/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../peep/Staff.h"
#include "../world/Sprite.h"
#include "GameAction.h"

/* rct2: 0x009929FC */
static constexpr const PeepSpriteType spriteTypes[] = {
    PeepSpriteType::Handyman,
    PeepSpriteType::Mechanic,
    PeepSpriteType::Security,
    PeepSpriteType::EntertainerPanda,
};

class StaffHireNewActionResult final : public GameActions::Result
{
public:
    StaffHireNewActionResult()
        : GameActions::Result(GameActions::Status::Ok, STR_CANT_HIRE_NEW_STAFF)
    {
    }
    StaffHireNewActionResult(GameActions::Status error, rct_string_id message)
        : GameActions::Result(error, STR_CANT_HIRE_NEW_STAFF, message)
    {
    }

    uint32_t peepSriteIndex = SPRITE_INDEX_NULL;
};

DEFINE_GAME_ACTION(StaffHireNewAction, GAME_COMMAND_HIRE_NEW_STAFF_MEMBER, StaffHireNewActionResult)
{
private:
    bool _autoPosition{};
    uint8_t _staffType{ EnumValue(StaffType::Count) };
    EntertainerCostume _entertainerType{ EntertainerCostume::Count };
    uint32_t _staffOrders{};

public:
    StaffHireNewAction() = default;
    StaffHireNewAction(bool autoPosition, StaffType staffType, EntertainerCostume entertainerType, uint32_t staffOrders)
        : _autoPosition(autoPosition)
        , _staffType(static_cast<uint8_t>(staffType))
        , _entertainerType(entertainerType)
        , _staffOrders(staffOrders)
    {
    }

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
    }

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;

private:
    GameActions::Result::Ptr QueryExecute(bool execute) const;
    void AutoPositionNewStaff(Peep * newPeep) const;
};
