/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MapHelpers.h"

#include "../../world/tile_element/Slope.h"
#include "../../world/tile_element/SurfaceElement.h"
#include "../Map.h"

#include <algorithm>

namespace OpenRCT2::World::MapGenerator
{
    static uint8_t GetBaseHeightOrZero(int32_t x, int32_t y)
    {
        auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
        return surfaceElement != nullptr ? surfaceElement->BaseHeight : 0;
    }

    /**
     * Not perfect, this still leaves some particular tiles unsmoothed.
     */
    int32_t smoothTileStrong(const TileCoordsXY pos)
    {
        auto surfaceElement = MapGetSurfaceElementAt(pos);
        if (surfaceElement == nullptr)
            return 0;

        surfaceElement->SetSlope(kTileSlopeFlat);

        auto x = pos.x, y = pos.y;
        auto raisedLand = 0;

        // Raise to edge height - 2
        uint8_t highest = surfaceElement->BaseHeight;
        highest = std::max(highest, GetBaseHeightOrZero(x - 1, y + 0));
        highest = std::max(highest, GetBaseHeightOrZero(x + 1, y + 0));
        highest = std::max(highest, GetBaseHeightOrZero(x + 0, y - 1));
        highest = std::max(highest, GetBaseHeightOrZero(x + 0, y + 1));
        if (surfaceElement->BaseHeight < highest - 2)
        {
            raisedLand = 1;
            surfaceElement->BaseHeight = surfaceElement->ClearanceHeight = highest - 2;
        }

        // Check corners
        int32_t doubleCorner = -1;
        uint8_t cornerHeights[4] = {
            GetBaseHeightOrZero(x - 1, y - 1),
            GetBaseHeightOrZero(x + 1, y - 1),
            GetBaseHeightOrZero(x + 1, y + 1),
            GetBaseHeightOrZero(x - 1, y + 1),
        };

        highest = surfaceElement->BaseHeight;
        for (std::size_t i = 0; i < std::size(cornerHeights); i++)
            highest = std::max(highest, cornerHeights[i]);

        if (highest >= surfaceElement->BaseHeight + 4)
        {
            int32_t count = 0;
            int32_t canCompensate = 1;
            for (std::size_t i = 0; i < std::size(cornerHeights); i++)
            {
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
                            highestOnLowestSide = std::max(GetBaseHeightOrZero(x + 1, y), GetBaseHeightOrZero(x, y + 1));
                            break;
                        case 1:
                            highestOnLowestSide = std::max(GetBaseHeightOrZero(x - 1, y), GetBaseHeightOrZero(x, y + 1));
                            break;
                        case 2:
                            highestOnLowestSide = std::max(GetBaseHeightOrZero(x - 1, y), GetBaseHeightOrZero(x, y - 1));
                            break;
                        case 3:
                            highestOnLowestSide = std::max(GetBaseHeightOrZero(x + 1, y), GetBaseHeightOrZero(x, y - 1));
                            break;
                    }

                    if (highestOnLowestSide > surfaceElement->BaseHeight)
                    {
                        surfaceElement->BaseHeight = surfaceElement->ClearanceHeight = highestOnLowestSide;
                        raisedLand = 1;
                        canCompensate = 0;
                    }
                }
            }

            if (count == 1 && canCompensate)
            {
                if (surfaceElement->BaseHeight < highest - 4)
                {
                    surfaceElement->BaseHeight = surfaceElement->ClearanceHeight = highest - 4;
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
                if (surfaceElement->BaseHeight < highest - 2)
                {
                    surfaceElement->BaseHeight = surfaceElement->ClearanceHeight = highest - 2;
                    raisedLand = 1;
                }
            }
        }

