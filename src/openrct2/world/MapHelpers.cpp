/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include "../core/Math.hpp"
#include "Map.h"
#include "MapHelpers.h"
#include "Surface.h"

/**
 * Not perfect, this still leaves some particular tiles unsmoothed.
 */
int32_t map_smooth(int32_t l, int32_t t, int32_t r, int32_t b)
{
    int32_t i, x, y, count, doubleCorner, raisedLand = 0;
    uint8_t highest, cornerHeights[4];
    rct_tile_element *tileElement, *tileElement2;
    for (y = t; y < b; y++) {
        for (x = l; x < r; x++) {
            tileElement = map_get_surface_element_at(x, y);
            tileElement->properties.surface.slope &= ~TILE_ELEMENT_SURFACE_SLOPE_MASK;

            // Raise to edge height - 2
            highest = tileElement->base_height;
            highest = std::max(highest, map_get_surface_element_at(x - 1, y + 0)->base_height);
            highest = std::max(highest, map_get_surface_element_at(x + 1, y + 0)->base_height);
            highest = std::max(highest, map_get_surface_element_at(x + 0, y - 1)->base_height);
            highest = std::max(highest, map_get_surface_element_at(x + 0, y + 1)->base_height);
            if (tileElement->base_height < highest - 2) {
                raisedLand = 1;
                tileElement->base_height = tileElement->clearance_height = highest - 2;
            }

            // Check corners
            doubleCorner = -1;
            cornerHeights[0] = map_get_surface_element_at(x - 1, y - 1)->base_height;
            cornerHeights[1] = map_get_surface_element_at(x + 1, y - 1)->base_height;
            cornerHeights[2] = map_get_surface_element_at(x + 1, y + 1)->base_height;
            cornerHeights[3] = map_get_surface_element_at(x - 1, y + 1)->base_height;
            highest = tileElement->base_height;
            for (i = 0; i < 4; i++)
                highest = std::max(highest, cornerHeights[i]);

            if (highest >= tileElement->base_height + 4) {
                count = 0;
                int32_t canCompensate = 1;
                for (i = 0; i < 4; i++)
                    if (cornerHeights[i] == highest){
                        count++;

                        // Check if surrounding corners aren't too high. The current tile
                        // can't compensate for all the height differences anymore if it has
                        // the extra height slope.
                        int32_t highestOnLowestSide;
                        switch (i){
                        default:
                        case 0:
                            highestOnLowestSide = std::max(
                                map_get_surface_element_at(x + 1, y)->base_height,
                                map_get_surface_element_at(x, y + 1)->base_height);
                            break;
                        case 1:
                            highestOnLowestSide = std::max(
                                map_get_surface_element_at(x - 1, y)->base_height,
                                map_get_surface_element_at(x, y + 1)->base_height);
                            break;
                        case 2:
                            highestOnLowestSide = std::max(
                                map_get_surface_element_at(x - 1, y)->base_height,
                                map_get_surface_element_at(x, y - 1)->base_height);
                            break;
                        case 3:
                            highestOnLowestSide = std::max(
                                map_get_surface_element_at(x + 1, y)->base_height,
                                map_get_surface_element_at(x, y - 1)->base_height);
                            break;
                        }

                        if (highestOnLowestSide > tileElement->base_height){
                            tileElement->base_height = tileElement->clearance_height = highestOnLowestSide;
                            raisedLand = 1;
                            canCompensate = 0;
                        }
                    }

                if (count == 1 && canCompensate) {
                    if (tileElement->base_height < highest - 4) {
                        tileElement->base_height = tileElement->clearance_height = highest - 4;
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
                } else {
                    if (tileElement->base_height < highest - 2) {
                        tileElement->base_height = tileElement->clearance_height = highest - 2;
                        raisedLand = 1;
                    }
                }
            }

            if (doubleCorner != -1) {
                tileElement->properties.surface.slope |= TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT;
                switch (doubleCorner) {
                case 0:
                    tileElement->properties.surface.slope |= TILE_ELEMENT_SLOPE_N_CORNER_DN;
                    break;
                case 1:
                    tileElement->properties.surface.slope |= TILE_ELEMENT_SLOPE_W_CORNER_DN;
                    break;
                case 2:
                    tileElement->properties.surface.slope |= TILE_ELEMENT_SLOPE_S_CORNER_DN;
                    break;
                case 3:
                    tileElement->properties.surface.slope |= TILE_ELEMENT_SLOPE_E_CORNER_DN;
                    break;
                }
            } else {
                // Corners
                tileElement2 = map_get_surface_element_at(x + 1, y + 1);
                if (tileElement2->base_height > tileElement->base_height)
                    tileElement->properties.surface.slope |= TILE_ELEMENT_SLOPE_N_CORNER_UP;

                tileElement2 = map_get_surface_element_at(x - 1, y + 1);
                if (tileElement2->base_height > tileElement->base_height)
                    tileElement->properties.surface.slope |= TILE_ELEMENT_SLOPE_W_CORNER_UP;

                tileElement2 = map_get_surface_element_at(x + 1, y - 1);
                if (tileElement2->base_height > tileElement->base_height)
                    tileElement->properties.surface.slope |= TILE_ELEMENT_SLOPE_E_CORNER_UP;

                tileElement2 = map_get_surface_element_at(x - 1, y - 1);
                if (tileElement2->base_height > tileElement->base_height)
                    tileElement->properties.surface.slope |= TILE_ELEMENT_SLOPE_S_CORNER_UP;

                // Sides
                tileElement2 = map_get_surface_element_at(x + 1, y + 0);
                if (tileElement2->base_height > tileElement->base_height)
                    tileElement->properties.surface.slope |= TILE_ELEMENT_SLOPE_NE_SIDE_UP;

                tileElement2 = map_get_surface_element_at(x - 1, y + 0);
                if (tileElement2->base_height > tileElement->base_height)
                    tileElement->properties.surface.slope |= TILE_ELEMENT_SLOPE_SW_SIDE_UP;

                tileElement2 = map_get_surface_element_at(x + 0, y - 1);
                if (tileElement2->base_height > tileElement->base_height)
                    tileElement->properties.surface.slope |= TILE_ELEMENT_SLOPE_SE_SIDE_UP;

                tileElement2 = map_get_surface_element_at(x + 0, y + 1);
                if (tileElement2->base_height > tileElement->base_height)
                    tileElement->properties.surface.slope |= TILE_ELEMENT_SLOPE_NW_SIDE_UP;

                // Raise
                if (tileElement->properties.surface.slope == TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
                {
                    tileElement->properties.surface.slope &= ~TILE_ELEMENT_SURFACE_SLOPE_MASK;
                    tileElement->base_height = tileElement->clearance_height += 2;
                }
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
    rct_tile_element *const surfaceElement = map_get_surface_element_at(x, y);

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
        struct { 
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
            rct_tile_element *neighbour_element = map_get_surface_element_at(x + x_offset, y + y_offset);
            neighbourHeightOffset.baseheight[index] = neighbour_element ? neighbour_element->base_height : surfaceElement->base_height;

            // Make the height relative to the current surface element
            neighbourHeightOffset.baseheight[index] -= surfaceElement->base_height;

            index++;
        }
    }

    // Count number from the three tiles that is currently higher
    int8_t thresholdW = Math::Clamp(0, neighbourHeightOffset.SW, 1) + Math::Clamp(0, neighbourHeightOffset.W, 1) + Math::Clamp(0, neighbourHeightOffset.NW, 1);
    int8_t thresholdN = Math::Clamp(0, neighbourHeightOffset.NW, 1) + Math::Clamp(0, neighbourHeightOffset.N, 1) + Math::Clamp(0, neighbourHeightOffset.NE, 1);
    int8_t thresholdE = Math::Clamp(0, neighbourHeightOffset.NE, 1) + Math::Clamp(0, neighbourHeightOffset.E, 1) + Math::Clamp(0, neighbourHeightOffset.SE, 1);
    int8_t thresholdS = Math::Clamp(0, neighbourHeightOffset.SE, 1) + Math::Clamp(0, neighbourHeightOffset.S, 1) + Math::Clamp(0, neighbourHeightOffset.SW, 1);

    uint8_t slope = TILE_ELEMENT_SLOPE_FLAT;
    slope |= (thresholdW >= 1) ? SLOPE_W_THRESHOLD_FLAGS : 0;
    slope |= (thresholdN >= 1) ? SLOPE_N_THRESHOLD_FLAGS : 0;
    slope |= (thresholdE >= 1) ? SLOPE_E_THRESHOLD_FLAGS : 0;
    slope |= (thresholdS >= 1) ? SLOPE_S_THRESHOLD_FLAGS : 0;

    // Set diagonal when three corners (one corner down) have been raised, and the middle one can be raised one more
    if ((slope == TILE_ELEMENT_SLOPE_W_CORNER_DN && neighbourHeightOffset.W >= 4) ||
        (slope == TILE_ELEMENT_SLOPE_S_CORNER_DN && neighbourHeightOffset.S >= 4) ||
        (slope == TILE_ELEMENT_SLOPE_E_CORNER_DN && neighbourHeightOffset.E >= 4) ||
        (slope == TILE_ELEMENT_SLOPE_N_CORNER_DN && neighbourHeightOffset.N >= 4))
    {
        slope |= TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT;
    }

    // Check if the calculated slope is the same already
    uint8_t currentSlope = surfaceElement->properties.surface.slope & TILE_ELEMENT_SURFACE_SLOPE_MASK;
    if (currentSlope == slope)
    {
        return 0;
    }

    // Remove old slope value
    surfaceElement->properties.surface.slope &= ~TILE_ELEMENT_SURFACE_SLOPE_MASK;
    if ((slope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP) == TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
    {
        // All corners are raised, raise the entire tile instead.
        surfaceElement->base_height = (surfaceElement->clearance_height += 2);
        uint8_t waterHeight = surface_get_water_height(surfaceElement) * 2;
        if (waterHeight <= surfaceElement->base_height)
        {
            surfaceElement->properties.surface.terrain &= ~TILE_ELEMENT_SURFACE_WATER_HEIGHT_MASK;
        }
    }
    else
    {
        // Apply the slope to this tile
        surfaceElement->properties.surface.slope |= slope;

        // Set correct clearance height
        if (slope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
            surfaceElement->clearance_height = surfaceElement->base_height + 4;
        else if (slope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
            surfaceElement->clearance_height = surfaceElement->base_height + 2;
    }

    return 1;
}
