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

enum class LandBuyRightSetting : uint8_t
{
    BuyLand,
    BuyConstructionRights,
    Count
};

DEFINE_GAME_ACTION(LandBuyRightsAction, GAME_COMMAND_BUY_LAND_RIGHTS, GameActions::Result)
{
private:
    MapRange _range;
    LandBuyRightSetting _setting{ LandBuyRightSetting::Count };

    constexpr static rct_string_id _ErrorTitles[] = { STR_CANT_BUY_LAND, STR_CANT_BUY_CONSTRUCTION_RIGHTS_HERE };

    GameActions::Result::Ptr QueryExecute(bool isExecuting) const;
    GameActions::Result::Ptr map_buy_land_rights_for_tile(const CoordsXY& loc, bool isExecuting) const;

public:
    LandBuyRightsAction() = default;

    LandBuyRightsAction(const MapRange& range, LandBuyRightSetting setting)
        : _range(range)
        , _setting(setting)
    {
    }

    LandBuyRightsAction(const CoordsXY& coord, LandBuyRightSetting setting)
        : _range(coord.x, coord.y, coord.x, coord.y)
        , _setting(setting)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override;

    GameActions::Result::Ptr Query() const override;

    GameActions::Result::Ptr Execute() const override;
};
