/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../OpenRCT2.h"
#include "../management/Finance.h"
#include "../object/ObjectLimits.h"
#include "../ride/Ride.h"
#include "../world/Banner.h"
#include "../world/LargeScenery.h"
#include "../world/MapAnimation.h"
#include "../world/Scenery.h"
#include "GameAction.h"

class LargeSceneryPlaceActionResult final : public GameActionResult
{
public:
    LargeSceneryPlaceActionResult()
        : GameActionResult(GA_ERROR::OK, STR_CANT_POSITION_THIS_HERE)
    {
    }
    LargeSceneryPlaceActionResult(GA_ERROR error)
        : GameActionResult(error, STR_CANT_POSITION_THIS_HERE)
    {
    }
    LargeSceneryPlaceActionResult(GA_ERROR error, rct_string_id message)
        : GameActionResult(error, STR_CANT_POSITION_THIS_HERE, message)
    {
    }
    LargeSceneryPlaceActionResult(GA_ERROR error, rct_string_id message, uint8_t* args)
        : GameActionResult(error, STR_CANT_POSITION_THIS_HERE, message, args)
    {
    }

    uint8_t GroundFlags{ 0 };
};

DEFINE_GAME_ACTION(LargeSceneryPlaceAction, GAME_COMMAND_PLACE_LARGE_SCENERY, LargeSceneryPlaceActionResult)
{
private:
    CoordsXYZD _loc;
    uint8_t _sceneryType{ std::numeric_limits<uint8_t>::max() };
    uint8_t _primaryColour;
    uint8_t _secondaryColour;
    BannerIndex _bannerId{ BANNER_INDEX_NULL };

public:
    LargeSceneryPlaceAction() = default;

    LargeSceneryPlaceAction(CoordsXYZD loc, uint8_t sceneryType, uint8_t primaryColour, uint8_t secondaryColour)
        : _loc(loc)
        , _sceneryType(sceneryType)
        , _primaryColour(primaryColour)
        , _secondaryColour(secondaryColour)
    {
        rct_scenery_entry* sceneryEntry = get_large_scenery_entry(_sceneryType);
        if (sceneryEntry != nullptr)
        {
            if (sceneryEntry->large_scenery.scrolling_mode != SCROLLING_MODE_NONE)
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

        stream << DS_TAG(_loc) << DS_TAG(_sceneryType) << DS_TAG(_primaryColour) << DS_TAG(_secondaryColour)
               << DS_TAG(_bannerId);
    }

    GameActionResult::Ptr Query() const override
    {
        auto res = std::make_unique<LargeSceneryPlaceActionResult>();
        res->ErrorTitle = STR_CANT_POSITION_THIS_HERE;
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
        int16_t surfaceHeight = tile_element_height(_loc.x, _loc.y) & 0xFFFF;
        res->Position.x = _loc.x + 16;
        res->Position.y = _loc.y + 16;
        res->Position.z = surfaceHeight;
        res->GroundFlags = 0;

        money32 supportsCost = 0;

        if (_primaryColour > TILE_ELEMENT_COLOUR_MASK || _secondaryColour > TILE_ELEMENT_COLOUR_MASK)
        {
            log_error(
                "Invalid game command for scenery placement, primaryColour = %u, secondaryColour = %u", _primaryColour,
                _secondaryColour);
            return std::make_unique<LargeSceneryPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS);
        }

        if (_sceneryType >= MAX_LARGE_SCENERY_OBJECTS)
        {
            log_error("Invalid game command for scenery placement, sceneryType = %u", _sceneryType);
            return std::make_unique<LargeSceneryPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS);
        }

        rct_scenery_entry* sceneryEntry = get_large_scenery_entry(_sceneryType);
        if (sceneryEntry == nullptr)
        {
            log_error("Invalid game command for scenery placement, sceneryType = %u", _sceneryType);
            return std::make_unique<LargeSceneryPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS);
        }

        uint32_t totalNumTiles = GetTotalNumTiles(sceneryEntry->large_scenery.tiles);
        int16_t maxHeight = GetMaxSurfaceHeight(sceneryEntry->large_scenery.tiles);

        if (_loc.z != 0)
        {
            maxHeight = _loc.z;
        }

        res->Position.z = maxHeight;

        if (sceneryEntry->large_scenery.scrolling_mode != SCROLLING_MODE_NONE)
        {
            if (_bannerId == BANNER_INDEX_NULL)
            {
                log_error("Banner Index not specified.");
                return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_TOO_MANY_BANNERS_IN_GAME);
            }

            if (gBanners[_bannerId].type != BANNER_NULL)
            {
                log_error("No free banners available");
                return std::make_unique<LargeSceneryPlaceActionResult>(GA_ERROR::NO_FREE_ELEMENTS);
            }
        }

        if (!map_check_free_elements_and_reorganise(totalNumTiles))
        {
            log_error("No free map elements available");
            return std::make_unique<LargeSceneryPlaceActionResult>(GA_ERROR::NO_FREE_ELEMENTS);
        }

        uint8_t tileNum = 0;
        for (rct_large_scenery_tile* tile = sceneryEntry->large_scenery.tiles; tile->x_offset != -1; tile++, tileNum++)
        {
            auto tempX = tile->x_offset;
            auto tempY = tile->y_offset;
            rotate_map_coordinates(&tempX, &tempY, _loc.direction);
            CoordsXY curTile = { tempX, tempY };

            curTile.x += _loc.x;
            curTile.y += _loc.y;

            int32_t zLow = (tile->z_offset + maxHeight) / 8;
            int32_t zHigh = (tile->z_clearance / 8) + zLow;

            QuarterTile quarterTile = QuarterTile{ static_cast<uint8_t>(tile->flags >> 12), 0 }.Rotate(_loc.direction);
            if (!map_can_construct_with_clear_at(
                    curTile.x, curTile.y, zLow, zHigh, &map_place_scenery_clear_func, quarterTile, GetFlags(), &supportsCost,
                    CREATE_CROSSING_MODE_NONE))
            {
                return std::make_unique<LargeSceneryPlaceActionResult>(
                    GA_ERROR::NO_CLEARANCE, gGameCommandErrorText, gCommonFormatArgs);
            }

            int32_t tempSceneryGroundFlags = gMapGroundFlags & (ELEMENT_IS_ABOVE_GROUND | ELEMENT_IS_UNDERGROUND);
            if (!gCheatsDisableClearanceChecks)
            {
                if ((gMapGroundFlags & ELEMENT_IS_UNDERWATER) || (gMapGroundFlags & ELEMENT_IS_UNDERGROUND))
                {
                    return std::make_unique<LargeSceneryPlaceActionResult>(
                        GA_ERROR::DISALLOWED, STR_CANT_BUILD_THIS_UNDERWATER);
                }
                if (res->GroundFlags && !(res->GroundFlags & tempSceneryGroundFlags))
                {
                    return std::make_unique<LargeSceneryPlaceActionResult>(
                        GA_ERROR::DISALLOWED, STR_CANT_BUILD_PARTLY_ABOVE_AND_PARTLY_BELOW_GROUND);
                }
            }

            res->GroundFlags = tempSceneryGroundFlags;

            if (curTile.x >= gMapSizeUnits || curTile.y >= gMapSizeUnits)
            {
                return std::make_unique<LargeSceneryPlaceActionResult>(GA_ERROR::DISALLOWED, STR_OFF_EDGE_OF_MAP);
            }

            if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !map_is_location_owned(curTile.x, curTile.y, zLow * 8)
                && !gCheatsSandboxMode)
            {
                return std::make_unique<LargeSceneryPlaceActionResult>(GA_ERROR::DISALLOWED, STR_LAND_NOT_OWNED_BY_PARK);
            }
        }

        // Force ride construction to recheck area
        _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_RECHECK;

        res->Cost = (sceneryEntry->large_scenery.price * 10) + supportsCost;
        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        auto res = std::make_unique<LargeSceneryPlaceActionResult>();
        res->ErrorTitle = STR_CANT_POSITION_THIS_HERE;

        int16_t surfaceHeight = tile_element_height(_loc.x, _loc.y) & 0xFFFF;
        res->Position.x = _loc.x + 16;
        res->Position.y = _loc.y + 16;
        res->Position.z = surfaceHeight;
        res->GroundFlags = 0;

        money32 supportsCost = 0;

        rct_scenery_entry* sceneryEntry = get_large_scenery_entry(_sceneryType);
        if (sceneryEntry == nullptr)
        {
            log_error("Invalid game command for scenery placement, sceneryType = %u", _sceneryType);
            return std::make_unique<LargeSceneryPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS);
        }

        if (sceneryEntry->large_scenery.tiles == nullptr)
        {
            log_error("Invalid large scenery object, sceneryType = %u", _sceneryType);
            return std::make_unique<LargeSceneryPlaceActionResult>(GA_ERROR::INVALID_PARAMETERS);
        }

        uint32_t totalNumTiles = GetTotalNumTiles(sceneryEntry->large_scenery.tiles);
        int16_t maxHeight = GetMaxSurfaceHeight(sceneryEntry->large_scenery.tiles);

        if (_loc.z != 0)
        {
            maxHeight = _loc.z;
        }

        res->Position.z = maxHeight;

        if (sceneryEntry->large_scenery.scrolling_mode != SCROLLING_MODE_NONE)
        {
            if (_bannerId == BANNER_INDEX_NULL)
            {
                log_error("No free banners available");
                return MakeResult(GA_ERROR::NO_FREE_ELEMENTS, STR_TOO_MANY_BANNERS_IN_GAME);
            }

            if (gBanners[_bannerId].type != BANNER_NULL)
            {
                log_error("No free banners available");
                return std::make_unique<LargeSceneryPlaceActionResult>(GA_ERROR::NO_FREE_ELEMENTS);
            }

            rct_banner* banner = &gBanners[_bannerId];
            banner->string_idx = STR_DEFAULT_SIGN;
            banner->colour = 2;
            banner->text_colour = 2;
            banner->flags = BANNER_FLAG_IS_LARGE_SCENERY;
            banner->type = 0;
            banner->x = _loc.x / 32;
            banner->y = _loc.y / 32;

            ride_id_t rideIndex = banner_get_closest_ride_index(_loc.x, _loc.y, maxHeight);
            if (rideIndex != RIDE_ID_NULL)
            {
                banner->ride_index = rideIndex;
                banner->flags |= BANNER_FLAG_LINKED_TO_RIDE;
            }
        }

        if (!map_check_free_elements_and_reorganise(totalNumTiles))
        {
            log_error("No free map elements available");
            return std::make_unique<LargeSceneryPlaceActionResult>(GA_ERROR::NO_FREE_ELEMENTS);
        }

        uint8_t tileNum = 0;
        for (rct_large_scenery_tile* tile = sceneryEntry->large_scenery.tiles; tile->x_offset != -1; tile++, tileNum++)
        {
            auto tempX = tile->x_offset;
            auto tempY = tile->y_offset;
            rotate_map_coordinates(&tempX, &tempY, _loc.direction);
            CoordsXY curTile = { tempX, tempY };

            curTile.x += _loc.x;
            curTile.y += _loc.y;

            int32_t zLow = (tile->z_offset + maxHeight) / 8;
            int32_t zHigh = (tile->z_clearance / 8) + zLow;

            QuarterTile quarterTile = QuarterTile{ static_cast<uint8_t>(tile->flags >> 12), 0 }.Rotate(_loc.direction);
            if (!map_can_construct_with_clear_at(
                    curTile.x, curTile.y, zLow, zHigh, &map_place_scenery_clear_func, quarterTile, GetFlags(), &supportsCost,
                    CREATE_CROSSING_MODE_NONE))
            {
                return std::make_unique<LargeSceneryPlaceActionResult>(
                    GA_ERROR::NO_CLEARANCE, gGameCommandErrorText, gCommonFormatArgs);
            }

            res->GroundFlags = gMapGroundFlags & (ELEMENT_IS_ABOVE_GROUND | ELEMENT_IS_UNDERGROUND);

            if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
            {
                footpath_remove_litter(curTile.x, curTile.y, zLow * 8);
                if (!gCheatsDisableClearanceChecks)
                {
                    wall_remove_at(curTile.x, curTile.y, zLow * 8, zHigh * 8);
                }
            }

            TileElement* newTileElement = tile_element_insert(
                curTile.x / 32, curTile.y / 32, zLow, quarterTile.GetBaseQuarterOccupied());
            Guard::Assert(newTileElement != nullptr);
            map_animation_create(MAP_ANIMATION_TYPE_LARGE_SCENERY, curTile.x, curTile.y, zLow);
            newTileElement->SetType(TILE_ELEMENT_TYPE_LARGE_SCENERY);
            newTileElement->clearance_height = zHigh;
            auto newSceneryElement = newTileElement->AsLargeScenery();

            SetNewLargeSceneryElement(*newSceneryElement, tileNum);

            if (tileNum == 0)
            {
                gSceneryTileElement = newTileElement;
            }
            map_invalidate_tile_full(curTile.x, curTile.y);
        }

        // Force ride construction to recheck area
        _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_RECHECK;

        res->Cost = (sceneryEntry->large_scenery.price * 10) + supportsCost;
        return res;
    }

