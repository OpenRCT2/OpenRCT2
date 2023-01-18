/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackRemoveAction.h"

#include "../management/Finance.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../ride/TrackDesign.h"
#include "../util/Util.h"
#include "../world/MapAnimation.h"
#include "../world/Surface.h"
#include "RideSetSettingAction.h"

using namespace OpenRCT2::TrackMetaData;

TrackRemoveAction::TrackRemoveAction(track_type_t trackType, int32_t sequence, const CoordsXYZD& origin)
    : _trackType(trackType)
    , _sequence(sequence)
    , _origin(origin)
{
    _origin.direction &= 3;
}

void TrackRemoveAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_origin);
    visitor.Visit("trackType", _trackType);
    visitor.Visit("sequence", _sequence);
}

uint16_t TrackRemoveAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

void TrackRemoveAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_trackType) << DS_TAG(_sequence) << DS_TAG(_origin);
}

GameActions::Result TrackRemoveAction::Query() const
{
    auto res = GameActions::Result();
    res.Position.x = _origin.x + 16;
    res.Position.y = _origin.y + 16;
    res.Position.z = _origin.z;
    res.Expenditure = ExpenditureType::RideConstruction;

    // Stations require some massaging of the track type for comparing
    auto comparableTrackType = _trackType;
    switch (_trackType)
    {
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            comparableTrackType = TrackElemType::EndStation;
            break;
    }

    bool found = false;
    bool isGhost = GetFlags() & GAME_COMMAND_FLAG_GHOST;
    TileElement* tileElement = MapGetFirstElementAt(_origin);

    do
    {
        if (tileElement == nullptr)
            break;

        if (tileElement->GetBaseZ() != _origin.z)
            continue;

        if (tileElement->GetType() != TileElementType::Track)
            continue;

        if ((tileElement->GetDirection()) != _origin.direction)
            continue;

        if (tileElement->AsTrack()->GetSequenceIndex() != _sequence)
            continue;

        if (tileElement->IsGhost() != isGhost)
            continue;

        auto tileTrackType = tileElement->AsTrack()->GetTrackType();
        switch (tileTrackType)
        {
            case TrackElemType::BeginStation:
            case TrackElemType::MiddleStation:
                tileTrackType = TrackElemType::EndStation;
                break;
        }

        if (tileTrackType != comparableTrackType)
            continue;

        found = true;
        break;
    } while (!(tileElement++)->IsLastForTile());

    if (!found)
    {
        LOG_WARNING(
            "Track Element not found. x = %d, y = %d, z = %d, d = %d, seq = %d.", _origin.x, _origin.y, _origin.z,
            _origin.direction, _sequence);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_NONE);
    }

    if (tileElement->AsTrack()->IsIndestructible())
    {
        return GameActions::Result(
            GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS,
            STR_YOU_ARE_NOT_ALLOWED_TO_REMOVE_THIS_SECTION);
    }

    RideId rideIndex = tileElement->AsTrack()->GetRideIndex();
    const auto trackType = tileElement->AsTrack()->GetTrackType();

    auto ride = GetRide(rideIndex);
    if (ride == nullptr)
    {
        LOG_WARNING("Ride not found. ride index = %d.", rideIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_NONE);
    }

    if (ride->type >= RIDE_TYPE_COUNT)
    {
        LOG_WARNING("Ride type not found. ride type = %d.", ride->type);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_NONE);
    }
    const auto& ted = GetTrackElementDescriptor(trackType);
    auto sequenceIndex = tileElement->AsTrack()->GetSequenceIndex();
    const rct_preview_track* trackBlock = ted.GetBlockForSequence(sequenceIndex);
    if (trackBlock == nullptr)
    {
        LOG_WARNING("Track block %d not found for track type %d.", sequenceIndex, trackType);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_NONE);
    }

    auto startLoc = _origin;
    startLoc.direction = tileElement->GetDirection();

    auto rotatedTrack = CoordsXYZ{ CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(startLoc.direction), trackBlock->z };
    startLoc.x -= rotatedTrack.x;
    startLoc.y -= rotatedTrack.y;
    startLoc.z -= rotatedTrack.z;
    res.Position.x = startLoc.x;
    res.Position.y = startLoc.y;
    res.Position.z = startLoc.z;

    money64 supportCosts = 0;

    trackBlock = ted.Block;
    for (; trackBlock->index != 255; trackBlock++)
    {
        rotatedTrack = CoordsXYZ{ CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(startLoc.direction), trackBlock->z };
        auto mapLoc = CoordsXYZ{ startLoc.x, startLoc.y, startLoc.z } + rotatedTrack;

        if (!LocationValid(mapLoc))
        {
            return GameActions::Result(
                GameActions::Status::NotOwned, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_LAND_NOT_OWNED_BY_PARK);
        }
        MapInvalidateTileFull(mapLoc);

        found = false;
        tileElement = MapGetFirstElementAt(mapLoc);
        do
        {
            if (tileElement == nullptr)
                break;

            if (tileElement->GetBaseZ() != mapLoc.z)
                continue;

            if (tileElement->GetType() != TileElementType::Track)
                continue;

            if (tileElement->GetDirection() != _origin.direction)
                continue;

            if (tileElement->AsTrack()->GetSequenceIndex() != trackBlock->index)
                continue;

            if (tileElement->AsTrack()->GetTrackType() != trackType)
                continue;

            if (tileElement->IsGhost() != isGhost)
                continue;

            found = true;
            break;
        } while (!(tileElement++)->IsLastForTile());

        if (!found)
        {
            LOG_WARNING(
                "Track Element not found. x = %d, y = %d, z = %d, d = %d, seq = %d.", mapLoc.x, mapLoc.y, mapLoc.z,
                _origin.direction, trackBlock->index);
            return GameActions::Result(GameActions::Status::Unknown, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_NONE);
        }

        int32_t entranceDirections = std::get<0>(ted.SequenceProperties);
        if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN && (tileElement->AsTrack()->GetSequenceIndex() == 0))
        {
            const auto removeElementResult = TrackRemoveStationElement({ mapLoc, _origin.direction }, rideIndex, 0);
            if (!removeElementResult.Successful)
            {
                return GameActions::Result(
                    GameActions::Status::Unknown, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, removeElementResult.Message);
            }
        }

        auto* surfaceElement = MapGetSurfaceElementAt(mapLoc);
        if (surfaceElement == nullptr)
        {
            LOG_WARNING("Surface Element not found. x = %d, y = %d", mapLoc.x, mapLoc.y);
            return GameActions::Result(GameActions::Status::Unknown, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_NONE);
        }

        int8_t _support_height = tileElement->base_height - surfaceElement->base_height;
        if (_support_height < 0)
        {
            _support_height = 10;
        }

        supportCosts += (_support_height / 2) * ride->GetRideTypeDescriptor().BuildCosts.SupportPrice;
    }

    money64 price = ride->GetRideTypeDescriptor().BuildCosts.TrackPrice;
    price *= ted.PriceModifier;
    price >>= 16;
    price = supportCosts + price;
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_EVER_BEEN_OPENED)
    {
        // 70% modifier for opened rides
        price = (price * 45875) / 65536;
    }

    res.Cost = -price;
    return res;
}

