/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackPlaceAction.h"

#include "../Diagnostic.h"
#include "../GameState.h"
#include "../core/Money.hpp"
#include "../core/Numerics.hpp"
#include "../management/Finance.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../ride/TrackDesign.h"
#include "../world/ConstructionClearance.h"
#include "../world/Footpath.h"
#include "../world/Map.h"
#include "../world/MapAnimation.h"
#include "../world/QuarterTile.h"
#include "../world/Wall.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/Slope.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TrackElement.h"
#include "ResultWithMessage.h"
#include "RideSetSettingAction.h"

namespace OpenRCT2::GameActions
{
    using namespace OpenRCT2::Numerics;
    using namespace OpenRCT2::TrackMetaData;

    TrackPlaceAction::TrackPlaceAction(
        RideId rideIndex, TrackElemType trackType, ride_type_t rideType, const CoordsXYZD& origin, int32_t brakeSpeed,
        int32_t colour, int32_t seatRotation, SelectedLiftAndInverted liftHillAndAlternativeState, bool fromTrackDesign)
        : _rideIndex(rideIndex)
        , _trackType(trackType)
        , _rideType(rideType)
        , _origin(origin)
        , _brakeSpeed(brakeSpeed)
        , _colour(colour)
        , _seatRotation(seatRotation)
        , _trackPlaceFlags(liftHillAndAlternativeState)
        , _fromTrackDesign(fromTrackDesign)
    {
        _origin.direction &= 3;
    }

