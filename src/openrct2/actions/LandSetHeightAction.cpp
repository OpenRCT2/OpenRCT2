/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LandSetHeightAction.h"

#include "../Context.h"
#include "../OpenRCT2.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/RideData.h"
#include "../windows/Intent.h"
#include "../world/ConstructionClearance.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/SmallScenery.h"
#include "../world/Surface.h"
#include "../world/TileElementsView.h"

namespace OpenRCT2
{

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
        if (gParkFlags & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES)
        {
            return GameActions::Result(GameActions::Status::Disallowed, STR_FORBIDDEN_BY_THE_LOCAL_AUTHORITY, STR_NONE);
        }

        StringId errorMessage = CheckParameters();
        if (errorMessage != STR_NONE)
        {
            return GameActions::Result(GameActions::Status::Disallowed, STR_NONE, errorMessage);
        }

        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
        {
            if (!MapIsLocationInPark(_coords))
            {
                return GameActions::Result(GameActions::Status::Disallowed, STR_LAND_NOT_OWNED_BY_PARK, STR_NONE);
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
                    auto res = GameActions::Result(GameActions::Status::Disallowed, STR_NONE, STR_NONE);
                    MapGetObstructionErrorText(tileElement, res);
                    return res;
                }
            }
            sceneryRemovalCost = GetSmallSceneryRemovalCost();
        }

        // Check for ride support limits
        if (!gCheatsDisableSupportLimits)
        {
            errorMessage = CheckRideSupports();
            if (errorMessage != STR_NONE)
            {
                return GameActions::Result(GameActions::Status::Disallowed, STR_NONE, errorMessage);
            }
        }

        auto* surfaceElement = MapGetSurfaceElementAt(_coords);
        if (surfaceElement == nullptr)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

        // We need to check if there is _currently_ a level crossing on the tile.
        // For that, we need the old height, so we can't use the _height variable.
        auto oldCoords = CoordsXYZ{ _coords, surfaceElement->GetBaseZ() };
        auto* pathElement = MapGetFootpathElement(oldCoords);
        if (pathElement != nullptr && pathElement->AsPath()->IsLevelCrossing(oldCoords))
        {
            return GameActions::Result(GameActions::Status::Disallowed, STR_REMOVE_LEVEL_CROSSING_FIRST, STR_NONE);
        }

        TileElement* tileElement = CheckFloatingStructures(reinterpret_cast<TileElement*>(surfaceElement), _height);
        if (tileElement != nullptr)
        {
            auto res = GameActions::Result(GameActions::Status::Disallowed, STR_NONE, STR_NONE);
            MapGetObstructionErrorText(tileElement, res);
            return res;
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

            auto clearResult = MapCanConstructWithClearAt(
                { _coords, _height * COORDS_Z_STEP, zCorner * COORDS_Z_STEP }, &MapSetLandHeightClearFunc, { 0b1111, 0 }, 0,
                CREATE_CROSSING_MODE_NONE);
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
        money32 cost = 0.00_GBP;
        auto surfaceHeight = TileElementHeight(_coords);
        FootpathRemoveLitter({ _coords, surfaceHeight });

        if (!gCheatsDisableClearanceChecks)
        {
            WallRemoveAt({ _coords, _height * 8 - 16, _height * 8 + 32 });
            cost += GetSmallSceneryRemovalCost();
            SmallSceneryRemoval();
        }

        auto* surfaceElement = MapGetSurfaceElementAt(_coords);
        if (surfaceElement == nullptr)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

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
        if (!LocationValid(_coords))
        {
            return STR_OFF_EDGE_OF_MAP;
        }

        auto mapSizeMax = GetMapSizeMaxXY();
        if (_coords.x > mapSizeMax.x || _coords.y > mapSizeMax.y)
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

        if (_height > MAXIMUM_LAND_HEIGHT - 2 && (_style & TILE_ELEMENT_SURFACE_SLOPE_MASK) != 0)
        {
            return STR_TOO_HIGH;
        }

        if (_height == MAXIMUM_LAND_HEIGHT - 2 && (_style & TILE_ELEMENT_SURFACE_DIAGONAL_FLAG))
        {
            return STR_TOO_HIGH;
        }

        return STR_NONE;
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

    money32 LandSetHeightAction::GetSmallSceneryRemovalCost() const
    {
        money32 cost{ 0 };

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

            RideObjectEntry* rideEntry = ride->GetRideEntry();
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
        return STR_NONE;
    }

    TileElement* LandSetHeightAction::CheckFloatingStructures(TileElement* surfaceElement, uint8_t zCorner) const
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
                if (zCorner > (waterHeight / COORDS_Z_STEP) - 2)
                {
                    return ++surfaceElement;
                }
            }
        }
        return nullptr;
    }

    money32 LandSetHeightAction::GetSurfaceHeightChangeCost(SurfaceElement* surfaceElement) const
    {
        money32 cost{ 0 };
        for (Direction i : ALL_DIRECTIONS)
        {
            int32_t cornerHeight = TileElementGetCornerHeight(surfaceElement, i);
            cornerHeight -= MapGetCornerHeight(_height, _style & TILE_ELEMENT_SURFACE_SLOPE_MASK, i);
            cost += 2.50_GBP * abs(cornerHeight);
        }
        return cost;
    }

    void LandSetHeightAction::SetSurfaceHeight(TileElement* surfaceElement) const
    {
        surfaceElement->BaseHeight = _height;
        surfaceElement->ClearanceHeight = _height;
        surfaceElement->AsSurface()->SetSlope(_style);
        int32_t waterHeight = surfaceElement->AsSurface()->GetWaterHeight() / COORDS_Z_STEP;
        if (waterHeight != 0 && waterHeight <= _height)
        {
            surfaceElement->AsSurface()->SetWaterHeight(0);
        }

        MapInvalidateTileFull(_coords);
    }

    int32_t LandSetHeightAction::MapSetLandHeightClearFunc(
        TileElement** tile_element, [[maybe_unused]] const CoordsXY& coords, [[maybe_unused]] uint8_t flags,
        [[maybe_unused]] money32* price)
    {
        if ((*tile_element)->GetType() == TileElementType::Surface)
            return 0;

        if ((*tile_element)->GetType() == TileElementType::SmallScenery)
            return 0;

        return 1;
    }
} // namespace OpenRCT2
