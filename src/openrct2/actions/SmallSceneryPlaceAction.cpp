/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SmallSceneryPlaceAction.h"

#include "../Cheats.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../object/ObjectEntryManager.h"
#include "../object/SmallSceneryEntry.h"
#include "../ride/Ride.h"
#include "../ride/TrackDesign.h"
#include "../world/ConstructionClearance.h"
#include "../world/Footpath.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/QuarterTile.h"
#include "../world/Scenery.h"
#include "../world/Wall.h"
#include "../world/tile_element/Slope.h"
#include "../world/tile_element/SmallSceneryElement.h"
#include "../world/tile_element/SurfaceElement.h"
#include "GameAction.h"
#include "SmallSceneryRemoveAction.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Numerics;

SmallSceneryPlaceAction::SmallSceneryPlaceAction(
    const CoordsXYZD& loc, uint8_t quadrant, ObjectEntryIndex sceneryType, uint8_t primaryColour, uint8_t secondaryColour,
    uint8_t tertiaryColour)
    : _loc(loc)
    , _quadrant(quadrant)
    , _sceneryType(sceneryType)
    , _primaryColour(primaryColour)
    , _secondaryColour(secondaryColour)
    , _tertiaryColour(tertiaryColour)
{
}

void SmallSceneryPlaceAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_loc);
    visitor.Visit("quadrant", _quadrant);
    visitor.Visit("object", _sceneryType);
    visitor.Visit("primaryColour", _primaryColour);
    visitor.Visit("secondaryColour", _secondaryColour);
    visitor.Visit("tertiaryColour", _tertiaryColour);
}

uint32_t SmallSceneryPlaceAction::GetCooldownTime() const
{
    return 20;
}

uint16_t SmallSceneryPlaceAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

void SmallSceneryPlaceAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_loc) << DS_TAG(_quadrant) << DS_TAG(_sceneryType) << DS_TAG(_primaryColour) << DS_TAG(_secondaryColour)
           << DS_TAG(_tertiaryColour);
}