    void TrackPlaceAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit(_origin);
        visitor.Visit("ride", _rideIndex);
        visitor.Visit("trackType", _trackType);
        visitor.Visit("rideType", _rideType);
        visitor.Visit("brakeSpeed", _brakeSpeed);
        visitor.Visit("colour", _colour);
        visitor.Visit("seatRotation", _seatRotation);
        visitor.Visit("trackPlaceFlags", _trackPlaceFlags.holder);
        visitor.Visit("isFromTrackDesign", _fromTrackDesign);
    }

    uint16_t TrackPlaceAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags();
    }

    void TrackPlaceAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_rideIndex) << DS_TAG(_trackType) << DS_TAG(_rideType) << DS_TAG(_origin) << DS_TAG(_brakeSpeed)
               << DS_TAG(_colour) << DS_TAG(_seatRotation) << DS_TAG(_trackPlaceFlags.holder);
    }

    Result TrackPlaceAction::Query(GameState_t& gameState) const
    {
        auto ride = GetRide(_rideIndex);
        if (ride == nullptr)
        {
            LOG_ERROR("Ride not found for rideIndex %d", _rideIndex.ToUnderlying());
            return Result(Status::invalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_ERR_RIDE_NOT_FOUND);
        }
        const auto* rideEntry = GetRideEntryByIndex(ride->subtype);
        if (rideEntry == nullptr)
        {
            LOG_ERROR("Invalid ride subtype for track placement, rideIndex = %d", _rideIndex.ToUnderlying());
            return Result(Status::invalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_UNKNOWN_OBJECT_TYPE);
        }

        if (!DirectionValid(_origin.direction))
        {
            LOG_ERROR("Invalid direction for track placement, direction = %d", _origin.direction);
            return Result(
                Status::invalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        if (_rideType != ride->type && !gameState.cheats.allowArbitraryRideTypeChanges)
        {
            return Result(Status::invalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, kStringIdNone);
        }

        if (_rideType > RIDE_TYPE_COUNT)
        {
            LOG_ERROR("Invalid ride type for track placement, rideType = %d", _rideType);
            return Result(
                Status::invalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        if (_brakeSpeed > kMaximumTrackSpeed)
        {
            LOG_WARNING("Invalid speed for track placement, speed = %d", _brakeSpeed);
            return Result(Status::invalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_SPEED_TOO_HIGH);
        }

        auto res = Result();
        res.expenditure = ExpenditureType::rideConstruction;
        res.position.x = _origin.x + 16;
        res.position.y = _origin.y + 16;
        res.position.z = _origin.z;

        auto resultData = TrackPlaceActionResult{};

        const auto& rtd = ride->getRideTypeDescriptor();

        if ((ride->lifecycleFlags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK) && _trackType == TrackElemType::endStation)
        {
            return Result(
                Status::disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_NOT_ALLOWED_TO_MODIFY_STATION);
        }

        if (!(GetActionFlags() & Flags::AllowWhilePaused))
        {
            if (GameIsPaused() && !gameState.cheats.buildInPauseMode)
            {
                return Result(
                    Status::disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                    STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED);
            }
        }

        if (!rtd.HasFlag(RtdFlag::isFlatRide))
        {
            if (_trackType == TrackElemType::onRidePhoto)
            {
                if (ride->lifecycleFlags & RIDE_LIFECYCLE_ON_RIDE_PHOTO)
                {
                    return Result(
                        Status::disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                        STR_ONLY_ONE_ON_RIDE_PHOTO_PER_RIDE);
                }
            }
            else if (_trackType == TrackElemType::cableLiftHill)
            {
                if (ride->lifecycleFlags & RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED)
                {
                    return Result(
                        Status::disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                        STR_ONLY_ONE_CABLE_LIFT_HILL_PER_RIDE);
                }
            }
            // Backwards steep lift hills are allowed, even on roller coasters that do not support forwards steep lift hills.
            if (_trackPlaceFlags.has(LiftHillAndInverted::liftHill) && !rtd.SupportsTrackGroup(TrackGroup::liftHillSteep)
                && !gameState.cheats.enableChainLiftOnAllTrack)
            {
                const auto& ted = GetTrackElementDescriptor(_trackType);
                if (ted.flags.has(TrackElementFlag::isSteepUp))
                {
                    return Result(
                        Status::disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_TOO_STEEP_FOR_LIFT_HILL);
                }
            }
        }

        const auto& ted = GetTrackElementDescriptor(_trackType);
        uint32_t numElements = 0;
        // First check if any of the track pieces are outside the park
        for (uint8_t i = 0; i < ted.numSequences; i++)
        {
            const auto& trackBlock = ted.sequences[i].clearance;
            auto rotatedTrack = CoordsXYZ{ CoordsXY{ trackBlock.x, trackBlock.y }.Rotate(_origin.direction), 0 };
            auto tileCoords = CoordsXYZ{ _origin.x, _origin.y, _origin.z } + rotatedTrack;

            if (!LocationValid(tileCoords))
            {
                return Result(Status::invalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_OFF_EDGE_OF_MAP);
            }
            if (!MapIsLocationOwned(tileCoords) && !gameState.cheats.sandboxMode)
            {
                return Result(Status::disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_LAND_NOT_OWNED_BY_PARK);
            }
            numElements++;
        }

        if (!CheckMapCapacity(numElements))
        {
            LOG_ERROR("Not enough free map elements to place track.");
            return Result(
                Status::noFreeElements, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_TILE_ELEMENT_LIMIT_REACHED);
        }

        if (!gameState.cheats.allowTrackPlaceInvalidHeights)
        {
            if (ted.flags.has(TrackElementFlag::startsAtHalfHeight))
            {
                if ((_origin.z & 0x0F) != 8)
                {
                    return Result(
                        Status::invalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_INVALID_HEIGHT);
                }
            }
            else
            {
                if ((_origin.z & 0x0F) != 0)
                {
                    return Result(
                        Status::invalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_INVALID_HEIGHT);
                }
            }
        }

        // If that is not the case, then perform the remaining checks
        auto clearanceHeight = rideEntry->Clearance;

        money64 costs = 0;
        money64 supportCosts = 0;
        for (int32_t blockIndex = 0; blockIndex < ted.numSequences; blockIndex++)
        {
            const auto& trackBlock = ted.sequences[blockIndex].clearance;
            auto rotatedTrack = CoordsXYZ{ CoordsXY{ trackBlock.x, trackBlock.y }.Rotate(_origin.direction), trackBlock.z };
            auto mapLoc = CoordsXYZ{ _origin.x, _origin.y, _origin.z } + rotatedTrack;
            auto quarterTile = trackBlock.quarterTile.Rotate(_origin.direction);

            if (mapLoc.z < 16)
            {
                return Result(Status::invalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_TOO_LOW);
            }

            int32_t baseZ = floor2(mapLoc.z, kCoordsZStep);

            int32_t clearanceZ = trackBlock.clearanceZ;
            if (trackBlock.flags & RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL && clearanceHeight > 24)
            {
                clearanceZ += 24;
            }
            else
            {
                clearanceZ += clearanceHeight;
            }

            clearanceZ = floor2(clearanceZ, kCoordsZStep) + baseZ;

            if (clearanceZ > kMaximumTrackHeight)
            {
                return Result(Status::invalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_TOO_HIGH);
            }

            auto crossingMode = (rtd.HasFlag(RtdFlag::supportsLevelCrossings) && _trackType == TrackElemType::flat)
                ? CreateCrossingMode::trackOverPath
                : CreateCrossingMode::none;
            // When placing from a track design, ignore track elements from the same ride to allow it to intersect itself.
            auto ignoreRideId = _fromTrackDesign ? _rideIndex : RideId::GetNull();
            auto canBuild = MapCanConstructWithClearAt(
                { mapLoc, baseZ, clearanceZ }, MapPlaceNonSceneryClearFunc, quarterTile, GetFlags(), kTileSlopeFlat,
                crossingMode, false, ignoreRideId);
            if (canBuild.error != Status::ok)
            {
                canBuild.errorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
                return canBuild;
            }
            costs += canBuild.cost;

            const auto clearanceData = canBuild.getData<ConstructClearResult>();
            uint8_t mapGroundFlags = clearanceData.GroundFlags & (ELEMENT_IS_ABOVE_GROUND | ELEMENT_IS_UNDERGROUND);
            if (!ted.flags.has(TrackElementFlag::canBePartlyUnderground))
            {
                if (resultData.GroundFlags != 0 && (resultData.GroundFlags & mapGroundFlags) == 0)
                {
                    return Result(
                        Status::disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                        STR_CANT_BUILD_PARTLY_ABOVE_AND_PARTLY_BELOW_GROUND);
                }
            }

            resultData.GroundFlags = mapGroundFlags;
            if (ted.flags.has(TrackElementFlag::onlyAboveGround))
            {
                if (resultData.GroundFlags & ELEMENT_IS_UNDERGROUND)
                {
                    return Result(
                        Status::disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                        STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND);
                }
            }

            if (ted.flags.has(TrackElementFlag::onlyUnderwater))
            { // No element has this flag
                if (clearanceData.GroundFlags & ELEMENT_IS_UNDERWATER)
                {
                    return Result(
                        Status::disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_CAN_ONLY_BUILD_THIS_UNDERWATER);
                }
            }

            if (clearanceData.GroundFlags & ELEMENT_IS_UNDERWATER && !gameState.cheats.disableClearanceChecks)
            {
                return Result(
                    Status::disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_RIDE_CANT_BUILD_THIS_UNDERWATER);
            }

            if (rtd.HasFlag(RtdFlag::trackMustBeOnWater) && !_trackDesignDrawingPreview)
            {
                auto surfaceElement = MapGetSurfaceElementAt(mapLoc);
                if (surfaceElement == nullptr)
                {
                    return Result(
                        Status::unknown, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_ERR_SURFACE_ELEMENT_NOT_FOUND);
                }

                auto waterHeight = surfaceElement->GetWaterHeight();
                if (waterHeight == 0)
                {
                    return Result(
                        Status::disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_CAN_ONLY_BUILD_THIS_ON_WATER);
                }

                if (waterHeight != baseZ)
                {
                    return Result(
                        Status::disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_CAN_ONLY_BUILD_THIS_ON_WATER);
                }
                waterHeight -= kLandHeightStep;
                if (waterHeight == surfaceElement->GetBaseZ())
                {
                    uint8_t slope = surfaceElement->GetSlope() & kTileSlopeRaisedCornersMask;
                    if (slope == kTileSlopeWCornerDown || slope == kTileSlopeSCornerDown || slope == kTileSlopeECornerDown
                        || slope == kTileSlopeNCornerDown)
                    {
                        return Result(
                            Status::disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                            STR_CAN_ONLY_BUILD_THIS_ON_WATER);
                    }
                }
            }

            if (ted.sequences[0].flags.has(SequenceFlag::trackOrigin) && blockIndex == 0)
            {
                const auto addElementResult = TrackAddStationElement(
                    { mapLoc, baseZ, _origin.direction }, _rideIndex, {}, _fromTrackDesign);
                if (!addElementResult.Successful)
                {
                    return Result(Status::unknown, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, addElementResult.Message);
                }
            }

            // 6c5648 12 push
            auto surfaceElement = MapGetSurfaceElementAt(mapLoc);
            if (surfaceElement == nullptr)
            {
                return Result(
                    Status::unknown, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_ERR_SURFACE_ELEMENT_NOT_FOUND);
            }

            if (!gameState.cheats.disableSupportLimits)
            {
                int32_t ride_height = clearanceZ - surfaceElement->GetBaseZ();
                if (ride_height >= 0)
                {
                    uint16_t maxHeight;

                    if (rtd.HasFlag(RtdFlag::listVehiclesSeparately) && rideEntry->maxHeight != 0)
                    {
                        maxHeight = rideEntry->maxHeight;
                    }
                    else
                    {
                        maxHeight = rtd.Heights.MaxHeight;
                    }

                    ride_height /= kCoordsZPerTinyZ;
                    if (ride_height > maxHeight && !_trackDesignDrawingPreview)
                    {
                        return Result(
                            Status::disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_TOO_HIGH_FOR_SUPPORTS);
                    }
                }
            }

            int32_t supportHeight = baseZ - surfaceElement->GetBaseZ();
            if (supportHeight < 0)
            {
                supportHeight = (10 * kCoordsZStep);
            }

            supportCosts += ((supportHeight / (2 * kCoordsZStep)) * rtd.BuildCosts.SupportPrice);
        }

        money64 price = rtd.BuildCosts.TrackPrice;
        price *= ted.priceModifier;

        price >>= 16;
        res.cost = costs + supportCosts + price;
        res.setData(std::move(resultData));

        return res;
    }

    Result TrackPlaceAction::Execute(GameState_t& gameState) const
    {
        auto ride = GetRide(_rideIndex);
        if (ride == nullptr)
        {
            LOG_ERROR("Invalid ride for track placement, rideIndex = %d", _rideIndex.ToUnderlying());
            return Result(Status::invalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_ERR_RIDE_NOT_FOUND);
        }

        const auto* rideEntry = GetRideEntryByIndex(ride->subtype);
        if (rideEntry == nullptr)
        {
            LOG_ERROR("Invalid ride subtype for track placement, rideIndex = %d", _rideIndex.ToUnderlying());
            return Result(Status::invalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_UNKNOWN_OBJECT_TYPE);
        }

        auto res = Result();
        res.expenditure = ExpenditureType::rideConstruction;
        res.position.x = _origin.x + 16;
        res.position.y = _origin.y + 16;
        res.position.z = _origin.z;

        auto resultData = TrackPlaceActionResult{};

        const auto& rtd = ride->getRideTypeDescriptor();

        const auto& ted = GetTrackElementDescriptor(_trackType);

        money64 costs = 0;
        money64 supportCosts = 0;
        const auto& block0 = ted.sequences[0].clearance;
        auto clearanceHeight = rideEntry->Clearance;
        CoordsXYZ originLocation = CoordsXYZ{ _origin.x, _origin.y, _origin.z }
            + CoordsXYZ{ CoordsXY{ block0.x, block0.y }.Rotate(_origin.direction), block0.z };
        for (int32_t blockIndex = 0; blockIndex < ted.numSequences; blockIndex++)
        {
            const auto& trackBlock = ted.sequences[blockIndex].clearance;
            auto rotatedTrack = CoordsXYZ{ CoordsXY{ trackBlock.x, trackBlock.y }.Rotate(_origin.direction), trackBlock.z };
            auto mapLoc = CoordsXYZ{ _origin.x, _origin.y, _origin.z } + rotatedTrack;

            auto quarterTile = trackBlock.quarterTile.Rotate(_origin.direction);

            int32_t baseZ = floor2(mapLoc.z, kCoordsZStep);
            int32_t clearanceZ = trackBlock.clearanceZ;
            if (trackBlock.flags & RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL && clearanceHeight > 24)
            {
                clearanceZ += 24;
            }
            else
            {
                clearanceZ += clearanceHeight;
            }

            clearanceZ = floor2(clearanceZ, kCoordsZStep) + baseZ;
            const auto mapLocWithClearance = CoordsXYRangedZ(mapLoc, baseZ, clearanceZ);

            auto crossingMode = (rtd.HasFlag(RtdFlag::supportsLevelCrossings) && _trackType == TrackElemType::flat)
                ? CreateCrossingMode::trackOverPath
                : CreateCrossingMode::none;
            // When placing from a track design, ignore track elements from the same ride to allow it to intersect itself.
            auto ignoreRideId = _fromTrackDesign ? _rideIndex : RideId::GetNull();
            auto canBuild = MapCanConstructWithClearAt(
                mapLocWithClearance, MapPlaceNonSceneryClearFunc, quarterTile, GetFlags().with(CommandFlag::apply),
                kTileSlopeFlat, crossingMode, false, ignoreRideId);
            if (canBuild.error != Status::ok)
            {
                canBuild.errorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
                return canBuild;
            }
            costs += canBuild.cost;

            // When building a level crossing, remove any pre-existing path furniture.
            if (crossingMode == CreateCrossingMode::trackOverPath && !GetFlags().has(CommandFlag::ghost))
            {
                auto footpathElement = MapGetFootpathElement(mapLoc);
                if (footpathElement != nullptr && footpathElement->HasAddition())
                {
                    footpathElement->SetAddition(0);
                }
            }

            if (!GetFlags().has(CommandFlag::ghost) && !gameState.cheats.disableClearanceChecks)
            {
                FootpathRemoveLitter(mapLoc);
                if (rtd.HasFlag(RtdFlag::noWallsAroundTrack))
                {
                    WallRemoveAt(mapLocWithClearance);
                }
                else
                {
                    // Remove walls in the directions this track intersects
                    uint8_t intersectingDirections = ted.sequences[blockIndex].allowedWallEdges;
                    intersectingDirections ^= 0x0F;
                    intersectingDirections = rol4(intersectingDirections, _origin.direction);
                    for (int32_t i = 0; i < kNumOrthogonalDirections; i++)
                    {
                        if (intersectingDirections & (1 << i))
                        {
                            WallRemoveIntersectingWalls(mapLocWithClearance, i);
                        }
                    }
                }
            }

            const auto clearanceData = canBuild.getData<ConstructClearResult>();
            uint8_t mapGroundFlags = clearanceData.GroundFlags & (ELEMENT_IS_ABOVE_GROUND | ELEMENT_IS_UNDERGROUND);
            if (!ted.flags.has(TrackElementFlag::canBePartlyUnderground))
            {
                if (resultData.GroundFlags != 0 && (resultData.GroundFlags & mapGroundFlags) == 0)
                {
                    return Result(
                        Status::disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                        STR_CANT_BUILD_PARTLY_ABOVE_AND_PARTLY_BELOW_GROUND);
                }
            }

            resultData.GroundFlags = mapGroundFlags;

            // 6c5648 12 push
            auto surfaceElement = MapGetSurfaceElementAt(mapLoc);
            if (surfaceElement == nullptr)
            {
                return Result(
                    Status::unknown, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_ERR_SURFACE_ELEMENT_NOT_FOUND);
            }

            int32_t supportHeight = baseZ - surfaceElement->GetBaseZ();
            if (supportHeight < 0)
            {
                supportHeight = (10 * kCoordsZStep);
            }

            supportCosts += (supportHeight / (2 * kCoordsZStep)) * rtd.BuildCosts.SupportPrice;

            bool isOrigin = false;
            if (!ride->overallView.IsNull())
            {
                if (!GetFlags().has(CommandFlag::noSpend))
                {
                    isOrigin = ted.sequences[0].flags.has(SequenceFlag::trackOrigin);
                }
            }

            if (isOrigin || ride->overallView.IsNull())
            {
                ride->overallView = mapLoc;
            }

            auto* trackElement = TileElementInsert<TrackElement>(mapLoc, quarterTile.GetBaseQuarterOccupied());
            if (trackElement == nullptr)
            {
                LOG_ERROR("Cannot create track element for ride = %d", _rideIndex.ToUnderlying());
                return Result(
                    Status::noFreeElements, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_TILE_ELEMENT_LIMIT_REACHED);
            }

            trackElement->SetClearanceZ(clearanceZ);
            trackElement->SetDirection(_origin.direction);
            trackElement->SetHasChain(_trackPlaceFlags.has(LiftHillAndInverted::liftHill));
            trackElement->SetSequenceIndex(blockIndex);
            trackElement->SetRideIndex(_rideIndex);
            trackElement->SetTrackType(_trackType);
            trackElement->SetRideType(_rideType);
            trackElement->SetGhost(GetFlags().has(CommandFlag::ghost));

            switch (_trackType)
            {
                case TrackElemType::waterfall:
                case TrackElemType::rapids:
                case TrackElemType::whirlpool:
                case TrackElemType::spinningTunnel:
                    MapAnimations::MarkTileForInvalidation(TileCoordsXY(mapLoc));
                    break;
                case TrackElemType::brakes:
                case TrackElemType::diagBrakes:
                    trackElement->SetBrakeClosed(true);
                    break;
                default:
                    break;
            }
            if (TrackTypeHasSpeedSetting(_trackType))
            {
                trackElement->SetBrakeBoosterSpeed(_brakeSpeed);
            }

            if (rtd.HasFlag(RtdFlag::hasLandscapeDoors))
            {
                trackElement->SetDoorAState(kLandEdgeDoorFrameClosed);
                trackElement->SetDoorBState(kLandEdgeDoorFrameClosed);
            }
            else
            {
                trackElement->SetSeatRotation(_seatRotation);
            }

            if (_trackPlaceFlags.has(LiftHillAndInverted::inverted))
            {
                trackElement->SetInverted(true);
            }
            trackElement->SetColourScheme(static_cast<RideColourScheme>(_colour));

            if (ted.sequences[0].flags.has(SequenceFlag::connectsToPath))
            {
                uint32_t connectionSides = ted.sequences[0].getEntranceConnectionSides();
                if (connectionSides != 0)
                {
                    if (!GetFlags().has(CommandFlag::ghost) && !gameState.cheats.disableClearanceChecks)
                    {
                        for (int32_t chosenDirection = bitScanForward(connectionSides); chosenDirection != -1;
                             chosenDirection = bitScanForward(connectionSides))
                        {
                            connectionSides &= ~(1 << chosenDirection);
                            CoordsXY tempLoc{ mapLoc.x, mapLoc.y };
                            int32_t tempDirection = (_origin.direction + chosenDirection) & 3;
                            tempLoc.x += CoordsDirectionDelta[tempDirection].x;
                            tempLoc.y += CoordsDirectionDelta[tempDirection].y;
                            tempDirection = DirectionReverse(tempDirection);
                            WallRemoveIntersectingWalls({ tempLoc, baseZ, clearanceZ }, tempDirection & 3);
                        }
                    }
                }
            }

            // If the placed tile is a station modify station properties.
            // Don't do this if the tile is a ghost to prevent desyncs
            // However, ghost tiles from track designs need to modify station data to display properly
            if (ted.sequences[0].flags.has(SequenceFlag::trackOrigin)
                && (!GetFlags().has(CommandFlag::ghost) || _fromTrackDesign))
            {
                if (blockIndex == 0)
                {
                    TrackAddStationElement({ mapLoc, _origin.direction }, _rideIndex, { CommandFlag::apply }, _fromTrackDesign);
                }
                ride->validateStations();
                ride->updateMaxVehicles();
            }

            auto* tileElement = trackElement->as<TileElement>();

            if (rtd.HasFlag(RtdFlag::trackMustBeOnWater))
            {
                auto* waterSurfaceElement = MapGetSurfaceElementAt(mapLoc);
                if (waterSurfaceElement != nullptr)
                {
                    waterSurfaceElement->SetHasTrackThatNeedsWater(true);
                    tileElement = waterSurfaceElement->as<TileElement>();
                }
            }

            if (!gameState.cheats.disableClearanceChecks || !GetFlags().has(CommandFlag::ghost))
            {
                FootpathConnectEdges(mapLoc, tileElement, GetFlags());
            }
            MapInvalidateTileFull(mapLoc);
        }

        // Update ride stats and block brake count if the piece was successfully built
        if (!GetFlags().has(CommandFlag::ghost))
        {
            switch (_trackType)
            {
                case TrackElemType::onRidePhoto:
                    ride->lifecycleFlags |= RIDE_LIFECYCLE_ON_RIDE_PHOTO;
                    InvalidateTestResults(*ride);
                    break;
                case TrackElemType::cableLiftHill:
                    ride->lifecycleFlags |= RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED;
                    ride->cableLiftLoc = originLocation;
                    InvalidateTestResults(*ride);
                    break;
                case TrackElemType::diagBlockBrakes:
                case TrackElemType::blockBrakes:
                {
                    ride->numBlockBrakes++;

                    auto newMode = RideModeGetBlockSectionedCounterpart(ride->mode);
                    if (ride->mode != newMode)
                    {
                        bool canSwitch = rtd.SupportsRideMode(newMode) || gameState.cheats.showAllOperatingModes;
                        if (canSwitch)
                        {
                            ride->windowInvalidateFlags.set(RideInvalidateFlag::operatingSettings);
                            auto rideSetSetting = RideSetSettingAction(
                                ride->id, RideSetSetting::Mode, static_cast<uint8_t>(newMode));
                            ExecuteNested(&rideSetSetting, gameState);
                        }
                    }

                    break;
                }
                default:
                    break;
            }

            switch (_trackType)
            {
                case TrackElemType::up25ToFlat:
                case TrackElemType::up60ToFlat:
                case TrackElemType::diagUp25ToFlat:
                case TrackElemType::diagUp60ToFlat:
                    if (!_trackPlaceFlags.has(LiftHillAndInverted::liftHill))
                        break;
                    [[fallthrough]];
                case TrackElemType::cableLiftHill:
                    ride->numBlockBrakes++;
                    break;
                default:
                    break;
            }
        }

        money64 price = rtd.BuildCosts.TrackPrice;
        price *= ted.priceModifier;

        price >>= 16;
        res.cost = costs + supportCosts + price;
        res.setData(std::move(resultData));

        return res;
    }

    bool TrackPlaceAction::CheckMapCapacity(int16_t numTiles) const
    {
        const auto& ted = GetTrackElementDescriptor(_trackType);
        for (uint8_t i = 0; i < ted.numSequences; i++)
        {
            const auto& trackBlock = ted.sequences[i].clearance;
            auto rotatedTrack = CoordsXY{ trackBlock.x, trackBlock.y }.Rotate(_origin.direction);

            auto tileCoords = CoordsXY{ _origin.x, _origin.y } + rotatedTrack;
            if (!MapCheckCapacityAndReorganise(tileCoords, numTiles))
            {
                return false;
            }
        }
        return true;
    }
} // namespace OpenRCT2::GameActions