private:
    int16_t GetTotalNumTiles(rct_large_scenery_tile * tiles) const
    {
        uint32_t totalNumTiles = 0;
        for (rct_large_scenery_tile* tile = tiles; tile->x_offset != -1; tile++)
        {
            totalNumTiles++;
        }
        return totalNumTiles;
    }

    int16_t GetMaxSurfaceHeight(rct_large_scenery_tile * tiles) const
    {
        int16_t maxHeight = -1;
        for (rct_large_scenery_tile* tile = tiles; tile->x_offset != -1; tile++)
        {
            auto tempX = tile->x_offset;
            auto tempY = tile->y_offset;
            rotate_map_coordinates(&tempX, &tempY, _loc.direction);
            CoordsXY curTile = { tempX, tempY };

            curTile.x += _loc.x;
            curTile.y += _loc.y;

            if (curTile.x >= 0x1FFF || curTile.y >= 0x1FFF || curTile.x < 0 || curTile.y < 0)
            {
                continue;
            }

            TileElement* tileElement = map_get_surface_element_at({ curTile.x, curTile.y });
            if (tileElement == nullptr)
                continue;

            SurfaceElement* surfaceElement = tileElement->AsSurface();
            int32_t height = surfaceElement->base_height * 8;
            int32_t slope = surfaceElement->GetSlope();

            if (slope & 0xF)
            {
                height += 16;
                if (slope & 0x10)
                {
                    height += 16;
                }
            }

            if (height > maxHeight)
            {
                maxHeight = height;
            }
        }
        return maxHeight;
    }

    void SetNewLargeSceneryElement(LargeSceneryElement & sceneryElement, uint8_t tileNum) const
    {
        sceneryElement.SetDirection(_loc.direction);
        sceneryElement.SetEntryIndex(_sceneryType);
        sceneryElement.SetSequenceIndex(tileNum);
        sceneryElement.SetPrimaryColour(_primaryColour);
        sceneryElement.SetSecondaryColour(_secondaryColour);

        if (_bannerId != BANNER_INDEX_NULL)
        {
            sceneryElement.SetBannerIndex(_bannerId);
        }

        if (GetFlags() & GAME_COMMAND_FLAG_GHOST)
        {
            sceneryElement.SetGhost(true);
        }
    }
};