GameActions::Result SmallSceneryPlaceAction::Query() const
{
    bool isOnWater = false;
    bool supportsRequired = false;
    if (_loc.z != 0)
    {
        supportsRequired = true;
    }
    int32_t landHeight = TileElementHeight(_loc);
    int16_t waterHeight = TileElementWaterHeight(_loc);

    int32_t surfaceHeight = landHeight;
    // If on water
    if (waterHeight > 0)
    {
        surfaceHeight = waterHeight;
    }
    auto res = GameActions::Result();
    auto centre = _loc.ToTileCentre();
    res.Position.x = centre.x;
    res.Position.y = centre.y;
    res.Position.z = surfaceHeight;
    if (_loc.z != 0)
    {
        surfaceHeight = _loc.z;
        res.Position.z = surfaceHeight;
    }

    if (!LocationValid(_loc))
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_OFF_EDGE_OF_MAP);
    }

    if (!MapCheckCapacityAndReorganise(_loc))
    {
        return GameActions::Result(
            GameActions::Status::NoFreeElements, STR_CANT_POSITION_THIS_HERE, STR_TILE_ELEMENT_LIMIT_REACHED);
    }

    auto maxSizeMax = GetMapSizeMaxXY();
    if (!_trackDesignDrawingPreview && (_loc.x > maxSizeMax.x || _loc.y > maxSizeMax.y))
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_ERR_VALUE_OUT_OF_RANGE);
    }

    auto* sceneryEntry = ObjectManager::GetObjectEntry<SmallSceneryEntry>(_sceneryType);
    if (sceneryEntry == nullptr)
    {
        LOG_ERROR("Small scenery object entry not found for sceneryType %u", _sceneryType);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_UNKNOWN_OBJECT_TYPE);
    }

    auto quadrant = _quadrant;
    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE) || !sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_DIAGONAL))
    {
        if (sceneryEntry->HasFlag(
                SMALL_SCENERY_FLAG_DIAGONAL | SMALL_SCENERY_FLAG_HALF_SPACE | SMALL_SCENERY_FLAG_THREE_QUARTERS))
        {
            quadrant = 0;
        }
    }

    // Check if sub tile height is any different compared to actual surface tile height
    auto loc2 = _loc;
    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE))
    {
        loc2 = loc2.ToTileCentre();
    }
    else
    {
        loc2.x += SceneryQuadrantOffsets[quadrant & 3].x;
        loc2.y += SceneryQuadrantOffsets[quadrant & 3].y;
    }
    landHeight = TileElementHeight(loc2);
    waterHeight = TileElementWaterHeight(loc2);

    surfaceHeight = landHeight;
    // If on water
    if (waterHeight > 0)
    {
        // BaseHeight2 is now the water height
        surfaceHeight = waterHeight;
        if (_loc.z == 0)
        {
            isOnWater = true;
        }
    }
    auto targetHeight = _loc.z;
    if (_loc.z == 0)
    {
        targetHeight = surfaceHeight;
    }

    auto& gameState = GetGameState();
    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gameState.Cheats.sandboxMode
        && !MapIsLocationOwned({ _loc.x, _loc.y, targetHeight }))
    {
        return GameActions::Result(GameActions::Status::NotOwned, STR_CANT_POSITION_THIS_HERE, STR_LAND_NOT_OWNED_BY_PARK);
    }

    auto* surfaceElement = MapGetSurfaceElementAt(_loc);

    if (surfaceElement != nullptr && !gameState.Cheats.disableClearanceChecks && surfaceElement->GetWaterHeight() > 0)
    {
        int32_t water_height = surfaceElement->GetWaterHeight() - 1;
        if (water_height > targetHeight)
        {
            return GameActions::Result(
                GameActions::Status::Disallowed, STR_CANT_POSITION_THIS_HERE, STR_CANT_BUILD_THIS_UNDERWATER);
        }
    }

    if (!gameState.Cheats.disableClearanceChecks && !(sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_STACKABLE)))
    {
        if (isOnWater)
        {
            return GameActions::Result(
                GameActions::Status::Disallowed, STR_CANT_POSITION_THIS_HERE, STR_CAN_ONLY_BUILD_THIS_ON_LAND);
        }

        if (surfaceElement != nullptr && surfaceElement->GetWaterHeight() > 0)
        {
            if (surfaceElement->GetWaterHeight() > targetHeight)
            {
                return GameActions::Result(
                    GameActions::Status::Disallowed, STR_CANT_POSITION_THIS_HERE, STR_CAN_ONLY_BUILD_THIS_ON_LAND);
            }
        }
    }

    if (!gameState.Cheats.disableClearanceChecks && (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_REQUIRE_FLAT_SURFACE))
        && !supportsRequired && !isOnWater && surfaceElement != nullptr && (surfaceElement->GetSlope() != kTileSlopeFlat))
    {
        return GameActions::Result(GameActions::Status::Disallowed, STR_CANT_POSITION_THIS_HERE, STR_LEVEL_LAND_REQUIRED);
    }

    if (!gameState.Cheats.disableSupportLimits && !(sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_STACKABLE)) && supportsRequired)
    {
        if (!isOnWater)
        {
            if (surfaceElement != nullptr)
            {
                if (surfaceElement->GetWaterHeight() > 0 || (surfaceElement->GetBaseZ()) != targetHeight)
                {
                    return GameActions::Result(
                        GameActions::Status::Disallowed, STR_CANT_POSITION_THIS_HERE, STR_LEVEL_LAND_REQUIRED);
                }
            }
        }
        else
        {
            return GameActions::Result(
                GameActions::Status::Disallowed, STR_CANT_POSITION_THIS_HERE, STR_CAN_ONLY_BUILD_THIS_ON_LAND);
        }
    }

    int32_t zLow = targetHeight;
    int32_t zHigh = zLow + ceil2(sceneryEntry->height, kCoordsZStep);
    uint8_t collisionQuadrants = 0b1111;
    auto quadRotation{ 0 };
    if (!(sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE)))
    {
        quadRotation = (quadrant ^ 2);
        collisionQuadrants = 0b0001;
    }
    if (!(sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HALF_SPACE)))
    {
        if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_DIAGONAL) && sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE))
        {
            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_THREE_QUARTERS))
            {
                quadRotation = ((quadrant ^ 2) + _loc.direction) & 3;
                collisionQuadrants = 0b1011;
            }
            else
            {
                quadRotation = (quadrant + _loc.direction) & 1;
                collisionQuadrants = 0b1010;
            }
        }
    }
    else
    {
        quadRotation = ((quadrant ^ 2) + _loc.direction) & 3;
        collisionQuadrants = 0b0011;
    }
    uint8_t supports = 0;
    if (!supportsRequired)
    {
        supports = 0b1111;
    }

    QuarterTile quarterTile = QuarterTile{ collisionQuadrants, supports }.Rotate(quadRotation);
    const auto isTree = sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_IS_TREE);
    auto canBuild = MapCanConstructWithClearAt(
        { _loc, zLow, zHigh }, &MapPlaceSceneryClearFunc, quarterTile, GetFlags(), CreateCrossingMode::none, isTree);
    if (canBuild.Error != GameActions::Status::Ok)
    {
        canBuild.ErrorTitle = STR_CANT_POSITION_THIS_HERE;
        return canBuild;
    }

    const auto clearanceData = canBuild.GetData<ConstructClearResult>();
    const uint8_t groundFlags = clearanceData.GroundFlags & (ELEMENT_IS_ABOVE_GROUND | ELEMENT_IS_UNDERGROUND);
    res.SetData(SmallSceneryPlaceActionResult{ groundFlags, 0, 0 });

    res.Expenditure = ExpenditureType::Landscaping;
    res.Cost = sceneryEntry->price + canBuild.Cost;

    return res;
}

