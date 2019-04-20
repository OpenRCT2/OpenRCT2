/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Context.h"
#include "../OpenRCT2.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/RideData.h"
#include "../windows/Intent.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/SmallScenery.h"
#include "../world/Sprite.h"
#include "../world/Surface.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(LandSetHeightAction, GAME_COMMAND_SET_LAND_HEIGHT, GameActionResult)
{
private:
    CoordsXY _coords;
    uint8_t _height;
    uint8_t _style;

public:
    LandSetHeightAction()
    {
    }
    LandSetHeightAction(CoordsXY coords, uint8_t height, uint8_t style)
        : _coords(coords)
        , _height(height)
        , _style(style)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::EDITOR_ONLY;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_coords) << DS_TAG(_height) << DS_TAG(_style);
    }

    GameActionResult::Ptr Query() const override
    {
        if (gParkFlags & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES)
        {
            return std::make_unique<GameActionResult>(GA_ERROR::DISALLOWED, STR_FORBIDDEN_BY_THE_LOCAL_AUTHORITY);
        }

        rct_string_id errorTitle = CheckParameters();
        if (errorTitle != STR_NONE)
        {
            return std::make_unique<GameActionResult>(GA_ERROR::DISALLOWED, errorTitle);
        }

        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
        {
            if (!map_is_location_in_park(_coords))
            {
                return std::make_unique<GameActionResult>(GA_ERROR::DISALLOWED, STR_LAND_NOT_OWNED_BY_PARK);
            }
        }

        money32 sceneryRemovalCost = 0;
        if (!gCheatsDisableClearanceChecks)
        {
            if (gParkFlags & PARK_FLAGS_FORBID_TREE_REMOVAL)
            {
                // Check for obstructing large trees
                TileElement* tileElement = CheckTreeObstructions();
                if (tileElement != nullptr)
                {
                    map_obstruction_set_error_text(tileElement);
                    return std::make_unique<GameActionResult>(GA_ERROR::DISALLOWED, gGameCommandErrorText);
                }
            }
            sceneryRemovalCost = GetSmallSceneryRemovalCost();
        }

        // Check for ride support limits
        if (!gCheatsDisableSupportLimits)
        {
            errorTitle = CheckRideSupports();
            if (errorTitle != STR_NONE)
            {
                return std::make_unique<GameActionResult>(GA_ERROR::DISALLOWED, errorTitle);
            }
        }

        TileElement* surfaceElement = map_get_surface_element_at(_coords);
        TileElement* tileElement = CheckFloatingStructures(surfaceElement, _height);
        if (tileElement != nullptr)
        {
            map_obstruction_set_error_text(tileElement);
            return std::make_unique<GameActionResult>(GA_ERROR::DISALLOWED, gGameCommandErrorText);
        }

        if (!gCheatsDisableClearanceChecks)
        {
            uint8_t zCorner = _height;
            if (_style & TILE_ELEMENT_SURFACE_RAISED_CORNERS_MASK)
            {
                zCorner += 2;
                if (_style & TILE_ELEMENT_SURFACE_DIAGONAL_FLAG)
                {
                    zCorner += 2;
                }
            }
            if (!map_can_construct_with_clear_at(
                    _coords.x, _coords.y, _height, zCorner, &map_set_land_height_clear_func, { 0b1111, 0 }, 0, nullptr,
                    CREATE_CROSSING_MODE_NONE))
            {
                return std::make_unique<GameActionResult>(
                    GA_ERROR::DISALLOWED, STR_NONE, gGameCommandErrorText, gCommonFormatArgs);
            }

            tileElement = CheckUnremovableObstructions(surfaceElement, zCorner);
            if (tileElement != nullptr)
            {
                map_obstruction_set_error_text(tileElement);
                return std::make_unique<GameActionResult>(GA_ERROR::DISALLOWED, gGameCommandErrorText);
            }
        }
        auto res = std::make_unique<GameActionResult>();
        res->Cost = sceneryRemovalCost + GetSurfaceHeightChangeCost(surfaceElement);
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        money32 cost = MONEY(0, 0);
        auto surfaceHeight = tile_element_height(_coords.x, _coords.y) & 0xFFFF;
        footpath_remove_litter(_coords.x, _coords.y, surfaceHeight);

        if (!gCheatsDisableClearanceChecks)
        {
            wall_remove_at(_coords.x, _coords.y, _height * 8 - 16, _height * 8 + 32);
            cost += GetSmallSceneryRemovalCost();
            SmallSceneryRemoval();
        }

        TileElement* surfaceElement = map_get_surface_element_at(_coords);
        cost += GetSurfaceHeightChangeCost(surfaceElement);
        SetSurfaceHeight(surfaceElement);

        auto res = std::make_unique<GameActionResult>();
        res->Position = { _coords.x + 16, _coords.y + 16, surfaceHeight };
        res->Cost = cost;
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
        return res;
    }