GameActions::Result TrackRemoveAction::Execute() const
{
    auto res = GameActions::Result();
    res.Position.x = _origin.x + 16;
    res.Position.y = _origin.y + 16;
    res.Position.z = _origin.z;
    res.Expenditure = ExpenditureType::RideConstruction;

    // Stations require some massaging of the track type for comparing
    auto comparableTrackType = _trackType;
    switch (_trackType)
    {
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            comparableTrackType = TrackElemType::EndStation;
            break;
    }

    bool found = false;
    bool isGhost = GetFlags() & GAME_COMMAND_FLAG_GHOST;
    TileElement* tileElement = MapGetFirstElementAt(_origin);

    do
    {
        if (tileElement == nullptr)
            break;

        if (tileElement->GetBaseZ() != _origin.z)
            continue;

        if (tileElement->GetType() != TileElementType::Track)
            continue;

        if ((tileElement->GetDirection()) != _origin.direction)
            continue;

        if (tileElement->AsTrack()->GetSequenceIndex() != _sequence)
            continue;

        if (tileElement->IsGhost() != isGhost)
            continue;

        auto tileTrackType = tileElement->AsTrack()->GetTrackType();
        switch (tileTrackType)
        {
            case TrackElemType::BeginStation:
            case TrackElemType::MiddleStation:
                tileTrackType = TrackElemType::EndStation;
                break;
        }

        if (tileTrackType != comparableTrackType)
            continue;

        found = true;
        break;
    } while (!(tileElement++)->IsLastForTile());

    if (!found)
    {
        LOG_WARNING(
            "Track Element not found. x = %d, y = %d, z = %d, d = %d, seq = %d.", _origin.x, _origin.y, _origin.z,
            _origin.direction, _sequence);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_NONE);
    }

    RideId rideIndex = tileElement->AsTrack()->GetRideIndex();
    const auto trackType = tileElement->AsTrack()->GetTrackType();
    bool isLiftHill = tileElement->AsTrack()->HasChain();

    auto ride = GetRide(rideIndex);
    if (ride == nullptr)
    {
        LOG_WARNING("Ride not found. ride index = %d.", rideIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_NONE);
    }
    const auto& ted = GetTrackElementDescriptor(trackType);
    auto sequenceIndex = tileElement->AsTrack()->GetSequenceIndex();
    const rct_preview_track* trackBlock = ted.GetBlockForSequence(sequenceIndex);
    if (trackBlock == nullptr)
    {
        LOG_WARNING("Track block %d not found for track type %d.", sequenceIndex, trackType);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_NONE);
    }

    auto startLoc = _origin;
    startLoc.direction = tileElement->GetDirection();

    auto rotatedTrackLoc = CoordsXYZ{ CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(startLoc.direction), trackBlock->z };
    startLoc.x -= rotatedTrackLoc.x;
    startLoc.y -= rotatedTrackLoc.y;
    startLoc.z -= rotatedTrackLoc.z;
    res.Position.x = startLoc.x;
    res.Position.y = startLoc.y;
    res.Position.z = startLoc.z;

    money64 supportCosts = 0;

    trackBlock = ted.Block;
    for (; trackBlock->index != 255; trackBlock++)
    {
        rotatedTrackLoc = CoordsXYZ{ CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(startLoc.direction), trackBlock->z };
        auto mapLoc = CoordsXYZ{ startLoc.x, startLoc.y, startLoc.z } + rotatedTrackLoc;

        MapInvalidateTileFull(mapLoc);

        found = false;
        tileElement = MapGetFirstElementAt(mapLoc);
        do
        {
            if (tileElement == nullptr)
                break;

            if (tileElement->GetBaseZ() != mapLoc.z)
                continue;

            if (tileElement->GetType() != TileElementType::Track)
                continue;

            if (tileElement->GetDirection() != _origin.direction)
                continue;

            if (tileElement->AsTrack()->GetSequenceIndex() != trackBlock->index)
                continue;

            if (tileElement->AsTrack()->GetTrackType() != trackType)
                continue;

            if (tileElement->IsGhost() != isGhost)
                continue;

            found = true;
            break;
        } while (!(tileElement++)->IsLastForTile());

        if (!found)
        {
            LOG_WARNING(
                "Track Element not found. x = %d, y = %d, z = %d, d = %d, seq = %d.", mapLoc.x, mapLoc.y, mapLoc.z,
                _origin.direction, trackBlock->index);
            return GameActions::Result(GameActions::Status::Unknown, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_NONE);
        }

        int32_t entranceDirections = std::get<0>(ted.SequenceProperties);
        if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN && (tileElement->AsTrack()->GetSequenceIndex() == 0))
        {
            const auto removeElementResult = TrackRemoveStationElement({ mapLoc, _origin.direction }, rideIndex, 0);
            if (!removeElementResult.Successful)
            {
                return GameActions::Result(
                    GameActions::Status::Unknown, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, removeElementResult.Message);
            }
        }

        auto* surfaceElement = MapGetSurfaceElementAt(mapLoc);
        if (surfaceElement == nullptr)
        {
            LOG_WARNING("Surface Element not found. x = %d, y = %d", mapLoc.x, mapLoc.y);
            return GameActions::Result(GameActions::Status::Unknown, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_NONE);
        }

        int8_t _support_height = tileElement->base_height - surfaceElement->base_height;
        if (_support_height < 0)
        {
            _support_height = 10;
        }

        supportCosts += (_support_height / 2) * ride->GetRideTypeDescriptor().BuildCosts.SupportPrice;

        // If the removed tile is a station modify station properties.
        // Don't do this if the ride is simulating and the tile is a ghost to prevent desyncs.
        if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN
            && !(ride->status == RideStatus::Simulating && tileElement->Flags & TILE_ELEMENT_FLAG_GHOST)
            && (tileElement->AsTrack()->GetSequenceIndex() == 0))
        {
            const auto removeElementResult = TrackRemoveStationElement(
                { mapLoc, _origin.direction }, rideIndex, GAME_COMMAND_FLAG_APPLY);
            if (!removeElementResult.Successful)
            {
                return GameActions::Result(
                    GameActions::Status::Unknown, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, removeElementResult.Message);
            }
        }

        if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER))
        {
            surfaceElement->SetHasTrackThatNeedsWater(false);
        }

        InvalidateTestResults(*ride);
        FootpathQueueChainReset();
        if (!gCheatsDisableClearanceChecks || !(tileElement->IsGhost()))
        {
            FootpathRemoveEdgesAt(mapLoc, tileElement);
        }
        TileElementRemove(tileElement);
        ride->ValidateStations();
        if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
        {
            ride->UpdateMaxVehicles();
        }
    }

    if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
    {
        switch (trackType)
        {
            case TrackElemType::OnRidePhoto:
                ride->lifecycle_flags &= ~RIDE_LIFECYCLE_ON_RIDE_PHOTO;
                break;
            case TrackElemType::CableLiftHill:
                ride->lifecycle_flags &= ~RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED;
                break;
            case TrackElemType::BlockBrakes:
                ride->num_block_brakes--;
                if (ride->num_block_brakes == 0)
                {
                    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_OPERATING;
                    RideMode newMode = RideMode::ContinuousCircuit;
                    if (ride->type == RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER)
                    {
                        newMode = RideMode::PoweredLaunch;
                    }

                    auto rideSetSetting = RideSetSettingAction(ride->id, RideSetSetting::Mode, static_cast<uint8_t>(newMode));
                    GameActions::ExecuteNested(&rideSetSetting);
                }

                break;
        }

        switch (trackType)
        {
            case TrackElemType::Up25ToFlat:
            case TrackElemType::Up60ToFlat:
            case TrackElemType::DiagUp25ToFlat:
            case TrackElemType::DiagUp60ToFlat:
                if (!isLiftHill)
                    break;
                [[fallthrough]];
            case TrackElemType::CableLiftHill:
                ride->num_block_brakes--;
                break;
        }
    }

    money64 price = ride->GetRideTypeDescriptor().BuildCosts.TrackPrice;
    price *= ted.PriceModifier;
    price >>= 16;
    price = supportCosts + price;
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_EVER_BEEN_OPENED)
    {
        // 70% modifier for opened rides
        price = (price * 45875) / 65536;
    }

    res.Cost = -price;
    return res;
}
