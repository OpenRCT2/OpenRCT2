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
#include "../actions/LandSetHeightAction.hpp"
#include "../audio/audio.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/RideData.h"
#include "../windows/Intent.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Sprite.h"
#include "../world/Surface.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(LandSmoothAction, GAME_COMMAND_EDIT_LAND_SMOOTH, GameActionResult)
{
private:
    CoordsXY _coords;
    MapRange _range;
    uint8_t _selectionType;
    bool _isLowering;

    constexpr static rct_string_id _ErrorTitles[] = { STR_CANT_LOWER_LAND_HERE, STR_CANT_RAISE_LAND_HERE };

public:
    LandSmoothAction()
    {
    }
    LandSmoothAction(CoordsXY coords, MapRange range, uint8_t selectionType, bool isLowering)
        : _coords(coords)
        , _range(range)
        , _selectionType(selectionType)
        , _isLowering(isLowering)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_coords) << DS_TAG(_range) << DS_TAG(_selectionType) << DS_TAG(_isLowering);
    }

    GameActionResult::Ptr Query() const override
    {
        return smooth_land(false);
    }

    GameActionResult::Ptr Execute() const override
    {
        return smooth_land(true);
    }

private:
    GameActionResult::Ptr QueryExecute(bool isExecuting) const
    {
        auto res = MakeResult();

        return res;
    }

    money32 smooth_land_tile(
        int32_t direction, bool isExecuting, int32_t x, int32_t y, TileElement * tileElement) const
    {
        int32_t targetBaseZ = tileElement->base_height;
        int32_t slope = tileElement->AsSurface()->GetSlope();
        if (_isLowering)
        {
            slope = tile_element_lower_styles[direction][slope];
            if (slope & SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT)
            {
                targetBaseZ -= 2;
                slope &= ~SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT;
            }
        }
        else
        {
            slope = tile_element_raise_styles[direction][slope];
            if (slope & SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT)
            {
                targetBaseZ += 2;
                slope &= ~SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT;
            }
        }

        auto landSetHeightAction = LandSetHeightAction({ x, y }, targetBaseZ, slope);
        landSetHeightAction.SetFlags(GetFlags());
        auto res = isExecuting ? GameActions::ExecuteNested(&landSetHeightAction)
                                                     : GameActions::QueryNested(&landSetHeightAction);

        if (res->Error == GA_ERROR::OK)
        {
            return res->Cost;
        }
        else
        {
            return MONEY32_UNDEFINED;
        }
    }

    money32 smooth_land_row_by_edge(
        bool isExecuting, int32_t x, int32_t y, int32_t expectedLandHeight1, int32_t expectedLandHeight2, int32_t stepX,
        int32_t stepY, int32_t direction1, int32_t direction2, int32_t checkDirection1, int32_t checkDirection2) const
    {
        uint8_t shouldContinue = 0xF;
        int32_t landChangePerTile = _isLowering ? 2 : -2;
        TileElement *tileElement, *nextTileElement;
        money32 totalCost = 0;

        // check if we need to start at all
        if (!map_is_location_valid({ x, y }) || !map_is_location_valid({ x + stepX, y + stepY }))
        {
            return 0;
        }
        tileElement = map_get_surface_element_at({ x, y });
        nextTileElement = map_get_surface_element_at({ x + stepX, y + stepY });
        if (tileElement == nullptr || nextTileElement == nullptr)
        {
            return 0;
        }
        if (tile_element_get_corner_height(tileElement, checkDirection1) != expectedLandHeight1 + landChangePerTile)
        {
            shouldContinue &= ~0x1;
        }
        if (tile_element_get_corner_height(tileElement, checkDirection2) != expectedLandHeight2 + landChangePerTile)
        {
            shouldContinue &= ~0x2;
        }
        if (tile_element_get_corner_height(tileElement, checkDirection1)
            != tile_element_get_corner_height(nextTileElement, direction1))
        {
            shouldContinue &= ~0x1;
        }
        if (tile_element_get_corner_height(tileElement, checkDirection2)
            != tile_element_get_corner_height(nextTileElement, direction2))
        {
            shouldContinue &= ~0x2;
        }
        while ((shouldContinue & 0x3) != 0)
        {
            shouldContinue = ((shouldContinue << 2) | 0x3) & shouldContinue;
            x += stepX;
            y += stepY;
            // check if we need to continue after raising the current tile
            // this needs to be checked before the tile is changed
            if (!map_is_location_valid({ x + stepX, y + stepY }))
            {
                shouldContinue &= ~0x3;
            }
            else
            {
                tileElement = nextTileElement;
                nextTileElement = map_get_surface_element_at({ x + stepX, y + stepY });
                if (nextTileElement == nullptr)
                {
                    shouldContinue &= ~0x3;
                }
                if (tile_element_get_corner_height(tileElement, direction1) + landChangePerTile
                    != tile_element_get_corner_height(tileElement, checkDirection1))
                {
                    shouldContinue &= ~0x1;
                }
                if (tile_element_get_corner_height(tileElement, direction2) + landChangePerTile
                    != tile_element_get_corner_height(tileElement, checkDirection2))
                {
                    shouldContinue &= ~0x2;
                }
                if ((shouldContinue & 0x1)
                    && tile_element_get_corner_height(tileElement, checkDirection1)
                        != tile_element_get_corner_height(nextTileElement, direction1))
                {
                    shouldContinue &= ~0x1;
                }
                if ((shouldContinue & 0x2)
                    && tile_element_get_corner_height(tileElement, checkDirection2)
                        != tile_element_get_corner_height(nextTileElement, direction2))
                {
                    shouldContinue &= ~0x2;
                }
            }
            expectedLandHeight1 += landChangePerTile;

            // change land of current tile
            int32_t targetBaseZ = tileElement->base_height;
            int32_t slope = tileElement->AsSurface()->GetSlope();
            int32_t oldSlope = slope;
            if (_isLowering)
            {
                if (shouldContinue & 0x4)
                {
                    slope = tile_element_lower_styles[direction1][slope];
                    if (slope & SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT)
                    {
                        targetBaseZ -= 2;
                        slope &= ~SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT;
                    }
                }
                if ((shouldContinue & 0x8)
                    && map_get_corner_height(tileElement->base_height, oldSlope, direction2)
                        == map_get_corner_height(targetBaseZ, slope, direction2))
                {
                    slope = tile_element_lower_styles[direction2][slope];
                    if (slope & SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT)
                    {
                        targetBaseZ -= 2;
                        slope &= ~SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT;
                    }
                }
            }
            else
            {
                if (shouldContinue & 0x4)
                {
                    slope = tile_element_raise_styles[direction1][slope];
                    if (slope & SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT)
                    {
                        targetBaseZ += 2;
                        slope &= ~SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT;
                    }
                }
                if ((shouldContinue & 0x8)
                    && map_get_corner_height(tileElement->base_height, oldSlope, direction2)
                        == map_get_corner_height(targetBaseZ, slope, direction2))
                {
                    slope = tile_element_raise_styles[direction2][slope];
                    if (slope & SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT)
                    {
                        targetBaseZ += 2;
                        slope &= ~SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT;
                    }
                }
            }
            auto landSetHeightAction = LandSetHeightAction({ x, y }, targetBaseZ, slope);
            landSetHeightAction.SetFlags(GetFlags());
            auto res = isExecuting ? GameActions::ExecuteNested(&landSetHeightAction)
                                                         : GameActions::QueryNested(&landSetHeightAction);
            if (res->Error == GA_ERROR::OK)
            {
                totalCost += res->Cost;
            }
        }
        return totalCost;
    }

    money32 smooth_land_row_by_corner(
        bool isExecuting, int32_t x, int32_t y, int32_t expectedLandHeight, int32_t stepX, int32_t stepY, int32_t direction,
        int32_t checkDirection) const
    {
        bool shouldContinue = true;
        TileElement *tileElement, *nextTileElement;
        money32 totalCost = 0;
        money32 result;
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
        if (!map_is_location_valid({ x, y }) || !map_is_location_valid({ x + stepX, y + stepY }))
        {
            return 0;
        }
        tileElement = map_get_surface_element_at({ x, y });
        nextTileElement = map_get_surface_element_at((x + stepX) >> 5, (y + stepY) >> 5);
        if (tileElement == nullptr || nextTileElement == nullptr)
        {
            return 0;
        }
        if (tile_element_get_corner_height(tileElement, checkDirection) != expectedLandHeight + (_isLowering ? 2 : -2))
        {
            return 0;
        }
        if (tile_element_get_corner_height(tileElement, checkDirection)
            != tile_element_get_corner_height(nextTileElement, direction))
        {
            return 0;
        }
        while (shouldContinue)
        {
            x += stepX;
            y += stepY;
            // check if we need to continue after raising the current tile
            // this needs to be checked before the tile is changed
            if (!map_is_location_valid({ x + stepX, y + stepY }))
            {
                shouldContinue = false;
            }
            else
            {
                tileElement = nextTileElement;
                nextTileElement = map_get_surface_element_at((x + stepX) >> 5, (y + stepY) >> 5);
                if (nextTileElement == nullptr)
                {
                    shouldContinue = false;
                }
                if (tile_element_get_corner_height(tileElement, direction) + landChangePerTile
                    != tile_element_get_corner_height(tileElement, checkDirection))
                {
                    shouldContinue = false;
                }
                if (shouldContinue
                    && tile_element_get_corner_height(tileElement, checkDirection)
                        != tile_element_get_corner_height(nextTileElement, direction))
                {
                    shouldContinue = false;
                }
            }
            if (stepX * stepY != 0)
            {
                totalCost += smooth_land_row_by_corner(
                    isExecuting, x, y, expectedLandHeight + (landChangePerTile / 2), 0, stepY, direction, checkDirection ^ 3
                    );
                totalCost += smooth_land_row_by_corner(
                    isExecuting, x, y, expectedLandHeight + (landChangePerTile / 2), stepX, 0, direction, checkDirection ^ 1
                    );
            }
            expectedLandHeight += landChangePerTile;
            // change land of current tile
            result = smooth_land_tile(direction, isExecuting, x, y, tileElement);
            if (result != MONEY32_UNDEFINED)
            {
                totalCost += result;
            }
        }
        return totalCost;
    }

    GameActionResult::Ptr smooth_land(bool isExecuting) const
    {
        // break up information in command
        const bool raiseLand = !_isLowering;
        const int32_t selectionType = _selectionType;
        const int32_t heightOffset = raiseLand ? 2 : -2;

        auto normRange = _range.Normalise();
        // Cap bounds to map
        auto l = std::max(normRange.GetLeft(), 32);
        auto t = std::max(normRange.GetTop(), 32);
        auto r = std::clamp(normRange.GetRight(), 0, (MAXIMUM_MAP_SIZE_TECHNICAL - 1) * 32);
        auto b = std::clamp(normRange.GetBottom(), 0, (MAXIMUM_MAP_SIZE_TECHNICAL - 1) * 32);
        auto validRange = MapRange{ l, t, r, b };

        // Play sound (only once)
        int32_t centreZ = tile_element_height(_coords.x, _coords.y) & 0xFFFF;


        auto res = MakeResult();
        res->ErrorTitle = _ErrorTitles[_isLowering ? 0 : 1];
        res->Position = { _coords.x, _coords.y, centreZ };

        // Do the smoothing
        switch (selectionType)
        {
            case MAP_SELECT_TYPE_FULL:
            {
                uint8_t minHeight = heightOffset + map_get_lowest_land_height(validRange.GetLeft(), validRange.GetRight(), validRange.GetTop(), validRange.GetBottom());
                uint8_t maxHeight = heightOffset
                    + map_get_highest_land_height(
                                        validRange.GetLeft(), validRange.GetRight(), validRange.GetTop(),
                                        validRange.GetBottom());

                // Smooth the 4 corners
                { // top-left
                    TileElement* tileElement = map_get_surface_element_at({ validRange.GetLeft(), validRange.GetTop() });
                    int32_t z = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 2), minHeight, maxHeight);
                    res->Cost += smooth_land_row_by_corner(
                        isExecuting, validRange.GetLeft(), validRange.GetTop(), z, -32, -32, 0, 2);
                }
                { // bottom-left
                    TileElement* tileElement = map_get_surface_element_at(
                        { validRange.GetLeft(), validRange.GetBottom() });
                    int32_t z = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 3), minHeight, maxHeight);
                    res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetBottom(), z, -32, 32, 1, 3);
                }
                { // bottom-right
                    TileElement* tileElement = map_get_surface_element_at(
                        { validRange.GetRight(), validRange.GetBottom() } );
                    int32_t z = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 0), minHeight, maxHeight);
                    res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetRight(), validRange.GetBottom(), z, 32, 32, 2, 0);
                }
                { // top-right
                    TileElement* tileElement = map_get_surface_element_at({ validRange.GetRight(), validRange.GetTop() });
                    int32_t z = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 1), minHeight, maxHeight);
                    res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetRight(), validRange.GetTop(), z, 32, -32, 3, 1);
                }

                // Smooth the edges
                TileElement* tileElement = nullptr;
                int32_t z1, z2;
                for (int32_t y = validRange.GetTop(); y <= validRange.GetBottom(); y += 32)
                {
                    tileElement = map_get_surface_element_at({ validRange.GetLeft(), y });
                    z1 = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 3), minHeight, maxHeight);
                    z2 = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 2), minHeight, maxHeight);
                    res->Cost += smooth_land_row_by_edge(isExecuting, validRange.GetLeft(), y, z1, z2, -32, 0, 0, 1, 3, 2);

                    tileElement = map_get_surface_element_at({ validRange.GetRight(), y });
                    z1 = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 1), minHeight, maxHeight);
                    z2 = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 0), minHeight, maxHeight);
                    res->Cost += smooth_land_row_by_edge(isExecuting, validRange.GetRight(), y, z1, z2, 32, 0, 2, 3, 1, 0);
                }

                for (int32_t x = validRange.GetLeft(); x <= validRange.GetRight(); x += 32)
                {
                    tileElement = map_get_surface_element_at({ x, validRange.GetTop() });
                    z1 = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 1), minHeight, maxHeight);
                    z2 = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 2), minHeight, maxHeight);
                    res->Cost += smooth_land_row_by_edge(isExecuting, x, validRange.GetTop(), z1, z2, 0, -32, 0, 3, 1, 2);

                    tileElement = map_get_surface_element_at({ x, validRange.GetBottom() });
                    z1 = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 0), minHeight, maxHeight);
                    z2 = std::clamp((uint8_t)tile_element_get_corner_height(tileElement, 3), minHeight, maxHeight);
                    res->Cost += smooth_land_row_by_edge(isExecuting, x, validRange.GetBottom(), z1, z2, 0, 32, 1, 2, 0, 3);
                }
                break;
            }
            case MAP_SELECT_TYPE_CORNER_0:
            case MAP_SELECT_TYPE_CORNER_1:
            case MAP_SELECT_TYPE_CORNER_2:
            case MAP_SELECT_TYPE_CORNER_3:
            {
                TileElement* tileElement = map_get_surface_element_at({ validRange.GetLeft(), validRange.GetTop() });
                uint8_t newBaseZ = tileElement->base_height;
                uint8_t newSlope = tileElement->AsSurface()->GetSlope();

                if (raiseLand)
                {
                    newSlope = tile_element_raise_styles[selectionType][newSlope];
                }
                else
                {
                    newSlope = tile_element_lower_styles[selectionType][newSlope];
                }

                if (newSlope & SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT)
                {
                    newBaseZ += heightOffset;
                    newSlope &= ~SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT;
                }

                // Smooth the corners
                int32_t z = map_get_corner_height(newBaseZ, newSlope, 2);
                res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, -32, -32, 0, 2);
                z = map_get_corner_height(newBaseZ, newSlope, 0);
                res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, 32, 32, 2, 0);
                z = map_get_corner_height(newBaseZ, newSlope, 3);
                res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, -32, 32, 1, 3);
                z = map_get_corner_height(newBaseZ, newSlope, 1);
                res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, 32, -32, 3, 1);

                // Smooth the edges
                switch (selectionType)
                {
                    case MAP_SELECT_TYPE_CORNER_0:
                        z = map_get_corner_height(newBaseZ, newSlope, 0);
                        res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, 32, 0, 3, 0);
                        res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, 0, 32, 1, 0);
                        z = map_get_corner_height(newBaseZ, newSlope, 3);
                        res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, -32, 0, 0, 3);
                        z = map_get_corner_height(newBaseZ, newSlope, 1);
                        res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, 0, -32, 0, 1);
                        break;
                    case MAP_SELECT_TYPE_CORNER_1:
                        z = map_get_corner_height(newBaseZ, newSlope, 1);
                        res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, 32, 0, 2, 1);
                        res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, 0, -32, 0, 1);
                        z = map_get_corner_height(newBaseZ, newSlope, 2);
                        res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, -32, 0, 1, 2);
                        z = map_get_corner_height(newBaseZ, newSlope, 0);
                        res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, 0, 32, 1, 0);
                        break;
                    case MAP_SELECT_TYPE_CORNER_2:
                        z = map_get_corner_height(newBaseZ, newSlope, 2);
                        res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, -32, 0, 1, 2);
                        res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, 0, -32, 3, 2);
                        z = map_get_corner_height(newBaseZ, newSlope, 1);
                        res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, 32, 0, 2, 1);
                        z = map_get_corner_height(newBaseZ, newSlope, 3);
                        res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, 0, 32, 2, 3);
                        break;
                    case MAP_SELECT_TYPE_CORNER_3:
                        z = map_get_corner_height(newBaseZ, newSlope, 3);
                        res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, -32, 0, 0, 3);
                        res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, 0, 32, 2, 3);
                        z = map_get_corner_height(newBaseZ, newSlope, 0);
                        res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, 32, 0, 3, 0);
                        z = map_get_corner_height(newBaseZ, newSlope, 2);
                        res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, 0, -32, 3, 2);
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
                TileElement* surfaceElement = map_get_surface_element_at({ validRange.GetLeft(), validRange.GetTop() });
                uint8_t newBaseZ = surfaceElement->base_height;
                uint8_t oldSlope = surfaceElement->AsSurface()->GetSlope();
                uint8_t newSlope = oldSlope;
                int32_t rowIndex = selectionType - (MAP_SELECT_TYPE_EDGE_0 - MAP_SELECT_TYPE_FULL - 1);

                if (raiseLand)
                {
                    newSlope = tile_element_raise_styles[rowIndex][oldSlope];
                }
                else
                {
                    newSlope = tile_element_lower_styles[rowIndex][oldSlope];
                }

                const bool changeBaseHeight = newSlope & SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT;
                if (changeBaseHeight)
                {
                    newBaseZ += heightOffset;
                    newSlope &= ~SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT;
                }

                const uint8_t edge = selectionType - MAP_SELECT_TYPE_EDGE_0;

                // Table with corners for each edge selection. The first two are the selected corners, the latter two are the
                // opposites
                static constexpr uint8_t cornerIndices[][4] = {
                    { 2, 3, 1, 0 }, // MAP_SELECT_TYPE_EDGE_0
                    { 3, 0, 2, 1 }, // MAP_SELECT_TYPE_EDGE_1
                    { 0, 1, 3, 2 }, // MAP_SELECT_TYPE_EDGE_2
                    { 1, 2, 0, 3 }, // MAP_SELECT_TYPE_EDGE_3
                };
                // Big coordinate offsets for the neigbouring tile for the given edge selection
                static constexpr sLocationXY8 stepOffsets[] = {
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
                uint8_t z1 = map_get_corner_height(newBaseZ, newSlope, c1);
                uint8_t z2 = map_get_corner_height(newBaseZ, newSlope, c2);
                uint8_t z3 = map_get_corner_height(newBaseZ, newSlope, c3);
                uint8_t z4 = map_get_corner_height(newBaseZ, newSlope, c4);
                // Smooth the edge at the top of the new slope
                res->Cost += smooth_land_row_by_edge(
                    isExecuting, validRange.GetLeft(), validRange.GetTop(), z1, z2, stepOffsets[edge].x, stepOffsets[edge].y, c3, c4, c1, c2);
                // Smooth the edge at the bottom of the new slope
                res->Cost += smooth_land_row_by_edge(
                    isExecuting, validRange.GetLeft(), validRange.GetTop(), z3, z4, -stepOffsets[edge].x, -stepOffsets[edge].y, c1, c2, c3, c4);

                // Smooth corners
                res->Cost += smooth_land_row_by_corner(
                    isExecuting, validRange.GetLeft(), validRange.GetTop(), z1, -stepOffsets[edge].y, stepOffsets[edge].x, c2, c1);
                res->Cost += smooth_land_row_by_corner(
                    isExecuting, validRange.GetLeft(), validRange.GetTop(), z2, stepOffsets[edge].y, -stepOffsets[edge].x, c1, c2);
                int32_t z = map_get_corner_height(newBaseZ, newSlope, 2);
                res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, -32, -32, 0, 2);
                z = map_get_corner_height(newBaseZ, newSlope, 0);
                res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, 32, 32, 2, 0);
                z = map_get_corner_height(newBaseZ, newSlope, 3);
                res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, -32, 32, 1, 3);
                z = map_get_corner_height(newBaseZ, newSlope, 1);
                res->Cost += smooth_land_row_by_corner(isExecuting, validRange.GetLeft(), validRange.GetTop(), z, 32, -32, 3, 1);
                break;
            }
        } // switch selectionType

        // Raise / lower the land tool selection area
        GameActionResult::Ptr result;
        if (raiseLand)
        {
            auto raiseLandAction = LandRaiseAction(
                { _coords.x, _coords.y }, validRange, selectionType);
            raiseLandAction.SetFlags(GetFlags());
            result = isExecuting ? GameActions::ExecuteNested(&raiseLandAction)
                                                  : GameActions::QueryNested(&raiseLandAction);
        }
        else
        {
            auto lowerLandAction = LandLowerAction({ _coords.x, _coords.y }, validRange, selectionType);
            lowerLandAction.SetFlags(GetFlags());
            result = isExecuting ? GameActions::ExecuteNested(&lowerLandAction)
                                                  : GameActions::QueryNested(&lowerLandAction);
        }
        if (result->Error != GA_ERROR::OK)
        {
            return result;
        }

        if (isExecuting)
        {
            audio_play_sound_at_location(SOUND_PLACE_ITEM, _coords.x, _coords.y, centreZ);
        }
        res->Cost += res->Cost;

        res->ExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;

        return res;
    }
};