GameActions::Result SmallSceneryPlaceAction::Execute() const
{
    bool supportsRequired = false;
    if (_loc.z != 0)
    {
        supportsRequired = true;
    }
    int32_t landHeight = TileElementHeight(_loc);
    int16_t waterHeight = TileElementWaterHeight(_loc);

    int32_t surfaceHeight = landHeight;
    // If on water
    if (waterHeight > 0)
    {
        surfaceHeight = waterHeight;
    }
    auto res = GameActions::Result();
    auto centre = _loc.ToTileCentre();
    res.Position.x = centre.x;
    res.Position.y = centre.y;
    res.Position.z = surfaceHeight;
    if (_loc.z != 0)
    {
        surfaceHeight = _loc.z;
        res.Position.z = surfaceHeight;
    }

    auto* sceneryEntry = ObjectManager::GetObjectEntry<SmallSceneryEntry>(_sceneryType);
    if (sceneryEntry == nullptr)
    {
        LOG_ERROR("Small scenery object entry not found for sceneryType %u", _sceneryType);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_UNKNOWN_OBJECT_TYPE);
    }

    auto quadrant = _quadrant;
    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE) || !sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_DIAGONAL))
    {
        if (sceneryEntry->HasFlag(
                SMALL_SCENERY_FLAG_DIAGONAL | SMALL_SCENERY_FLAG_HALF_SPACE | SMALL_SCENERY_FLAG_THREE_QUARTERS))
        {
            quadrant = 0;
        }
    }

    // Check if sub tile height is any different compared to actual surface tile height
    int32_t x2 = _loc.x;
    int32_t y2 = _loc.y;
    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE))
    {
        x2 += 16;
        y2 += 16;
    }
    else
    {
        x2 += SceneryQuadrantOffsets[quadrant & 3].x;
        y2 += SceneryQuadrantOffsets[quadrant & 3].y;
    }
    landHeight = TileElementHeight({ x2, y2 });
    waterHeight = TileElementWaterHeight({ x2, y2 });

    surfaceHeight = landHeight;
    // If on water
    if (waterHeight > 0)
    {
        // BaseHeight2 is now the water height
        surfaceHeight = waterHeight;
    }
    auto targetHeight = _loc.z;
    if (_loc.z == 0)
    {
        targetHeight = surfaceHeight;
    }

    if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
    {
        FootpathRemoveLitter({ _loc, targetHeight });
        if (!GetGameState().Cheats.disableClearanceChecks && (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_NO_WALLS)))
        {
            WallRemoveAt({ _loc, targetHeight, targetHeight + sceneryEntry->height });
        }
    }

    int32_t zLow = targetHeight;
    int32_t zHigh = zLow + ceil2(sceneryEntry->height, 8);
    uint8_t collisionQuadrants = 0b1111;
    auto quadRotation{ 0 };
    if (!(sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE)))
    {
        quadRotation = (quadrant ^ 2);
        collisionQuadrants = 0b0001;
    }
    if (!(sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HALF_SPACE)))
    {
        if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_DIAGONAL) && sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE))
        {
            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_THREE_QUARTERS))
            {
                quadRotation = ((quadrant ^ 2) + _loc.direction) & 3;
                collisionQuadrants = 0b1011;
            }
            else
            {
                quadRotation = (quadrant + _loc.direction) & 1;
                collisionQuadrants = 0b1010;
            }
        }
    }
    else
    {
        quadRotation = ((quadrant ^ 2) + _loc.direction) & 3;
        collisionQuadrants = 0b0011;
    }
    uint8_t supports = 0;
    if (!supportsRequired)
    {
        supports = 0b1111;
    }

    QuarterTile quarterTile = QuarterTile{ collisionQuadrants, supports }.Rotate(quadRotation);
    const auto isTree = sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_IS_TREE);
    auto canBuild = MapCanConstructWithClearAt(
        { _loc, zLow, zHigh }, &MapPlaceSceneryClearFunc, quarterTile, GetFlags() | GAME_COMMAND_FLAG_APPLY,
        CreateCrossingMode::none, isTree);
    if (canBuild.Error != GameActions::Status::Ok)
    {
        canBuild.ErrorTitle = STR_CANT_POSITION_THIS_HERE;
        return canBuild;
    }

    res.Expenditure = ExpenditureType::Landscaping;
    res.Cost = sceneryEntry->price + canBuild.Cost;

    auto* sceneryElement = TileElementInsert<SmallSceneryElement>(
        CoordsXYZ{ _loc, zLow }, quarterTile.GetBaseQuarterOccupied());
    if (sceneryElement == nullptr)
    {
        return GameActions::Result(
            GameActions::Status::NoFreeElements, STR_CANT_POSITION_THIS_HERE, STR_TILE_ELEMENT_LIMIT_REACHED);
    }

    sceneryElement->SetDirection(_loc.direction);
    sceneryElement->SetSceneryQuadrant(quadrant);
    sceneryElement->SetEntryIndex(_sceneryType);
    sceneryElement->SetAge(0);
    sceneryElement->SetPrimaryColour(_primaryColour);
    sceneryElement->SetSecondaryColour(_secondaryColour);
    sceneryElement->SetTertiaryColour(_tertiaryColour);
    sceneryElement->SetClearanceZ(sceneryElement->GetBaseZ() + sceneryEntry->height + 7);
    sceneryElement->SetGhost(GetFlags() & GAME_COMMAND_FLAG_GHOST);
    if (supportsRequired)
    {
        sceneryElement->SetNeedsSupports();
    }

    const auto clearanceData = canBuild.GetData<ConstructClearResult>();
    const uint8_t groundFlags = clearanceData.GroundFlags & (ELEMENT_IS_ABOVE_GROUND | ELEMENT_IS_UNDERGROUND);
    res.SetData(SmallSceneryPlaceActionResult{ groundFlags, sceneryElement->GetBaseZ(), sceneryElement->GetSceneryQuadrant() });

    MapInvalidateTileFull(_loc);
    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_ANIMATED))
    {
        MapAnimationCreate(MAP_ANIMATION_TYPE_SMALL_SCENERY, CoordsXYZ{ _loc, sceneryElement->GetBaseZ() });
    }

    return res;
}
