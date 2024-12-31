/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include "../audio/audio.h"
#include "../interface/Window.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/RideData.h"
#include "../windows/Intent.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/SurfaceData.h"
#include "../world/tile_element/Slope.h"
#include "../world/tile_element/SurfaceElement.h"

#include <algorithm>

using namespace OpenRCT2;

LandSmoothAction::LandSmoothAction(const CoordsXY& coords, MapRange range, uint8_t selectionType, bool isLowering)
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

GameActions::Result LandSmoothAction::Query() const
{
    return SmoothLand(false);
}

GameActions::Result LandSmoothAction::Execute() const
{
    return SmoothLand(true);
}

GameActions::Result LandSmoothAction::SmoothLandTile(
    int32_t direction, bool isExecuting, const CoordsXY& loc, SurfaceElement* surfaceElement) const
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
    auto res = isExecuting ? GameActions::ExecuteNested(&landSetHeightAction) : GameActions::QueryNested(&landSetHeightAction);

    return res;
}

money64 LandSmoothAction::SmoothLandRowByEdge(
    bool isExecuting, const CoordsXY& loc, int32_t expectedLandHeight1, int32_t expectedLandHeight2, int32_t stepX,
    int32_t stepY, int32_t direction1, int32_t direction2, int32_t checkDirection1, int32_t checkDirection2) const
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
        auto res = isExecuting ? GameActions::ExecuteNested(&landSetHeightAction)
                               : GameActions::QueryNested(&landSetHeightAction);
        if (res.Error == GameActions::Status::Ok)
        {
            totalCost += res.Cost;
        }
    }
    return totalCost;
}

money64 LandSmoothAction::SmoothLandRowByCorner(
    bool isExecuting, const CoordsXY& loc, int32_t expectedLandHeight, int32_t stepX, int32_t stepY, int32_t direction,
    int32_t checkDirection) const
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
    if (TileElementGetCornerHeight(surfaceElement, checkDirection) != TileElementGetCornerHeight(nextSurfaceElement, direction))
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
                isExecuting, nextLoc, expectedLandHeight + (landChangePerTile / 2), 0, stepY, direction, checkDirection ^ 3);
            totalCost += SmoothLandRowByCorner(
                isExecuting, nextLoc, expectedLandHeight + (landChangePerTile / 2), stepX, 0, direction, checkDirection ^ 1);
        }
        expectedLandHeight += landChangePerTile;
        // change land of current tile
        auto result = SmoothLandTile(direction, isExecuting, nextLoc, surfaceElement);
        if (result.Error == GameActions::Status::Ok)
        {
            totalCost += result.Cost;
        }
    }
    return totalCost;
}

