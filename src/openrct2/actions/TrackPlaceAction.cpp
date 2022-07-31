/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
    auto ride = get_ride(_rideIndex);
    if (ride == nullptr)
    {
        log_warning("Invalid ride for track placement, rideIndex = %d", _rideIndex.ToUnderlying());
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_NONE);
    }
    rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);
    if (rideEntry == nullptr)
    {
        log_warning("Invalid ride subtype for track placement, rideIndex = %d", _rideIndex.ToUnderlying());
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_NONE);
    }

    if (!direction_valid(_origin.direction))
    {
        log_warning("Invalid direction for track placement, direction = %d", _origin.direction);
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
        log_warning("Invalid ride type for track placement, rideType = %d", _rideType);
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
        if (game_is_paused() && !gCheatsBuildInPauseMode)
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
    const rct_preview_track* trackBlock = ted.Block;
    uint32_t numElements = 0;
    // First check if any of the track pieces are outside the park
    for (; trackBlock->index != 0xFF; trackBlock++)
    {
        auto rotatedTrack = CoordsXYZ{ CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(_origin.direction), 0 };
        auto tileCoords = CoordsXYZ{ _origin.x, _origin.y, _origin.z } + rotatedTrack;

        if (!LocationValid(tileCoords) || (!map_is_location_owned(tileCoords) && !gCheatsSandboxMode))
        {
            return GameActions::Result(
                GameActions::Status::Disallowed, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_LAND_NOT_OWNED_BY_PARK);
        }
        numElements++;
    }

    if (!CheckMapCapacity(numElements))
    {
        log_warning("Not enough free map elements to place track.");
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
                    GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                    STR_CONSTRUCTION_ERR_UNKNOWN);
            }
        }
        else
        {
            if ((_origin.z & 0x0F) != 0)
            {
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE,
                    STR_CONSTRUCTION_ERR_UNKNOWN);
            }
        }
    }

    // If that is not the case, then perform the remaining checks
    trackBlock = ted.Block;

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

        int32_t baseZ = floor2(mapLoc.z, COORDS_Z_STEP);

        int32_t clearanceZ = trackBlock->var_07;
        if (trackBlock->flags & RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL
            && ride->GetRideTypeDescriptor().Heights.ClearanceHeight > 24)
        {
            clearanceZ += 24;
        }
        else
        {
            clearanceZ += ride->GetRideTypeDescriptor().Heights.ClearanceHeight;
        }

        clearanceZ = floor2(clearanceZ, COORDS_Z_STEP) + baseZ;

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
            { mapLoc, baseZ, clearanceZ }, &map_place_non_scenery_clear_func, quarterTile, GetFlags(), crossingMode);
        if (canBuild.Error != GameActions::Status::Ok)
        {
            canBuild.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
            return canBuild;
        }
        costs += canBuild.Cost;

        // When building a level crossing, remove any pre-existing path furniture.
        if (crossingMode == CREATE_CROSSING_MODE_TRACK_OVER_PATH)
        {
            auto footpathElement = map_get_footpath_element(mapLoc);
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
            auto surfaceElement = map_get_surface_element_at(mapLoc);
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
            const auto addElementResult = track_add_station_element(
                { mapLoc, baseZ, _origin.direction }, _rideIndex, 0, _fromTrackDesign);
            if (!addElementResult.Successful)
            {
                return GameActions::Result(
                    GameActions::Status::Unknown, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, addElementResult.Message);
            }
        }

        // 6c5648 12 push
        auto surfaceElement = map_get_surface_element_at(mapLoc);
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
    auto ride = get_ride(_rideIndex);
    if (ride == nullptr)
    {
        log_warning("Invalid ride for track placement, rideIndex = %d", _rideIndex.ToUnderlying());
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_NONE);
    }

    rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);
    if (rideEntry == nullptr)
    {
        log_warning("Invalid ride subtype for track placement, rideIndex = %d", _rideIndex.ToUnderlying());
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
    const rct_preview_track* trackBlock = ted.Block;
    for (int32_t blockIndex = 0; trackBlock->index != 0xFF; trackBlock++, blockIndex++)
    {
        auto rotatedTrack = CoordsXYZ{ CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(_origin.direction), trackBlock->z };
        auto mapLoc = CoordsXYZ{ _origin.x, _origin.y, _origin.z } + rotatedTrack;

        auto quarterTile = trackBlock->var_08.Rotate(_origin.direction);

        int32_t baseZ = floor2(mapLoc.z, COORDS_Z_STEP);
        int32_t clearanceZ = trackBlock->var_07;
        if (trackBlock->flags & RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL
            && ride->GetRideTypeDescriptor().Heights.ClearanceHeight > 24)
        {
            clearanceZ += 24;
        }
        else
        {
            clearanceZ += ride->GetRideTypeDescriptor().Heights.ClearanceHeight;
        }

        clearanceZ = floor2(clearanceZ, COORDS_Z_STEP) + baseZ;
        const auto mapLocWithClearance = CoordsXYRangedZ(mapLoc, baseZ, clearanceZ);

        uint8_t crossingMode = (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_SUPPORTS_LEVEL_CROSSINGS)
                                && _trackType == TrackElemType::Flat)
            ? CREATE_CROSSING_MODE_TRACK_OVER_PATH
            : CREATE_CROSSING_MODE_NONE;
        auto canBuild = MapCanConstructWithClearAt(
            mapLocWithClearance, &map_place_non_scenery_clear_func, quarterTile, GetFlags() | GAME_COMMAND_FLAG_APPLY,
            crossingMode);
        if (canBuild.Error != GameActions::Status::Ok)
        {
            canBuild.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
            return canBuild;
        }
        costs += canBuild.Cost;

        if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST) && !gCheatsDisableClearanceChecks)
        {
            footpath_remove_litter(mapLoc);
            if (rideTypeFlags & RIDE_TYPE_FLAG_TRACK_NO_WALLS)
            {
                wall_remove_at(mapLocWithClearance);
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
                        wall_remove_intersecting_walls(mapLocWithClearance, i);
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
        auto surfaceElement = map_get_surface_element_at(mapLoc);
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

        if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
        {
            invalidate_test_results(ride);
            switch (_trackType)
            {
                case TrackElemType::OnRidePhoto:
                    ride->lifecycle_flags |= RIDE_LIFECYCLE_ON_RIDE_PHOTO;
                    break;
                case TrackElemType::CableLiftHill:
                    if (trackBlock->index != 0)
                        break;
                    ride->lifecycle_flags |= RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED;
                    ride->CableLiftLoc = mapLoc;
                    break;
                case TrackElemType::BlockBrakes:
                {
                    ride->num_block_brakes++;
                    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_OPERATING;

                    // change the current mode to its circuit blocked equivalent
                    RideMode newMode = RideMode::ContinuousCircuitBlockSectioned;
                    if (ride->mode == RideMode::PoweredLaunch)
                    {
                        if (ride->GetRideTypeDescriptor().RideModes
                                & (1ULL << static_cast<uint8_t>(RideMode::PoweredLaunchBlockSectioned))
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

            if (trackBlock->index == 0)
            {
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
        }

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
            log_warning("Cannot create track element for ride = %d", _rideIndex.ToUnderlying());
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
                map_animation_create(MAP_ANIMATION_TYPE_TRACK_WATERFALL, CoordsXYZ{ mapLoc, trackElement->GetBaseZ() });
                break;
            case TrackElemType::Rapids:
                map_animation_create(MAP_ANIMATION_TYPE_TRACK_RAPIDS, CoordsXYZ{ mapLoc, trackElement->GetBaseZ() });
                break;
            case TrackElemType::Whirlpool:
                map_animation_create(MAP_ANIMATION_TYPE_TRACK_WHIRLPOOL, CoordsXYZ{ mapLoc, trackElement->GetBaseZ() });
                break;
            case TrackElemType::SpinningTunnel:
                map_animation_create(MAP_ANIMATION_TYPE_TRACK_SPINNINGTUNNEL, CoordsXYZ{ mapLoc, trackElement->GetBaseZ() });
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
                    for (int32_t chosenDirection = bitscanforward(availableDirections); chosenDirection != -1;
                         chosenDirection = bitscanforward(availableDirections))
                    {
                        availableDirections &= ~(1 << chosenDirection);
                        CoordsXY tempLoc{ mapLoc.x, mapLoc.y };
                        int32_t tempDirection = (_origin.direction + chosenDirection) & 3;
                        tempLoc.x += CoordsDirectionDelta[tempDirection].x;
                        tempLoc.y += CoordsDirectionDelta[tempDirection].y;
                        tempDirection = direction_reverse(tempDirection);
                        wall_remove_intersecting_walls({ tempLoc, baseZ, clearanceZ }, tempDirection & 3);
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
                track_add_station_element({ mapLoc, _origin.direction }, _rideIndex, GAME_COMMAND_FLAG_APPLY, _fromTrackDesign);
            }
            ride->ValidateStations();
            ride->UpdateMaxVehicles();
        }

        auto* tileElement = trackElement->as<TileElement>();

        if (rideTypeFlags & RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER)
        {
            auto* waterSurfaceElement = map_get_surface_element_at(mapLoc);
            if (waterSurfaceElement != nullptr)
            {
                waterSurfaceElement->SetHasTrackThatNeedsWater(true);
                tileElement = waterSurfaceElement->as<TileElement>();
            }
        }

        if (!gCheatsDisableClearanceChecks || !(GetFlags() & GAME_COMMAND_FLAG_GHOST))
        {
            footpath_connect_edges(mapLoc, tileElement, GetFlags());
        }
        map_invalidate_tile_full(mapLoc);
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
    for (const rct_preview_track* trackBlock = ted.Block; trackBlock->index != 0xFF; trackBlock++)
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
