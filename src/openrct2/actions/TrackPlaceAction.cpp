/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include "../world/MapAnimation.h"
#include "../world/QuarterTile.h"
#include "../world/Wall.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/Slope.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TrackElement.h"
#include "RideSetSettingAction.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Numerics;
using namespace OpenRCT2::TrackMetaData;

TrackPlaceAction::TrackPlaceAction(
    RideId rideIndex, OpenRCT2::TrackElemType trackType, ride_type_t rideType, const CoordsXYZD& origin, int32_t brakeSpeed,
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

GameActions::Result TrackPlaceAction::Query() const
{
    auto ride = GetRide(_rideIndex);
    if (ride == nullptr)
    {
        LOG_ERROR("Ride not found for rideIndex %d", _rideIndex.ToUnderlying());
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_ERR_RIDE_NOT_FOUND);
    }
    const auto* rideEntry = GetRideEntryByIndex(ride->subtype);
    if (rideEntry == nullptr)
    {
        LOG_ERROR("Invalid ride subtype for track placement, rideIndex = %d", _rideIndex.ToUnderlying());
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_UNKNOWN_OBJECT_TYPE);
    }

    if (!DirectionValid(_origin.direction))
    {
        LOG_ERROR("Invalid direction for track placement, direction = %d", _origin.direction);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_ERR_VALUE_OUT_OF_RANGE);
    }

    auto& gameState = GetGameState();
    if (_rideType != ride->type && !gameState.Cheats.allowArbitraryRideTypeChanges)
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, kStringIdNone);
    }

    if (_rideType > RIDE_TYPE_COUNT)
    {
        LOG_ERROR("Invalid ride type for track placement, rideType = %d", _rideType);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_ERR_VALUE_OUT_OF_RANGE);
    }

    if (_brakeSpeed > kMaximumTrackSpeed)
    {
        LOG_WARNING("Invalid speed for track placement, speed = %d", _brakeSpeed);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_SPEED_TOO_HIGH);
    }

    auto res = GameActions::Result();
    res.Expenditure = ExpenditureType::RideConstruction;
    res.Position.x = _origin.x + 16;
    res.Position.y = _origin.y + 16;
    res.Position.z = _origin.z;

    auto resultData = TrackPlaceActionResult{};

    const auto& rtd = ride->GetRideTypeDescriptor();

    if ((ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK) && _trackType == TrackElemType::EndStation)
    {
        return GameActions::Result(
            GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_NOT_ALLOWED_TO_MODIFY_STATION);
    }

    if (!(GetActionFlags() & GameActions::Flags::AllowWhilePaused))
    {
        if (GameIsPaused() && !gameState.Cheats.buildInPauseMode)
        {
            return GameActions::Result(
                GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED);
        }
    }

    if (!rtd.HasFlag(RtdFlag::isFlatRide))
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
        if (_trackPlaceFlags.has(LiftHillAndInverted::liftHill) && !rtd.SupportsTrackGroup(TrackGroup::liftHillSteep)
            && !gameState.Cheats.enableChainLiftOnAllTrack)
        {
            const auto& ted = GetTrackElementDescriptor(_trackType);
            if (ted.flags & TRACK_ELEM_FLAG_IS_STEEP_UP)
            {
                return GameActions::Result(
                    GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                    STR_TOO_STEEP_FOR_LIFT_HILL);
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
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_OFF_EDGE_OF_MAP);
        }
        if (!MapIsLocationOwned(tileCoords) && !gameState.Cheats.sandboxMode)
        {
            return GameActions::Result(
                GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_LAND_NOT_OWNED_BY_PARK);
        }
        numElements++;
    }

    if (!CheckMapCapacity(numElements))
    {
        LOG_ERROR("Not enough free map elements to place track.");
        return GameActions::Result(
            GameActions::Status::NoFreeElements, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
            STR_TILE_ELEMENT_LIMIT_REACHED);
    }

    if (!gameState.Cheats.allowTrackPlaceInvalidHeights)
    {
        if (ted.flags & TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT)
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
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_TOO_LOW);
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

        if (clearanceZ > MAX_TRACK_HEIGHT)
        {
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_TOO_HIGH);
        }

        auto crossingMode = (rtd.HasFlag(RtdFlag::supportsLevelCrossings) && _trackType == TrackElemType::Flat)
            ? CreateCrossingMode::trackOverPath
            : CreateCrossingMode::none;
        auto canBuild = MapCanConstructWithClearAt(
            { mapLoc, baseZ, clearanceZ }, &MapPlaceNonSceneryClearFunc, quarterTile, GetFlags(), crossingMode);
        if (canBuild.Error != GameActions::Status::Ok)
        {
            canBuild.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
            return canBuild;
        }
        costs += canBuild.Cost;

        const auto clearanceData = canBuild.GetData<ConstructClearResult>();
        uint8_t mapGroundFlags = clearanceData.GroundFlags & (ELEMENT_IS_ABOVE_GROUND | ELEMENT_IS_UNDERGROUND);
        if (!(ted.flags & TRACK_ELEM_FLAG_CAN_BE_PARTLY_UNDERGROUND))
        {
            if (resultData.GroundFlags != 0 && (resultData.GroundFlags & mapGroundFlags) == 0)
            {
                return GameActions::Result(
                    GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                    STR_CANT_BUILD_PARTLY_ABOVE_AND_PARTLY_BELOW_GROUND);
            }
        }

        resultData.GroundFlags = mapGroundFlags;
        if (ted.flags & TRACK_ELEM_FLAG_ONLY_ABOVE_GROUND)
        {
            if (resultData.GroundFlags & ELEMENT_IS_UNDERGROUND)
            {
                return GameActions::Result(
                    GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                    STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND);
            }
        }

        if (ted.flags & TRACK_ELEM_FLAG_ONLY_UNDERWATER)
        { // No element has this flag
            if (clearanceData.GroundFlags & ELEMENT_IS_UNDERWATER)
            {
                return GameActions::Result(
                    GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                    STR_CAN_ONLY_BUILD_THIS_UNDERWATER);
            }
        }

        if (clearanceData.GroundFlags & ELEMENT_IS_UNDERWATER && !gameState.Cheats.disableClearanceChecks)
        {
            return GameActions::Result(
                GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                STR_RIDE_CANT_BUILD_THIS_UNDERWATER);
        }

        if (rtd.HasFlag(RtdFlag::trackMustBeOnWater) && !_trackDesignDrawingPreview)
        {
            auto surfaceElement = MapGetSurfaceElementAt(mapLoc);
            if (surfaceElement == nullptr)
            {
                return GameActions::Result(
                    GameActions::Status::Unknown, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                    STR_ERR_SURFACE_ELEMENT_NOT_FOUND);
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
            waterHeight -= kLandHeightStep;
            if (waterHeight == surfaceElement->GetBaseZ())
            {
                uint8_t slope = surfaceElement->GetSlope() & kTileSlopeRaisedCornersMask;
                if (slope == kTileSlopeWCornerDown || slope == kTileSlopeSCornerDown || slope == kTileSlopeECornerDown
                    || slope == kTileSlopeNCornerDown)
                {
                    return GameActions::Result(
                        GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                        STR_CAN_ONLY_BUILD_THIS_ON_WATER);
                }
            }
        }

        int32_t entranceDirections = ted.sequences[0].flags;
        if ((entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN) && blockIndex == 0)
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
            return GameActions::Result(
                GameActions::Status::Unknown, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                STR_ERR_SURFACE_ELEMENT_NOT_FOUND);
        }

        if (!gameState.Cheats.disableSupportLimits)
        {
            int32_t ride_height = clearanceZ - surfaceElement->GetBaseZ();
            if (ride_height >= 0)
            {
                uint16_t maxHeight;

                if (rtd.HasFlag(RtdFlag::listVehiclesSeparately) && rideEntry->max_height != 0)
                {
                    maxHeight = rideEntry->max_height;
                }
                else
                {
                    maxHeight = rtd.Heights.MaxHeight;
                }

                ride_height /= kCoordsZPerTinyZ;
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
            supportHeight = (10 * kCoordsZStep);
        }

        supportCosts += ((supportHeight / (2 * kCoordsZStep)) * rtd.BuildCosts.SupportPrice);
    }

    money64 price = rtd.BuildCosts.TrackPrice;
    price *= ted.priceModifier;

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
        LOG_ERROR("Invalid ride for track placement, rideIndex = %d", _rideIndex.ToUnderlying());
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_ERR_RIDE_NOT_FOUND);
    }

    const auto* rideEntry = GetRideEntryByIndex(ride->subtype);
    if (rideEntry == nullptr)
    {
        LOG_ERROR("Invalid ride subtype for track placement, rideIndex = %d", _rideIndex.ToUnderlying());
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_UNKNOWN_OBJECT_TYPE);
    }

    auto res = GameActions::Result();
    res.Expenditure = ExpenditureType::RideConstruction;
    res.Position.x = _origin.x + 16;
    res.Position.y = _origin.y + 16;
    res.Position.z = _origin.z;

    auto resultData = TrackPlaceActionResult{};

    const auto& rtd = ride->GetRideTypeDescriptor();

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

        auto crossingMode = (rtd.HasFlag(RtdFlag::supportsLevelCrossings) && _trackType == TrackElemType::Flat)
            ? CreateCrossingMode::trackOverPath
            : CreateCrossingMode::none;
        auto canBuild = MapCanConstructWithClearAt(
            mapLocWithClearance, &MapPlaceNonSceneryClearFunc, quarterTile, GetFlags() | GAME_COMMAND_FLAG_APPLY, crossingMode);
        if (canBuild.Error != GameActions::Status::Ok)
        {
            canBuild.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
            return canBuild;
        }
        costs += canBuild.Cost;

        // When building a level crossing, remove any pre-existing path furniture.
        if (crossingMode == CreateCrossingMode::trackOverPath && !(GetFlags() & GAME_COMMAND_FLAG_GHOST))
        {
            auto footpathElement = MapGetFootpathElement(mapLoc);
            if (footpathElement != nullptr && footpathElement->HasAddition())
            {
                footpathElement->SetAddition(0);
            }
        }

        auto& gameState = GetGameState();
        if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST) && !gameState.Cheats.disableClearanceChecks)
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
                intersectingDirections = Numerics::rol4(intersectingDirections, _origin.direction);
                for (int32_t i = 0; i < kNumOrthogonalDirections; i++)
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
        if (!(ted.flags & TRACK_ELEM_FLAG_CAN_BE_PARTLY_UNDERGROUND))
        {
            if (resultData.GroundFlags != 0 && (resultData.GroundFlags & mapGroundFlags) == 0)
            {
                return GameActions::Result(
                    GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                    STR_CANT_BUILD_PARTLY_ABOVE_AND_PARTLY_BELOW_GROUND);
            }
        }

        resultData.GroundFlags = mapGroundFlags;

        // 6c5648 12 push
        auto surfaceElement = MapGetSurfaceElementAt(mapLoc);
        if (surfaceElement == nullptr)
        {
            return GameActions::Result(
                GameActions::Status::Unknown, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                STR_ERR_SURFACE_ELEMENT_NOT_FOUND);
        }

        int32_t supportHeight = baseZ - surfaceElement->GetBaseZ();
        if (supportHeight < 0)
        {
            supportHeight = (10 * kCoordsZStep);
        }

        supportCosts += (supportHeight / (2 * kCoordsZStep)) * rtd.BuildCosts.SupportPrice;

        int32_t entranceDirections = 0;
        if (!ride->overall_view.IsNull())
        {
            if (!(GetFlags() & GAME_COMMAND_FLAG_NO_SPEND))
            {
                entranceDirections = ted.sequences[0].flags;
            }
        }

        if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN || ride->overall_view.IsNull())
        {
            ride->overall_view = mapLoc;
        }

        auto* trackElement = TileElementInsert<TrackElement>(mapLoc, quarterTile.GetBaseQuarterOccupied());
        if (trackElement == nullptr)
        {
            LOG_ERROR("Cannot create track element for ride = %d", _rideIndex.ToUnderlying());
            return GameActions::Result(
                GameActions::Status::NoFreeElements, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                STR_TILE_ELEMENT_LIMIT_REACHED);
        }

        trackElement->SetClearanceZ(clearanceZ);
        trackElement->SetDirection(_origin.direction);
        trackElement->SetHasChain(_trackPlaceFlags.has(LiftHillAndInverted::liftHill));
        trackElement->SetSequenceIndex(blockIndex);
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
            case TrackElemType::Brakes:
            case TrackElemType::DiagBrakes:
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
            trackElement->SetDoorAState(LANDSCAPE_DOOR_CLOSED);
            trackElement->SetDoorBState(LANDSCAPE_DOOR_CLOSED);
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

        entranceDirections = ted.sequences[0].flags;
        if (entranceDirections & TRACK_SEQUENCE_FLAG_CONNECTS_TO_PATH)
        {
            uint32_t availableDirections = entranceDirections & 0x0F;
            if (availableDirections != 0)
            {
                if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST) && !gameState.Cheats.disableClearanceChecks)
                {
                    for (int32_t chosenDirection = Numerics::bitScanForward(availableDirections); chosenDirection != -1;
                         chosenDirection = Numerics::bitScanForward(availableDirections))
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
        // Don't do this if the tile is a ghost to prevent desyncs
        // However, ghost tiles from track designs need to modify station data to display properly
        if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN && (!(GetFlags() & GAME_COMMAND_FLAG_GHOST) || _fromTrackDesign))
        {
            if (blockIndex == 0)
            {
                TrackAddStationElement({ mapLoc, _origin.direction }, _rideIndex, GAME_COMMAND_FLAG_APPLY, _fromTrackDesign);
            }
            ride->ValidateStations();
            ride->UpdateMaxVehicles();
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

        if (!gameState.Cheats.disableClearanceChecks || !(GetFlags() & GAME_COMMAND_FLAG_GHOST))
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
            case TrackElemType::DiagBlockBrakes:
            case TrackElemType::BlockBrakes:
            {
                ride->num_block_brakes++;
                ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_OPERATING;

                // change the current mode to its circuit blocked equivalent
                RideMode newMode = RideMode::ContinuousCircuitBlockSectioned;
                if (ride->mode == RideMode::PoweredLaunch)
                {
                    if (rtd.SupportsRideMode(RideMode::PoweredLaunchBlockSectioned)
                        || GetGameState().Cheats.showAllOperatingModes)
                        newMode = RideMode::PoweredLaunchBlockSectioned;
                    else
                        newMode = RideMode::PoweredLaunch;
                }

                auto rideSetSetting = RideSetSettingAction(ride->id, RideSetSetting::Mode, static_cast<uint8_t>(newMode));
                GameActions::ExecuteNested(&rideSetSetting);
                break;
            }
            default:
                break;
        }

        switch (_trackType)
        {
            case TrackElemType::Up25ToFlat:
            case TrackElemType::Up60ToFlat:
            case TrackElemType::DiagUp25ToFlat:
            case TrackElemType::DiagUp60ToFlat:
                if (!_trackPlaceFlags.has(LiftHillAndInverted::liftHill))
                    break;
                [[fallthrough]];
            case TrackElemType::CableLiftHill:
                ride->num_block_brakes++;
                break;
            default:
                break;
        }
    }

    money64 price = rtd.BuildCosts.TrackPrice;
    price *= ted.priceModifier;

    price >>= 16;
    res.Cost = costs + supportCosts + price;
    res.SetData(std::move(resultData));

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
