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
#include "../OpenRCT2.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../world/Footpath.h"
#include "../world/Location.hpp"
#include "../world/Park.h"
#include "../world/Wall.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(FootpathSceneryPlaceAction, GAME_COMMAND_PLACE_FOOTPATH_SCENERY, GameActionResult)
{
private:
    int32_t _x;
    int32_t _y;
    int32_t _z;

public:
    FootpathSceneryPlaceAction() = default;
    FootpathSceneryPlaceAction(int32_t x, int32_t y, int32_t z)
        : _x(x)
        , _y(y)
        , _z(z)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_x) << DS_TAG(_y) << DS_TAG(_z);
    }

    GameActionResult::Ptr Query() const override
    {
        GameActionResult::Ptr res = std::make_unique<GameActionResult>();

        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        GameActionResult::Ptr res = std::make_unique<GameActionResult>();
        return res;
    }
};
