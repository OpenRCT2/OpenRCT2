/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../management/Finance.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../ride/TrackDesign.h"
#include "../util/Util.h"
#include "../world/MapAnimation.h"
#include "../world/Surface.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(TrackRemoveAction, GAME_COMMAND_REMOVE_TRACK, GameActionResult)
{
private:
    int32_t _trackType{ 0 };
    int32_t _sequence{ 0 };
    CoordsXYZD _origin;

public:
    TrackRemoveAction()
    {
    }

    TrackRemoveAction(int32_t trackType, int32_t sequence, const CoordsXYZD& origin)
        : _trackType(trackType)
        , _sequence(sequence)
        , _origin(origin)
    {
        _origin.direction &= 3;
    }

    void AcceptParameters(GameActionParameterVisitor & visitor) override
    {
        visitor.Visit(_origin);
        visitor.Visit("trackType", _trackType);
        visitor.Visit("sequence", _sequence);
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_trackType) << DS_TAG(_sequence) << DS_TAG(_origin);
    }

    GameActionResult::Ptr Query() const override
    {
        auto res = MakeResult();
        res->Position.x = _origin.x + 16;
        res->Position.y = _origin.y + 16;
        res->Position.z = _origin.z;
        res->Expenditure = ExpenditureType::RideConstruction;

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
        TileElement* tileElement = map_get_first_element_at(_origin);

        do
        {
            if (tileElement == nullptr)
                break;

            if (tileElement->GetBaseZ() != _origin.z)
                continue;

            if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
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
            log_warning(
                "Track Element not found. x = %d, y = %d, z = %d, d = %d, seq = %d.", _origin.x, _origin.y, _origin.z,
                _origin.direction, _sequence);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS);
        }

        if (tileElement->AsTrack()->IsIndestructible())
        {
            return MakeResult(
                GA_ERROR::DISALLOWED, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_YOU_ARE_NOT_ALLOWED_TO_REMOVE_THIS_SECTION);
        }

        ride_id_t rideIndex = tileElement->AsTrack()->GetRideIndex();
        auto trackType = tileElement->AsTrack()->GetTrackType();

        auto ride = get_ride(rideIndex);
        if (ride == nullptr)
        {
            log_warning("Ride not found. ride index = %d.", rideIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS);
        }
        const rct_preview_track* trackBlock = get_track_def_from_ride(ride, trackType);
        trackBlock += tileElement->AsTrack()->GetSequenceIndex();

        auto startLoc = _origin;
        startLoc.direction = tileElement->GetDirection();

        auto rotatedTrack = CoordsXYZ{ CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(startLoc.direction), trackBlock->z };
        startLoc.x -= rotatedTrack.x;
        startLoc.y -= rotatedTrack.y;
        startLoc.z -= rotatedTrack.z;
        res->Position.x = startLoc.x;
        res->Position.y = startLoc.y;
        res->Position.z = startLoc.z;

        money32 cost = 0;

        trackBlock = get_track_def_from_ride(ride, trackType);
        for (; trackBlock->index != 255; trackBlock++)
        {
            rotatedTrack = CoordsXYZ{ CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(startLoc.direction), trackBlock->z };
            auto mapLoc = CoordsXYZ{ startLoc.x, startLoc.y, startLoc.z } + rotatedTrack;

            if (!LocationValid(mapLoc))
            {
                return MakeResult(GA_ERROR::NOT_OWNED, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_LAND_NOT_OWNED_BY_PARK);
            }
            map_invalidate_tile_full(mapLoc);

            found = false;
            tileElement = map_get_first_element_at(mapLoc);
            do
            {
                if (tileElement == nullptr)
                    break;

                if (tileElement->GetBaseZ() != mapLoc.z)
                    continue;

                if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
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
                log_warning(
                    "Track Element not found. x = %d, y = %d, z = %d, d = %d, seq = %d.", mapLoc.x, mapLoc.y, mapLoc.z,
                    _origin.direction, trackBlock->index);
                return MakeResult(GA_ERROR::UNKNOWN, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS);
            }

            int32_t entranceDirections;
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
            {
                entranceDirections = FlatRideTrackSequenceProperties[trackType][0];
            }
            else
            {
                entranceDirections = TrackSequenceProperties[trackType][0];
            }

            if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN && (tileElement->AsTrack()->GetSequenceIndex() == 0))
            {
                if (!track_remove_station_element({ mapLoc, _origin.direction }, rideIndex, 0))
                {
                    return MakeResult(GA_ERROR::UNKNOWN, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, gGameCommandErrorText);
                }
            }

            auto* surfaceElement = map_get_surface_element_at(mapLoc);
            if (surfaceElement == nullptr)
            {
                log_warning("Surface Element not found. x = %d, y = %d", mapLoc.x, mapLoc.y);
                return MakeResult(GA_ERROR::UNKNOWN, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS);
            }

            int8_t _support_height = tileElement->base_height - surfaceElement->base_height;
            if (_support_height < 0)
            {
                _support_height = 10;
            }

            cost += (_support_height / 2) * RideTypeDescriptors[ride->type].BuildCosts.SupportPrice;
        }

        money32 price = RideTypeDescriptors[ride->type].BuildCosts.TrackPrice;
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
        {
            price *= FlatRideTrackPricing[trackType];
        }
        else
        {
            price *= TrackPricing[trackType];
        }
        price >>= 16;
        price = (price + cost) / 2;
        if (ride->lifecycle_flags & RIDE_LIFECYCLE_EVER_BEEN_OPENED)
            price *= -7;
        else
            price *= -10;

        res->Cost = price;
        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        auto res = MakeResult();
        res->Position.x = _origin.x + 16;
        res->Position.y = _origin.y + 16;
        res->Position.z = _origin.z;
        res->Expenditure = ExpenditureType::RideConstruction;

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
        TileElement* tileElement = map_get_first_element_at(_origin);

        do
        {
            if (tileElement == nullptr)
                break;

            if (tileElement->GetBaseZ() != _origin.z)
                continue;

            if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
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
            log_warning(
                "Track Element not found. x = %d, y = %d, z = %d, d = %d, seq = %d.", _origin.x, _origin.y, _origin.z,
                _origin.direction, _sequence);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS);
        }

        ride_id_t rideIndex = tileElement->AsTrack()->GetRideIndex();
        auto trackType = tileElement->AsTrack()->GetTrackType();
        bool isLiftHill = tileElement->AsTrack()->HasChain();

        auto ride = get_ride(rideIndex);
        if (ride == nullptr)
        {
            log_warning("Ride not found. ride index = %d.", rideIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS);
        }
        const rct_preview_track* trackBlock = get_track_def_from_ride(ride, trackType);
        trackBlock += tileElement->AsTrack()->GetSequenceIndex();

        auto startLoc = _origin;
        startLoc.direction = tileElement->GetDirection();

        auto rotatedTrackLoc = CoordsXYZ{ CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(startLoc.direction), trackBlock->z };
        startLoc.x -= rotatedTrackLoc.x;
        startLoc.y -= rotatedTrackLoc.y;
        startLoc.z -= rotatedTrackLoc.z;
        res->Position.x = startLoc.x;
        res->Position.y = startLoc.y;
        res->Position.z = startLoc.z;
        money32 cost = 0;

        trackBlock = get_track_def_from_ride(ride, trackType);
        for (; trackBlock->index != 255; trackBlock++)
        {
            rotatedTrackLoc = CoordsXYZ{ CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(startLoc.direction), trackBlock->z };
            auto mapLoc = CoordsXYZ{ startLoc.x, startLoc.y, startLoc.z } + rotatedTrackLoc;

            map_invalidate_tile_full(mapLoc);

            found = false;
            tileElement = map_get_first_element_at(mapLoc);
            do
            {
                if (tileElement == nullptr)
                    break;

                if (tileElement->GetBaseZ() != mapLoc.z)
                    continue;

                if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
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
                log_warning(
                    "Track Element not found. x = %d, y = %d, z = %d, d = %d, seq = %d.", mapLoc.x, mapLoc.y, mapLoc.z,
                    _origin.direction, trackBlock->index);
                return MakeResult(GA_ERROR::UNKNOWN, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS);
            }

            int32_t entranceDirections;
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
            {
                entranceDirections = FlatRideTrackSequenceProperties[trackType][0];
            }
            else
            {
                entranceDirections = TrackSequenceProperties[trackType][0];
            }

            if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN && (tileElement->AsTrack()->GetSequenceIndex() == 0))
            {
                if (!track_remove_station_element({ mapLoc, _origin.direction }, rideIndex, 0))
                {
                    return MakeResult(GA_ERROR::UNKNOWN, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, gGameCommandErrorText);
                }
            }

            auto* surfaceElement = map_get_surface_element_at(mapLoc);
            if (surfaceElement == nullptr)
            {
                log_warning("Surface Element not found. x = %d, y = %d", mapLoc.x, mapLoc.y);
                return MakeResult(GA_ERROR::UNKNOWN, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS);
            }

            int8_t _support_height = tileElement->base_height - surfaceElement->base_height;
            if (_support_height < 0)
            {
                _support_height = 10;
            }

            cost += (_support_height / 2) * RideTypeDescriptors[ride->type].BuildCosts.SupportPrice;

            if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN && (tileElement->AsTrack()->GetSequenceIndex() == 0))
            {
                if (!track_remove_station_element({ mapLoc, _origin.direction }, rideIndex, GAME_COMMAND_FLAG_APPLY))
                {
                    return MakeResult(GA_ERROR::UNKNOWN, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, gGameCommandErrorText);
                }
            }

            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER))
            {
                surfaceElement->SetHasTrackThatNeedsWater(false);
            }

            invalidate_test_results(ride);
            footpath_queue_chain_reset();
            if (!gCheatsDisableClearanceChecks || !(tileElement->IsGhost()))
            {
                footpath_remove_edges_at(mapLoc, tileElement);
            }
            tile_element_remove(tileElement);
            sub_6CB945(ride);
            if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
            {
                ride->UpdateMaxVehicles();
            }
        }

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
                    ride->mode = RIDE_MODE_CONTINUOUS_CIRCUIT;
                    if (ride->type == RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER)
                    {
                        ride->mode = RIDE_MODE_POWERED_LAUNCH;
                    }
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

        money32 price = RideTypeDescriptors[ride->type].BuildCosts.TrackPrice;
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
        {
            price *= FlatRideTrackPricing[trackType];
        }
        else
        {
            price *= TrackPricing[trackType];
        }
        price >>= 16;
        price = (price + cost) / 2;
        if (ride->lifecycle_flags & RIDE_LIFECYCLE_EVER_BEEN_OPENED)
            price *= -7;
        else
            price *= -10;

        res->Cost = price;
        return res;
    }
};
