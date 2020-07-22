/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../OpenRCT2.h"
#include "../management/Finance.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../ride/TrackDesign.h"
#include "../world/Banner.h"
#include "../world/LargeScenery.h"
#include "../world/MapAnimation.h"
#include "../world/Scenery.h"
#include "../world/SmallScenery.h"
#include "../world/Surface.h"
#include "../world/Wall.h"
#include "GameAction.h"

class WallPlaceActionResult final : public GameActionResult
{
public:
    WallPlaceActionResult()
        : GameActionResult(GA_ERROR::OK, STR_CANT_BUILD_PARK_ENTRANCE_HERE)
    {
    }

    WallPlaceActionResult(GA_ERROR err)
        : GameActionResult(err, STR_CANT_BUILD_PARK_ENTRANCE_HERE)
    {
    }

    WallPlaceActionResult(GA_ERROR err, rct_string_id msg)
        : GameActionResult(err, STR_CANT_BUILD_PARK_ENTRANCE_HERE, msg)
    {
    }

    WallPlaceActionResult(GA_ERROR error, rct_string_id msg, uint8_t* args)
        : GameActionResult(error, STR_CANT_BUILD_PARK_ENTRANCE_HERE, msg, args)
    {
    }

    TileElement* tileElement = nullptr;
};

