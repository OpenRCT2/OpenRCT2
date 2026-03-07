/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FootpathDragSlope.h"

#include "Footpath.h"
#include "Map.h"
#include "tile_element/Slope.h"
#include "tile_element/SurfaceElement.h"

#include <algorithm>
#include <vector>

namespace OpenRCT2
{
    // Terrain height info for a row of tiles along the primary drag axis
    struct SliceInfo
    {
        int32_t primaryCoord;
        int32_t baseZ;
        int32_t maxZ;
    };

    // Get surface height including raised corners and steep slopes
    static int32_t getMaxSurfaceHeight(const SurfaceElement& surface)
    {
        int32_t z = surface.GetBaseZ();
        uint8_t slope = surface.GetSlope();
        if (slope & kTileSlopeRaisedCornersMask)
            z += kPathHeightStep;
        if (slope & kTileSlopeDiagonalFlag)
            z += kPathHeightStep;
        return z;
    }

    std::vector<FootpathDragPlacement> calculateConnectedPathSlopes(MapRange range, CoordsXY dragStart)
    {
        std::vector<FootpathDragPlacement> result;

        int32_t dragWidth = range.GetX2() - range.GetX1();
        int32_t dragHeight = range.GetY2() - range.GetY1();
        CoordsXY startPos = dragStart.ToTileStart();

        startPos.x = std::clamp(startPos.x, range.GetX1(), range.GetX2());
        startPos.y = std::clamp(startPos.y, range.GetY1(), range.GetY2());

        bool primaryIsX = dragWidth >= dragHeight;
        int32_t primaryStart, primaryEnd, primaryStep;
        int32_t secondaryStart, secondaryEnd;
        Direction slopeDirection;

        if (primaryIsX)
        {
            secondaryStart = range.GetY1();
            secondaryEnd = range.GetY2();

            if (startPos.x <= (range.GetX1() + range.GetX2()) / 2)
            {
                primaryStart = range.GetX1();
                primaryEnd = range.GetX2();
                primaryStep = kCoordsXYStep;
                slopeDirection = TILE_ELEMENT_DIRECTION_EAST;
            }
            else
            {
                primaryStart = range.GetX2();
                primaryEnd = range.GetX1();
                primaryStep = -kCoordsXYStep;
                slopeDirection = TILE_ELEMENT_DIRECTION_WEST;
            }
        }
        else
        {
            secondaryStart = range.GetX1();
            secondaryEnd = range.GetX2();

            if (startPos.y <= (range.GetY1() + range.GetY2()) / 2)
            {
                primaryStart = range.GetY1();
                primaryEnd = range.GetY2();
                primaryStep = kCoordsXYStep;
                slopeDirection = TILE_ELEMENT_DIRECTION_NORTH;
            }
            else
            {
                primaryStart = range.GetY2();
                primaryEnd = range.GetY1();
                primaryStep = -kCoordsXYStep;
                slopeDirection = TILE_ELEMENT_DIRECTION_SOUTH;
            }
        }

        // Gather terrain heights: baseZ for going down, maxZ for going up
        std::vector<SliceInfo> slices;

        for (int32_t primary = primaryStart; primaryStep > 0 ? primary <= primaryEnd : primary >= primaryEnd;
             primary += primaryStep)
        {
            int32_t sliceBaseZ = INT32_MIN;
            int32_t sliceMaxZ = INT32_MIN;

            for (int32_t secondary = secondaryStart; secondary <= secondaryEnd; secondary += kCoordsXYStep)
            {
                CoordsXY pos = primaryIsX ? CoordsXY{ primary, secondary } : CoordsXY{ secondary, primary };
                auto* surfaceElement = MapGetSurfaceElementAt(TileCoordsXY(pos));
                if (surfaceElement != nullptr)
                {
                    int32_t base = surfaceElement->GetBaseZ();
                    int32_t top = getMaxSurfaceHeight(*surfaceElement);

                    sliceBaseZ = std::max(sliceBaseZ, base);
                    sliceMaxZ = std::max(sliceMaxZ, top);
                }
            }

            if (sliceBaseZ == INT32_MIN)
                continue;

            slices.push_back({ primary, sliceBaseZ, sliceMaxZ });
        }

        if (slices.empty())
            return {};

        // Filter out obstacle spikes (isolated height jumps)
        if (slices.size() > 1)
        {
            for (size_t i = 0; i < slices.size(); i++)
            {
                int32_t prevZ = (i > 0) ? slices[i - 1].maxZ : slices[1].maxZ;
                int32_t nextZ = (i + 1 < slices.size()) ? slices[i + 1].maxZ : slices[i - 1].maxZ;

                bool spikeFromPrev = (slices[i].maxZ - prevZ) > kPathHeightStep;
                bool spikeFromNext = (slices[i].maxZ - nextZ) > kPathHeightStep;

                if (spikeFromPrev && spikeFromNext)
                {
                    int32_t newZ = std::min(prevZ, nextZ);
                    slices[i].maxZ = newZ;
                    slices[i].baseZ = std::min(slices[i].baseZ, newZ);
                }
            }
        }

        // Build path: terrain slopes (maxZ > baseZ) slope ON tile, raised blocks slope BEFORE
        int32_t secondaryCount = (secondaryEnd - secondaryStart) / kCoordsXYStep + 1;
        result.reserve(slices.size() * secondaryCount);

        // Initial height: start at baseZ for slopes heading up, midpoint for steep diagonals,
        // but 3-corner-up tiles aren't traversable so always maxZ. WTF so many edge cases!?
        bool firstTileIsRaiseType = false;
        {
            CoordsXY firstPos = primaryIsX ? CoordsXY{ slices[0].primaryCoord, secondaryStart }
                                           : CoordsXY{ secondaryStart, slices[0].primaryCoord };
            auto* firstSurf = MapGetSurfaceElementAt(TileCoordsXY(firstPos));
            if (firstSurf != nullptr)
            {
                uint8_t slope = firstSurf->GetSlope();
                uint8_t corners = slope & kTileSlopeRaisedCornersMask;
                bool isDiag = (slope & kTileSlopeDiagonalFlag) != 0;
                firstTileIsRaiseType = !isDiag
                    && (corners == 0b0111 || corners == 0b1011 || corners == 0b1101 || corners == 0b1110);
            }
        }
        bool firstTileIsSlope = (slices[0].maxZ - slices[0].baseZ) >= kPathHeightStep && !firstTileIsRaiseType;
        bool firstTileIsSteep = (slices[0].maxZ - slices[0].baseZ) >= 2 * kPathHeightStep;
        bool nextTileIsHigher = (slices.size() > 1) && (slices[1].maxZ > slices[0].maxZ); // > not >=
        int32_t currentPathZ;
        if (firstTileIsSlope && nextTileIsHigher)
            currentPathZ = firstTileIsSteep ? slices[0].baseZ + kPathHeightStep : slices[0].baseZ;
        else
            currentPathZ = slices[0].maxZ;

        // Track slope direction to distinguish landing from descent
        bool justWentUp = false;
        bool justWentDown = false;

        for (size_t i = 0; i < slices.size(); i++)
        {
            int32_t thisBaseZ = slices[i].baseZ;
            int32_t thisMaxZ = slices[i].maxZ;
            int32_t nextMaxZ = (i + 1 < slices.size()) ? slices[i + 1].maxZ : thisMaxZ;
            int32_t nextBaseZ = (i + 1 < slices.size()) ? slices[i + 1].baseZ : thisBaseZ;

            bool thisIsSlope = (thisMaxZ - thisBaseZ) >= kPathHeightStep;
            bool nextIsRaisedBlock = (nextMaxZ == nextBaseZ) && (nextBaseZ > currentPathZ);

            int32_t useZ = currentPathZ;
            FootpathSlope useSlope = { FootpathSlopeType::flat, 0 };

            bool isLastTile = (i == slices.size() - 1);

            // Going UP
            if (thisIsSlope && thisMaxZ > currentPathZ)
            {
                useSlope = { FootpathSlopeType::sloped, slopeDirection };
                currentPathZ += kPathHeightStep;
                justWentUp = true;
                justWentDown = false;
            }
            else if (nextIsRaisedBlock && (nextBaseZ - currentPathZ) >= kPathHeightStep)
            {
                useSlope = { FootpathSlopeType::sloped, slopeDirection };
                currentPathZ += kPathHeightStep;
                justWentUp = true;
                justWentDown = false;
            }
            // Going DOWN: on slopes go down ON tile, on flat blocks go down AFTER
            else if ((currentPathZ - nextMaxZ) >= kPathHeightStep)
            {
                bool thisIsFlatBlock = !thisIsSlope && (thisMaxZ >= currentPathZ);
                if (!thisIsFlatBlock)
                {
                    useZ = currentPathZ - kPathHeightStep;
                    useSlope = { FootpathSlopeType::sloped, DirectionReverse(slopeDirection) };
                    currentPathZ -= kPathHeightStep;
                    justWentDown = true;
                }
                else
                {
                    justWentDown = false;
                }
                justWentUp = false;
            }
            // Last tile: check if we should slope down (not if we just came up)
            else if (isLastTile && thisIsSlope && !justWentUp && currentPathZ >= thisMaxZ)
            {
                bool shouldSlopeDown = false;

                if (justWentDown)
                {
                    shouldSlopeDown = true;
                }
                else
                {
                    CoordsXY tilePos = primaryIsX ? CoordsXY{ slices[i].primaryCoord, secondaryStart }
                                                  : CoordsXY{ secondaryStart, slices[i].primaryCoord };
                    auto* surfaceElement = MapGetSurfaceElementAt(TileCoordsXY(tilePos));

                    if (surfaceElement != nullptr)
                    {
                        uint8_t slope = surfaceElement->GetSlope();
                        uint8_t corners = slope & kTileSlopeRaisedCornersMask;
                        bool isDiagonal = (slope & kTileSlopeDiagonalFlag) != 0;

                        if (isDiagonal)
                        {
                            // Diagonal: down corner indicates descent direction
                            Direction downDirection = TILE_ELEMENT_DIRECTION_WEST;
                            if (corners == kTileSlopeNCornerDown)
                                downDirection = TILE_ELEMENT_DIRECTION_NORTH;
                            else if (corners == kTileSlopeECornerDown)
                                downDirection = TILE_ELEMENT_DIRECTION_EAST;
                            else if (corners == kTileSlopeSCornerDown)
                                downDirection = TILE_ELEMENT_DIRECTION_SOUTH;
                            else if (corners == kTileSlopeWCornerDown)
                                downDirection = TILE_ELEMENT_DIRECTION_WEST;

                            if (downDirection == slopeDirection)
                                shouldSlopeDown = true;
                        }
                        else
                        {
                            auto terrainPlacement = FootpathGetOnTerrainPlacement(*surfaceElement);
                            if (terrainPlacement.slope.type == FootpathSlopeType::sloped
                                && terrainPlacement.slope.direction == DirectionReverse(slopeDirection))
                            {
                                shouldSlopeDown = true;
                            }
                        }
                    }
                }

                if (shouldSlopeDown)
                {
                    useZ = currentPathZ - kPathHeightStep;
                    useSlope = { FootpathSlopeType::sloped, DirectionReverse(slopeDirection) };
                    currentPathZ -= kPathHeightStep;
                }
                justWentUp = false;
                justWentDown = false;
            }
            else
            {
                useZ = thisMaxZ;
                currentPathZ = thisMaxZ;
                justWentUp = false;
                justWentDown = false;
            }

            for (int32_t secondary = secondaryStart; secondary <= secondaryEnd; secondary += kCoordsXYStep)
            {
                CoordsXY pos = primaryIsX ? CoordsXY{ slices[i].primaryCoord, secondary }
                                          : CoordsXY{ secondary, slices[i].primaryCoord };

                int32_t tileZ = useZ;
                FootpathSlope tileSlope = useSlope;

                // Handle 3-corner-up tiles (raise flat paths to match terrain)
                auto* surfaceElement = MapGetSurfaceElementAt(TileCoordsXY(pos));
                if (surfaceElement != nullptr)
                {
                    auto terrainPlacement = FootpathGetOnTerrainPlacement(*surfaceElement);
                    uint8_t rawSlope = surfaceElement->GetSlope();
                    uint8_t corners = rawSlope & kTileSlopeRaisedCornersMask;
                    bool isDiagonal = (rawSlope & kTileSlopeDiagonalFlag) != 0;
                    bool isRaiseType = !isDiagonal
                        && (corners == 0b0111 || corners == 0b1011 || corners == 0b1101 || corners == 0b1110);

                    if (isRaiseType && tileSlope.type == FootpathSlopeType::flat && terrainPlacement.baseZ > tileZ)
                        tileZ = terrainPlacement.baseZ;
                }

                result.push_back({ CoordsXYZ(pos.x, pos.y, tileZ), tileSlope });
            }
        }

        return result;
    }

} // namespace OpenRCT2
