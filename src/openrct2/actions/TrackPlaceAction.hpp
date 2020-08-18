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

class TrackPlaceActionResult final : public GameActionResult
{
public:
    TrackPlaceActionResult()
        : GameActionResult(GA_ERROR::OK, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE)
    {
    }
    TrackPlaceActionResult(GA_ERROR error)
        : GameActionResult(error, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE)
    {
    }
    TrackPlaceActionResult(GA_ERROR error, rct_string_id message)
        : GameActionResult(error, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, message)
    {
    }
    TrackPlaceActionResult(GA_ERROR error, rct_string_id message, uint8_t* args)
        : GameActionResult(error, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, message, args)
    {
    }

    uint8_t GroundFlags{ 0 };
};

DEFINE_GAME_ACTION(TrackPlaceAction, GAME_COMMAND_PLACE_TRACK, TrackPlaceActionResult)
{
private:
    NetworkRideId_t _rideIndex;
    int32_t _trackType;
    CoordsXYZD _origin;
    int32_t _brakeSpeed;
    int32_t _colour;
    int32_t _seatRotation;
    int32_t _trackPlaceFlags;
    bool _fromTrackDesign;

public:
    TrackPlaceAction()
    {
    }

    TrackPlaceAction(
        NetworkRideId_t rideIndex, int32_t trackType, const CoordsXYZD& origin, int32_t brakeSpeed, int32_t colour,
        int32_t seatRotation, int32_t liftHillAndAlternativeState, bool fromTrackDesign)
        : _rideIndex(rideIndex)
        , _trackType(trackType)
        , _origin(origin)
        , _brakeSpeed(brakeSpeed)
        , _colour(colour)
        , _seatRotation(seatRotation)
        , _trackPlaceFlags(liftHillAndAlternativeState)
        , _fromTrackDesign(fromTrackDesign)
    {
        _origin.direction &= 3;
    }

    void AcceptParameters(GameActionParameterVisitor & visitor) override
    {
        visitor.Visit(_origin);
        visitor.Visit("ride", _rideIndex);
        visitor.Visit("trackType", _trackType);
        visitor.Visit("brakeSpeed", _brakeSpeed);
        visitor.Visit("colour", _colour);
        visitor.Visit("seatRotation", _seatRotation);
        visitor.Visit("trackPlaceFlags", _trackPlaceFlags);
        visitor.Visit("isFromTrackDesign", _fromTrackDesign);
    }

    uint16_t GetActionFlags() const override final
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_rideIndex) << DS_TAG(_trackType) << DS_TAG(_origin) << DS_TAG(_brakeSpeed) << DS_TAG(_colour)
               << DS_TAG(_seatRotation) << DS_TAG(_trackPlaceFlags);
    }

    GameActionResult::Ptr Query() const override
    {
        auto ride = get_ride(_rideIndex);
        if (ride == nullptr)
        {
            log_warning("Invalid ride for track placement, rideIndex = %d", static_cast<int32_t>(_rideIndex));
            return std::make_unique<TrackPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }
        rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);
        if (rideEntry == nullptr)
        {
            log_warning("Invalid ride subtype for track placement, rideIndex = %d", static_cast<int32_t>(_rideIndex));
            return std::make_unique<TrackPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        if (!direction_valid(_origin.direction))
        {
            log_warning("Invalid direction for track placement, direction = %d", _origin.direction);
            return std::make_unique<TrackPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        auto res = std::make_unique<TrackPlaceActionResult>();
        res->Expenditure = ExpenditureType::RideConstruction;
        res->Position.x = _origin.x + 16;
        res->Position.y = _origin.y + 16;
        res->Position.z = _origin.z;

        res->GroundFlags = 0;

        uint32_t rideTypeFlags = RideTypeDescriptors[ride->type].Flags;

        if ((ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK) && _trackType == TrackElemType::EndStation)
        {
            return std::make_unique<TrackPlaceActionResult>(GA_ERROR::DISALLOWED, STR_NOT_ALLOWED_TO_MODIFY_STATION);
        }

        if (!(GetActionFlags() & GA_FLAGS::ALLOW_WHILE_PAUSED))
        {
            if (game_is_paused() && !gCheatsBuildInPauseMode)
            {
                return std::make_unique<TrackPlaceActionResult>(
                    GA_ERROR::DISALLOWED, STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED);
            }
        }

        if (!(rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE))
        {
            if (_trackType == TrackElemType::OnRidePhoto)
            {
                if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO)
                {
                    return std::make_unique<TrackPlaceActionResult>(GA_ERROR::DISALLOWED, STR_ONLY_ONE_ON_RIDE_PHOTO_PER_RIDE);
                }
            }
            else if (_trackType == TrackElemType::CableLiftHill)
            {
                if (ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED)
                {
                    return std::make_unique<TrackPlaceActionResult>(
                        GA_ERROR::DISALLOWED, STR_ONLY_ONE_CABLE_LIFT_HILL_PER_RIDE);
                }
            }
            // Backwards steep lift hills are allowed, even on roller coasters that do not support forwards steep lift hills.
            if ((_trackPlaceFlags & CONSTRUCTION_LIFT_HILL_SELECTED)
                && !RideTypeDescriptors[ride->type].SupportsTrackPiece(TRACK_LIFT_HILL_STEEP)
                && !gCheatsEnableChainLiftOnAllTrack)
            {
                if (TrackFlags[_trackType] & TRACK_ELEM_FLAG_IS_STEEP_UP)
                {
                    return std::make_unique<TrackPlaceActionResult>(GA_ERROR::DISALLOWED, STR_TOO_STEEP_FOR_LIFT_HILL);
                }
            }
        }

        money32 cost = 0;
        const rct_preview_track* trackBlock = get_track_def_from_ride(ride, _trackType);
        uint32_t numElements = 0;
        // First check if any of the track pieces are outside the park
        for (; trackBlock->index != 0xFF; trackBlock++)
        {
            auto rotatedTrack = CoordsXYZ{ CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(_origin.direction), 0 };
            auto tileCoords = CoordsXYZ{ _origin.x, _origin.y, _origin.z } + rotatedTrack;

            if (!LocationValid(tileCoords) || (!map_is_location_owned(tileCoords) && !gCheatsSandboxMode))
            {
                return std::make_unique<TrackPlaceActionResult>(GA_ERROR::DISALLOWED, STR_LAND_NOT_OWNED_BY_PARK);
            }
            numElements++;
        }

        if (!map_check_free_elements_and_reorganise(numElements))
        {
            log_warning("Not enough free map elments to place track.");
            return std::make_unique<TrackPlaceActionResult>(GA_ERROR::NO_FREE_ELEMENTS, STR_TILE_ELEMENT_LIMIT_REACHED);
        }
        const uint16_t* trackFlags = (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE) ? FlatTrackFlags : TrackFlags;
        if (!gCheatsAllowTrackPlaceInvalidHeights)
        {
            if (trackFlags[_trackType] & TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT)
            {
                if ((_origin.z & 0x0F) != 8)
                {
                    return std::make_unique<TrackPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_CONSTRUCTION_ERR_UNKNOWN);
                }
            }
            else
            {
                if ((_origin.z & 0x0F) != 0)
                {
                    return std::make_unique<TrackPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_CONSTRUCTION_ERR_UNKNOWN);
                }
            }
        }

        // If that is not the case, then perform the remaining checks
        trackBlock = get_track_def_from_ride(ride, _trackType);

        for (int32_t blockIndex = 0; trackBlock->index != 0xFF; trackBlock++, blockIndex++)
        {
            auto rotatedTrack = CoordsXYZ{ CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(_origin.direction), trackBlock->z };
            auto mapLoc = CoordsXYZ{ _origin.x, _origin.y, _origin.z } + rotatedTrack;
            auto quarterTile = trackBlock->var_08.Rotate(_origin.direction);

            if (mapLoc.z < 16)
            {
                return std::make_unique<TrackPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_TOO_LOW);
            }

            int32_t baseZ = floor2(mapLoc.z, COORDS_Z_STEP);

            int32_t clearanceZ = trackBlock->var_07;
            if (trackBlock->flags & RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL
                && RideTypeDescriptors[ride->type].Heights.ClearanceHeight > 24)
            {
                clearanceZ += 24;
            }
            else
            {
                clearanceZ += RideTypeDescriptors[ride->type].Heights.ClearanceHeight;
            }

            clearanceZ = floor2(clearanceZ, COORDS_Z_STEP) + baseZ;

            if (clearanceZ > MAX_TRACK_HEIGHT)
            {
                return std::make_unique<TrackPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_TOO_HIGH);
            }

            uint8_t crossingMode = (RideTypeDescriptors[ride->type].HasFlag(RIDE_TYPE_FLAG_SUPPORTS_LEVEL_CROSSINGS)
                                    && _trackType == TrackElemType::Flat)
                ? CREATE_CROSSING_MODE_TRACK_OVER_PATH
                : CREATE_CROSSING_MODE_NONE;
            if (!map_can_construct_with_clear_at(
                    { mapLoc, baseZ, clearanceZ }, &map_place_non_scenery_clear_func, quarterTile, GetFlags(), &cost,
                    crossingMode))
            {
                return std::make_unique<TrackPlaceActionResult>(
                    GA_ERROR::NO_CLEARANCE, gGameCommandErrorText, gCommonFormatArgs);
            }

            // When building a level crossing, remove any pre-existing path furniture.
            if (crossingMode == CREATE_CROSSING_MODE_TRACK_OVER_PATH)
            {
                auto footpathElement = map_get_footpath_element(mapLoc);
                if (footpathElement != nullptr && footpathElement->AsPath()->HasAddition())
                {
                    footpathElement->AsPath()->SetAddition(0);
                }
            }

            uint8_t mapGroundFlags = gMapGroundFlags & (ELEMENT_IS_ABOVE_GROUND | ELEMENT_IS_UNDERGROUND);
            if (res->GroundFlags != 0 && (res->GroundFlags & mapGroundFlags) == 0)
            {
                return std::make_unique<TrackPlaceActionResult>(
                    GA_ERROR::DISALLOWED, STR_CANT_BUILD_PARTLY_ABOVE_AND_PARTLY_BELOW_GROUND);
            }

            res->GroundFlags = mapGroundFlags;
            if (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE)
            {
                if (FlatTrackFlags[_trackType] & TRACK_ELEM_FLAG_ONLY_ABOVE_GROUND)
                {
                    if (res->GroundFlags & ELEMENT_IS_UNDERGROUND)
                    {
                        return std::make_unique<TrackPlaceActionResult>(
                            GA_ERROR::DISALLOWED, STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND);
                    }
                }
            }
            else
            {
                if (TrackFlags[_trackType] & TRACK_ELEM_FLAG_ONLY_ABOVE_GROUND)
                {
                    if (res->GroundFlags & ELEMENT_IS_UNDERGROUND)
                    {
                        return std::make_unique<TrackPlaceActionResult>(
                            GA_ERROR::DISALLOWED, STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND);
                    }
                }
            }

            if (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE)
            {
                if (FlatTrackFlags[_trackType] & TRACK_ELEM_FLAG_ONLY_UNDERWATER)
                {
                    if (!(gMapGroundFlags & ELEMENT_IS_UNDERWATER))
                    {
                        return std::make_unique<TrackPlaceActionResult>(
                            GA_ERROR::DISALLOWED, STR_CAN_ONLY_BUILD_THIS_UNDERWATER);
                    }
                }
            }
            else
            {
                if (TrackFlags[_trackType] & TRACK_ELEM_FLAG_ONLY_UNDERWATER)
                { // No element has this flag
                    if (gMapGroundFlags & ELEMENT_IS_UNDERWATER)
                    {
                        return std::make_unique<TrackPlaceActionResult>(
                            GA_ERROR::DISALLOWED, STR_CAN_ONLY_BUILD_THIS_UNDERWATER);
                    }
                }
            }

            if (gMapGroundFlags & ELEMENT_IS_UNDERWATER && !gCheatsDisableClearanceChecks)
            {
                return std::make_unique<TrackPlaceActionResult>(GA_ERROR::DISALLOWED, STR_RIDE_CANT_BUILD_THIS_UNDERWATER);
            }

            if ((rideTypeFlags & RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER) && !byte_9D8150)
            {
                auto surfaceElement = map_get_surface_element_at(mapLoc);
                if (surfaceElement == nullptr)
                    return std::make_unique<TrackPlaceActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

                auto waterHeight = surfaceElement->GetWaterHeight();
                if (waterHeight == 0)
                {
                    return std::make_unique<TrackPlaceActionResult>(GA_ERROR::DISALLOWED, STR_CAN_ONLY_BUILD_THIS_ON_WATER);
                }

                if (waterHeight != baseZ)
                {
                    return std::make_unique<TrackPlaceActionResult>(GA_ERROR::DISALLOWED, STR_CAN_ONLY_BUILD_THIS_ON_WATER);
                }
                waterHeight -= LAND_HEIGHT_STEP;
                if (waterHeight == surfaceElement->GetBaseZ())
                {
                    uint8_t slope = surfaceElement->GetSlope() & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP;
                    if (slope == TILE_ELEMENT_SLOPE_W_CORNER_DN || slope == TILE_ELEMENT_SLOPE_S_CORNER_DN
                        || slope == TILE_ELEMENT_SLOPE_E_CORNER_DN || slope == TILE_ELEMENT_SLOPE_N_CORNER_DN)
                    {
                        return std::make_unique<TrackPlaceActionResult>(GA_ERROR::DISALLOWED, STR_CAN_ONLY_BUILD_THIS_ON_WATER);
                    }
                }
            }

            int32_t entranceDirections;
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
            {
                entranceDirections = FlatRideTrackSequenceProperties[_trackType][0];
            }
            else
            {
                entranceDirections = TrackSequenceProperties[_trackType][0];
            }
            if ((entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN) && trackBlock->index == 0)
            {
                if (!track_add_station_element({ mapLoc, baseZ, _origin.direction }, _rideIndex, 0, _fromTrackDesign))
                {
                    return std::make_unique<TrackPlaceActionResult>(GA_ERROR::UNKNOWN, gGameCommandErrorText);
                }
            }

            // 6c5648 12 push
            auto surfaceElement = map_get_surface_element_at(mapLoc);
            if (surfaceElement == nullptr)
                return std::make_unique<TrackPlaceActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

            if (!gCheatsDisableSupportLimits)
            {
                int32_t ride_height = clearanceZ - surfaceElement->GetBaseZ();
                if (ride_height >= 0)
                {
                    uint16_t maxHeight;

                    if (RideTypeDescriptors[ride->type].HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY)
                        && rideEntry->max_height != 0)
                    {
                        maxHeight = rideEntry->max_height;
                    }
                    else
                    {
                        maxHeight = RideTypeDescriptors[ride->type].Heights.MaxHeight;
                    }

                    ride_height /= COORDS_Z_PER_TINY_Z;
                    if (ride_height > maxHeight && !byte_9D8150)
                    {
                        return std::make_unique<TrackPlaceActionResult>(GA_ERROR::DISALLOWED, STR_TOO_HIGH_FOR_SUPPORTS);
                    }
                }
            }

            int32_t supportHeight = baseZ - surfaceElement->GetBaseZ();
            if (supportHeight < 0)
            {
                supportHeight = (10 * COORDS_Z_STEP);
            }

            cost += ((supportHeight / (2 * COORDS_Z_STEP)) * RideTypeDescriptors[ride->type].BuildCosts.SupportPrice) * 5;
        }

        money32 price = RideTypeDescriptors[ride->type].BuildCosts.TrackPrice;
        price *= (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE) ? FlatRideTrackPricing[_trackType] : TrackPricing[_trackType];

        price >>= 16;
        res->Cost = cost + ((price / 2) * 10);
        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        auto ride = get_ride(_rideIndex);
        if (ride == nullptr)
        {
            log_warning("Invalid ride for track placement, rideIndex = %d", static_cast<int32_t>(_rideIndex));
            return std::make_unique<TrackPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS);
        }

        rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);
        if (rideEntry == nullptr)
        {
            log_warning("Invalid ride subtype for track placement, rideIndex = %d", static_cast<int32_t>(_rideIndex));
            return std::make_unique<TrackPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS);
        }

        auto res = std::make_unique<TrackPlaceActionResult>();
        res->Expenditure = ExpenditureType::RideConstruction;
        res->Position.x = _origin.x + 16;
        res->Position.y = _origin.y + 16;
        res->Position.z = _origin.z;

        res->GroundFlags = 0;

        uint32_t rideTypeFlags = RideTypeDescriptors[ride->type].Flags;

        const uint8_t(*wallEdges)[16];
        if (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE)
        {
            wallEdges = &FlatRideTrackSequenceElementAllowedWallEdges[_trackType];
        }
        else
        {
            wallEdges = &TrackSequenceElementAllowedWallEdges[_trackType];
        }

        money32 cost = 0;
        const rct_preview_track* trackBlock = get_track_def_from_ride(ride, _trackType);

        trackBlock = get_track_def_from_ride(ride, _trackType);
        for (int32_t blockIndex = 0; trackBlock->index != 0xFF; trackBlock++, blockIndex++)
        {
            auto rotatedTrack = CoordsXYZ{ CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(_origin.direction), trackBlock->z };
            auto mapLoc = CoordsXYZ{ _origin.x, _origin.y, _origin.z } + rotatedTrack;

            auto quarterTile = trackBlock->var_08.Rotate(_origin.direction);

            int32_t baseZ = floor2(mapLoc.z, COORDS_Z_STEP);
            int32_t clearanceZ = trackBlock->var_07;
            if (trackBlock->flags & RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL
                && RideTypeDescriptors[ride->type].Heights.ClearanceHeight > 24)
            {
                clearanceZ += 24;
            }
            else
            {
                clearanceZ += RideTypeDescriptors[ride->type].Heights.ClearanceHeight;
            }

            clearanceZ = floor2(clearanceZ, COORDS_Z_STEP) + baseZ;
            const auto mapLocWithClearance = CoordsXYRangedZ(mapLoc, baseZ, clearanceZ);

            uint8_t crossingMode = (RideTypeDescriptors[ride->type].HasFlag(RIDE_TYPE_FLAG_SUPPORTS_LEVEL_CROSSINGS)
                                    && _trackType == TrackElemType::Flat)
                ? CREATE_CROSSING_MODE_TRACK_OVER_PATH
                : CREATE_CROSSING_MODE_NONE;
            if (!map_can_construct_with_clear_at(
                    mapLocWithClearance, &map_place_non_scenery_clear_func, quarterTile, GetFlags() | GAME_COMMAND_FLAG_APPLY,
                    &cost, crossingMode))
            {
                return std::make_unique<TrackPlaceActionResult>(
                    GA_ERROR::NO_CLEARANCE, gGameCommandErrorText, gCommonFormatArgs);
            }

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
                    uint8_t intersectingDirections = (*wallEdges)[blockIndex];
                    intersectingDirections ^= 0x0F;
                    intersectingDirections = rol4(intersectingDirections, _origin.direction);
                    for (int32_t i = 0; i < NumOrthogonalDirections; i++)
                    {
                        if (intersectingDirections & (1 << i))
                        {
                            wall_remove_intersecting_walls(mapLocWithClearance, i);
                        }
                    }
                }
            }

            uint8_t mapGroundFlags = gMapGroundFlags & (ELEMENT_IS_ABOVE_GROUND | ELEMENT_IS_UNDERGROUND);
            if (res->GroundFlags != 0 && (res->GroundFlags & mapGroundFlags) == 0)
            {
                return std::make_unique<TrackPlaceActionResult>(
                    GA_ERROR::DISALLOWED, STR_CANT_BUILD_PARTLY_ABOVE_AND_PARTLY_BELOW_GROUND);
            }

            res->GroundFlags = mapGroundFlags;

            // 6c5648 12 push
            auto surfaceElement = map_get_surface_element_at(mapLoc);
            if (surfaceElement == nullptr)
                return std::make_unique<TrackPlaceActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

            int32_t supportHeight = baseZ - surfaceElement->GetBaseZ();
            if (supportHeight < 0)
            {
                supportHeight = (10 * COORDS_Z_STEP);
            }

            cost += ((supportHeight / (2 * COORDS_Z_STEP)) * RideTypeDescriptors[ride->type].BuildCosts.SupportPrice) * 5;

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
                    ride->num_block_brakes++;
                    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_OPERATING;

                    ride->mode = RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED;
                    if (ride->type == RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER)
                        ride->mode = RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED;

                    break;
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

            int32_t entranceDirections = 0;
            if (!ride->overall_view.isNull())
            {
                if (!(GetFlags() & GAME_COMMAND_FLAG_NO_SPEND))
                {
                    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
                    {
                        entranceDirections = FlatRideTrackSequenceProperties[_trackType][0];
                    }
                    else
                    {
                        entranceDirections = TrackSequenceProperties[_trackType][0];
                    }
                }
            }

            if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN || ride->overall_view.isNull())
            {
                ride->overall_view = mapLoc;
            }

            auto tileElement = tile_element_insert(mapLoc, quarterTile.GetBaseQuarterOccupied());
            assert(tileElement != nullptr);
            tileElement->SetClearanceZ(clearanceZ);
            tileElement->SetType(TILE_ELEMENT_TYPE_TRACK);
            tileElement->SetDirection(_origin.direction);
            if (_trackPlaceFlags & CONSTRUCTION_LIFT_HILL_SELECTED)
            {
                tileElement->AsTrack()->SetHasChain(true);
            }

            tileElement->AsTrack()->SetSequenceIndex(trackBlock->index);
            tileElement->AsTrack()->SetRideIndex(_rideIndex);
            tileElement->AsTrack()->SetTrackType(_trackType);
            if (GetFlags() & GAME_COMMAND_FLAG_GHOST)
            {
                tileElement->SetGhost(true);
            }

            switch (_trackType)
            {
                case TrackElemType::Waterfall:
                    map_animation_create(MAP_ANIMATION_TYPE_TRACK_WATERFALL, CoordsXYZ{ mapLoc, tileElement->GetBaseZ() });
                    break;
                case TrackElemType::Rapids:
                    map_animation_create(MAP_ANIMATION_TYPE_TRACK_RAPIDS, CoordsXYZ{ mapLoc, tileElement->GetBaseZ() });
                    break;
                case TrackElemType::Whirlpool:
                    map_animation_create(MAP_ANIMATION_TYPE_TRACK_WHIRLPOOL, CoordsXYZ{ mapLoc, tileElement->GetBaseZ() });
                    break;
                case TrackElemType::SpinningTunnel:
                    map_animation_create(MAP_ANIMATION_TYPE_TRACK_SPINNINGTUNNEL, CoordsXYZ{ mapLoc, tileElement->GetBaseZ() });
                    break;
            }
            if (TrackTypeHasSpeedSetting(_trackType))
            {
                tileElement->AsTrack()->SetBrakeBoosterSpeed(_brakeSpeed);
            }
            else
            {
                tileElement->AsTrack()->SetSeatRotation(_seatRotation);
            }

            if (_trackPlaceFlags & RIDE_TYPE_ALTERNATIVE_TRACK_TYPE)
            {
                tileElement->AsTrack()->SetInverted(true);
            }
            tileElement->AsTrack()->SetColourScheme(_colour);

            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
            {
                entranceDirections = FlatRideTrackSequenceProperties[_trackType][0];
            }
            else
            {
                entranceDirections = TrackSequenceProperties[_trackType][0];
            }

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

            if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN)
            {
                if (trackBlock->index == 0)
                {
                    track_add_station_element(
                        { mapLoc, _origin.direction }, _rideIndex, GAME_COMMAND_FLAG_APPLY, _fromTrackDesign);
                }
                sub_6CB945(ride);
                ride->UpdateMaxVehicles();
            }

            if (rideTypeFlags & RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER)
            {
                auto* waterSurfaceElement = map_get_surface_element_at(mapLoc);
                if (waterSurfaceElement != nullptr)
                {
                    waterSurfaceElement->SetHasTrackThatNeedsWater(true);
                    tileElement = reinterpret_cast<TileElement*>(waterSurfaceElement);
                }
            }

            if (!gCheatsDisableClearanceChecks || !(GetFlags() & GAME_COMMAND_FLAG_GHOST))
            {
                footpath_connect_edges(mapLoc, tileElement, GetFlags());
            }
            map_invalidate_tile_full(mapLoc);
        }

        money32 price = RideTypeDescriptors[ride->type].BuildCosts.TrackPrice;
        price *= (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE) ? FlatRideTrackPricing[_trackType] : TrackPricing[_trackType];

        price >>= 16;
        res->Cost = cost + ((price / 2) * 10);
        return res;
    }
};
