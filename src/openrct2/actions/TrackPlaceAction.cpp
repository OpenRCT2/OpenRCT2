/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackPlaceAction.h"

#include "../core/Numerics.hpp"
#include "../management/Finance.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../ride/TrackDesign.h"
#include "../util/Math.hpp"
#include "../world/ConstructionClearance.h"
#include "../world/MapAnimation.h"
#include "../world/Surface.h"
#include "RideSetSettingAction.h"

using namespace OpenRCT2::TrackMetaData;

TrackPlaceAction::TrackPlaceAction(
    RideId rideIndex, int32_t trackType, ride_type_t rideType, const CoordsXYZD& origin, int32_t brakeSpeed, int32_t colour,
    int32_t seatRotation, int32_t liftHillAndAlternativeState, bool fromTrackDesign)
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
    visitor.Visit("trackPlaceFlags", _trackPlaceFlags);
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
           << DS_TAG(_colour) << DS_TAG(_seatRotation) << DS_TAG(_trackPlaceFlags);
}

GameActions::Result TrackPlaceAction::Query() const
{
    auto ride = GetRide(_rideIndex);
    if (ride == nullptr)
    {
        LOG_WARNING("Invalid ride for track placement, rideIndex = %d", _rideIndex.ToUnderlying());
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_NONE);
    }
    const auto* rideEntry = GetRideEntryByIndex(ride->subtype);
    if (rideEntry == nullptr)
    {
        LOG_WARNING("Invalid ride subtype for track placement, rideIndex = %d", _rideIndex.ToUnderlying());
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_NONE);
    }

    if (!DirectionValid(_origin.direction))
    {
        LOG_WARNING("Invalid direction for track placement, direction = %d", _origin.direction);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_NONE);
    }

    if (_rideType != ride->type && !gCheatsAllowArbitraryRideTypeChanges)
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_NONE);
    }

    if (_rideType > RIDE_TYPE_COUNT)
    {
        LOG_WARNING("Invalid ride type for track placement, rideType = %d", _rideType);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_NONE);
    }

    auto res = GameActions::Result();
    res.Expenditure = ExpenditureType::RideConstruction;
    res.Position.x = _origin.x + 16;
    res.Position.y = _origin.y + 16;
    res.Position.z = _origin.z;

    auto resultData = TrackPlaceActionResult{};

    uint32_t rideTypeFlags = ride->GetRideTypeDescriptor().Flags;

    if ((ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK) && _trackType == TrackElemType::EndStation)
    {
        return GameActions::Result(
            GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_NOT_ALLOWED_TO_MODIFY_STATION);
    }

    if (!(GetActionFlags() & GameActions::Flags::AllowWhilePaused))
    {
        if (GameIsPaused() && !gCheatsBuildInPauseMode)
        {
            return GameActions::Result(
                GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED);
        }
    }

    if (!(rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE))
    {
        if (_trackType == TrackElemType::OnRidePhoto)
        {
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO)
            {
                return GameActions::Result(
                    GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                    STR_ONLY_ONE_ON_RIDE_PHOTO_PER_RIDE);
            }
        }
        else if (_trackType == TrackElemType::CableLiftHill)
        {
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED)
            {
                return GameActions::Result(
                    GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                    STR_ONLY_ONE_CABLE_LIFT_HILL_PER_RIDE);
            }
        }
        // Backwards steep lift hills are allowed, even on roller coasters that do not support forwards steep lift hills.
        if ((_trackPlaceFlags & CONSTRUCTION_LIFT_HILL_SELECTED)
            && !ride->GetRideTypeDescriptor().SupportsTrackPiece(TRACK_LIFT_HILL_STEEP) && !gCheatsEnableChainLiftOnAllTrack)
        {
            const auto& ted = GetTrackElementDescriptor(_trackType);
            if (ted.Flags & TRACK_ELEM_FLAG_IS_STEEP_UP)
            {
                return GameActions::Result(
                    GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                    STR_TOO_STEEP_FOR_LIFT_HILL);
            }
        }
    }

    const auto& ted = GetTrackElementDescriptor(_trackType);
    const PreviewTrack* trackBlock = ted.Block;
    uint32_t numElements = 0;
    // First check if any of the track pieces are outside the park
    for (; trackBlock->index != 0xFF; trackBlock++)
    {
        auto rotatedTrack = CoordsXYZ{ CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(_origin.direction), 0 };
        auto tileCoords = CoordsXYZ{ _origin.x, _origin.y, _origin.z } + rotatedTrack;

        if (!LocationValid(tileCoords) || (!MapIsLocationOwned(tileCoords) && !gCheatsSandboxMode))
        {
            return GameActions::Result(
                GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_LAND_NOT_OWNED_BY_PARK);
        }
        numElements++;
    }

    if (!CheckMapCapacity(numElements))
    {
        LOG_WARNING("Not enough free map elements to place track.");
        return GameActions::Result(
            GameActions::Status::NoFreeElements, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
            STR_TILE_ELEMENT_LIMIT_REACHED);
    }

    if (!gCheatsAllowTrackPlaceInvalidHeights)
    {
        if (ted.Flags & TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT)
        {
            if ((_origin.z & 0x0F) != 8)
            {
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_INVALID_HEIGHT);
            }
        }
        else
        {
            if ((_origin.z & 0x0F) != 0)
            {
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_INVALID_HEIGHT);
            }
        }
    }

    // If that is not the case, then perform the remaining checks
    trackBlock = ted.Block;
    auto clearanceHeight = rideEntry->Clearance;

    money32 costs = 0;
    money64 supportCosts = 0;
    for (int32_t blockIndex = 0; trackBlock->index != 0xFF; trackBlock++, blockIndex++)
    {
        auto rotatedTrack = CoordsXYZ{ CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(_origin.direction), trackBlock->z };
        auto mapLoc = CoordsXYZ{ _origin.x, _origin.y, _origin.z } + rotatedTrack;
        auto quarterTile = trackBlock->var_08.Rotate(_origin.direction);

        if (mapLoc.z < 16)
        {
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_TOO_LOW);
        }

        int32_t baseZ = Floor2(mapLoc.z, COORDS_Z_STEP);

        int32_t clearanceZ = trackBlock->var_07;
        if (trackBlock->flags & RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL && clearanceHeight > 24)
        {
            clearanceZ += 24;
        }
        else
        {
            clearanceZ += clearanceHeight;
        }

        clearanceZ = Floor2(clearanceZ, COORDS_Z_STEP) + baseZ;

        if (clearanceZ > MAX_TRACK_HEIGHT)
        {
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_TOO_HIGH);
        }

        uint8_t crossingMode = (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_SUPPORTS_LEVEL_CROSSINGS)
                                && _trackType == TrackElemType::Flat)
            ? CREATE_CROSSING_MODE_TRACK_OVER_PATH
            : CREATE_CROSSING_MODE_NONE;
        auto canBuild = MapCanConstructWithClearAt(
            { mapLoc, baseZ, clearanceZ }, &MapPlaceNonSceneryClearFunc, quarterTile, GetFlags(), crossingMode);
        if (canBuild.Error != GameActions::Status::Ok)
        {
            canBuild.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
            return canBuild;
        }
        costs += canBuild.Cost;

        // When building a level crossing, remove any pre-existing path furniture.
        if (crossingMode == CREATE_CROSSING_MODE_TRACK_OVER_PATH)
        {
            auto footpathElement = MapGetFootpathElement(mapLoc);
            if (footpathElement != nullptr && footpathElement->AsPath()->HasAddition())
            {
                footpathElement->AsPath()->SetAddition(0);
            }
        }

        const auto clearanceData = canBuild.GetData<ConstructClearResult>();
        uint8_t mapGroundFlags = clearanceData.GroundFlags & (ELEMENT_IS_ABOVE_GROUND | ELEMENT_IS_UNDERGROUND);
        if (resultData.GroundFlags != 0 && (resultData.GroundFlags & mapGroundFlags) == 0)
        {
            return GameActions::Result(
                GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                STR_CANT_BUILD_PARTLY_ABOVE_AND_PARTLY_BELOW_GROUND);
        }

        resultData.GroundFlags = mapGroundFlags;
        if (ted.Flags & TRACK_ELEM_FLAG_ONLY_ABOVE_GROUND)
        {
            if (resultData.GroundFlags & ELEMENT_IS_UNDERGROUND)
            {
                return GameActions::Result(
                    GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                    STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND);
            }
        }

        if (ted.Flags & TRACK_ELEM_FLAG_ONLY_UNDERWATER)
        { // No element has this flag
            if (clearanceData.GroundFlags & ELEMENT_IS_UNDERWATER)
            {
                return GameActions::Result(
                    GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                    STR_CAN_ONLY_BUILD_THIS_UNDERWATER);
            }
        }

        if (clearanceData.GroundFlags & ELEMENT_IS_UNDERWATER && !gCheatsDisableClearanceChecks)
        {
            return GameActions::Result(
                GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                STR_RIDE_CANT_BUILD_THIS_UNDERWATER);
        }

        if ((rideTypeFlags & RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER) && !_trackDesignDrawingPreview)
        {
            auto surfaceElement = MapGetSurfaceElementAt(mapLoc);
            if (surfaceElement == nullptr)
            {
                return GameActions::Result(
                    GameActions::Status::Unknown, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_NONE);
            }

            auto waterHeight = surfaceElement->GetWaterHeight();
            if (waterHeight == 0)
            {
                return GameActions::Result(
                    GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                    STR_CAN_ONLY_BUILD_THIS_ON_WATER);
            }

            if (waterHeight != baseZ)
            {
                return GameActions::Result(
                    GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                    STR_CAN_ONLY_BUILD_THIS_ON_WATER);
            }
            waterHeight -= LAND_HEIGHT_STEP;
            if (waterHeight == surfaceElement->GetBaseZ())
            {
                uint8_t slope = surfaceElement->GetSlope() & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP;
                if (slope == TILE_ELEMENT_SLOPE_W_CORNER_DN || slope == TILE_ELEMENT_SLOPE_S_CORNER_DN
                    || slope == TILE_ELEMENT_SLOPE_E_CORNER_DN || slope == TILE_ELEMENT_SLOPE_N_CORNER_DN)
                {
                    return GameActions::Result(
                        GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                        STR_CAN_ONLY_BUILD_THIS_ON_WATER);
                }
            }
        }

        int32_t entranceDirections = std::get<0>(ted.SequenceProperties);
        if ((entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN) && trackBlock->index == 0)
        {
            const auto addElementResult = TrackAddStationElement(
                { mapLoc, baseZ, _origin.direction }, _rideIndex, 0, _fromTrackDesign);
            if (!addElementResult.Successful)
            {
                return GameActions::Result(
                    GameActions::Status::Unknown, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, addElementResult.Message);
            }
        }

        // 6c5648 12 push
        auto surfaceElement = MapGetSurfaceElementAt(mapLoc);
        if (surfaceElement == nullptr)
        {
            return GameActions::Result(GameActions::Status::Unknown, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_NONE);
        }

        if (!gCheatsDisableSupportLimits)
        {
            int32_t ride_height = clearanceZ - surfaceElement->GetBaseZ();
            if (ride_height >= 0)
            {
                uint16_t maxHeight;

                if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY)
                    && rideEntry->max_height != 0)
                {
                    maxHeight = rideEntry->max_height;
                }
                else
                {
                    maxHeight = ride->GetRideTypeDescriptor().Heights.MaxHeight;
                }

                ride_height /= COORDS_Z_PER_TINY_Z;
                if (ride_height > maxHeight && !_trackDesignDrawingPreview)
                {
                    return GameActions::Result(
                        GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                        STR_TOO_HIGH_FOR_SUPPORTS);
                }
            }
        }

        int32_t supportHeight = baseZ - surfaceElement->GetBaseZ();
        if (supportHeight < 0)
        {
            supportHeight = (10 * COORDS_Z_STEP);
        }

        supportCosts += ((supportHeight / (2 * COORDS_Z_STEP)) * ride->GetRideTypeDescriptor().BuildCosts.SupportPrice);
    }

    money64 price = ride->GetRideTypeDescriptor().BuildCosts.TrackPrice;
    price *= ted.PriceModifier;

    price >>= 16;
    res.Cost = costs + supportCosts + price;
    res.SetData(std::move(resultData));

    return res;
}

