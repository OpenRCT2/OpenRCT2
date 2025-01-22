/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LargeSceneryPlaceAction.h"

#include "../Diagnostic.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../management/Finance.h"
#include "../object/LargeSceneryEntry.h"
#include "../object/ObjectEntryManager.h"
#include "../object/ObjectLimits.h"
#include "../ride/Ride.h"
#include "../ride/RideConstruction.h"
#include "../world/Banner.h"
#include "../world/ConstructionClearance.h"
#include "../world/Footpath.h"
#include "../world/MapAnimation.h"
#include "../world/QuarterTile.h"
#include "../world/Wall.h"
#include "../world/tile_element/LargeSceneryElement.h"
#include "../world/tile_element/Slope.h"
#include "../world/tile_element/SurfaceElement.h"

using namespace OpenRCT2;

LargeSceneryPlaceAction::LargeSceneryPlaceAction(
    const CoordsXYZD& loc, ObjectEntryIndex sceneryType, uint8_t primaryColour, uint8_t secondaryColour, uint8_t tertiaryColour)
    : _loc(loc)
    , _sceneryType(sceneryType)
    , _primaryColour(primaryColour)
    , _secondaryColour(secondaryColour)
    , _tertiaryColour(tertiaryColour)
{
}

void LargeSceneryPlaceAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_loc);
    visitor.Visit("object", _sceneryType);
    visitor.Visit("primaryColour", _primaryColour);
    visitor.Visit("secondaryColour", _secondaryColour);
    visitor.Visit("tertiaryColour", _tertiaryColour);
}

uint16_t LargeSceneryPlaceAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

void LargeSceneryPlaceAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_loc) << DS_TAG(_sceneryType) << DS_TAG(_primaryColour) << DS_TAG(_secondaryColour)
           << DS_TAG(_tertiaryColour);
}

GameActions::Result LargeSceneryPlaceAction::Query() const
{
    auto res = GameActions::Result();
    res.ErrorTitle = STR_CANT_POSITION_THIS_HERE;
    res.Expenditure = ExpenditureType::Landscaping;
    int16_t surfaceHeight = TileElementHeight(_loc);
    res.Position.x = _loc.x + 16;
    res.Position.y = _loc.y + 16;
    res.Position.z = surfaceHeight;

    auto resultData = LargeSceneryPlaceActionResult{};

    auto& gameState = GetGameState();

    money64 supportsCost = 0;

    if (_primaryColour >= COLOUR_COUNT)
    {
        LOG_ERROR("Invalid primary colour %u", _primaryColour);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_ERR_INVALID_COLOUR);
    }
    else if (_secondaryColour >= COLOUR_COUNT)
    {
        LOG_ERROR("Invalid secondary colour %u", _secondaryColour);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_ERR_INVALID_COLOUR);
    }
    else if (_tertiaryColour >= COLOUR_COUNT)
    {
        LOG_ERROR("Invalid tertiary colour %u", _tertiaryColour);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_ERR_INVALID_COLOUR);
    }
    else if (_sceneryType >= kMaxLargeSceneryObjects)
    {
        LOG_ERROR("Invalid sceneryType %u", _sceneryType);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_ERR_VALUE_OUT_OF_RANGE);
    }

    auto* sceneryEntry = ObjectManager::GetObjectEntry<LargeSceneryEntry>(_sceneryType);
    if (sceneryEntry == nullptr)
    {
        LOG_ERROR("Large scenery entry not found for sceneryType %u", _sceneryType);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_UNKNOWN_OBJECT_TYPE);
    }

    const auto totalNumTiles = sceneryEntry->tiles.size();
    int16_t maxHeight = GetMaxSurfaceHeight(sceneryEntry->tiles);

    if (_loc.z != 0)
    {
        maxHeight = _loc.z;
    }

    res.Position.z = maxHeight;

    if (sceneryEntry->scrolling_mode != SCROLLING_MODE_NONE)
    {
        if (HasReachedBannerLimit())
        {
            LOG_ERROR("No free banners available");
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_TOO_MANY_BANNERS_IN_GAME);
        }
    }

    for (auto& tile : sceneryEntry->tiles)
    {
        auto curTile = CoordsXY{ tile.offset }.Rotate(_loc.direction);

        curTile.x += _loc.x;
        curTile.y += _loc.y;

        int32_t zLow = tile.offset.z + maxHeight;
        int32_t zHigh = tile.zClearance + zLow;

        QuarterTile quarterTile = QuarterTile{ tile.corners, 0 }.Rotate(_loc.direction);
        const auto isTree = (sceneryEntry->flags & LARGE_SCENERY_FLAG_IS_TREE) != 0;
        auto canBuild = MapCanConstructWithClearAt(
            { curTile, zLow, zHigh }, &MapPlaceSceneryClearFunc, quarterTile, GetFlags(), CreateCrossingMode::none, isTree);
        if (canBuild.Error != GameActions::Status::Ok)
        {
            canBuild.ErrorTitle = STR_CANT_POSITION_THIS_HERE;
            return canBuild;
        }

        supportsCost += canBuild.Cost;

        const auto clearanceData = canBuild.GetData<ConstructClearResult>();
        int32_t tempSceneryGroundFlags = clearanceData.GroundFlags & (ELEMENT_IS_ABOVE_GROUND | ELEMENT_IS_UNDERGROUND);
        if (!gameState.Cheats.disableClearanceChecks)
        {
            if ((clearanceData.GroundFlags & ELEMENT_IS_UNDERWATER) || (clearanceData.GroundFlags & ELEMENT_IS_UNDERGROUND))
            {
                return GameActions::Result(
                    GameActions::Status::Disallowed, STR_CANT_POSITION_THIS_HERE, STR_CANT_BUILD_THIS_UNDERWATER);
            }
            if (resultData.GroundFlags && !(resultData.GroundFlags & tempSceneryGroundFlags))
            {
                return GameActions::Result(
                    GameActions::Status::Disallowed, STR_CANT_POSITION_THIS_HERE,
                    STR_CANT_BUILD_PARTLY_ABOVE_AND_PARTLY_BELOW_GROUND);
            }
        }

        resultData.GroundFlags = tempSceneryGroundFlags;

        if (!LocationValid(curTile) || MapIsEdge(curTile))
        {
            return GameActions::Result(GameActions::Status::Disallowed, STR_CANT_POSITION_THIS_HERE, STR_OFF_EDGE_OF_MAP);
        }

        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !MapIsLocationOwned({ curTile, zLow })
            && !gameState.Cheats.sandboxMode)
        {
            return GameActions::Result(
                GameActions::Status::Disallowed, STR_CANT_POSITION_THIS_HERE, STR_LAND_NOT_OWNED_BY_PARK);
        }
    }

    if (!CheckMapCapacity(sceneryEntry->tiles, totalNumTiles))
    {
        LOG_ERROR("No free map elements available");
        return GameActions::Result(
            GameActions::Status::NoFreeElements, STR_CANT_POSITION_THIS_HERE, STR_TILE_ELEMENT_LIMIT_REACHED);
    }

    // Force ride construction to recheck area
    _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_RECHECK;

    res.Cost = sceneryEntry->price + supportsCost;
    res.SetData(std::move(resultData));

    return res;
}

