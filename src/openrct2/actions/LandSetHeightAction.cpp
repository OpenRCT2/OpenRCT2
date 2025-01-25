/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LandSetHeightAction.h"

#include "../Context.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../object/SmallSceneryEntry.h"
#include "../ride/RideData.h"
#include "../windows/Intent.h"
#include "../world/ConstructionClearance.h"
#include "../world/Footpath.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/TileElementsView.h"
#include "../world/Wall.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/Slope.h"
#include "../world/tile_element/SmallSceneryElement.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TrackElement.h"

using namespace OpenRCT2;

LandSetHeightAction::LandSetHeightAction(const CoordsXY& coords, uint8_t height, uint8_t style)
    : _coords(coords)
    , _height(height)
    , _style(style)
{
}

void LandSetHeightAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_coords);
    visitor.Visit("height", _height);
    visitor.Visit("style", _style);
}

uint16_t LandSetHeightAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::EditorOnly;
}

void LandSetHeightAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_coords) << DS_TAG(_height) << DS_TAG(_style);
}

GameActions::Result LandSetHeightAction::Query() const
{
    auto& gameState = GetGameState();
    if (gameState.Park.Flags & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES)
    {
        return GameActions::Result(GameActions::Status::Disallowed, STR_FORBIDDEN_BY_THE_LOCAL_AUTHORITY, kStringIdNone);
    }

    StringId errorMessage = CheckParameters();
    if (errorMessage != kStringIdNone)
    {
        return GameActions::Result(GameActions::Status::Disallowed, kStringIdNone, errorMessage);
    }

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gameState.Cheats.sandboxMode)
    {
        if (!MapIsLocationInPark(_coords))
        {
            return GameActions::Result(GameActions::Status::Disallowed, STR_LAND_NOT_OWNED_BY_PARK, kStringIdNone);
        }
    }

    money64 sceneryRemovalCost = 0;
    if (!gameState.Cheats.disableClearanceChecks)
    {
        if (gameState.Park.Flags & PARK_FLAGS_FORBID_TREE_REMOVAL)
        {
            // Check for obstructing large trees
            TileElement* tileElement = CheckTreeObstructions();
            if (tileElement != nullptr)
            {
                auto res = GameActions::Result(GameActions::Status::Disallowed, kStringIdNone, kStringIdNone);
                MapGetObstructionErrorText(tileElement, res);
                return res;
            }
        }
        sceneryRemovalCost = GetSmallSceneryRemovalCost();
    }

    // Check for ride support limits
    if (!gameState.Cheats.disableSupportLimits)
    {
        errorMessage = CheckRideSupports();
        if (errorMessage != kStringIdNone)
        {
            return GameActions::Result(GameActions::Status::Disallowed, kStringIdNone, errorMessage);
        }
    }

    auto* surfaceElement = MapGetSurfaceElementAt(_coords);
    if (surfaceElement == nullptr)
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_SURFACE_ELEMENT_NOT_FOUND);

    // We need to check if there is _currently_ a level crossing on the tile.
    // For that, we need the old height, so we can't use the _height variable.
    auto oldCoords = CoordsXYZ{ _coords, surfaceElement->GetBaseZ() };
    auto* pathElement = MapGetFootpathElement(oldCoords);
    if (pathElement != nullptr && pathElement->AsPath()->IsLevelCrossing(oldCoords))
    {
        return GameActions::Result(GameActions::Status::Disallowed, STR_REMOVE_LEVEL_CROSSING_FIRST, kStringIdNone);
    }

    TileElement* tileElement = CheckFloatingStructures(reinterpret_cast<TileElement*>(surfaceElement), _height);
    if (tileElement != nullptr)
    {
        auto res = GameActions::Result(GameActions::Status::Disallowed, kStringIdNone, kStringIdNone);
        MapGetObstructionErrorText(tileElement, res);
        return res;
    }

    if (!gameState.Cheats.disableClearanceChecks)
    {
        uint8_t zCorner = _height;
        if (_style & kTileSlopeRaisedCornersMask)
        {
            zCorner += 2;
            if (_style & kTileSlopeDiagonalFlag)
            {
                zCorner += 2;
            }
        }

        auto clearResult = MapCanConstructWithClearAt(
            { _coords, _height * kCoordsZStep, zCorner * kCoordsZStep }, &MapSetLandHeightClearFunc, { 0b1111, 0 }, 0,
            CreateCrossingMode::none);
        if (clearResult.Error != GameActions::Status::Ok)
        {
            clearResult.Error = GameActions::Status::Disallowed;
            return clearResult;
        }
    }
    auto res = GameActions::Result();
    res.Cost = sceneryRemovalCost + GetSurfaceHeightChangeCost(surfaceElement);
    res.Expenditure = ExpenditureType::Landscaping;
    return res;
}

GameActions::Result LandSetHeightAction::Execute() const
{
    money64 cost = 0.00_GBP;
    auto surfaceHeight = TileElementHeight(_coords);
    FootpathRemoveLitter({ _coords, surfaceHeight });

    if (!GetGameState().Cheats.disableClearanceChecks)
    {
        WallRemoveAt({ _coords, _height * 8 - 16, _height * 8 + 32 });
        cost += GetSmallSceneryRemovalCost();
        SmallSceneryRemoval();
    }

    auto* surfaceElement = MapGetSurfaceElementAt(_coords);
    if (surfaceElement == nullptr)
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_SURFACE_ELEMENT_NOT_FOUND);

    cost += GetSurfaceHeightChangeCost(surfaceElement);
    SetSurfaceHeight(reinterpret_cast<TileElement*>(surfaceElement));

    auto res = GameActions::Result();
    res.Position = { _coords.x + 16, _coords.y + 16, surfaceHeight };
    res.Cost = cost;
    res.Expenditure = ExpenditureType::Landscaping;
    return res;
}

