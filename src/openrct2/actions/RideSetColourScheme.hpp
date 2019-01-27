/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Cheats.h"
#include "../common.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/Ride.h"
#include "../world/Park.h"
#include "../world/Sprite.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(RideSetColourSchemeAction, GAME_COMMAND_SET_COLOUR_SCHEME, GameActionResult)
{
private:
    int32_t _x = 0, _y = 0, _z = 0, _trackType = 0;
    Direction _direction = Direction::WEST;
    uint16_t _newColourScheme = 0;

public:
    RideSetColourSchemeAction() = default;
    RideSetColourSchemeAction(int32_t x, int32_t y, int32_t z, Direction direction, int32_t trackType, uint16_t newColourScheme)
        : _x(x)
        , _y(y)
        , _z(z)
        , _trackType(trackType)
        , _direction(direction)
        , _newColourScheme(newColourScheme)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_x) << DS_TAG(_y) << DS_TAG(_z) << DS_TAG(_direction) << DS_TAG(_trackType)
               << DS_TAG(_newColourScheme);
    }

    GameActionResult::Ptr Query() const override
    {
        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        GameActionResult::Ptr res = std::make_unique<GameActionResult>();
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
        res->ErrorTitle = STR_CANT_SET_COLOUR_SCHEME;

        int32_t x = _x, y = _y, z = _z;
        sub_6C683D(&x, &y, &z, _direction, _trackType, _newColourScheme, nullptr, 4);

        return res;
    }
};
