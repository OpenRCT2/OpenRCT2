/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Cheats.h"
#include "../OpenRCT2.h"
#include "../common.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/Ride.h"
#include "../world/TileElement.h"
#include "../world/Park.h"
#include "../world/SmallScenery.h"
#include "../world/Sprite.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(SmallSceneryPlaceAction, GAME_COMMAND_PLACE_SCENERY, GameActionResult)
{
private:
    CoordsXYZ _loc;
    uint8_t _rotation;
    uint8_t _quadrant;
    uint8_t _sceneryType;
    uint8_t _primaryColour;
    uint8_t _secondaryColour;

public:
    SmallSceneryPlaceAction() = default;

    SmallSceneryPlaceAction(
        CoordsXYZ loc, uint8_t quadrant, uint8_t rotation, uint8_t sceneryType,
        uint8_t primaryColour, uint8_t secondaryColour)
        : _loc(loc)
        , _rotation(rotation)
        , _quadrant(quadrant)
        , _sceneryType(sceneryType)
        , _primaryColour(primaryColour)
        , _secondaryColour(secondaryColour)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc) << DS_TAG(_rotation) << DS_TAG(_quadrant) << DS_TAG(_sceneryType) << DS_TAG(_primaryColour) << DS_TAG(_secondaryColour);
    }

    GameActionResult::Ptr Query() const override
    {
        bool isOnWater = false;
        bool supportsRequired = false;
        if (_loc.z != 0)
        {
            supportsRequired = true;
        }
        int32_t baseHeight = tile_element_height(_loc.x, _loc.y);
        // If on water
        if (baseHeight & 0xFFFF0000)
        {
            baseHeight >>= 16;
        }
        auto res = MakeResult();
        res->Position.x = _loc.x + 16;
        res->Position.y = _loc.y + 16;
        res->Position.z = baseHeight;
        if (_loc.z != 0)
        {
            baseHeight = _loc.z;
            res->Position.z = baseHeight;
        }

        if (!map_check_free_elements_and_reorganise(1))
        {
            return MakeResult(GA_ERROR::NO_FREE_ELEMENTS, STR_NONE);
        }

        if (!byte_9D8150 && (_loc.x > gMapSizeMaxXY || _loc.y > gMapSizeMaxXY))
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        rct_scenery_entry* sceneryEntry = get_small_scenery_entry(_sceneryType);
        if (sceneryEntry == nullptr)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        auto quadrant = _quadrant;
        if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_FULL_TILE)
            || !scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_DIAGONAL))
        {
            if (scenery_small_entry_has_flag(
                    sceneryEntry,
                    SMALL_SCENERY_FLAG_DIAGONAL | SMALL_SCENERY_FLAG_HALF_SPACE | SMALL_SCENERY_FLAG_THREE_QUARTERS))
            {
                quadrant = 0;
            }
        }

        // Check if sub tile height is any different compared to actual surface tile height
        int32_t x2 = _loc.x;
        int32_t y2 = _loc.y;
        if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_FULL_TILE))
        {
            x2 += 16;
            y2 += 16;
        }
        else
        {
            x2 += ScenerySubTileOffsets[quadrant & 3].x - 1;
            y2 += ScenerySubTileOffsets[quadrant & 3].y - 1;
        }
        baseHeight = tile_element_height(x2, y2);
        // If on water
        if (baseHeight & 0xFFFF0000)
        {
            // base_height2 is now the water height
            baseHeight >>= 16;
            if (_loc.z == 0)
            {
                isOnWater = true;
            }
        }
        auto targetHeight = _loc.z;
        if (_loc.z == 0)
        {
            targetHeight = baseHeight;
        }

        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode
            && !map_is_location_owned(_loc.x, _loc.y, targetHeight))
        {
            return MakeResult(GA_ERROR::NOT_OWNED, STR_NONE);
        }

        TileElement* surfaceElement = map_get_surface_element_at({ _loc.x, _loc.y });

        if (surfaceElement != nullptr && !gCheatsDisableClearanceChecks && surfaceElement->AsSurface()->GetWaterHeight() > 0)
        {
            int32_t water_height = (surfaceElement->AsSurface()->GetWaterHeight() * 16) - 1;
            if (water_height > targetHeight)
            {
                return MakeResult(GA_ERROR::DISALLOWED, STR_CANT_BUILD_THIS_UNDERWATER);
            }
        }

        if (!gCheatsDisableClearanceChecks && !(scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_STACKABLE)))
        {
            if (isOnWater)
            {
                return MakeResult(GA_ERROR::DISALLOWED, STR_CAN_ONLY_BUILD_THIS_ON_LAND);
            }

            if (surfaceElement != nullptr && surfaceElement->AsSurface()->GetWaterHeight() > 0)
            {
                if ((surfaceElement->AsSurface()->GetWaterHeight() * 16) > targetHeight)
                {
                    return MakeResult(GA_ERROR::DISALLOWED, STR_CAN_ONLY_BUILD_THIS_ON_LAND);
                }
            }
        }

        if (!gCheatsDisableClearanceChecks
            && (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_REQUIRE_FLAT_SURFACE)) && !supportsRequired
            && !isOnWater && surfaceElement != nullptr && (surfaceElement->AsSurface()->GetSlope() != TILE_ELEMENT_SLOPE_FLAT))
        {
            return MakeResult(GA_ERROR::DISALLOWED, STR_LEVEL_LAND_REQUIRED);
        }

        if (!gCheatsDisableSupportLimits && !(scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_STACKABLE))
            && supportsRequired)
        {
            if (!isOnWater)
            {
                if (surfaceElement != nullptr)
                {
                    if (surfaceElement->AsSurface()->GetWaterHeight() || (surfaceElement->base_height * 8) != targetHeight)
                    {
                        return MakeResult(GA_ERROR::DISALLOWED, STR_LEVEL_LAND_REQUIRED);
                    }
                }
            }
            else
            {
                return MakeResult(GA_ERROR::DISALLOWED, STR_CAN_ONLY_BUILD_THIS_ON_LAND);
            }
        }

        int32_t zLow = targetHeight / 8;
        int32_t zHigh = zLow + ceil2(sceneryEntry->small_scenery.height, 8) / 8;
        uint8_t collisionQuadrants = 0b1111;
        auto quadRotation{ 0 };
        if (!(scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_FULL_TILE)))
        {
            quadRotation = (quadrant ^ 2);
            collisionQuadrants = 0b0001;
        }
        if (!(scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_HALF_SPACE)))
        {
            if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_DIAGONAL)
                && scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_FULL_TILE))
            {
                if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_THREE_QUARTERS))
                {
                    quadRotation = ((quadrant ^ 2) + _rotation) & 3;
                    collisionQuadrants = 0b1011;
                }
                else
                {
                    quadRotation = (quadrant + _rotation) & 1;
                    collisionQuadrants = 0b1010;
                }
            }
        }
        else
        {
            quadRotation = ((quadrant ^ 2) + _rotation) & 3;
            collisionQuadrants = 0b0011;
        }
        uint8_t supports = 0;
        if (!supportsRequired)
        {
            supports |= 0xF0;
        }

        QuarterTile quarterTile = QuarterTile{ collisionQuadrants, supports }.Rotate(quadRotation);
        money32 clearCost = 0;

        if (!gCheatsDisableClearanceChecks
            && !map_can_construct_with_clear_at(
                   _loc.x, _loc.y, zLow, zHigh, &map_place_scenery_clear_func, quarterTile, GetFlags(), &clearCost,
                   CREATE_CROSSING_MODE_NONE))
        {
            return MakeResult(GA_ERROR::DISALLOWED, STR_NONE, gGameCommandErrorText, gCommonFormatArgs);
        }

        gSceneryGroundFlags = gMapGroundFlags & (ELEMENT_IS_ABOVE_GROUND | ELEMENT_IS_UNDERGROUND);

        auto res = MakeResult();
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
        res->Cost = (sceneryEntry->small_scenery.price * 10) + clearCost;

        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        bool isOnWater = false;
        bool supportsRequired = false;
        if (_loc.z != 0)
        {
            supportsRequired = true;
        }
        int32_t baseHeight = tile_element_height(_loc.x, _loc.y);
        // If on water
        if (baseHeight & 0xFFFF0000)
        {
            baseHeight >>= 16;
        }
        auto res = MakeResult();
        res->Position.x = _loc.x + 16;
        res->Position.y = _loc.y + 16;
        res->Position.z = baseHeight;
        if (_loc.z != 0)
        {
            baseHeight = _loc.z;
            res->Position.z = baseHeight;
        }

        rct_scenery_entry* sceneryEntry = get_small_scenery_entry(_sceneryType);
        if (sceneryEntry == nullptr)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        auto quadrant = _quadrant;
        if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_FULL_TILE)
            || !scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_DIAGONAL))
        {
            if (scenery_small_entry_has_flag(
                    sceneryEntry,
                    SMALL_SCENERY_FLAG_DIAGONAL | SMALL_SCENERY_FLAG_HALF_SPACE | SMALL_SCENERY_FLAG_THREE_QUARTERS))
            {
                quadrant = 0;
            }
        }

        // Check if sub tile height is any different compared to actual surface tile height
        int32_t x2 = _loc.x;
        int32_t y2 = _loc.y;
        if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_FULL_TILE))
        {
            x2 += 16;
            y2 += 16;
        }
        else
        {
            x2 += ScenerySubTileOffsets[quadrant & 3].x - 1;
            y2 += ScenerySubTileOffsets[quadrant & 3].y - 1;
        }
        baseHeight = tile_element_height(x2, y2);
        // If on water
        if (baseHeight & 0xFFFF0000)
        {
            // base_height2 is now the water height
            baseHeight >>= 16;
            if (_loc.z == 0)
            {
                isOnWater = true;
            }
        }
        auto targetHeight = _loc.z;
        if (_loc.z == 0)
        {
            targetHeight = baseHeight;
        }

        if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
        {
            footpath_remove_litter(_loc.x, _loc.y, targetHeight);
            if (!gCheatsDisableClearanceChecks && (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_NO_WALLS)))
            {
                wall_remove_at(_loc.x, _loc.y, targetHeight, targetHeight + sceneryEntry->small_scenery.height);
            }
        }

        int32_t zLow = targetHeight / 8;
        int32_t zHigh = zLow + ceil2(sceneryEntry->small_scenery.height, 8) / 8;
        uint8_t collisionQuadrants = 0b1111;
        auto quadRotation{ 0 };
        if (!(scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_FULL_TILE)))
        {
            quadRotation = (quadrant ^ 2);
            collisionQuadrants = 0b0001;
        }
        if (!(scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_HALF_SPACE)))
        {
            if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_DIAGONAL)
                && scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_FULL_TILE))
            {
                if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_THREE_QUARTERS))
                {
                    quadRotation = ((quadrant ^ 2) + _rotation) & 3;
                    collisionQuadrants = 0b1011;
                }
                else
                {
                    quadRotation = (quadrant + _rotation) & 1;
                    collisionQuadrants = 0b1010;
                }
            }
        }
        else
        {
            quadRotation = ((quadrant ^ 2) + _rotation) & 3;
            collisionQuadrants = 0b0011;
        }
        uint8_t supports = 0;
        if (!supportsRequired)
        {
            supports |= 0xF0;
        }

        QuarterTile quarterTile = QuarterTile{ collisionQuadrants, supports }.Rotate(quadRotation);
        money32 clearCost = 0;

        if (!gCheatsDisableClearanceChecks
            && !map_can_construct_with_clear_at(
                   _loc.x, _loc.y, zLow, zHigh, &map_place_scenery_clear_func, quarterTile, GetFlags(), &clearCost,
                   CREATE_CROSSING_MODE_NONE))
        {
            return MakeResult(GA_ERROR::DISALLOWED, STR_NONE, gGameCommandErrorText, gCommonFormatArgs);
        }

        gSceneryGroundFlags = gMapGroundFlags & (ELEMENT_IS_ABOVE_GROUND | ELEMENT_IS_UNDERGROUND);

        auto res = MakeResult();
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
        res->Cost = (sceneryEntry->small_scenery.price * 10) + clearCost;

        TileElement* newElement = tile_element_insert(_loc.x / 32, _loc.y / 32, zLow, collisionQuadrants);
        assert(newElement != nullptr);
        gSceneryTileElement = newElement;
        newElement->SetType(TILE_ELEMENT_TYPE_SMALL_SCENERY);
        newElement->SetDirection(_rotation);
        SmallSceneryElement* sceneryElement = newElement->AsSmallScenery();
        sceneryElement->SetSceneryQuadrant(quadrant);
        sceneryElement->SetEntryIndex(_sceneryType);
        sceneryElement->SetAge(0);
        sceneryElement->SetPrimaryColour(_primaryColour);
        sceneryElement->SetSecondaryColour(_secondaryColour);
        newElement->clearance_height = newElement->base_height + ((sceneryEntry->small_scenery.height + 7) / 8);

        if (supportsRequired)
        {
            sceneryElement->SetNeedsSupports();
        }

        if (GetFlags() & GAME_COMMAND_FLAG_GHOST)
        {
            newElement->flags |= TILE_ELEMENT_FLAG_GHOST;
        }

        map_invalidate_tile_full(_loc.x, _loc.y);
        if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_ANIMATED))
        {
            map_animation_create(2, _loc.x, _loc.y, newElement->base_height);
        }

        return res;
    }
};
