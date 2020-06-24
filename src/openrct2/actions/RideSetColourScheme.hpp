/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
    CoordsXYZD _loc{ 0, 0, 0, 0 };
    int32_t _trackType = 0;
    uint16_t _newColourScheme = 0;

public:
    RideSetColourSchemeAction() = default;
    RideSetColourSchemeAction(const CoordsXYZD& location, int32_t trackType, uint16_t newColourScheme)
        : _loc(location)
        , _trackType(trackType)
        , _newColourScheme(newColourScheme)
    {
    }

    void AcceptParameters(GameActionParameterVisitor & visitor) override
    {
        visitor.Visit(_loc);
        visitor.Visit("trackType", _trackType);
        visitor.Visit("colourScheme", _newColourScheme);
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc) << DS_TAG(_trackType) << DS_TAG(_newColourScheme);
    }

    GameActionResult::Ptr Query() const override
    {
        if (!LocationValid(_loc))
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_LAND_NOT_OWNED_BY_PARK);
        }
        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        GameActionResult::Ptr res = std::make_unique<GameActionResult>();
        res->Expenditure = ExpenditureType::RideConstruction;
        res->ErrorTitle = STR_CANT_SET_COLOUR_SCHEME;

        sub_6C683D(_loc, _trackType, _newColourScheme, nullptr, TRACK_ELEMENT_SET_COLOUR_SCHEME);

        return res;
    }
};
