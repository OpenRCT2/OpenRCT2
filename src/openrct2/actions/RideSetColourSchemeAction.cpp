/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideSetColourSchemeAction.h"

#include "../Cheats.h"
#include "../common.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/Ride.h"
#include "../ride/RideConstruction.h"
#include "../world/Park.h"

RideSetColourSchemeAction::RideSetColourSchemeAction(
    const CoordsXYZD& location, track_type_t trackType, uint16_t newColourScheme)
    : _loc(location)
    , _trackType(trackType)
    , _newColourScheme(newColourScheme)
{
}

void RideSetColourSchemeAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_loc);
    visitor.Visit("trackType", _trackType);
    visitor.Visit("colourScheme", _newColourScheme);
}

uint16_t RideSetColourSchemeAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void RideSetColourSchemeAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_loc) << DS_TAG(_trackType) << DS_TAG(_newColourScheme);
}

GameActions::Result RideSetColourSchemeAction::Query() const
{
    if (!LocationValid(_loc))
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_SET_COLOUR_SCHEME, STR_LAND_NOT_OWNED_BY_PARK);
    }
    return GameActions::Result();
}

GameActions::Result RideSetColourSchemeAction::Execute() const
{
    GameActions::Result res = GameActions::Result();
    res.Expenditure = ExpenditureType::RideConstruction;
    res.ErrorTitle = STR_CANT_SET_COLOUR_SCHEME;

    GetTrackElementOriginAndApplyChanges(_loc, _trackType, _newColourScheme, nullptr, TRACK_ELEMENT_SET_COLOUR_SCHEME);

    return res;
}
