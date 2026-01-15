/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LandSmoothAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../OpenRCT2.h"
#include "../actions/LandLowerAction.h"
#include "../actions/LandRaiseAction.h"
#include "../actions/LandSetHeightAction.h"
#include "../audio/Audio.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/RideData.h"
#include "../windows/Intent.h"
#include "../world/Map.h"
#include "../world/MapSelection.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/SurfaceData.h"
#include "../world/tile_element/Slope.h"
#include "../world/tile_element/SurfaceElement.h"

#include <algorithm>

namespace OpenRCT2::GameActions
{
    LandSmoothAction::LandSmoothAction(const CoordsXY& coords, MapRange range, MapSelectType selectionType, bool isLowering)
        : _coords(coords)
        , _range(range)
        , _selectionType(selectionType)
        , _isLowering(isLowering)
    {
    }

    void LandSmoothAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit(_coords);
        visitor.Visit(_range);
        visitor.Visit("selectionType", _selectionType);
        visitor.Visit("isLowering", _isLowering);
    }

    uint16_t LandSmoothAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags();
    }

    void LandSmoothAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_coords) << DS_TAG(_range) << DS_TAG(_selectionType) << DS_TAG(_isLowering);
    }

    Result LandSmoothAction::Query(GameState_t& gameState) const
    {
        return SmoothLand(gameState, false);
    }

    Result LandSmoothAction::Execute(GameState_t& gameState) const
    {
        return SmoothLand(gameState, true);
    }

    Result LandSmoothAction::SmoothLandTile(
        GameState_t& gameState, int32_t direction, bool isExecuting, const CoordsXY& loc, SurfaceElement* surfaceElement) const
    {
        int32_t targetBaseZ = surfaceElement->BaseHeight;
        int32_t slope = surfaceElement->GetSlope();
        if (_isLowering)
        {
            slope = LowerSurfaceCornerFlags(direction, slope);
            if (slope & kTileSlopeRaiseOrLowerBaseHeight)
            {
                targetBaseZ -= 2;
                slope &= ~kTileSlopeRaiseOrLowerBaseHeight;
            }
        }
        else
        {
            slope = RaiseSurfaceCornerFlags(direction, slope);
            if (slope & kTileSlopeRaiseOrLowerBaseHeight)
            {
                targetBaseZ += 2;
                slope &= ~kTileSlopeRaiseOrLowerBaseHeight;
            }
        }

        auto landSetHeightAction = LandSetHeightAction(loc, targetBaseZ, slope);
        landSetHeightAction.SetFlags(GetFlags());
        auto res = isExecuting ? ExecuteNested(&landSetHeightAction, gameState) : QueryNested(&landSetHeightAction, gameState);

        return res;
    }

    money64 LandSmoothAction::SmoothLandRowByEdge(
        GameState_t& gameState, bool isExecuting, const CoordsXY& loc, int32_t expectedLandHeight1, int32_t expectedLandHeight2,
        int32_t stepX, int32_t stepY, int32_t direction1, int32_t direction2, int32_t checkDirection1,
        int32_t checkDirection2) const
    {
        uint8_t shouldContinue = 0xF;
        int32_t landChangePerTile = _isLowering ? 2 : -2;
        money64 totalCost = 0;

        // check if we need to start at all
        if (!LocationValid(loc) || !LocationValid({ loc.x + stepX, loc.y + stepY }))
        {
            return 0;
        }
        auto surfaceElement = MapGetSurfaceElementAt(loc);
        auto nextSurfaceElement = MapGetSurfaceElementAt(CoordsXY{ loc.x + stepX, loc.y + stepY });
        if (surfaceElement == nullptr || nextSurfaceElement == nullptr)
        {
            return 0;
        }
        if (TileElementGetCornerHeight(surfaceElement, checkDirection1) != expectedLandHeight1 + landChangePerTile)
        {
            shouldContinue &= ~0x1;
        }
        if (TileElementGetCornerHeight(surfaceElement, checkDirection2) != expectedLandHeight2 + landChangePerTile)
        {
            shouldContinue &= ~0x2;
        }
        if (TileElementGetCornerHeight(surfaceElement, checkDirection1)
            != TileElementGetCornerHeight(nextSurfaceElement, direction1))
        {
            shouldContinue &= ~0x1;
        }
        if (TileElementGetCornerHeight(surfaceElement, checkDirection2)
            != TileElementGetCornerHeight(nextSurfaceElement, direction2))
        {
            shouldContinue &= ~0x2;
        }
        auto nextLoc = loc;
        while ((shouldContinue & 0x3) != 0)
        {
            shouldContinue = ((shouldContinue << 2) | 0x3) & shouldContinue;
            nextLoc.x += stepX;
            nextLoc.y += stepY;
            // check if we need to continue after raising the current tile
            // this needs to be checked before the tile is changed
            if (!LocationValid({ nextLoc.x + stepX, nextLoc.y + stepY }))
            {
                shouldContinue &= ~0x3;
            }
            else
            {
                surfaceElement = nextSurfaceElement;
                nextSurfaceElement = MapGetSurfaceElementAt(CoordsXY{ nextLoc.x + stepX, nextLoc.y + stepY });
                if (nextSurfaceElement == nullptr)
                {
                    shouldContinue &= ~0x3;
                }
                if (TileElementGetCornerHeight(surfaceElement, direction1) + landChangePerTile
                    != TileElementGetCornerHeight(surfaceElement, checkDirection1))
                {
                    shouldContinue &= ~0x1;
                }
                if (TileElementGetCornerHeight(surfaceElement, direction2) + landChangePerTile
                    != TileElementGetCornerHeight(surfaceElement, checkDirection2))
                {
                    shouldContinue &= ~0x2;
                }
                if ((shouldContinue & 0x1)
                    && TileElementGetCornerHeight(surfaceElement, checkDirection1)
                        != TileElementGetCornerHeight(nextSurfaceElement, direction1))
                {
                    shouldContinue &= ~0x1;
                }
                if ((shouldContinue & 0x2)
                    && TileElementGetCornerHeight(surfaceElement, checkDirection2)
                        != TileElementGetCornerHeight(nextSurfaceElement, direction2))
                {
                    shouldContinue &= ~0x2;
                }
            }
            expectedLandHeight1 += landChangePerTile;

            // change land of current tile
            int32_t targetBaseZ = surfaceElement->BaseHeight;
            int32_t slope = surfaceElement->GetSlope();
            int32_t oldSlope = slope;
            if (_isLowering)
            {
                if (shouldContinue & 0x4)
                {
                    slope = LowerSurfaceCornerFlags(direction1, slope);
                    if (slope & kTileSlopeRaiseOrLowerBaseHeight)
                    {
                        targetBaseZ -= 2;
                        slope &= ~kTileSlopeRaiseOrLowerBaseHeight;
                    }
                }
                if ((shouldContinue & 0x8)
                    && MapGetCornerHeight(surfaceElement->BaseHeight, oldSlope, direction2)
                        == MapGetCornerHeight(targetBaseZ, slope, direction2))
                {
                    slope = LowerSurfaceCornerFlags(direction2, slope);
                    if (slope & kTileSlopeRaiseOrLowerBaseHeight)
                    {
                        targetBaseZ -= 2;
                        slope &= ~kTileSlopeRaiseOrLowerBaseHeight;
                    }
                }
            }
            else
            {
                if (shouldContinue & 0x4)
                {
                    slope = RaiseSurfaceCornerFlags(direction1, slope);
                    if (slope & kTileSlopeRaiseOrLowerBaseHeight)
                    {
                        targetBaseZ += 2;
                        slope &= ~kTileSlopeRaiseOrLowerBaseHeight;
                    }
                }
                if ((shouldContinue & 0x8)
                    && MapGetCornerHeight(surfaceElement->BaseHeight, oldSlope, direction2)
                        == MapGetCornerHeight(targetBaseZ, slope, direction2))
                {
                    slope = RaiseSurfaceCornerFlags(direction2, slope);
                    if (slope & kTileSlopeRaiseOrLowerBaseHeight)
                    {
                        targetBaseZ += 2;
                        slope &= ~kTileSlopeRaiseOrLowerBaseHeight;
                    }
                }
            }
            auto landSetHeightAction = LandSetHeightAction(nextLoc, targetBaseZ, slope);
            landSetHeightAction.SetFlags(GetFlags());
            auto res = isExecuting ? ExecuteNested(&landSetHeightAction, gameState)
                                   : QueryNested(&landSetHeightAction, gameState);
            if (res.error == Status::ok)
            {
                totalCost += res.cost;
            }
        }
        return totalCost;
    }

    money64 LandSmoothAction::SmoothLandRowByCorner(
        GameState_t& gameState, bool isExecuting, const CoordsXY& loc, int32_t expectedLandHeight, int32_t stepX, int32_t stepY,
        int32_t direction, int32_t checkDirection) const
    {
        bool shouldContinue = true;
        money64 totalCost = 0;
        int32_t landChangePerTile;
        if (stepX == 0 || stepY == 0)
        {
            landChangePerTile = _isLowering ? 2 : -2;
        }
        else
        {
            landChangePerTile = _isLowering ? 4 : -4;
        }

        // check if we need to start at all
        if (!LocationValid(loc) || !LocationValid({ loc.x + stepX, loc.y + stepY }))
        {
            return 0;
        }
        auto surfaceElement = MapGetSurfaceElementAt(loc);
        auto nextSurfaceElement = MapGetSurfaceElementAt(CoordsXY{ loc.x + stepX, loc.y + stepY });
        if (surfaceElement == nullptr || nextSurfaceElement == nullptr)
        {
            return 0;
        }
        if (TileElementGetCornerHeight(surfaceElement, checkDirection) != expectedLandHeight + (_isLowering ? 2 : -2))
        {
            return 0;
        }
        if (TileElementGetCornerHeight(surfaceElement, checkDirection)
            != TileElementGetCornerHeight(nextSurfaceElement, direction))
        {
            return 0;
        }

        auto nextLoc = loc;
        while (shouldContinue)
        {
            nextLoc.x += stepX;
            nextLoc.y += stepY;
            // check if we need to continue after raising the current tile
            // this needs to be checked before the tile is changed
            if (!LocationValid({ nextLoc.x + stepX, nextLoc.y + stepY }))
            {
                shouldContinue = false;
            }
            else
            {
                surfaceElement = nextSurfaceElement;
                nextSurfaceElement = MapGetSurfaceElementAt(CoordsXY{ nextLoc.x + stepX, nextLoc.y + stepY });
                if (nextSurfaceElement == nullptr)
                {
                    shouldContinue = false;
                }
                if (TileElementGetCornerHeight(surfaceElement, direction) + landChangePerTile
                    != TileElementGetCornerHeight(surfaceElement, checkDirection))
                {
                    shouldContinue = false;
                }
                if (shouldContinue
                    && TileElementGetCornerHeight(surfaceElement, checkDirection)
                        != TileElementGetCornerHeight(nextSurfaceElement, direction))
                {
                    shouldContinue = false;
                }
            }
            if (stepX * stepY != 0)
            {
                totalCost += SmoothLandRowByCorner(
                    gameState, isExecuting, nextLoc, expectedLandHeight + (landChangePerTile / 2), 0, stepY, direction,
                    checkDirection ^ 3);
                totalCost += SmoothLandRowByCorner(
                    gameState, isExecuting, nextLoc, expectedLandHeight + (landChangePerTile / 2), stepX, 0, direction,
                    checkDirection ^ 1);
            }
            expectedLandHeight += landChangePerTile;
            // change land of current tile
            auto result = SmoothLandTile(gameState, direction, isExecuting, nextLoc, surfaceElement);
            if (result.error == Status::ok)
            {
                totalCost += result.cost;
            }
        }
        return totalCost;
    }

    Result LandSmoothAction::SmoothLand(GameState_t& gameState, bool isExecuting) const
    {
        const bool raiseLand = !_isLowering;
        const auto selectionType = _selectionType;
        const int32_t heightOffset = raiseLand ? 2 : -2;

        auto normRange = _range.Normalise();
        // Cap bounds to map
        auto l = std::max(normRange.GetX1(), 32);
        auto t = std::max(normRange.GetY1(), 32);
        auto r = std::clamp(normRange.GetX2(), 0, kMaximumTileStartXY);
        auto b = std::clamp(normRange.GetY2(), 0, kMaximumTileStartXY);
        auto validRange = MapRange{ l, t, r, b };

        int32_t centreZ = TileElementHeight(_coords);

        auto res = Result();
        res.errorTitle = kErrorTitles[_isLowering ? 0 : 1];
        res.expenditure = ExpenditureType::landscaping;
        res.position = { _coords.x, _coords.y, centreZ };

        // Do the smoothing
        switch (selectionType)
        {
            case MapSelectType::full:
            {
                uint8_t minHeight = heightOffset + MapGetLowestLandHeight(validRange);
                uint8_t maxHeight = heightOffset + MapGetHighestLandHeight(validRange);

                // Smooth the 4 corners
                { // top-left
                    auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ validRange.GetX1(), validRange.GetY1() });
                    if (surfaceElement != nullptr)
                    {
                        int32_t z = std::clamp(
                            static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 2)), minHeight, maxHeight);
                        res.cost += SmoothLandRowByCorner(
                            gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, -32, -32, 0, 2);
                    }
                }
                { // bottom-left
                    auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ validRange.GetX1(), validRange.GetY2() });
                    if (surfaceElement != nullptr)
                    {
                        int32_t z = std::clamp(
                            static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 3)), minHeight, maxHeight);
                        res.cost += SmoothLandRowByCorner(
                            gameState, isExecuting, { validRange.GetX1(), validRange.GetY2() }, z, -32, 32, 1, 3);
                    }
                }
                { // bottom-right
                    auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ validRange.GetX2(), validRange.GetY2() });
                    if (surfaceElement != nullptr)
                    {
                        int32_t z = std::clamp(
                            static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 0)), minHeight, maxHeight);
                        res.cost += SmoothLandRowByCorner(
                            gameState, isExecuting, { validRange.GetX2(), validRange.GetY2() }, z, 32, 32, 2, 0);
                    }
                }
                { // top-right
                    auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ validRange.GetX2(), validRange.GetY1() });
                    if (surfaceElement != nullptr)
                    {
                        int32_t z = std::clamp(
                            static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 1)), minHeight, maxHeight);
                        res.cost += SmoothLandRowByCorner(
                            gameState, isExecuting, { validRange.GetX2(), validRange.GetY1() }, z, 32, -32, 3, 1);
                    }
                }

                // Smooth the edges
                int32_t z1, z2;
                for (int32_t y = validRange.GetY1(); y <= validRange.GetY2(); y += kCoordsXYStep)
                {
                    auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ validRange.GetX1(), y });
                    if (surfaceElement != nullptr)
                    {
                        z1 = std::clamp(
                            static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 3)), minHeight, maxHeight);
                        z2 = std::clamp(
                            static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 2)), minHeight, maxHeight);
                        res.cost += SmoothLandRowByEdge(
                            gameState, isExecuting, { validRange.GetX1(), y }, z1, z2, -32, 0, 0, 1, 3, 2);
                    }

                    surfaceElement = MapGetSurfaceElementAt(CoordsXY{ validRange.GetX2(), y });
                    if (surfaceElement != nullptr)
                    {
                        z1 = std::clamp(
                            static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 1)), minHeight, maxHeight);
                        z2 = std::clamp(
                            static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 0)), minHeight, maxHeight);
                        res.cost += SmoothLandRowByEdge(
                            gameState, isExecuting, { validRange.GetX2(), y }, z1, z2, 32, 0, 2, 3, 1, 0);
                    }
                }

                for (int32_t x = validRange.GetX1(); x <= validRange.GetX2(); x += kCoordsXYStep)
                {
                    auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ x, validRange.GetY1() });
                    if (surfaceElement != nullptr)
                    {
                        z1 = std::clamp(
                            static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 1)), minHeight, maxHeight);
                        z2 = std::clamp(
                            static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 2)), minHeight, maxHeight);
                        res.cost += SmoothLandRowByEdge(
                            gameState, isExecuting, { x, validRange.GetY1() }, z1, z2, 0, -32, 0, 3, 1, 2);
                    }

                    surfaceElement = MapGetSurfaceElementAt(CoordsXY{ x, validRange.GetY2() });
                    if (surfaceElement != nullptr)
                    {
                        z1 = std::clamp(
                            static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 0)), minHeight, maxHeight);
                        z2 = std::clamp(
                            static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 3)), minHeight, maxHeight);
                        res.cost += SmoothLandRowByEdge(
                            gameState, isExecuting, { x, validRange.GetY2() }, z1, z2, 0, 32, 1, 2, 0, 3);
                    }
                }
                break;
            }
            case MapSelectType::corner0:
            case MapSelectType::corner1:
            case MapSelectType::corner2:
            case MapSelectType::corner3:
            {
                auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ validRange.GetX1(), validRange.GetY1() });
                if (surfaceElement == nullptr)
                    break;
                uint8_t newBaseZ = surfaceElement->BaseHeight;
                uint8_t newSlope = surfaceElement->GetSlope();
                auto direction = static_cast<Direction>(selectionType);

                if (raiseLand)
                {
                    newSlope = RaiseSurfaceCornerFlags(direction, newSlope);
                }
                else
                {
                    newSlope = LowerSurfaceCornerFlags(direction, newSlope);
                }

                if (newSlope & kTileSlopeRaiseOrLowerBaseHeight)
                {
                    newBaseZ += heightOffset;
                    newSlope &= ~kTileSlopeRaiseOrLowerBaseHeight;
                }

                // Smooth the corners
                int32_t z = MapGetCornerHeight(newBaseZ, newSlope, 2);
                res.cost += SmoothLandRowByCorner(
                    gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, -32, -32, 0, 2);
                z = MapGetCornerHeight(newBaseZ, newSlope, 0);
                res.cost += SmoothLandRowByCorner(
                    gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, 32, 32, 2, 0);
                z = MapGetCornerHeight(newBaseZ, newSlope, 3);
                res.cost += SmoothLandRowByCorner(
                    gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, -32, 32, 1, 3);
                z = MapGetCornerHeight(newBaseZ, newSlope, 1);
                res.cost += SmoothLandRowByCorner(
                    gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, 32, -32, 3, 1);

                // Smooth the edges
                switch (selectionType)
                {
                    case MapSelectType::corner0:
                        z = MapGetCornerHeight(newBaseZ, newSlope, 0);
                        res.cost += SmoothLandRowByCorner(
                            gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, 32, 0, 3, 0);
                        res.cost += SmoothLandRowByCorner(
                            gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, 0, 32, 1, 0);
                        z = MapGetCornerHeight(newBaseZ, newSlope, 3);
                        res.cost += SmoothLandRowByCorner(
                            gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, -32, 0, 0, 3);
                        z = MapGetCornerHeight(newBaseZ, newSlope, 1);
                        res.cost += SmoothLandRowByCorner(
                            gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, 0, -32, 0, 1);
                        break;
                    case MapSelectType::corner1:
                        z = MapGetCornerHeight(newBaseZ, newSlope, 1);
                        res.cost += SmoothLandRowByCorner(
                            gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, 32, 0, 2, 1);
                        res.cost += SmoothLandRowByCorner(
                            gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, 0, -32, 0, 1);
                        z = MapGetCornerHeight(newBaseZ, newSlope, 2);
                        res.cost += SmoothLandRowByCorner(
                            gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, -32, 0, 1, 2);
                        z = MapGetCornerHeight(newBaseZ, newSlope, 0);
                        res.cost += SmoothLandRowByCorner(
                            gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, 0, 32, 1, 0);
                        break;
                    case MapSelectType::corner2:
                        z = MapGetCornerHeight(newBaseZ, newSlope, 2);
                        res.cost += SmoothLandRowByCorner(
                            gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, -32, 0, 1, 2);
                        res.cost += SmoothLandRowByCorner(
                            gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, 0, -32, 3, 2);
                        z = MapGetCornerHeight(newBaseZ, newSlope, 1);
                        res.cost += SmoothLandRowByCorner(
                            gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, 32, 0, 2, 1);
                        z = MapGetCornerHeight(newBaseZ, newSlope, 3);
                        res.cost += SmoothLandRowByCorner(
                            gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, 0, 32, 2, 3);
                        break;
                    case MapSelectType::corner3:
                        z = MapGetCornerHeight(newBaseZ, newSlope, 3);
                        res.cost += SmoothLandRowByCorner(
                            gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, -32, 0, 0, 3);
                        res.cost += SmoothLandRowByCorner(
                            gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, 0, 32, 2, 3);
                        z = MapGetCornerHeight(newBaseZ, newSlope, 0);
                        res.cost += SmoothLandRowByCorner(
                            gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, 32, 0, 3, 0);
                        z = MapGetCornerHeight(newBaseZ, newSlope, 2);
                        res.cost += SmoothLandRowByCorner(
                            gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, 0, -32, 3, 2);
                        break;
                    default:
                        break;
                }
                break;
            }
            case MapSelectType::edge0:
            case MapSelectType::edge1:
            case MapSelectType::edge2:
            case MapSelectType::edge3:
            {
                // TODO: Handle smoothing by edge
                // Get the two corners to raise
                auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ validRange.GetX1(), validRange.GetY1() });
                if (surfaceElement == nullptr)
                    break;
                uint8_t newBaseZ = surfaceElement->BaseHeight;
                uint8_t oldSlope = surfaceElement->GetSlope();
                int32_t rowIndex = EnumValue(selectionType)
                    - (EnumValue(MapSelectType::edge0) - EnumValue(MapSelectType::full) - 1);
                uint8_t newSlope = raiseLand ? RaiseSurfaceCornerFlags(rowIndex, oldSlope)
                                             : LowerSurfaceCornerFlags(rowIndex, oldSlope);

                const bool changeBaseHeight = newSlope & kTileSlopeRaiseOrLowerBaseHeight;
                if (changeBaseHeight)
                {
                    newBaseZ += heightOffset;
                    newSlope &= ~kTileSlopeRaiseOrLowerBaseHeight;
                }

                const uint8_t edge = EnumValue(selectionType) - EnumValue(MapSelectType::edge0);

                // Table with corners for each edge selection. The first two are the selected corners, the latter
                // two are the opposites
                static constexpr uint8_t cornerIndices[][4] = {
                    { 2, 3, 1, 0 }, // MapSelectType::edge0
                    { 3, 0, 2, 1 }, // MapSelectType::edge1
                    { 0, 1, 3, 2 }, // MapSelectType::edge2
                    { 1, 2, 0, 3 }, // MapSelectType::edge3
                };
                // Big coordinate offsets for the neighbouring tile for the given edge selection
                static constexpr CoordsXY stepOffsets[] = {
                    { -32, 0 },
                    { 0, 32 },
                    { 32, 0 },
                    { 0, -32 },
                };

                // Smooth higher and lower edges
                uint8_t c1 = cornerIndices[edge][0];
                uint8_t c2 = cornerIndices[edge][1];
                uint8_t c3 = cornerIndices[edge][2];
                uint8_t c4 = cornerIndices[edge][3];
                uint8_t z1 = MapGetCornerHeight(newBaseZ, newSlope, c1);
                uint8_t z2 = MapGetCornerHeight(newBaseZ, newSlope, c2);
                uint8_t z3 = MapGetCornerHeight(newBaseZ, newSlope, c3);
                uint8_t z4 = MapGetCornerHeight(newBaseZ, newSlope, c4);
                // Smooth the edge at the top of the new slope
                res.cost += SmoothLandRowByEdge(
                    gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z1, z2, stepOffsets[edge].x,
                    stepOffsets[edge].y, c3, c4, c1, c2);
                // Smooth the edge at the bottom of the new slope
                res.cost += SmoothLandRowByEdge(
                    gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z3, z4, -stepOffsets[edge].x,
                    -stepOffsets[edge].y, c1, c2, c3, c4);

                // Smooth corners
                res.cost += SmoothLandRowByCorner(
                    gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z1, -stepOffsets[edge].y,
                    stepOffsets[edge].x, c2, c1);
                res.cost += SmoothLandRowByCorner(
                    gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z2, stepOffsets[edge].y,
                    -stepOffsets[edge].x, c1, c2);
                int32_t z = MapGetCornerHeight(newBaseZ, newSlope, 2);
                res.cost += SmoothLandRowByCorner(
                    gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, -32, -32, 0, 2);
                z = MapGetCornerHeight(newBaseZ, newSlope, 0);
                res.cost += SmoothLandRowByCorner(
                    gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, 32, 32, 2, 0);
                z = MapGetCornerHeight(newBaseZ, newSlope, 3);
                res.cost += SmoothLandRowByCorner(
                    gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, -32, 32, 1, 3);
                z = MapGetCornerHeight(newBaseZ, newSlope, 1);
                res.cost += SmoothLandRowByCorner(
                    gameState, isExecuting, { validRange.GetX1(), validRange.GetY1() }, z, 32, -32, 3, 1);
                break;
            }
            default:
                LOG_ERROR("Invalid map selection type %u", _selectionType);
                return Result(Status::invalidParameters, std::get<StringId>(res.errorTitle), STR_ERR_VALUE_OUT_OF_RANGE);
        } // switch selectionType

        // Raise / lower the land tool selection area
        Result result;
        if (raiseLand)
        {
            auto raiseLandAction = LandRaiseAction({ _coords.x, _coords.y }, validRange, selectionType);
            raiseLandAction.SetFlags(GetFlags());
            result = isExecuting ? ExecuteNested(&raiseLandAction, gameState) : QueryNested(&raiseLandAction, gameState);
        }
        else
        {
            auto lowerLandAction = LandLowerAction({ _coords.x, _coords.y }, validRange, selectionType);
            lowerLandAction.SetFlags(GetFlags());
            result = isExecuting ? ExecuteNested(&lowerLandAction, gameState) : QueryNested(&lowerLandAction, gameState);
        }
        if (result.error != Status::ok)
        {
            return result;
        }

        if (isExecuting)
        {
            Audio::Play3D(Audio::SoundId::placeItem, { _coords.x, _coords.y, centreZ });
        }
        res.cost += result.cost;
        return res;
    }
} // namespace OpenRCT2::GameActions