GameActions::Result LandSmoothAction::SmoothLand(bool isExecuting) const
{
    const bool raiseLand = !_isLowering;
    const int32_t selectionType = _selectionType;
    const int32_t heightOffset = raiseLand ? 2 : -2;

    auto normRange = _range.Normalise();
    // Cap bounds to map
    auto l = std::max(normRange.GetLeft(), 32);
    auto t = std::max(normRange.GetTop(), 32);
    auto r = std::clamp(normRange.GetRight(), 0, MAXIMUM_TILE_START_XY);
    auto b = std::clamp(normRange.GetBottom(), 0, MAXIMUM_TILE_START_XY);
    auto validRange = MapRange{ l, t, r, b };

    int32_t centreZ = TileElementHeight(_coords);

    auto res = GameActions::Result();
    res.ErrorTitle = _ErrorTitles[_isLowering ? 0 : 1];
    res.Expenditure = ExpenditureType::Landscaping;
    res.Position = { _coords.x, _coords.y, centreZ };

    // Do the smoothing
    switch (selectionType)
    {
        case MAP_SELECT_TYPE_FULL:
        {
            uint8_t minHeight = heightOffset + MapGetLowestLandHeight(validRange);
            uint8_t maxHeight = heightOffset + MapGetHighestLandHeight(validRange);

            // Smooth the 4 corners
            { // top-left
                auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ validRange.GetLeft(), validRange.GetTop() });
                if (surfaceElement != nullptr)
                {
                    int32_t z = std::clamp(
                        static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 2)), minHeight, maxHeight);
                    res.Cost += SmoothLandRowByCorner(
                        isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, -32, -32, 0, 2);
                }
            }
            { // bottom-left
                auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ validRange.GetLeft(), validRange.GetBottom() });
                if (surfaceElement != nullptr)
                {
                    int32_t z = std::clamp(
                        static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 3)), minHeight, maxHeight);
                    res.Cost += SmoothLandRowByCorner(
                        isExecuting, { validRange.GetLeft(), validRange.GetBottom() }, z, -32, 32, 1, 3);
                }
            }
            { // bottom-right
                auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ validRange.GetRight(), validRange.GetBottom() });
                if (surfaceElement != nullptr)
                {
                    int32_t z = std::clamp(
                        static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 0)), minHeight, maxHeight);
                    res.Cost += SmoothLandRowByCorner(
                        isExecuting, { validRange.GetRight(), validRange.GetBottom() }, z, 32, 32, 2, 0);
                }
            }
            { // top-right
                auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ validRange.GetRight(), validRange.GetTop() });
                if (surfaceElement != nullptr)
                {
                    int32_t z = std::clamp(
                        static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 1)), minHeight, maxHeight);
                    res.Cost += SmoothLandRowByCorner(
                        isExecuting, { validRange.GetRight(), validRange.GetTop() }, z, 32, -32, 3, 1);
                }
            }

            // Smooth the edges
            int32_t z1, z2;
            for (int32_t y = validRange.GetTop(); y <= validRange.GetBottom(); y += kCoordsXYStep)
            {
                auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ validRange.GetLeft(), y });
                if (surfaceElement != nullptr)
                {
                    z1 = std::clamp(static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 3)), minHeight, maxHeight);
                    z2 = std::clamp(static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 2)), minHeight, maxHeight);
                    res.Cost += SmoothLandRowByEdge(isExecuting, { validRange.GetLeft(), y }, z1, z2, -32, 0, 0, 1, 3, 2);
                }

                surfaceElement = MapGetSurfaceElementAt(CoordsXY{ validRange.GetRight(), y });
                if (surfaceElement != nullptr)
                {
                    z1 = std::clamp(static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 1)), minHeight, maxHeight);
                    z2 = std::clamp(static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 0)), minHeight, maxHeight);
                    res.Cost += SmoothLandRowByEdge(isExecuting, { validRange.GetRight(), y }, z1, z2, 32, 0, 2, 3, 1, 0);
                }
            }

            for (int32_t x = validRange.GetLeft(); x <= validRange.GetRight(); x += kCoordsXYStep)
            {
                auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ x, validRange.GetTop() });
                if (surfaceElement != nullptr)
                {
                    z1 = std::clamp(static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 1)), minHeight, maxHeight);
                    z2 = std::clamp(static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 2)), minHeight, maxHeight);
                    res.Cost += SmoothLandRowByEdge(isExecuting, { x, validRange.GetTop() }, z1, z2, 0, -32, 0, 3, 1, 2);
                }

                surfaceElement = MapGetSurfaceElementAt(CoordsXY{ x, validRange.GetBottom() });
                if (surfaceElement != nullptr)
                {
                    z1 = std::clamp(static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 0)), minHeight, maxHeight);
                    z2 = std::clamp(static_cast<uint8_t>(TileElementGetCornerHeight(surfaceElement, 3)), minHeight, maxHeight);
                    res.Cost += SmoothLandRowByEdge(isExecuting, { x, validRange.GetBottom() }, z1, z2, 0, 32, 1, 2, 0, 3);
                }
            }
            break;
        }
        case MAP_SELECT_TYPE_CORNER_0:
        case MAP_SELECT_TYPE_CORNER_1:
        case MAP_SELECT_TYPE_CORNER_2:
        case MAP_SELECT_TYPE_CORNER_3:
        {
            auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ validRange.GetLeft(), validRange.GetTop() });
            if (surfaceElement == nullptr)
                break;
            uint8_t newBaseZ = surfaceElement->BaseHeight;
            uint8_t newSlope = surfaceElement->GetSlope();

            if (raiseLand)
            {
                newSlope = RaiseSurfaceCornerFlags(selectionType, newSlope);
            }
            else
            {
                newSlope = LowerSurfaceCornerFlags(selectionType, newSlope);
            }

            if (newSlope & kTileSlopeRaiseOrLowerBaseHeight)
            {
                newBaseZ += heightOffset;
                newSlope &= ~kTileSlopeRaiseOrLowerBaseHeight;
            }

            // Smooth the corners
            int32_t z = MapGetCornerHeight(newBaseZ, newSlope, 2);
            res.Cost += SmoothLandRowByCorner(isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, -32, -32, 0, 2);
            z = MapGetCornerHeight(newBaseZ, newSlope, 0);
            res.Cost += SmoothLandRowByCorner(isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, 32, 32, 2, 0);
            z = MapGetCornerHeight(newBaseZ, newSlope, 3);
            res.Cost += SmoothLandRowByCorner(isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, -32, 32, 1, 3);
            z = MapGetCornerHeight(newBaseZ, newSlope, 1);
            res.Cost += SmoothLandRowByCorner(isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, 32, -32, 3, 1);

            // Smooth the edges
            switch (selectionType)
            {
                case MAP_SELECT_TYPE_CORNER_0:
                    z = MapGetCornerHeight(newBaseZ, newSlope, 0);
                    res.Cost += SmoothLandRowByCorner(
                        isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, 32, 0, 3, 0);
                    res.Cost += SmoothLandRowByCorner(
                        isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, 0, 32, 1, 0);
                    z = MapGetCornerHeight(newBaseZ, newSlope, 3);
                    res.Cost += SmoothLandRowByCorner(
                        isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, -32, 0, 0, 3);
                    z = MapGetCornerHeight(newBaseZ, newSlope, 1);
                    res.Cost += SmoothLandRowByCorner(
                        isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, 0, -32, 0, 1);
                    break;
                case MAP_SELECT_TYPE_CORNER_1:
                    z = MapGetCornerHeight(newBaseZ, newSlope, 1);
                    res.Cost += SmoothLandRowByCorner(
                        isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, 32, 0, 2, 1);
                    res.Cost += SmoothLandRowByCorner(
                        isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, 0, -32, 0, 1);
                    z = MapGetCornerHeight(newBaseZ, newSlope, 2);
                    res.Cost += SmoothLandRowByCorner(
                        isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, -32, 0, 1, 2);
                    z = MapGetCornerHeight(newBaseZ, newSlope, 0);
                    res.Cost += SmoothLandRowByCorner(
                        isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, 0, 32, 1, 0);
                    break;
                case MAP_SELECT_TYPE_CORNER_2:
                    z = MapGetCornerHeight(newBaseZ, newSlope, 2);
                    res.Cost += SmoothLandRowByCorner(
                        isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, -32, 0, 1, 2);
                    res.Cost += SmoothLandRowByCorner(
                        isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, 0, -32, 3, 2);
                    z = MapGetCornerHeight(newBaseZ, newSlope, 1);
                    res.Cost += SmoothLandRowByCorner(
                        isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, 32, 0, 2, 1);
                    z = MapGetCornerHeight(newBaseZ, newSlope, 3);
                    res.Cost += SmoothLandRowByCorner(
                        isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, 0, 32, 2, 3);
                    break;
                case MAP_SELECT_TYPE_CORNER_3:
                    z = MapGetCornerHeight(newBaseZ, newSlope, 3);
                    res.Cost += SmoothLandRowByCorner(
                        isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, -32, 0, 0, 3);
                    res.Cost += SmoothLandRowByCorner(
                        isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, 0, 32, 2, 3);
                    z = MapGetCornerHeight(newBaseZ, newSlope, 0);
                    res.Cost += SmoothLandRowByCorner(
                        isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, 32, 0, 3, 0);
                    z = MapGetCornerHeight(newBaseZ, newSlope, 2);
                    res.Cost += SmoothLandRowByCorner(
                        isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, 0, -32, 3, 2);
                    break;
            }
            break;
        }
        case MAP_SELECT_TYPE_EDGE_0:
        case MAP_SELECT_TYPE_EDGE_1:
        case MAP_SELECT_TYPE_EDGE_2:
        case MAP_SELECT_TYPE_EDGE_3:
        {
            // TODO: Handle smoothing by edge
            // Get the two corners to raise
            auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ validRange.GetLeft(), validRange.GetTop() });
            if (surfaceElement == nullptr)
                break;
            uint8_t newBaseZ = surfaceElement->BaseHeight;
            uint8_t oldSlope = surfaceElement->GetSlope();
            int32_t rowIndex = selectionType - (MAP_SELECT_TYPE_EDGE_0 - MAP_SELECT_TYPE_FULL - 1);
            uint8_t newSlope = raiseLand ? RaiseSurfaceCornerFlags(rowIndex, oldSlope)
                                         : LowerSurfaceCornerFlags(rowIndex, oldSlope);

            const bool changeBaseHeight = newSlope & kTileSlopeRaiseOrLowerBaseHeight;
            if (changeBaseHeight)
            {
                newBaseZ += heightOffset;
                newSlope &= ~kTileSlopeRaiseOrLowerBaseHeight;
            }

            const uint8_t edge = selectionType - MAP_SELECT_TYPE_EDGE_0;

            // Table with corners for each edge selection. The first two are the selected corners, the latter
            // two are the opposites
            static constexpr uint8_t cornerIndices[][4] = {
                { 2, 3, 1, 0 }, // MAP_SELECT_TYPE_EDGE_0
                { 3, 0, 2, 1 }, // MAP_SELECT_TYPE_EDGE_1
                { 0, 1, 3, 2 }, // MAP_SELECT_TYPE_EDGE_2
                { 1, 2, 0, 3 }, // MAP_SELECT_TYPE_EDGE_3
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
            res.Cost += SmoothLandRowByEdge(
                isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z1, z2, stepOffsets[edge].x, stepOffsets[edge].y,
                c3, c4, c1, c2);
            // Smooth the edge at the bottom of the new slope
            res.Cost += SmoothLandRowByEdge(
                isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z3, z4, -stepOffsets[edge].x, -stepOffsets[edge].y,
                c1, c2, c3, c4);

            // Smooth corners
            res.Cost += SmoothLandRowByCorner(
                isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z1, -stepOffsets[edge].y, stepOffsets[edge].x, c2,
                c1);
            res.Cost += SmoothLandRowByCorner(
                isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z2, stepOffsets[edge].y, -stepOffsets[edge].x, c1,
                c2);
            int32_t z = MapGetCornerHeight(newBaseZ, newSlope, 2);
            res.Cost += SmoothLandRowByCorner(isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, -32, -32, 0, 2);
            z = MapGetCornerHeight(newBaseZ, newSlope, 0);
            res.Cost += SmoothLandRowByCorner(isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, 32, 32, 2, 0);
            z = MapGetCornerHeight(newBaseZ, newSlope, 3);
            res.Cost += SmoothLandRowByCorner(isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, -32, 32, 1, 3);
            z = MapGetCornerHeight(newBaseZ, newSlope, 1);
            res.Cost += SmoothLandRowByCorner(isExecuting, { validRange.GetLeft(), validRange.GetTop() }, z, 32, -32, 3, 1);
            break;
        }
        default:
            LOG_ERROR("Invalid map selection type %u", _selectionType);
            return GameActions::Result(
                GameActions::Status::InvalidParameters, std::get<StringId>(res.ErrorTitle), STR_ERR_VALUE_OUT_OF_RANGE);
    } // switch selectionType

    // Raise / lower the land tool selection area
    GameActions::Result result;
    if (raiseLand)
    {
        auto raiseLandAction = LandRaiseAction({ _coords.x, _coords.y }, validRange, selectionType);
        raiseLandAction.SetFlags(GetFlags());
        result = isExecuting ? GameActions::ExecuteNested(&raiseLandAction) : GameActions::QueryNested(&raiseLandAction);
    }
    else
    {
        auto lowerLandAction = LandLowerAction({ _coords.x, _coords.y }, validRange, selectionType);
        lowerLandAction.SetFlags(GetFlags());
        result = isExecuting ? GameActions::ExecuteNested(&lowerLandAction) : GameActions::QueryNested(&lowerLandAction);
    }
    if (result.Error != GameActions::Status::Ok)
    {
        return result;
    }

    if (isExecuting)
    {
        OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, { _coords.x, _coords.y, centreZ });
    }
    res.Cost += result.Cost;
    return res;
}
