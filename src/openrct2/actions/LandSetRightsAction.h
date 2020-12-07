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

enum class LandSetRightSetting : uint8_t
{
    UnownLand,
    UnownConstructionRights,
    SetForSale,
    SetConstructionRightsForSale,
    SetOwnershipWithChecks,
    Count
};

DEFINE_GAME_ACTION(LandSetRightsAction, GAME_COMMAND_SET_LAND_OWNERSHIP, GameActions::Result)
{
private:
    MapRange _range;
    LandSetRightSetting _setting{ LandSetRightSetting::Count };
    uint8_t _ownership{};

    GameActions::Result::Ptr QueryExecute(bool isExecuting) const;
    GameActions::Result::Ptr map_buy_land_rights_for_tile(const CoordsXY& loc, bool isExecuting) const;

public:
    LandSetRightsAction() = default;

    LandSetRightsAction(const MapRange& range, LandSetRightSetting setting, uint8_t ownership = 0)
        : _range(range)
        , _setting(setting)
        , _ownership(ownership)
    {
    }

    LandSetRightsAction(const CoordsXY& coord, LandSetRightSetting setting, uint8_t ownership = 0)
        : _range(coord.x, coord.y, coord.x, coord.y)
        , _setting(setting)
        , _ownership(ownership)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GameActions::Flags::EditorOnly;
    }

    void Serialise(DataSerialiser & stream) override;

    GameActions::Result::Ptr Query() const override;

    GameActions::Result::Ptr Execute() const override;
};