GameActions::Result LargeSceneryPlaceAction::Execute() const
{
    auto res = GameActions::Result();
    res.ErrorTitle = STR_CANT_POSITION_THIS_HERE;
    res.Expenditure = ExpenditureType::Landscaping;

    int16_t surfaceHeight = TileElementHeight(_loc);
    res.Position.x = _loc.x + 16;
    res.Position.y = _loc.y + 16;
    res.Position.z = surfaceHeight;

    auto resultData = LargeSceneryPlaceActionResult{};

    money64 supportsCost = 0;

    auto* sceneryEntry = ObjectManager::GetObjectEntry<LargeSceneryEntry>(_sceneryType);
    if (sceneryEntry == nullptr)
    {
        LOG_ERROR("Large scenery entry not found for sceneryType = %u", _sceneryType);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_UNKNOWN_OBJECT_TYPE);
    }

    if (sceneryEntry->tiles.empty())
    {
        LOG_ERROR("Invalid large scenery object, sceneryType = %u", _sceneryType);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, kStringIdNone);
    }

    int16_t maxHeight = GetMaxSurfaceHeight(sceneryEntry->tiles);

    if (_loc.z != 0)
    {
        maxHeight = _loc.z;
    }

    res.Position.z = maxHeight;

    // Allocate banner
    Banner* banner = nullptr;
    if (sceneryEntry->scrolling_mode != SCROLLING_MODE_NONE)
    {
        banner = CreateBanner();
        if (banner == nullptr)
        {
            LOG_ERROR("No free banners available");
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_TOO_MANY_BANNERS_IN_GAME);
        }

        banner->text = {};
        banner->colour = 2;
        banner->text_colour = 2;
        banner->flags = BANNER_FLAG_IS_LARGE_SCENERY;
        banner->type = 0;
        banner->position = TileCoordsXY(_loc);

        RideId rideIndex = BannerGetClosestRideIndex({ _loc, maxHeight });
        if (!rideIndex.IsNull())
        {
            banner->ride_index = rideIndex;
            banner->flags |= BANNER_FLAG_LINKED_TO_RIDE;
        }

        resultData.bannerId = banner->id;
    }

    for (auto& tile : sceneryEntry->tiles)
    {
        auto curTile = CoordsXY{ tile.offset }.Rotate(_loc.direction);

        curTile.x += _loc.x;
        curTile.y += _loc.y;

        int32_t zLow = tile.offset.z + maxHeight;
        int32_t zHigh = tile.zClearance + zLow;

        QuarterTile quarterTile = QuarterTile{ tile.corners, 0 }.Rotate(_loc.direction);
        const auto isTree = (sceneryEntry->flags & LARGE_SCENERY_FLAG_IS_TREE) != 0;
        auto canBuild = MapCanConstructWithClearAt(
            { curTile, zLow, zHigh }, &MapPlaceSceneryClearFunc, quarterTile, GetFlags(), CreateCrossingMode::none, isTree);
        if (canBuild.Error != GameActions::Status::Ok)
        {
            if (banner != nullptr)
            {
                DeleteBanner(banner->id);
            }
            canBuild.ErrorTitle = STR_CANT_POSITION_THIS_HERE;
            return canBuild;
        }

        supportsCost += canBuild.Cost;

        const auto clearanceData = canBuild.GetData<ConstructClearResult>();
        resultData.GroundFlags = clearanceData.GroundFlags & (ELEMENT_IS_ABOVE_GROUND | ELEMENT_IS_UNDERGROUND);

        if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
        {
            FootpathRemoveLitter({ curTile, zLow });
            if (!GetGameState().Cheats.disableClearanceChecks)
            {
                WallRemoveAt({ curTile, zLow, zHigh });
            }
        }

        auto* newSceneryElement = TileElementInsert<LargeSceneryElement>(
            CoordsXYZ{ curTile.x, curTile.y, zLow }, quarterTile.GetBaseQuarterOccupied());
        Guard::Assert(newSceneryElement != nullptr);
        newSceneryElement->SetClearanceZ(zHigh);

        SetNewLargeSceneryElement(*newSceneryElement, tile.index);
        if (banner != nullptr)
        {
            newSceneryElement->SetBannerIndex(banner->id);
        }

        MapAnimationCreate(MAP_ANIMATION_TYPE_LARGE_SCENERY, { curTile, zLow });
        MapInvalidateTileFull(curTile);

        if (tile.index == 0)
        {
            resultData.firstTileHeight = zLow;
        }
    }

    // Force ride construction to recheck area
    _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_RECHECK;

    res.Cost = sceneryEntry->price + supportsCost;
    res.SetData(std::move(resultData));

    return res;
}