        if (doubleCorner != -1)
        {
            uint8_t slope = surfaceElement->GetSlope() | kTileSlopeDiagonalFlag;
            switch (doubleCorner)
            {
                case 0:
                    slope |= kTileSlopeNCornerDown;
                    break;
                case 1:
                    slope |= kTileSlopeWCornerDown;
                    break;
                case 2:
                    slope |= kTileSlopeSCornerDown;
                    break;
                case 3:
                    slope |= kTileSlopeECornerDown;
                    break;
            }
            surfaceElement->SetSlope(slope);
        }
        else
        {
            uint8_t slope = surfaceElement->GetSlope();
            // Corners
            auto surfaceElement2 = MapGetSurfaceElementAt(TileCoordsXY{ x + 1, y + 1 });
            if (surfaceElement2 != nullptr && surfaceElement2->BaseHeight > surfaceElement->BaseHeight)
                slope |= kTileSlopeNCornerUp;

            surfaceElement2 = MapGetSurfaceElementAt(TileCoordsXY{ x - 1, y + 1 });
            if (surfaceElement2 != nullptr && surfaceElement2->BaseHeight > surfaceElement->BaseHeight)
                slope |= kTileSlopeWCornerUp;

            surfaceElement2 = MapGetSurfaceElementAt(TileCoordsXY{ x + 1, y - 1 });
            if (surfaceElement2 != nullptr && surfaceElement2->BaseHeight > surfaceElement->BaseHeight)
                slope |= kTileSlopeECornerUp;

            surfaceElement2 = MapGetSurfaceElementAt(TileCoordsXY{ x - 1, y - 1 });
            if (surfaceElement2 != nullptr && surfaceElement2->BaseHeight > surfaceElement->BaseHeight)
                slope |= kTileSlopeSCornerUp;

            // Sides
            surfaceElement2 = MapGetSurfaceElementAt(TileCoordsXY{ x + 1, y + 0 });
            if (surfaceElement2 != nullptr && surfaceElement2->BaseHeight > surfaceElement->BaseHeight)
                slope |= kTileSlopeNESideUp;

            surfaceElement2 = MapGetSurfaceElementAt(TileCoordsXY{ x - 1, y + 0 });
            if (surfaceElement2 != nullptr && surfaceElement2->BaseHeight > surfaceElement->BaseHeight)
                slope |= kTileSlopeSWSideUp;

            surfaceElement2 = MapGetSurfaceElementAt(TileCoordsXY{ x + 0, y - 1 });
            if (surfaceElement2 != nullptr && surfaceElement2->BaseHeight > surfaceElement->BaseHeight)
                slope |= kTileSlopeSESideUp;

            surfaceElement2 = MapGetSurfaceElementAt(TileCoordsXY{ x + 0, y + 1 });
            if (surfaceElement2 != nullptr && surfaceElement2->BaseHeight > surfaceElement->BaseHeight)
                slope |= kTileSlopeNWSideUp;

            // Raise
            if (slope == kTileSlopeRaisedCornersMask)
            {
                slope = kTileSlopeFlat;
                surfaceElement->BaseHeight = surfaceElement->ClearanceHeight += 2;
            }
            surfaceElement->SetSlope(slope);
        }

        return raisedLand;
    }

    /**
     * Raises the corners based on the height offset of neighbour tiles.
     * This does not change the base height, unless all corners have been raised.
     * @returns 0 if no edits were made, 1 otherwise
     */
    int32_t smoothTileWeak(const TileCoordsXY tileCoords)
    {
        auto* const surfaceElement = MapGetSurfaceElementAt(tileCoords);
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
                auto* neighbourSurfaceElement = MapGetSurfaceElementAt(tileCoords + TileCoordsXY{ x_offset, y_offset });
                neighbourHeightOffset.baseheight[index] = neighbourSurfaceElement != nullptr
                    ? neighbourSurfaceElement->BaseHeight
                    : surfaceElement->BaseHeight;

                // Make the height relative to the current surface element
                neighbourHeightOffset.baseheight[index] -= surfaceElement->BaseHeight;

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

        uint8_t slope = kTileSlopeFlat;
        slope |= (thresholdW >= 1) ? SLOPE_W_THRESHOLD_FLAGS : 0;
        slope |= (thresholdN >= 1) ? SLOPE_N_THRESHOLD_FLAGS : 0;
        slope |= (thresholdE >= 1) ? SLOPE_E_THRESHOLD_FLAGS : 0;
        slope |= (thresholdS >= 1) ? SLOPE_S_THRESHOLD_FLAGS : 0;

        // Set diagonal when three corners (one corner down) have been raised, and the middle one can be raised one more
        if ((slope == kTileSlopeWCornerDown && neighbourHeightOffset.W >= 4)
            || (slope == kTileSlopeSCornerDown && neighbourHeightOffset.S >= 4)
            || (slope == kTileSlopeECornerDown && neighbourHeightOffset.E >= 4)
            || (slope == kTileSlopeNCornerDown && neighbourHeightOffset.N >= 4))
        {
            slope |= kTileSlopeDiagonalFlag;
        }

        // Check if the calculated slope is the same already
        uint8_t currentSlope = surfaceElement->GetSlope();
        if (currentSlope == slope)
        {
            return 0;
        }

        if ((slope & kTileSlopeRaisedCornersMask) == kTileSlopeRaisedCornersMask)
        {
            // All corners are raised, raise the entire tile instead.
            surfaceElement->SetSlope(kTileSlopeFlat);
            surfaceElement->BaseHeight = (surfaceElement->ClearanceHeight += 2);
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
            if (slope & kTileSlopeDiagonalFlag)
                surfaceElement->ClearanceHeight = surfaceElement->BaseHeight + 4;
            else if (slope & kTileSlopeRaisedCornersMask)
                surfaceElement->ClearanceHeight = surfaceElement->BaseHeight + 2;
        }

        return 1;
    }

    void smoothMap(TileCoordsXY mapSize, SmoothFunction smoothFunc)
    {
        int numIterations = 0;
        while (true)
        {
            auto numTilesChanged = 0;

            for (auto y = 1; y < mapSize.y - 1; y++)
            {
                for (auto x = 1; x < mapSize.x - 1; x++)
                {
                    numTilesChanged += smoothFunc({ x, y });
                }
            }

            if (numTilesChanged == 0 || numIterations == 10)
                break;

            numIterations++;
        }
    }
} // namespace OpenRCT2::World::MapGenerator
