/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackRemoveAction.h"

#include "../Diagnostic.h"
#include "../GameState.h"
#include "../management/Finance.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../ride/TrackDesign.h"
#include "../world/Footpath.h"
#include "../world/Map.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TrackElement.h"
#include "ResultWithMessage.h"
#include "RideSetSettingAction.h"

namespace OpenRCT2::GameActions
{
    using namespace OpenRCT2::TrackMetaData;

    TrackRemoveAction::TrackRemoveAction(TrackElemType trackType, int32_t sequence, const CoordsXYZD& origin)
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

    static TrackElemType normaliseTrackType(TrackElemType trackType)
    {
        switch (trackType)
        {
            case TrackElemType::beginStation:
            case TrackElemType::middleStation:
                return TrackElemType::endStation;
            default:
                return trackType;
        }
    }

    void TrackRemoveAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_trackType) << DS_TAG(_sequence) << DS_TAG(_origin);
    }

    Result TrackRemoveAction::Query(GameState_t& gameState) const
    {
        auto res = Result();
        res.position.x = _origin.x + 16;
        res.position.y = _origin.y + 16;
        res.position.z = _origin.z;
        res.expenditure = ExpenditureType::rideConstruction;

        auto comparableTrackType = normaliseTrackType(_trackType);

        bool found = false;
        bool isGhost = GetFlags().has(CommandFlag::ghost);
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

            auto tileTrackType = normaliseTrackType(tileElement->AsTrack()->GetTrackType());

            if (tileTrackType != comparableTrackType)
                continue;

            found = true;
            break;
        } while (!(tileElement++)->IsLastForTile());

        if (!found)
        {
            LOG_ERROR(
                "Track Element not found. x = %d, y = %d, z = %d, d = %d, seq = %d.", _origin.x, _origin.y, _origin.z,
                _origin.direction, _sequence);
            return Result(Status::invalidParameters, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_ERR_TRACK_ELEMENT_NOT_FOUND);
        }

        if (tileElement->AsTrack()->IsIndestructible())
        {
            return Result(
                Status::disallowed, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_YOU_ARE_NOT_ALLOWED_TO_REMOVE_THIS_SECTION);
        }

        RideId rideIndex = tileElement->AsTrack()->GetRideIndex();
        const auto trackType = tileElement->AsTrack()->GetTrackType();

        auto ride = GetRide(rideIndex);
        if (ride == nullptr)
        {
            LOG_ERROR("Ride not found for rideIndex %d.", rideIndex);
            return Result(Status::invalidParameters, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_ERR_RIDE_NOT_FOUND);
        }

        if (ride->type >= RIDE_TYPE_COUNT)
        {
            LOG_ERROR("Ride type not found. ride type = %d.", ride->type);
            return Result(Status::invalidParameters, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_ERR_VALUE_OUT_OF_RANGE);
        }
        const auto& ted = GetTrackElementDescriptor(trackType);
        auto sequenceIndex = tileElement->AsTrack()->GetSequenceIndex();
        if (sequenceIndex >= ted.numSequences)
        {
            LOG_ERROR("Track block %d not found for track type %d.", sequenceIndex, trackType);
            return Result(Status::invalidParameters, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_ERR_TRACK_BLOCK_NOT_FOUND);
        }
        const auto& currentTrackBlock = ted.sequences[sequenceIndex].clearance;

        auto startLoc = _origin;
        startLoc.direction = tileElement->GetDirection();

        auto rotatedTrack = CoordsXYZ{ CoordsXY{ currentTrackBlock.x, currentTrackBlock.y }.Rotate(startLoc.direction),
                                       currentTrackBlock.z };
        startLoc.x -= rotatedTrack.x;
        startLoc.y -= rotatedTrack.y;
        startLoc.z -= rotatedTrack.z;
        res.position.x = startLoc.x;
        res.position.y = startLoc.y;
        res.position.z = startLoc.z;

        money64 supportCosts = 0;

        for (uint8_t i = 0; i < ted.numSequences; i++)
        {
            const auto& trackBlock = ted.sequences[i].clearance;
            rotatedTrack = CoordsXYZ{ CoordsXY{ trackBlock.x, trackBlock.y }.Rotate(startLoc.direction), trackBlock.z };
            auto mapLoc = CoordsXYZ{ startLoc.x, startLoc.y, startLoc.z } + rotatedTrack;

            if (!LocationValid(mapLoc))
            {
                return Result(Status::invalidParameters, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_OFF_EDGE_OF_MAP);
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

                if (tileElement->AsTrack()->GetSequenceIndex() != i)
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
                LOG_ERROR(
                    "Track Element not found. x = %d, y = %d, z = %d, d = %d, seq = %d.", mapLoc.x, mapLoc.y, mapLoc.z,
                    _origin.direction, i);
                return Result(Status::unknown, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_ERR_TRACK_ELEMENT_NOT_FOUND);
            }

            if (ted.sequences[0].flags.has(SequenceFlag::trackOrigin) && (tileElement->AsTrack()->GetSequenceIndex() == 0))
            {
                const auto removeElementResult = TrackRemoveStationElement({ mapLoc, _origin.direction }, rideIndex, {});
                if (!removeElementResult.Successful)
                {
                    return Result(Status::unknown, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, removeElementResult.Message);
                }
            }

            auto* surfaceElement = MapGetSurfaceElementAt(mapLoc);
            if (surfaceElement == nullptr)
            {
                LOG_ERROR("Surface Element not found. x = %d, y = %d", mapLoc.x, mapLoc.y);
                return Result(Status::unknown, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_ERR_SURFACE_ELEMENT_NOT_FOUND);
            }

            int16_t _support_height = tileElement->BaseHeight - surfaceElement->BaseHeight;
            if (_support_height < 0)
            {
                _support_height = 10;
            }

            supportCosts += (_support_height / 2) * ride->getRideTypeDescriptor().BuildCosts.SupportPrice;
        }

        money64 price = ride->getRideTypeDescriptor().BuildCosts.TrackPrice;
        price *= ted.priceModifier;
        price >>= 16;
        price = supportCosts + price;
        if (ride->lifecycleFlags & RIDE_LIFECYCLE_EVER_BEEN_OPENED)
        {
            // 70% modifier for opened rides
            price = (price * 45875) / 65536;
        }

        res.cost = -price;
        return res;
    }

    Result TrackRemoveAction::Execute(GameState_t& gameState) const
    {
        auto res = Result();
        res.position.x = _origin.x + 16;
        res.position.y = _origin.y + 16;
        res.position.z = _origin.z;
        res.expenditure = ExpenditureType::rideConstruction;

        auto comparableTrackType = normaliseTrackType(_trackType);

        bool found = false;
        bool isGhost = GetFlags().has(CommandFlag::ghost);
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

            auto tileTrackType = normaliseTrackType(tileElement->AsTrack()->GetTrackType());

            if (tileTrackType != comparableTrackType)
                continue;

            found = true;
            break;
        } while (!(tileElement++)->IsLastForTile());

        if (!found)
        {
            LOG_ERROR(
                "Track Element not found. x = %d, y = %d, z = %d, d = %d, seq = %d.", _origin.x, _origin.y, _origin.z,
                _origin.direction, _sequence);
            return Result(Status::invalidParameters, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_ERR_TRACK_ELEMENT_NOT_FOUND);
        }

        RideId rideIndex = tileElement->AsTrack()->GetRideIndex();
        const auto trackType = tileElement->AsTrack()->GetTrackType();
        bool isLiftHill = tileElement->AsTrack()->HasChain();

        auto ride = GetRide(rideIndex);
        if (ride == nullptr)
        {
            LOG_ERROR("Ride not found. ride index = %d.", rideIndex);
            return Result(Status::invalidParameters, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_ERR_RIDE_NOT_FOUND);
        }
        const auto& ted = GetTrackElementDescriptor(trackType);
        auto sequenceIndex = tileElement->AsTrack()->GetSequenceIndex();
        if (sequenceIndex >= ted.numSequences)
        {
            LOG_ERROR("Track block %d not found for track type %d.", sequenceIndex, trackType);
            return Result(Status::invalidParameters, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_ERR_TRACK_BLOCK_NOT_FOUND);
        }

        auto startLoc = _origin;
        startLoc.direction = tileElement->GetDirection();

        const auto& currentTrackBlock = ted.sequences[sequenceIndex].clearance;
        auto rotatedTrackLoc = CoordsXYZ{ CoordsXY{ currentTrackBlock.x, currentTrackBlock.y }.Rotate(startLoc.direction),
                                          currentTrackBlock.z };
        startLoc.x -= rotatedTrackLoc.x;
        startLoc.y -= rotatedTrackLoc.y;
        startLoc.z -= rotatedTrackLoc.z;
        res.position.x = startLoc.x;
        res.position.y = startLoc.y;
        res.position.z = startLoc.z;

        money64 supportCosts = 0;

        for (uint8_t i = 0; i < ted.numSequences; i++)
        {
            const auto& trackBlock = ted.sequences[i].clearance;

            rotatedTrackLoc = CoordsXYZ{ CoordsXY{ trackBlock.x, trackBlock.y }.Rotate(startLoc.direction), trackBlock.z };
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

                if (tileElement->AsTrack()->GetSequenceIndex() != i)
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
                LOG_ERROR(
                    "Track Element not found. x = %d, y = %d, z = %d, d = %d, seq = %d.", mapLoc.x, mapLoc.y, mapLoc.z,
                    _origin.direction, i);
                return Result(Status::unknown, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_ERR_TRACK_ELEMENT_NOT_FOUND);
            }

            if (ted.sequences[0].flags.has(SequenceFlag::trackOrigin) && (tileElement->AsTrack()->GetSequenceIndex() == 0))
            {
                const auto removeElementResult = TrackRemoveStationElement({ mapLoc, _origin.direction }, rideIndex, {});
                if (!removeElementResult.Successful)
                {
                    return Result(Status::unknown, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, removeElementResult.Message);
                }
            }

            auto* surfaceElement = MapGetSurfaceElementAt(mapLoc);
            if (surfaceElement == nullptr)
            {
                LOG_ERROR("Surface Element not found. x = %d, y = %d", mapLoc.x, mapLoc.y);
                return Result(Status::unknown, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_ERR_SURFACE_ELEMENT_NOT_FOUND);
            }

            int16_t _support_height = tileElement->BaseHeight - surfaceElement->BaseHeight;
            if (_support_height < 0)
            {
                _support_height = 10;
            }

            supportCosts += (_support_height / 2) * ride->getRideTypeDescriptor().BuildCosts.SupportPrice;

            // If the removed tile is a station modify station properties.
            // Don't do this if the ride is simulating and the tile is a ghost to prevent desyncs.
            if (ted.sequences[0].flags.has(SequenceFlag::trackOrigin)
                && (!GetFlags().has(CommandFlag::ghost) || (GetFlags().has(CommandFlag::trackDesign)))
                && (tileElement->AsTrack()->GetSequenceIndex() == 0))
            {
                const auto removeElementResult = TrackRemoveStationElement(
                    { mapLoc, _origin.direction }, rideIndex, { CommandFlag::apply });
                if (!removeElementResult.Successful)
                {
                    return Result(Status::unknown, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, removeElementResult.Message);
                }
            }

            if (ride->getRideTypeDescriptor().HasFlag(RtdFlag::trackMustBeOnWater))
            {
                surfaceElement->SetHasTrackThatNeedsWater(false);
            }

            InvalidateTestResults(*ride);
            FootpathQueueChainReset();
            if (!gameState.cheats.disableClearanceChecks || !(tileElement->IsGhost()))
            {
                FootpathRemoveEdgesAt(mapLoc, tileElement);
            }
            TileElementRemove(tileElement);
            ride->validateStations();
            if (!GetFlags().has(CommandFlag::ghost))
            {
                ride->updateMaxVehicles();
            }
        }

        if (!GetFlags().has(CommandFlag::ghost))
        {
            switch (trackType)
            {
                case TrackElemType::onRidePhoto:
                    ride->lifecycleFlags &= ~RIDE_LIFECYCLE_ON_RIDE_PHOTO;
                    break;
                case TrackElemType::cableLiftHill:
                    ride->lifecycleFlags &= ~RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED;
                    break;
                case TrackElemType::blockBrakes:
                case TrackElemType::diagBlockBrakes:
                    ride->numBlockBrakes--;
                    if (ride->numBlockBrakes == 0 && ride->isBlockSectioned())
                    {
                        RideMode newMode = RideMode::continuousCircuit;
                        if (ride->mode == RideMode::poweredLaunchBlockSectioned)
                        {
                            // Depending on the ride, the equivalent will be powered launch with or without passing station.
                            newMode = ride->getRideTypeDescriptor().DefaultMode;
                        }

                        if (ride->mode != newMode)
                        {
                            bool canSwitch = ride->getRideTypeDescriptor().SupportsRideMode(newMode)
                                || gameState.cheats.showAllOperatingModes;
                            if (canSwitch)
                            {
                                ride->windowInvalidateFlags.set(RideInvalidateFlag::operatingSettings);
                                auto rideSetSetting = RideSetSettingAction(
                                    ride->id, RideSetSetting::Mode, static_cast<uint8_t>(newMode));
                                ExecuteNested(&rideSetSetting, gameState);
                            }
                        }
                    }

                    break;
                default:
                    break;
            }

            switch (trackType)
            {
                case TrackElemType::up25ToFlat:
                case TrackElemType::up60ToFlat:
                case TrackElemType::diagUp25ToFlat:
                case TrackElemType::diagUp60ToFlat:
                    if (!isLiftHill)
                        break;
                    [[fallthrough]];
                case TrackElemType::cableLiftHill:
                    ride->numBlockBrakes--;
                    break;
                default:
                    break;
            }
        }

        money64 price = ride->getRideTypeDescriptor().BuildCosts.TrackPrice;
        price *= ted.priceModifier;
        price >>= 16;
        price = supportCosts + price;
        if (ride->lifecycleFlags & RIDE_LIFECYCLE_EVER_BEEN_OPENED)
        {
            // 70% modifier for opened rides
            price = (price * 45875) / 65536;
        }

        res.cost = -price;
        return res;
    }
} // namespace OpenRCT2::GameActions