GameActions::Result TrackPlaceAction::Execute() const
{
    auto ride = GetRide(_rideIndex);
    if (ride == nullptr)
    {
        LOG_WARNING("Invalid ride for track placement, rideIndex = %d", _rideIndex.ToUnderlying());
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_NONE);
    }

    const auto* rideEntry = GetRideEntryByIndex(ride->subtype);
    if (rideEntry == nullptr)
    {
        LOG_WARNING("Invalid ride subtype for track placement, rideIndex = %d", _rideIndex.ToUnderlying());
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_NONE);
    }

    auto res = GameActions::Result();
    res.Expenditure = ExpenditureType::RideConstruction;
    res.Position.x = _origin.x + 16;
    res.Position.y = _origin.y + 16;
    res.Position.z = _origin.z;

    auto resultData = TrackPlaceActionResult{};

    uint32_t rideTypeFlags = ride->GetRideTypeDescriptor().Flags;

    const auto& ted = GetTrackElementDescriptor(_trackType);
    const auto& wallEdges = ted.SequenceElementAllowedWallEdges;

    money32 costs = 0;
    money64 supportCosts = 0;
    const PreviewTrack* trackBlock = ted.Block;
    auto clearanceHeight = rideEntry->Clearance;
    CoordsXYZ originLocation = CoordsXYZ{ _origin.x, _origin.y, _origin.z }
        + CoordsXYZ{ CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(_origin.direction), trackBlock->z };
    for (int32_t blockIndex = 0; trackBlock->index != 0xFF; trackBlock++, blockIndex++)
    {
        auto rotatedTrack = CoordsXYZ{ CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(_origin.direction), trackBlock->z };
        auto mapLoc = CoordsXYZ{ _origin.x, _origin.y, _origin.z } + rotatedTrack;

        auto quarterTile = trackBlock->var_08.Rotate(_origin.direction);

        int32_t baseZ = Floor2(mapLoc.z, COORDS_Z_STEP);
        int32_t clearanceZ = trackBlock->var_07;
        if (trackBlock->flags & RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL && clearanceHeight > 24)
        {
            clearanceZ += 24;
        }
        else
        {
            clearanceZ += clearanceHeight;
        }

        clearanceZ = Floor2(clearanceZ, COORDS_Z_STEP) + baseZ;
        const auto mapLocWithClearance = CoordsXYRangedZ(mapLoc, baseZ, clearanceZ);

        uint8_t crossingMode = (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_SUPPORTS_LEVEL_CROSSINGS)
                                && _trackType == TrackElemType::Flat)
            ? CREATE_CROSSING_MODE_TRACK_OVER_PATH
            : CREATE_CROSSING_MODE_NONE;
        auto canBuild = MapCanConstructWithClearAt(
            mapLocWithClearance, &MapPlaceNonSceneryClearFunc, quarterTile, GetFlags() | GAME_COMMAND_FLAG_APPLY, crossingMode);
        if (canBuild.Error != GameActions::Status::Ok)
        {
            canBuild.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
            return canBuild;
        }
        costs += canBuild.Cost;

        if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST) && !gCheatsDisableClearanceChecks)
        {
            FootpathRemoveLitter(mapLoc);
            if (rideTypeFlags & RIDE_TYPE_FLAG_TRACK_NO_WALLS)
            {
                WallRemoveAt(mapLocWithClearance);
            }
            else
            {
                // Remove walls in the directions this track intersects
                uint8_t intersectingDirections = wallEdges[blockIndex];
                intersectingDirections ^= 0x0F;
                intersectingDirections = Numerics::rol4(intersectingDirections, _origin.direction);
                for (int32_t i = 0; i < NumOrthogonalDirections; i++)
                {
                    if (intersectingDirections & (1 << i))
                    {
                        WallRemoveIntersectingWalls(mapLocWithClearance, i);
                    }
                }
            }
        }

        const auto clearanceData = canBuild.GetData<ConstructClearResult>();
        uint8_t mapGroundFlags = clearanceData.GroundFlags & (ELEMENT_IS_ABOVE_GROUND | ELEMENT_IS_UNDERGROUND);
        if (resultData.GroundFlags != 0 && (resultData.GroundFlags & mapGroundFlags) == 0)
        {
            return GameActions::Result(
                GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                STR_CANT_BUILD_PARTLY_ABOVE_AND_PARTLY_BELOW_GROUND);
        }

        resultData.GroundFlags = mapGroundFlags;

        // 6c5648 12 push
        auto surfaceElement = MapGetSurfaceElementAt(mapLoc);
        if (surfaceElement == nullptr)
        {
            return GameActions::Result(GameActions::Status::Unknown, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_NONE);
        }

        int32_t supportHeight = baseZ - surfaceElement->GetBaseZ();
        if (supportHeight < 0)
        {
            supportHeight = (10 * COORDS_Z_STEP);
        }

        supportCosts += (supportHeight / (2 * COORDS_Z_STEP)) * ride->GetRideTypeDescriptor().BuildCosts.SupportPrice;

        int32_t entranceDirections = 0;
        if (!ride->overall_view.IsNull())
        {
            if (!(GetFlags() & GAME_COMMAND_FLAG_NO_SPEND))
            {
                entranceDirections = std::get<0>(ted.SequenceProperties);
            }
        }

        if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN || ride->overall_view.IsNull())
        {
            ride->overall_view = mapLoc;
        }

        auto* trackElement = TileElementInsert<TrackElement>(mapLoc, quarterTile.GetBaseQuarterOccupied());
        if (trackElement == nullptr)
        {
            LOG_WARNING("Cannot create track element for ride = %d", _rideIndex.ToUnderlying());
            return GameActions::Result(
                GameActions::Status::NoFreeElements, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                STR_TILE_ELEMENT_LIMIT_REACHED);
        }

        trackElement->SetClearanceZ(clearanceZ);
        trackElement->SetDirection(_origin.direction);
        trackElement->SetHasChain(_trackPlaceFlags & CONSTRUCTION_LIFT_HILL_SELECTED);
        trackElement->SetSequenceIndex(trackBlock->index);
        trackElement->SetRideIndex(_rideIndex);
        trackElement->SetTrackType(_trackType);
        trackElement->SetRideType(_rideType);
        trackElement->SetGhost(GetFlags() & GAME_COMMAND_FLAG_GHOST);

        switch (_trackType)
        {
            case TrackElemType::Waterfall:
                MapAnimationCreate(MAP_ANIMATION_TYPE_TRACK_WATERFALL, CoordsXYZ{ mapLoc, trackElement->GetBaseZ() });
                break;
            case TrackElemType::Rapids:
                MapAnimationCreate(MAP_ANIMATION_TYPE_TRACK_RAPIDS, CoordsXYZ{ mapLoc, trackElement->GetBaseZ() });
                break;
            case TrackElemType::Whirlpool:
                MapAnimationCreate(MAP_ANIMATION_TYPE_TRACK_WHIRLPOOL, CoordsXYZ{ mapLoc, trackElement->GetBaseZ() });
                break;
            case TrackElemType::SpinningTunnel:
                MapAnimationCreate(MAP_ANIMATION_TYPE_TRACK_SPINNINGTUNNEL, CoordsXYZ{ mapLoc, trackElement->GetBaseZ() });
                break;
        }
        if (TrackTypeHasSpeedSetting(_trackType))
        {
            trackElement->SetBrakeBoosterSpeed(_brakeSpeed);
        }
        else if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_LANDSCAPE_DOORS))
        {
            trackElement->SetDoorAState(LANDSCAPE_DOOR_CLOSED);
            trackElement->SetDoorBState(LANDSCAPE_DOOR_CLOSED);
        }
        else
        {
            trackElement->SetSeatRotation(_seatRotation);
        }

        if (_trackPlaceFlags & RIDE_TYPE_ALTERNATIVE_TRACK_TYPE)
        {
            trackElement->SetInverted(true);
        }
        trackElement->SetColourScheme(_colour);

        entranceDirections = std::get<0>(ted.SequenceProperties);
        if (entranceDirections & TRACK_SEQUENCE_FLAG_CONNECTS_TO_PATH)
        {
            uint8_t availableDirections = entranceDirections & 0x0F;
            if (availableDirections != 0)
            {
                if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST) && !gCheatsDisableClearanceChecks)
                {
                    for (int32_t chosenDirection = UtilBitScanForward(availableDirections); chosenDirection != -1;
                         chosenDirection = UtilBitScanForward(availableDirections))
                    {
                        availableDirections &= ~(1 << chosenDirection);
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
        // Don't do this if the ride is simulating and the tile is a ghost to prevent desyncs.
        if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN
            && !(ride->status == RideStatus::Simulating && GetFlags() & GAME_COMMAND_FLAG_GHOST))
        {
            if (trackBlock->index == 0)
            {
                TrackAddStationElement({ mapLoc, _origin.direction }, _rideIndex, GAME_COMMAND_FLAG_APPLY, _fromTrackDesign);
            }
            ride->ValidateStations();
            ride->UpdateMaxVehicles();
        }

        auto* tileElement = trackElement->as<TileElement>();

        if (rideTypeFlags & RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER)
        {
            auto* waterSurfaceElement = MapGetSurfaceElementAt(mapLoc);
            if (waterSurfaceElement != nullptr)
            {
                waterSurfaceElement->SetHasTrackThatNeedsWater(true);
                tileElement = waterSurfaceElement->as<TileElement>();
            }
        }

        if (!gCheatsDisableClearanceChecks || !(GetFlags() & GAME_COMMAND_FLAG_GHOST))
        {
            FootpathConnectEdges(mapLoc, tileElement, GetFlags());
        }
        MapInvalidateTileFull(mapLoc);
    }

    // Update ride stats and block brake count if the piece was successfully built
    if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
    {
        InvalidateTestResults(*ride);
        switch (_trackType)
        {
            case TrackElemType::OnRidePhoto:
                ride->lifecycle_flags |= RIDE_LIFECYCLE_ON_RIDE_PHOTO;
                break;
            case TrackElemType::CableLiftHill:
                ride->lifecycle_flags |= RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED;
                ride->CableLiftLoc = originLocation;
                break;
            case TrackElemType::BlockBrakes:
            {
                ride->num_block_brakes++;
                ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_OPERATING;

                // change the current mode to its circuit blocked equivalent
                RideMode newMode = RideMode::ContinuousCircuitBlockSectioned;
                if (ride->mode == RideMode::PoweredLaunch)
                {
                    if (ride->GetRideTypeDescriptor().SupportsRideMode(RideMode::PoweredLaunchBlockSectioned)
                        || gCheatsShowAllOperatingModes)
                        newMode = RideMode::PoweredLaunchBlockSectioned;
                    else
                        newMode = RideMode::PoweredLaunch;
                }

                auto rideSetSetting = RideSetSettingAction(ride->id, RideSetSetting::Mode, static_cast<uint8_t>(newMode));
                GameActions::ExecuteNested(&rideSetSetting);
                break;
            }
        }

        switch (_trackType)
        {
            case TrackElemType::Up25ToFlat:
            case TrackElemType::Up60ToFlat:
            case TrackElemType::DiagUp25ToFlat:
            case TrackElemType::DiagUp60ToFlat:
                if (!(_trackPlaceFlags & CONSTRUCTION_LIFT_HILL_SELECTED))
                    break;
                [[fallthrough]];
            case TrackElemType::CableLiftHill:
                ride->num_block_brakes++;
                break;
        }
    }

    money64 price = ride->GetRideTypeDescriptor().BuildCosts.TrackPrice;
    price *= ted.PriceModifier;

    price >>= 16;
    res.Cost = costs + supportCosts + price;
    res.SetData(std::move(resultData));

    return res;
}

bool TrackPlaceAction::CheckMapCapacity(int16_t numTiles) const
{
    const auto& ted = GetTrackElementDescriptor(_trackType);
    for (const PreviewTrack* trackBlock = ted.Block; trackBlock->index != 0xFF; trackBlock++)
    {
        auto rotatedTrack = CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(_origin.direction);

        auto tileCoords = CoordsXY{ _origin.x, _origin.y } + rotatedTrack;
        if (!MapCheckCapacityAndReorganise(tileCoords, numTiles))
        {
            return false;
        }
    }
    return true;
}
