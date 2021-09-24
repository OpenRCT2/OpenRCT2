/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MapHelpers.h"

#include "Map.h"
#include "Surface.h"

#include <algorithm>

static uint8_t getBaseHeightOrZero(int32_t x, int32_t y)
{
    auto surfaceElement = map_get_surface_element_at(TileCoordsXY{ x, y }.ToCoordsXY());
    return surfaceElement != nullptr ? surfaceElement->base_height : 0;
}

/**
 * Not perfect, this still leaves some particular tiles unsmoothed.
 */
int32_t map_smooth(int32_t l, int32_t t, int32_t r, int32_t b)
{
    int32_t i, x, y, count, doubleCorner, raisedLand = 0;
    uint8_t highest, cornerHeights[4];
    for (y = t; y < b; y++)
    {
        for (x = l; x < r; x++)
        {
            auto surfaceElement = map_get_surface_element_at(TileCoordsXY{ x, y }.ToCoordsXY());
            if (surfaceElement == nullptr)
                continue;
            surfaceElement->SetSlope(TILE_ELEMENT_SLOPE_FLAT);

            // Raise to edge height - 2
            highest = surfaceElement->base_height;
            highest = std::max(highest, getBaseHeightOrZero(x - 1, y + 0));
            highest = std::max(highest, getBaseHeightOrZero(x + 1, y + 0));
            highest = std::max(highest, getBaseHeightOrZero(x + 0, y - 1));
            highest = std::max(highest, getBaseHeightOrZero(x + 0, y + 1));
            if (surfaceElement->base_height < highest - 2)
            {
                raisedLand = 1;
                surfaceElement->base_height = surfaceElement->clearance_height = highest - 2;
            }

            // Check corners
            doubleCorner = -1;
            cornerHeights[0] = getBaseHeightOrZero(x - 1, y - 1);
            cornerHeights[1] = getBaseHeightOrZero(x + 1, y - 1);
            cornerHeights[2] = getBaseHeightOrZero(x + 1, y + 1);
            cornerHeights[3] = getBaseHeightOrZero(x - 1, y + 1);
            highest = surfaceElement->base_height;
            for (i = 0; i < 4; i++)
                highest = std::max(highest, cornerHeights[i]);

            if (highest >= surfaceElement->base_height + 4)
            {
                count = 0;
                int32_t canCompensate = 1;
                for (i = 0; i < 4; i++)
                    if (cornerHeights[i] == highest)
                    {
                        count++;

                        // Check if surrounding corners aren't too high. The current tile
                        // can't compensate for all the height differences anymore if it has
                        // the extra height slope.
                        int32_t highestOnLowestSide;
                        switch (i)
                        {
                            default:
                            case 0:
                                highestOnLowestSide = std::max(getBaseHeightOrZero(x + 1, y), getBaseHeightOrZero(x, y + 1));
                                break;
                            case 1:
                                highestOnLowestSide = std::max(getBaseHeightOrZero(x - 1, y), getBaseHeightOrZero(x, y + 1));
                                break;
                            case 2:
                                highestOnLowestSide = std::max(getBaseHeightOrZero(x - 1, y), getBaseHeightOrZero(x, y - 1));
                                break;
                            case 3:
                                highestOnLowestSide = std::max(getBaseHeightOrZero(x + 1, y), getBaseHeightOrZero(x, y - 1));
                                break;
                        }

                        if (highestOnLowestSide > surfaceElement->base_height)
                        {
                            surfaceElement->base_height = surfaceElement->clearance_height = highestOnLowestSide;
                            raisedLand = 1;
                            canCompensate = 0;
                        }
                    }

                if (count == 1 && canCompensate)
                {
                    if (surfaceElement->base_height < highest - 4)
                    {
                        surfaceElement->base_height = surfaceElement->clearance_height = highest - 4;
                        raisedLand = 1;
                    }
                    if (cornerHeights[0] == highest && cornerHeights[2] <= cornerHeights[0] - 4)
                        doubleCorner = 0;
                    else if (cornerHeights[1] == highest && cornerHeights[3] <= cornerHeights[1] - 4)
                        doubleCorner = 1;
                    else if (cornerHeights[2] == highest && cornerHeights[0] <= cornerHeights[2] - 4)
                        doubleCorner = 2;
                    else if (cornerHeights[3] == highest && cornerHeights[1] <= cornerHeights[3] - 4)
                        doubleCorner = 3;
                }
                else
                {
                    if (surfaceElement->base_height < highest - 2)
                    {
                        surfaceElement->base_height = surfaceElement->clearance_height = highest - 2;
                        raisedLand = 1;
                    }
                }
            }

            if (doubleCorner != -1)
            {
                uint8_t slope = surfaceElement->GetSlope() | TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT;
                switch (doubleCorner)
                {
                    case 0:
                        slope |= TILE_ELEMENT_SLOPE_N_CORNER_DN;
                        break;
                    case 1:
                        slope |= TILE_ELEMENT_SLOPE_W_CORNER_DN;
                        break;
                    case 2:
                        slope |= TILE_ELEMENT_SLOPE_S_CORNER_DN;
                        break;
                    case 3:
                        slope |= TILE_ELEMENT_SLOPE_E_CORNER_DN;
                        break;
                }
                surfaceElement->SetSlope(slope);
            }
            else
            {
                uint8_t slope = surfaceElement->GetSlope();
                // Corners
                auto surfaceElement2 = map_get_surface_element_at(TileCoordsXY{ x + 1, y + 1 }.ToCoordsXY());
                if (surfaceElement2 != nullptr && surfaceElement2->base_height > surfaceElement->base_height)
                    slope |= TILE_ELEMENT_SLOPE_N_CORNER_UP;

                surfaceElement2 = map_get_surface_element_at(TileCoordsXY{ x - 1, y + 1 }.ToCoordsXY());
                if (surfaceElement2 != nullptr && surfaceElement2->base_height > surfaceElement->base_height)
                    slope |= TILE_ELEMENT_SLOPE_W_CORNER_UP;

                surfaceElement2 = map_get_surface_element_at(TileCoordsXY{ x + 1, y - 1 }.ToCoordsXY());
                if (surfaceElement2 != nullptr && surfaceElement2->base_height > surfaceElement->base_height)
                    slope |= TILE_ELEMENT_SLOPE_E_CORNER_UP;

                surfaceElement2 = map_get_surface_element_at(TileCoordsXY{ x - 1, y - 1 }.ToCoordsXY());
                if (surfaceElement2 != nullptr && surfaceElement2->base_height > surfaceElement->base_height)
                    slope |= TILE_ELEMENT_SLOPE_S_CORNER_UP;

                // Sides
                surfaceElement2 = map_get_surface_element_at(TileCoordsXY{ x + 1, y + 0 }.ToCoordsXY());
                if (surfaceElement2 != nullptr && surfaceElement2->base_height > surfaceElement->base_height)
                    slope |= TILE_ELEMENT_SLOPE_NE_SIDE_UP;

                surfaceElement2 = map_get_surface_element_at(TileCoordsXY{ x - 1, y + 0 }.ToCoordsXY());
                if (surfaceElement2 != nullptr && surfaceElement2->base_height > surfaceElement->base_height)
                    slope |= TILE_ELEMENT_SLOPE_SW_SIDE_UP;

                surfaceElement2 = map_get_surface_element_at(TileCoordsXY{ x + 0, y - 1 }.ToCoordsXY());
                if (surfaceElement2 != nullptr && surfaceElement2->base_height > surfaceElement->base_height)
                    slope |= TILE_ELEMENT_SLOPE_SE_SIDE_UP;

                surfaceElement2 = map_get_surface_element_at(TileCoordsXY{ x + 0, y + 1 }.ToCoordsXY());
                if (surfaceElement2 != nullptr && surfaceElement2->base_height > surfaceElement->base_height)
                    slope |= TILE_ELEMENT_SLOPE_NW_SIDE_UP;

                // Raise
                if (slope == TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
                {
                    slope = TILE_ELEMENT_SLOPE_FLAT;
                    surfaceElement->base_height = surfaceElement->clearance_height += 2;
                }
                surfaceElement->SetSlope(slope);
            }
        }
    }

    return raisedLand;
}

/**
 * Raises the corners based on the height offset of neighbour tiles.
 * This does not change the base height, unless all corners have been raised.
 * @returns 0 if no edits were made, 1 otherwise
 */
int32_t tile_smooth(int32_t x, int32_t y)
{
    auto* const surfaceElement = map_get_surface_element_at(TileCoordsXY{ x, y }.ToCoordsXY());
    if (surfaceElement == nullptr)
        return 0;

    // +-----+-----+-----+
    // |  W  | NW  |  N  |
    // |  2  |  1  |  0  |
    // +-----+-----+-----+
    // | SW  |  _  | NE  |
    // |  4  |     |  3  |
    // +-----+-----+-----+
    // |  S  | SE  |  E  |
    // |  7  |  6  |  5  |
    // +-----+-----+-----+

    union
    {
        int32_t baseheight[8];
        struct
        {
            int32_t N;
            int32_t NW;
            int32_t W;
            int32_t NE;
            int32_t SW;
            int32_t E;
            int32_t SE;
            int32_t S;
        };
    } neighbourHeightOffset = {};

    // Find the neighbour base heights
    for (int32_t index = 0, y_offset = -1; y_offset <= 1; y_offset++)
    {
        for (int32_t x_offset = -1; x_offset <= 1; x_offset++)
        {
            // Skip self
            if (y_offset == 0 && x_offset == 0)
                continue;

            // Get neighbour height. If the element is not valid (outside of map) assume the same height
            auto* neighbourSurfaceElement = map_get_surface_element_at(TileCoordsXY{ x + x_offset, y + y_offset }.ToCoordsXY());
            neighbourHeightOffset.baseheight[index] = neighbourSurfaceElement != nullptr ? neighbourSurfaceElement->base_height
                                                                                         : surfaceElement->base_height;

            // Make the height relative to the current surface element
            neighbourHeightOffset.baseheight[index] -= surfaceElement->base_height;

            index++;
        }
    }

    // Count number from the three tiles that is currently higher
    int8_t thresholdW = std::clamp(neighbourHeightOffset.SW, 0, 1) + std::clamp(neighbourHeightOffset.W, 0, 1)
        + std::clamp(neighbourHeightOffset.NW, 0, 1);
    int8_t thresholdN = std::clamp(neighbourHeightOffset.NW, 0, 1) + std::clamp(neighbourHeightOffset.N, 0, 1)
        + std::clamp(neighbourHeightOffset.NE, 0, 1);
    int8_t thresholdE = std::clamp(neighbourHeightOffset.NE, 0, 1) + std::clamp(neighbourHeightOffset.E, 0, 1)
        + std::clamp(neighbourHeightOffset.SE, 0, 1);
    int8_t thresholdS = std::clamp(neighbourHeightOffset.SE, 0, 1) + std::clamp(neighbourHeightOffset.S, 0, 1)
        + std::clamp(neighbourHeightOffset.SW, 0, 1);

    uint8_t slope = TILE_ELEMENT_SLOPE_FLAT;
    slope |= (thresholdW >= 1) ? SLOPE_W_THRESHOLD_FLAGS : 0;
    slope |= (thresholdN >= 1) ? SLOPE_N_THRESHOLD_FLAGS : 0;
    slope |= (thresholdE >= 1) ? SLOPE_E_THRESHOLD_FLAGS : 0;
    slope |= (thresholdS >= 1) ? SLOPE_S_THRESHOLD_FLAGS : 0;

    // Set diagonal when three corners (one corner down) have been raised, and the middle one can be raised one more
    if ((slope == TILE_ELEMENT_SLOPE_W_CORNER_DN && neighbourHeightOffset.W >= 4)
        || (slope == TILE_ELEMENT_SLOPE_S_CORNER_DN && neighbourHeightOffset.S >= 4)
        || (slope == TILE_ELEMENT_SLOPE_E_CORNER_DN && neighbourHeightOffset.E >= 4)
        || (slope == TILE_ELEMENT_SLOPE_N_CORNER_DN && neighbourHeightOffset.N >= 4))
    {
        slope |= TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT;
    }

    // Check if the calculated slope is the same already
    uint8_t currentSlope = surfaceElement->GetSlope();
    if (currentSlope == slope)
    {
        return 0;
    }

    if ((slope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP) == TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
    {
        // All corners are raised, raise the entire tile instead.
        surfaceElement->SetSlope(TILE_ELEMENT_SLOPE_FLAT);
        surfaceElement->base_height = (surfaceElement->clearance_height += 2);
        if (surfaceElement->GetWaterHeight() <= surfaceElement->GetBaseZ())
        {
            surfaceElement->SetWaterHeight(0);
        }
    }
    else
    {
        // Apply the slope to this tile
        surfaceElement->SetSlope(slope);

        // Set correct clearance height
        if (slope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
            surfaceElement->clearance_height = surfaceElement->base_height + 4;
        else if (slope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
            surfaceElement->clearance_height = surfaceElement->base_height + 2;
    }

    return 1;
}
