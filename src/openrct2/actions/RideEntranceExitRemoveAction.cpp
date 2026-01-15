/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideEntranceExitRemoveAction.h"

#include "../Diagnostic.h"
#include "../ride/Ride.h"
#include "../ride/Station.h"
#include "../world/Entrance.h"
#include "../world/Map.h"
#include "../world/TileElementsView.h"
#include "../world/tile_element/EntranceElement.h"

namespace OpenRCT2::GameActions
{
    RideEntranceExitRemoveAction::RideEntranceExitRemoveAction(
        const CoordsXY& loc, RideId rideIndex, StationIndex stationNum, bool isExit)
        : _loc(loc)
        , _rideIndex(rideIndex)
        , _stationNum(stationNum)
        , _isExit(isExit)
    {
    }

    void RideEntranceExitRemoveAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit(_loc);
        visitor.Visit("ride", _rideIndex);
        visitor.Visit("station", _stationNum);
        visitor.Visit("isExit", _isExit);
    }

    uint16_t RideEntranceExitRemoveAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags();
    }

    void RideEntranceExitRemoveAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc) << DS_TAG(_rideIndex) << DS_TAG(_stationNum) << DS_TAG(_isExit);
    }

    static TileElement* FindEntranceElement(
        const CoordsXY& loc, RideId rideIndex, StationIndex stationNum, int32_t entranceType)
    {
        for (auto* entranceElement : TileElementsView<EntranceElement>(loc))
        {
            if (entranceElement->GetRideIndex() != rideIndex)
                continue;

            if (entranceElement->GetStationIndex() != stationNum)
                continue;

            if (entranceElement->GetEntranceType() != entranceType)
                continue;

            return entranceElement->as<TileElement>();
        }
        return nullptr;
    }

    Result RideEntranceExitRemoveAction::Query(GameState_t& gameState) const
    {
        auto ride = GetRide(_rideIndex);
        if (ride == nullptr)
        {
            LOG_ERROR("Ride not found for rideIndex %u", _rideIndex.ToUnderlying());
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_RIDE_NOT_FOUND);
        }

        if (ride->status != RideStatus::closed && ride->status != RideStatus::simulating)
        {
            return Result(Status::invalidParameters, STR_MUST_BE_CLOSED_FIRST, kStringIdNone);
        }

        if (ride->lifecycleFlags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)
        {
            return Result(Status::invalidParameters, STR_NOT_ALLOWED_TO_MODIFY_STATION, kStringIdNone);
        }

        if (!LocationValid(_loc))
        {
            return Result(Status::invalidParameters, STR_CANT_REMOVE_THIS, STR_OFF_EDGE_OF_MAP);
        }

        auto* entranceElement = FindEntranceElement(
            _loc, _rideIndex, _stationNum, _isExit ? ENTRANCE_TYPE_RIDE_EXIT : ENTRANCE_TYPE_RIDE_ENTRANCE);

        // If we are trying to remove a ghost and the element we found is real, return an error, but don't log a warning
        if (entranceElement != nullptr && (GetFlags().has(CommandFlag::ghost)) && !(entranceElement->IsGhost()))
        {
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_GHOST_ELEMENT_NOT_FOUND);
        }
        else if (entranceElement == nullptr)
        {
            LOG_ERROR(
                "Entrance/exit element not found. x = %d, y = %d, ride = %u, station = %u", _loc.x, _loc.y,
                _rideIndex.ToUnderlying(), _stationNum.ToUnderlying());
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_ENTRANCE_ELEMENT_NOT_FOUND);
        }

        return Result();
    }

    Result RideEntranceExitRemoveAction::Execute(GameState_t& gameState) const
    {
        auto ride = GetRide(_rideIndex);
        if (ride == nullptr)
        {
            LOG_ERROR("Ride not found for rideIndex %u", _rideIndex.ToUnderlying());
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_RIDE_NOT_FOUND);
        }

        const bool isGhost = GetFlags().has(CommandFlag::ghost);
        if (!isGhost)
        {
            RideClearForConstruction(*ride);
            ride->removePeeps();
            InvalidateTestResults(*ride);
        }

        auto* entranceElement = FindEntranceElement(
            _loc, _rideIndex, _stationNum, _isExit ? ENTRANCE_TYPE_RIDE_EXIT : ENTRANCE_TYPE_RIDE_ENTRANCE);

        // If we are trying to remove a ghost and the element we found is real, return an error, but don't log a warning
        if (entranceElement != nullptr && isGhost && !(entranceElement->IsGhost()))
        {
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_GHOST_ELEMENT_NOT_FOUND);
        }
        else if (entranceElement == nullptr)
        {
            LOG_ERROR(
                "Entrance/exit element not found. x = %d, y = %d, ride = %u, station = %d", _loc.x, _loc.y,
                _rideIndex.ToUnderlying(), _stationNum);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_ENTRANCE_ELEMENT_NOT_FOUND);
        }

        auto res = Result();
        res.position.x = _loc.x + 16;
        res.position.y = _loc.y + 16;
        res.position.z = TileElementHeight(res.position);

        FootpathQueueChainReset();
        MazeEntranceHedgeReplacement({ _loc, entranceElement });
        FootpathRemoveEdgesAt(_loc, entranceElement);

        TileElementRemove(entranceElement);

        auto& station = ride->getStation(_stationNum);
        if (_isExit)
        {
            station.Exit.SetNull();
        }
        else
        {
            station.Entrance.SetNull();
        }

        FootpathUpdateQueueChains();

        MapInvalidateTileFull(_loc);
        return res;
    }
} // namespace OpenRCT2::GameActions
