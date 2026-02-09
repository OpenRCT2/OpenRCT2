/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideSetColourSchemeAction.h"

#include "../Cheats.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/Ride.h"
#include "../ride/RideConstruction.h"
#include "../world/Map.h"
#include "../world/Park.h"

namespace OpenRCT2::GameActions
{
    RideSetColourSchemeAction::RideSetColourSchemeAction(
        const CoordsXYZD& location, TrackElemType trackType, uint16_t newColourScheme)
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
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void RideSetColourSchemeAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc) << DS_TAG(_trackType) << DS_TAG(_newColourScheme);
    }

    Result RideSetColourSchemeAction::Query(GameState_t& gameState) const
    {
        if (!LocationValid(_loc))
        {
            return Result(Status::invalidParameters, STR_CANT_SET_COLOUR_SCHEME, STR_OFF_EDGE_OF_MAP);
        }
        // Find the relevant track piece, prefer sequence 0 (logic copied from GetTrackElementOriginAndApplyChanges)
        auto trackElement = MapGetTrackElementAtOfTypeSeq(_loc, _trackType, 0);
        if (trackElement == nullptr)
        {
            trackElement = MapGetTrackElementAtOfType(_loc, _trackType);
            if (trackElement == nullptr)
            {
                return Result(Status::invalidParameters, STR_CANT_SET_COLOUR_SCHEME, STR_INVALID_TRACK_PARAMETERS);
            }
        }
        if (_newColourScheme >= kNumRideColourSchemes)
        {
            return Result(Status::invalidParameters, STR_CANT_SET_COLOUR_SCHEME, STR_INVALID_COLOUR_SCHEME_PARAMETER);
        }
        return Result();
    }

    Result RideSetColourSchemeAction::Execute(GameState_t& gameState) const
    {
        Result res = Result();
        res.expenditure = ExpenditureType::rideConstruction;
        res.errorTitle = STR_CANT_SET_COLOUR_SCHEME;

        GetTrackElementOriginAndApplyChanges(
            _loc, _trackType, _newColourScheme, nullptr, { TrackElementSetFlag::colourScheme });

        return res;
    }
} // namespace OpenRCT2::GameActions