private:
    rct_string_id CheckParameters() const
    {
        if (_coords.x > gMapSizeMaxXY || _coords.y > gMapSizeMaxXY)
        {
            return STR_OFF_EDGE_OF_MAP;
        }

        if (_height < MINIMUM_LAND_HEIGHT)
        {
            return STR_TOO_LOW;
        }

        // Divide by 2 and subtract 7 to get the in-game units.
        if (_height > MAXIMUM_LAND_HEIGHT)
        {
            return STR_TOO_HIGH;
        }
        else if (_height > MAXIMUM_LAND_HEIGHT - 2 && (_style & TILE_ELEMENT_SURFACE_SLOPE_MASK) != 0)
        {
            return STR_TOO_HIGH;
        }

        if (_height == MAXIMUM_LAND_HEIGHT - 2 && (_style & TILE_ELEMENT_SURFACE_DIAGONAL_FLAG))
        {
            return STR_TOO_HIGH;
        }

        return STR_NONE;
    }

    TileElement* CheckTreeObstructions() const
    {
        TileElement* tileElement = map_get_first_element_at(_coords.x / 32, _coords.y / 32);
        do
        {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
                continue;
            if (_height > tileElement->clearance_height)
                continue;
            if (_height + 4 < tileElement->base_height)
                continue;
            rct_scenery_entry* sceneryEntry = tileElement->AsSmallScenery()->GetEntry();
            if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_IS_TREE))
            {
                return tileElement;
            }
        } while (!(tileElement++)->IsLastForTile());
        return nullptr;
    }

    money32 GetSmallSceneryRemovalCost() const
    {
        money32 cost{ 0 };
        TileElement* tileElement = map_get_first_element_at(_coords.x / 32, _coords.y / 32);
        do
        {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
                continue;
            if (_height > tileElement->clearance_height)
                continue;
            if (_height + 4 < tileElement->base_height)
                continue;
            rct_scenery_entry* sceneryEntry = tileElement->AsSmallScenery()->GetEntry();
            cost += MONEY(sceneryEntry->small_scenery.removal_price, 0);
        } while (!(tileElement++)->IsLastForTile());
        return cost;
    }

    void SmallSceneryRemoval() const
    {
        TileElement* tileElement = map_get_first_element_at(_coords.x / 32, _coords.y / 32);
        do
        {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
                continue;
            if (_height > tileElement->clearance_height)
                continue;
            if (_height + 4 < tileElement->base_height)
                continue;
            tile_element_remove(tileElement--);
        } while (!(tileElement++)->IsLastForTile());
    }

    rct_string_id CheckRideSupports() const
    {
        TileElement* tileElement = map_get_first_element_at(_coords.x / 32, _coords.y / 32);
        do
        {
            if (tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK)
            {
                ride_id_t rideIndex = tileElement->AsTrack()->GetRideIndex();
                Ride* ride = get_ride(rideIndex);
                if (ride != nullptr)
                {
                    rct_ride_entry* rideEntry = ride->GetRideEntry();
                    if (rideEntry != nullptr)
                    {
                        int32_t maxHeight = rideEntry->max_height;
                        if (maxHeight == 0)
                        {
                            maxHeight = RideData5[get_ride(rideIndex)->type].max_height;
                        }
                        int32_t zDelta = tileElement->clearance_height - _height;
                        if (zDelta >= 0 && zDelta / 2 > maxHeight)
                        {
                            return STR_SUPPORTS_CANT_BE_EXTENDED;
                        }
                    }
                }
            }
        } while (!(tileElement++)->IsLastForTile());
        return STR_NONE;
    }

    TileElement* CheckFloatingStructures(TileElement * surfaceElement, uint8_t zCorner) const
    {
        if (surfaceElement->AsSurface()->HasTrackThatNeedsWater())
        {
            uint32_t waterHeight = surfaceElement->AsSurface()->GetWaterHeight();
            if (waterHeight != 0)
            {
                if (_style & TILE_ELEMENT_SURFACE_SLOPE_MASK)
                {
                    zCorner += 2;
                    if (_style & TILE_ELEMENT_SURFACE_DIAGONAL_FLAG)
                    {
                        zCorner += 2;
                    }
                }
                if (zCorner > waterHeight * 2 - 2)
                {
                    return ++surfaceElement;
                }
            }
        }
        return nullptr;
    }

    TileElement* CheckUnremovableObstructions(TileElement * surfaceElement, uint8_t zCorner) const
    {
        TileElement* tileElement = map_get_first_element_at(_coords.x / 32, _coords.y / 32);
        do
        {
            int32_t elementType = tileElement->GetType();

            // Wall's and Small Scenery are removed and therefore do not need checked
            if (elementType == TILE_ELEMENT_TYPE_WALL)
                continue;
            if (elementType == TILE_ELEMENT_TYPE_SMALL_SCENERY)
                continue;
            if (tileElement->IsGhost())
                continue;
            if (tileElement == surfaceElement)
                continue;
            if (tileElement > surfaceElement)
            {
                if (zCorner > tileElement->base_height)
                {
                    return tileElement;
                }
                continue;
            }
            if (_height < tileElement->clearance_height)
            {
                return tileElement;
            }
        } while (!(tileElement++)->IsLastForTile());
        return nullptr;
    }

    money32 GetSurfaceHeightChangeCost(TileElement * surfaceElement) const
    {
        money32 cost{ 0 };
        for (int32_t i = 0; i < 4; i += 1)
        {
            int32_t cornerHeight = tile_element_get_corner_height(surfaceElement, i);
            cornerHeight -= map_get_corner_height(_height, _style & TILE_ELEMENT_SURFACE_SLOPE_MASK, i);
            cost += MONEY(abs(cornerHeight) * 5 / 2, 0);
        }
        return cost;
    }

    void SetSurfaceHeight(TileElement * surfaceElement) const
    {
        surfaceElement->base_height = _height;
        surfaceElement->clearance_height = _height;
        surfaceElement->AsSurface()->SetSlope(_style);
        int32_t waterHeight = surfaceElement->AsSurface()->GetWaterHeight();
        if (waterHeight != 0 && waterHeight <= _height / 2)
        {
            surfaceElement->AsSurface()->SetWaterHeight(0);
        }

        map_invalidate_tile_full(_coords.x, _coords.y);
    }

    /**
     *
     *  rct2: 0x00663CB9
     */
    static int32_t map_set_land_height_clear_func(
        TileElement * *tile_element, [[maybe_unused]] int32_t x, [[maybe_unused]] int32_t y, [[maybe_unused]] uint8_t flags,
        [[maybe_unused]] money32 * price)
    {
        if ((*tile_element)->GetType() == TILE_ELEMENT_TYPE_SURFACE)
            return 0;

        if ((*tile_element)->GetType() == TILE_ELEMENT_TYPE_SMALL_SCENERY)
            return 0;

        return 1;
    }
};