DEFINE_GAME_ACTION(WallPlaceAction, GAME_COMMAND_PLACE_WALL, WallPlaceActionResult)
{
private:
    int32_t _wallType{ -1 };
    CoordsXYZ _loc;
    Direction _edge{ INVALID_DIRECTION };
    int32_t _primaryColour{ COLOUR_BLACK };
    int32_t _secondaryColour{ COLOUR_BLACK };
    int32_t _tertiaryColour{ COLOUR_BLACK };
    BannerIndex _bannerId{ BANNER_INDEX_NULL };

public:
    WallPlaceAction() = default;

    WallPlaceAction(
        int32_t wallType, const CoordsXYZ& loc, uint8_t edge, int32_t primaryColour, int32_t secondaryColour,
        int32_t tertiaryColour)
        : _wallType(wallType)
        , _loc(loc)
        , _edge(edge)
        , _primaryColour(primaryColour)
        , _secondaryColour(secondaryColour)
        , _tertiaryColour(tertiaryColour)
    {
        rct_scenery_entry* sceneryEntry = get_wall_entry(_wallType);
        if (sceneryEntry != nullptr)
        {
            if (sceneryEntry->wall.scrolling_mode != SCROLLING_MODE_NONE)
            {
                _bannerId = create_new_banner(0);
            }
        }
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_wallType) << DS_TAG(_loc) << DS_TAG(_edge) << DS_TAG(_primaryColour) << DS_TAG(_secondaryColour)
               << DS_TAG(_tertiaryColour) << DS_TAG(_bannerId);
    }

    GameActionResult::Ptr Query() const override
    {
        auto res = std::make_unique<WallPlaceActionResult>();
        res->ErrorTitle = STR_CANT_BUILD_PARK_ENTRANCE_HERE;
        res->Position = _loc;

        res->Expenditure = ExpenditureType::Landscaping;
        res->Position.x += 16;
        res->Position.y += 16;

        if (_loc.z == 0)
        {
            res->Position.z = tile_element_height(res->Position);
        }

        if (!LocationValid(_loc))
        {
            return MakeResult(GA_ERROR::NOT_OWNED);
        }

        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !(GetFlags() & GAME_COMMAND_FLAG_PATH_SCENERY)
            && !gCheatsSandboxMode)
        {
            if (_loc.z == 0)
            {
                if (!map_is_location_in_park(_loc))
                {
                    return std::make_unique<WallPlaceActionResult>(GA_ERROR::NOT_OWNED);
                }
            }
            else if (!map_is_location_owned(_loc))
            {
                return std::make_unique<WallPlaceActionResult>(GA_ERROR::NOT_OWNED);
            }
        }
        else if (!byte_9D8150 && (_loc.x > gMapSizeMaxXY || _loc.y > gMapSizeMaxXY))
        {
            log_error("Invalid x/y coordinates. x = %d y = %d", _loc.x, _loc.y);
            return std::make_unique<WallPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS);
        }

        if (_edge > 3)
        {
            return std::make_unique<WallPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS);
        }

        uint8_t edgeSlope = 0;
        auto targetHeight = _loc.z;
        if (targetHeight == 0)
        {
            auto* surfaceElement = map_get_surface_element_at(_loc);
            if (surfaceElement == nullptr)
            {
                log_error("Surface element not found at %d, %d.", _loc.x, _loc.y);
                return std::make_unique<WallPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS);
            }
            targetHeight = surfaceElement->GetBaseZ();

            uint8_t slope = surfaceElement->GetSlope();
            edgeSlope = LandSlopeToWallSlope[slope][_edge & 3];
            if (edgeSlope & EDGE_SLOPE_ELEVATED)
            {
                targetHeight += 16;
                edgeSlope &= ~EDGE_SLOPE_ELEVATED;
            }
        }

        auto* surfaceElement = map_get_surface_element_at(_loc);
        if (surfaceElement == nullptr)
        {
            log_error("Surface element not found at %d, %d.", _loc.x, _loc.y);
            return std::make_unique<WallPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS);
        }

        if (surfaceElement->GetWaterHeight() > 0)
        {
            uint16_t waterHeight = surfaceElement->GetWaterHeight();

            if (targetHeight < waterHeight && !gCheatsDisableClearanceChecks)
            {
                return std::make_unique<WallPlaceActionResult>(GA_ERROR::DISALLOWED, STR_CANT_BUILD_THIS_UNDERWATER);
            }
        }

        if (targetHeight < surfaceElement->GetBaseZ() && !gCheatsDisableClearanceChecks)
        {
            return std::make_unique<WallPlaceActionResult>(GA_ERROR::DISALLOWED, STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND);
        }

        if (!(edgeSlope & (EDGE_SLOPE_UPWARDS | EDGE_SLOPE_DOWNWARDS)))
        {
            uint8_t newEdge = (_edge + 2) & 3;
            uint8_t newBaseHeight = surfaceElement->base_height;
            newBaseHeight += 2;
            if (surfaceElement->GetSlope() & (1 << newEdge))
            {
                if (targetHeight / 8 < newBaseHeight)
                {
                    return std::make_unique<WallPlaceActionResult>(GA_ERROR::DISALLOWED, STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND);
                }

                if (surfaceElement->GetSlope() & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
                {
                    newEdge = (newEdge - 1) & 3;

                    if (surfaceElement->GetSlope() & (1 << newEdge))
                    {
                        newEdge = (newEdge + 2) & 3;
                        if (surfaceElement->GetSlope() & (1 << newEdge))
                        {
                            newBaseHeight += 2;
                            if (targetHeight / 8 < newBaseHeight)
                            {
                                return std::make_unique<WallPlaceActionResult>(
                                    GA_ERROR::DISALLOWED, STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND);
                            }
                            newBaseHeight -= 2;
                        }
                    }
                }
            }

            newEdge = (_edge + 3) & 3;
            if (surfaceElement->GetSlope() & (1 << newEdge))
            {
                if (targetHeight / 8 < newBaseHeight)
                {
                    return std::make_unique<WallPlaceActionResult>(GA_ERROR::DISALLOWED, STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND);
                }

                if (surfaceElement->GetSlope() & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
                {
                    newEdge = (newEdge - 1) & 3;

                    if (surfaceElement->GetSlope() & (1 << newEdge))
                    {
                        newEdge = (newEdge + 2) & 3;
                        if (surfaceElement->GetSlope() & (1 << newEdge))
                        {
                            newBaseHeight += 2;
                            if (targetHeight / 8 < newBaseHeight)
                            {
                                return std::make_unique<WallPlaceActionResult>(
                                    GA_ERROR::DISALLOWED, STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND);
                            }
                        }
                    }
                }
            }
        }

        rct_scenery_entry* wallEntry = get_wall_entry(_wallType);

        if (wallEntry == nullptr)
        {
            log_error("Wall Type not found %d", _wallType);
            return std::make_unique<WallPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS);
        }

        if (wallEntry->wall.scrolling_mode != SCROLLING_MODE_NONE)
        {
            if (_bannerId == BANNER_INDEX_NULL)
            {
                log_error("Banner Index not specified.");
                return std::make_unique<WallPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_TOO_MANY_BANNERS_IN_GAME);
            }

            auto banner = GetBanner(_bannerId);
            if (!banner->IsNull())
            {
                log_error("No free banners available");
                return std::make_unique<WallPlaceActionResult>(GA_ERROR::NO_FREE_ELEMENTS);
            }
        }

        uint8_t clearanceHeight = targetHeight / 8;
        if (edgeSlope & (EDGE_SLOPE_UPWARDS | EDGE_SLOPE_DOWNWARDS))
        {
            if (wallEntry->wall.flags & WALL_SCENERY_CANT_BUILD_ON_SLOPE)
            {
                return std::make_unique<WallPlaceActionResult>(GA_ERROR::DISALLOWED, STR_ERR_UNABLE_TO_BUILD_THIS_ON_SLOPE);
            }
            clearanceHeight += 2;
        }
        clearanceHeight += wallEntry->wall.height;

        bool wallAcrossTrack = false;
        if (!(GetFlags() & GAME_COMMAND_FLAG_PATH_SCENERY) && !gCheatsDisableClearanceChecks)
        {
            auto result = WallCheckObstruction(wallEntry, targetHeight / 8, clearanceHeight, &wallAcrossTrack);
            if (result->Error != GA_ERROR::OK)
            {
                return result;
            }
        }

        if (!map_check_free_elements_and_reorganise(1))
        {
            return MakeResult(GA_ERROR::NO_FREE_ELEMENTS, STR_TILE_ELEMENT_LIMIT_REACHED);
        }

        res->Cost = wallEntry->wall.price;
        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        auto res = std::make_unique<WallPlaceActionResult>();
        res->ErrorTitle = STR_CANT_BUILD_PARK_ENTRANCE_HERE;
        res->Position = _loc;

        res->Expenditure = ExpenditureType::Landscaping;
        res->Position.x += 16;
        res->Position.y += 16;

        if (res->Position.z == 0)
        {
            res->Position.z = tile_element_height(res->Position);
        }

        uint8_t edgeSlope = 0;
        auto targetHeight = _loc.z;
        if (targetHeight == 0)
        {
            auto* surfaceElement = map_get_surface_element_at(_loc);
            if (surfaceElement == nullptr)
            {
                log_error("Surface element not found at %d, %d.", _loc.x, _loc.y);
                return std::make_unique<WallPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS);
            }
            targetHeight = surfaceElement->GetBaseZ();

            uint8_t slope = surfaceElement->GetSlope();
            edgeSlope = LandSlopeToWallSlope[slope][_edge & 3];
            if (edgeSlope & EDGE_SLOPE_ELEVATED)
            {
                targetHeight += 16;
                edgeSlope &= ~EDGE_SLOPE_ELEVATED;
            }
        }
        auto targetLoc = CoordsXYZ(_loc, targetHeight);

        rct_scenery_entry* wallEntry = get_wall_entry(_wallType);

        if (wallEntry == nullptr)
        {
            log_error("Wall Type not found %d", _wallType);
            return std::make_unique<WallPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS);
        }

        uint8_t clearanceHeight = targetHeight / COORDS_Z_STEP;
        if (edgeSlope & (EDGE_SLOPE_UPWARDS | EDGE_SLOPE_DOWNWARDS))
        {
            clearanceHeight += 2;
        }
        clearanceHeight += wallEntry->wall.height;

        bool wallAcrossTrack = false;
        if (!(GetFlags() & GAME_COMMAND_FLAG_PATH_SCENERY) && !gCheatsDisableClearanceChecks)
        {
            auto result = WallCheckObstruction(wallEntry, targetHeight / COORDS_Z_STEP, clearanceHeight, &wallAcrossTrack);
            if (result->Error != GA_ERROR::OK)
            {
                return result;
            }
        }

        if (!map_check_free_elements_and_reorganise(1))
        {
            return MakeResult(GA_ERROR::NO_FREE_ELEMENTS, STR_TILE_ELEMENT_LIMIT_REACHED);
        }

        if (wallEntry->wall.scrolling_mode != SCROLLING_MODE_NONE)
        {
            if (_bannerId == BANNER_INDEX_NULL)
            {
                log_error("Banner Index not specified.");
                return std::make_unique<WallPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_TOO_MANY_BANNERS_IN_GAME);
            }

            auto banner = GetBanner(_bannerId);
            if (!banner->IsNull())
            {
                log_error("No free banners available");
                return std::make_unique<WallPlaceActionResult>(GA_ERROR::NO_FREE_ELEMENTS);
            }

            banner->text = {};
            banner->colour = COLOUR_WHITE;
            banner->text_colour = COLOUR_WHITE;
            banner->flags = BANNER_FLAG_IS_WALL;
            banner->type = 0; // Banner must be deleted after this point in an early return
            banner->position = TileCoordsXY(_loc);

            ride_id_t rideIndex = banner_get_closest_ride_index(targetLoc);
            if (rideIndex != RIDE_ID_NULL)
            {
                banner->ride_index = rideIndex;
                banner->flags |= BANNER_FLAG_LINKED_TO_RIDE;
            }
        }

        TileElement* tileElement = tile_element_insert(targetLoc, 0b0000);
        assert(tileElement != nullptr);

        map_animation_create(MAP_ANIMATION_TYPE_WALL, targetLoc);

        tileElement->SetType(TILE_ELEMENT_TYPE_WALL);
        WallElement* wallElement = tileElement->AsWall();
        wallElement->clearance_height = clearanceHeight;
        wallElement->SetDirection(_edge);
        wallElement->SetSlope(edgeSlope);

        wallElement->SetPrimaryColour(_primaryColour);
        wallElement->SetSecondaryColour(_secondaryColour);

        if (wallAcrossTrack)
        {
            wallElement->SetAcrossTrack(true);
        }

        wallElement->SetEntryIndex(_wallType);
        if (_bannerId != BANNER_INDEX_NULL)
        {
            wallElement->SetBannerIndex(_bannerId);
        }

        if (wallEntry->wall.flags & WALL_SCENERY_HAS_TERNARY_COLOUR)
        {
            wallElement->SetTertiaryColour(_tertiaryColour);
        }

        if (GetFlags() & GAME_COMMAND_FLAG_GHOST)
        {
            wallElement->SetGhost(true);
        }

        res->tileElement = tileElement;
        map_invalidate_tile_zoom1({ _loc, wallElement->GetBaseZ(), wallElement->GetBaseZ() + 72 });

        res->Cost = wallEntry->wall.price;
        return res;
    }