bool LargeSceneryPlaceAction::CheckMapCapacity(std::span<const LargeSceneryTile> tiles, size_t numTiles) const
{
    for (auto& tile : tiles)
    {
        auto curTile = CoordsXY{ tile.offset }.Rotate(_loc.direction);

        curTile.x += _loc.x;
        curTile.y += _loc.y;
        if (!MapCheckCapacityAndReorganise(curTile, numTiles))
        {
            return false;
        }
    }
    return true;
}

int16_t LargeSceneryPlaceAction::GetMaxSurfaceHeight(std::span<const LargeSceneryTile> tiles) const
{
    int16_t maxHeight = -1;
    for (auto& tile : tiles)
    {
        auto curTile = CoordsXY{ tile.offset }.Rotate(_loc.direction);

        curTile.x += _loc.x;
        curTile.y += _loc.y;

        if (!MapIsLocationValid(curTile))
        {
            continue;
        }

        auto* surfaceElement = MapGetSurfaceElementAt(curTile);
        if (surfaceElement == nullptr)
            continue;

        int32_t baseZ = surfaceElement->GetBaseZ();
        int32_t slope = surfaceElement->GetSlope();

        if ((slope & kTileSlopeRaisedCornersMask) != kTileSlopeFlat)
        {
            baseZ += kLandHeightStep;
            if (slope & kTileSlopeDiagonalFlag)
            {
                baseZ += kLandHeightStep;
            }
        }

        if (baseZ > maxHeight)
        {
            maxHeight = baseZ;
        }
    }
    return maxHeight;
}

void LargeSceneryPlaceAction::SetNewLargeSceneryElement(LargeSceneryElement& sceneryElement, uint8_t tileNum) const
{
    sceneryElement.SetDirection(_loc.direction);
    sceneryElement.SetEntryIndex(_sceneryType);
    sceneryElement.SetSequenceIndex(tileNum);
    sceneryElement.SetPrimaryColour(_primaryColour);
    sceneryElement.SetSecondaryColour(_secondaryColour);
    sceneryElement.SetTertiaryColour(_tertiaryColour);

    if (GetFlags() & GAME_COMMAND_FLAG_GHOST)
    {
        sceneryElement.SetGhost(true);
    }
}