StringId LandSetHeightAction::CheckParameters() const
{
    if (!LocationValid(_coords) || MapIsEdge(_coords))
    {
        return STR_OFF_EDGE_OF_MAP;
    }

    if (_height < kMinimumLandHeight)
    {
        return STR_TOO_LOW;
    }

    // Divide by 2 and subtract 7 to get the in-game units.
    if (_height > kMaximumLandHeight)
    {
        return STR_TOO_HIGH;
    }

    if (_height > kMaximumLandHeight - 2 && (_style & kTileSlopeMask) != 0)
    {
        return STR_TOO_HIGH;
    }

    if (_height == kMaximumLandHeight - 2 && (_style & kTileSlopeDiagonalFlag))
    {
        return STR_TOO_HIGH;
    }

    return kStringIdNone;
}

TileElement* LandSetHeightAction::CheckTreeObstructions() const
{
    for (auto* sceneryElement : TileElementsView<SmallSceneryElement>(_coords))
    {
        if (_height > sceneryElement->ClearanceHeight)
            continue;
        if (_height + 4 < sceneryElement->BaseHeight)
            continue;

        auto* sceneryEntry = sceneryElement->GetEntry();
        if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_IS_TREE))
            continue;

        return sceneryElement->as<TileElement>();
    }
    return nullptr;
}

money64 LandSetHeightAction::GetSmallSceneryRemovalCost() const
{
    money64 cost{ 0 };

    for (auto* sceneryElement : TileElementsView<SmallSceneryElement>(_coords))
    {
        if (_height > sceneryElement->ClearanceHeight)
            continue;
        if (_height + 4 < sceneryElement->BaseHeight)
            continue;

        auto* sceneryEntry = sceneryElement->GetEntry();
        if (sceneryEntry == nullptr)
            continue;

        cost += sceneryEntry->removal_price;
    }

    return cost;
}

void LandSetHeightAction::SmallSceneryRemoval() const
{
    TileElement* tileElement = MapGetFirstElementAt(_coords);
    do
    {
        if (tileElement == nullptr)
            break;
        if (tileElement->GetType() != TileElementType::SmallScenery)
            continue;
        if (_height > tileElement->ClearanceHeight)
            continue;
        if (_height + 4 < tileElement->BaseHeight)
            continue;
        TileElementRemove(tileElement--);
    } while (!(tileElement++)->IsLastForTile());
}

StringId LandSetHeightAction::CheckRideSupports() const
{
    for (auto* trackElement : TileElementsView<TrackElement>(_coords))
    {
        RideId rideIndex = trackElement->GetRideIndex();

        auto ride = GetRide(rideIndex);
        if (ride == nullptr)
            continue;

        const auto* rideEntry = ride->GetRideEntry();
        if (rideEntry == nullptr)
            continue;

        int32_t maxHeight = rideEntry->max_height;
        if (maxHeight == 0)
        {
            maxHeight = ride->GetRideTypeDescriptor().Heights.MaxHeight;
        }

        int32_t zDelta = trackElement->ClearanceHeight - _height;
        if (zDelta >= 0 && zDelta / 2 > maxHeight)
        {
            return STR_SUPPORTS_CANT_BE_EXTENDED;
        }
    }
    return kStringIdNone;
}

TileElement* LandSetHeightAction::CheckFloatingStructures(TileElement* surfaceElement, uint8_t zCorner) const
{
    if (surfaceElement->AsSurface()->HasTrackThatNeedsWater())
    {
        uint32_t waterHeight = surfaceElement->AsSurface()->GetWaterHeight();
        if (waterHeight != 0)
        {
            if (_style & kTileSlopeMask)
            {
                zCorner += 2;
                if (_style & kTileSlopeDiagonalFlag)
                {
                    zCorner += 2;
                }
            }
            if (zCorner > (waterHeight / kCoordsZStep) - 2)
            {
                return ++surfaceElement;
            }
        }
    }
    return nullptr;
}

money64 LandSetHeightAction::GetSurfaceHeightChangeCost(SurfaceElement* surfaceElement) const
{
    money64 cost{ 0 };
    for (Direction i : kAllDirections)
    {
        int32_t cornerHeight = TileElementGetCornerHeight(surfaceElement, i);
        cornerHeight -= MapGetCornerHeight(_height, _style & kTileSlopeMask, i);
        cost += 2.50_GBP * abs(cornerHeight);
    }
    return cost;
}

void LandSetHeightAction::SetSurfaceHeight(TileElement* surfaceElement) const
{
    surfaceElement->BaseHeight = _height;
    surfaceElement->ClearanceHeight = _height;
    surfaceElement->AsSurface()->SetSlope(_style);
    int32_t waterHeight = surfaceElement->AsSurface()->GetWaterHeight() / kCoordsZStep;
    if (waterHeight != 0 && waterHeight <= _height)
    {
        surfaceElement->AsSurface()->SetWaterHeight(0);
    }

    MapInvalidateTileFull(_coords);
}

int32_t LandSetHeightAction::MapSetLandHeightClearFunc(
    TileElement** tile_element, [[maybe_unused]] const CoordsXY& coords, [[maybe_unused]] uint8_t flags,
    [[maybe_unused]] money64* price)
{
    if ((*tile_element)->GetType() == TileElementType::Surface)
        return 0;

    if ((*tile_element)->GetType() == TileElementType::SmallScenery)
        return 0;

    return 1;
}