private:
    /**
     *
     *  rct2: 0x006E5CBA
     */
    bool WallCheckObstructionWithTrack(rct_scenery_entry * wall, int32_t z0, TrackElement * trackElement, bool* wallAcrossTrack)
        const
    {
        track_type_t trackType = trackElement->GetTrackType();
        int32_t sequence = trackElement->GetSequenceIndex();
        int32_t direction = (_edge - trackElement->GetDirection()) & TILE_ELEMENT_DIRECTION_MASK;
        auto ride = get_ride(trackElement->GetRideIndex());
        if (ride == nullptr)
        {
            return false;
        }

        if (TrackIsAllowedWallEdges(ride->type, trackType, sequence, direction))
        {
            return true;
        }

        if (!(wall->wall.flags & WALL_SCENERY_IS_DOOR))
        {
            return false;
        }

        if (!(RideTypeDescriptors[ride->type].Flags & RIDE_TYPE_FLAG_ALLOW_DOORS_ON_TRACK))
        {
            return false;
        }

        *wallAcrossTrack = true;
        if (z0 & 1)
        {
            return false;
        }

        int32_t z;
        if (sequence == 0)
        {
            if (TrackSequenceProperties[trackType][0] & TRACK_SEQUENCE_FLAG_DISALLOW_DOORS)
            {
                return false;
            }

            if (TrackDefinitions[trackType].bank_start == 0)
            {
                if (!(TrackCoordinates[trackType].rotation_begin & 4))
                {
                    direction = direction_reverse(trackElement->GetDirection());
                    if (direction == _edge)
                    {
                        const rct_preview_track* trackBlock = &TrackBlocks[trackType][sequence];
                        z = TrackCoordinates[trackType].z_begin;
                        z = trackElement->base_height + ((z - trackBlock->z) * 8);
                        if (z == z0)
                        {
                            return true;
                        }
                    }
                }
            }
        }

        const rct_preview_track* trackBlock = &TrackBlocks[trackType][sequence + 1];
        if (trackBlock->index != 0xFF)
        {
            return false;
        }

        if (TrackDefinitions[trackType].bank_end != 0)
        {
            return false;
        }

        direction = TrackCoordinates[trackType].rotation_end;
        if (direction & 4)
        {
            return false;
        }

        direction = (trackElement->GetDirection() + TrackCoordinates[trackType].rotation_end) & TILE_ELEMENT_DIRECTION_MASK;
        if (direction != _edge)
        {
            return false;
        }

        trackBlock = &TrackBlocks[trackType][sequence];
        z = TrackCoordinates[trackType].z_end;
        z = trackElement->base_height + ((z - trackBlock->z) * 8);
        return z == z0;
    }

    /**
     *
     *  rct2: 0x006E5C1A
     */
    GameActionResult::Ptr WallCheckObstruction(rct_scenery_entry * wall, int32_t z0, int32_t z1, bool* wallAcrossTrack) const
    {
        int32_t entryType, sequence;
        rct_scenery_entry* entry;
        rct_large_scenery_tile* tile;

        *wallAcrossTrack = false;
        gMapGroundFlags = ELEMENT_IS_ABOVE_GROUND;
        if (map_is_location_at_edge(_loc))
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_OFF_EDGE_OF_MAP);
        }

        TileElement* tileElement = map_get_first_element_at(_loc);
        do
        {
            if (tileElement == nullptr)
                break;
            int32_t elementType = tileElement->GetType();
            if (elementType == TILE_ELEMENT_TYPE_SURFACE)
                continue;
            if (tileElement->IsGhost())
                continue;
            if (z0 >= tileElement->clearance_height)
                continue;
            if (z1 <= tileElement->base_height)
                continue;
            if (elementType == TILE_ELEMENT_TYPE_WALL)
            {
                int32_t direction = tileElement->GetDirection();
                if (_edge == direction)
                {
                    auto res = MakeResult(GA_ERROR::NO_CLEARANCE, STR_NONE);
                    map_obstruction_set_error_text(tileElement, *res);
                    return res;
                }
                continue;
            }
            if (tileElement->GetOccupiedQuadrants() == 0)
                continue;
            auto res = MakeResult(GA_ERROR::NO_CLEARANCE, STR_NONE);
            switch (elementType)
            {
                case TILE_ELEMENT_TYPE_ENTRANCE:
                    map_obstruction_set_error_text(tileElement, *res);
                    return res;
                case TILE_ELEMENT_TYPE_PATH:
                    if (tileElement->AsPath()->GetEdges() & (1 << _edge))
                    {
                        map_obstruction_set_error_text(tileElement, *res);
                        return res;
                    }
                    break;
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                    entryType = tileElement->AsLargeScenery()->GetEntryIndex();
                    sequence = tileElement->AsLargeScenery()->GetSequenceIndex();
                    entry = get_large_scenery_entry(entryType);
                    tile = &entry->large_scenery.tiles[sequence];
                    {
                        int32_t direction = ((_edge - tileElement->GetDirection()) & TILE_ELEMENT_DIRECTION_MASK) + 8;
                        if (!(tile->flags & (1 << direction)))
                        {
                            map_obstruction_set_error_text(tileElement, *res);
                            return res;
                        }
                    }
                    break;
                case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                    entry = tileElement->AsSmallScenery()->GetEntry();
                    if (scenery_small_entry_has_flag(entry, SMALL_SCENERY_FLAG_NO_WALLS))
                    {
                        map_obstruction_set_error_text(tileElement, *res);
                        return res;
                    }
                    break;
                case TILE_ELEMENT_TYPE_TRACK:
                    if (!WallCheckObstructionWithTrack(wall, z0, tileElement->AsTrack(), wallAcrossTrack))
                    {
                        return res;
                    }
                    break;
            }
        } while (!(tileElement++)->IsLastForTile());

        return MakeResult();
    }

    /**
     * Gets whether the given track type can have a wall placed on the edge of the given direction.
     * Some thin tracks for example are allowed to have walls either side of the track, but wider tracks can not.
     */
    static bool TrackIsAllowedWallEdges(uint8_t rideType, uint8_t trackType, uint8_t trackSequence, uint8_t direction)
    {
        if (!ride_type_has_flag(rideType, RIDE_TYPE_FLAG_TRACK_NO_WALLS))
        {
            if (ride_type_has_flag(rideType, RIDE_TYPE_FLAG_FLAT_RIDE))
            {
                if (FlatRideTrackSequenceElementAllowedWallEdges[trackType][trackSequence] & (1 << direction))
                {
                    return true;
                }
            }
            else
            {
                if (TrackSequenceElementAllowedWallEdges[trackType][trackSequence] & (1 << direction))
                {
                    return true;
                }
            }
        }
        return false;
    }
};
