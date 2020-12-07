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

DEFINE_GAME_ACTION(RideSetColourSchemeAction, GAME_COMMAND_SET_COLOUR_SCHEME, GameActions::Result)
{
private:
    CoordsXYZD _loc;
    int32_t _trackType{};
    uint16_t _newColourScheme{};

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
        return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc) << DS_TAG(_trackType) << DS_TAG(_newColourScheme);
    }

    GameActions::Result::Ptr Query() const override
    {
        if (!LocationValid(_loc))
        {
            return MakeResult(GameActions::Status::InvalidParameters, STR_LAND_NOT_OWNED_BY_PARK);
        }
        return std::make_unique<GameActions::Result>();
    }

    GameActions::Result::Ptr Execute() const override
    {
        GameActions::Result::Ptr res = std::make_unique<GameActions::Result>();
        res->Expenditure = ExpenditureType::RideConstruction;
        res->ErrorTitle = STR_CANT_SET_COLOUR_SCHEME;

        sub_6C683D(_loc, _trackType, _newColourScheme, nullptr, TRACK_ELEMENT_SET_COLOUR_SCHEME);

        return res;
    }
};
