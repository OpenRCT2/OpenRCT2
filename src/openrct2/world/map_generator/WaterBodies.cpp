/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WaterBodies.h"

#include "../../GameState.h"
#include "../../scenario/Scenario.h"
#include "HeightMap.hpp"
#include "MapGen.h"

namespace OpenRCT2::World::MapGenerator
{
    void generateRivers(const Settings& settings, HeightMap& heightMap)
    {
        for (auto i = 0; i < settings.numRiverbeds; i++)
        {
            const bool riverEastWest = ScenarioRand() % 2;
            const auto riverWidth = (settings.minRiverWidth + ScenarioRandMax(settings.maxRiverWidth - settings.minRiverWidth))
                * heightMap.density;

            // NB: this is dividing by two to match SimplexNoise.cpp
            const auto riverbedHeight = std::max(settings.heightmapLow / 2 - 4, 0);

            // We'll be varying the bank width as we meander
            uint8_t riverbankWidth = settings.riverbankWidth * heightMap.density;
            uint8_t totalRiverWidth = riverWidth + 2 * riverbankWidth;
            uint8_t easternBankOffset = riverWidth + riverbankWidth;

            // Pivot: generate a random X position
            auto xStartPos = ScenarioRandMax(0.7 * heightMap.width);

            for (auto yPos = 0u; yPos < heightMap.height; yPos++)
            {
                for (auto xOffset = 0u; xOffset < totalRiverWidth; xOffset++)
                {
                    auto pos = TileCoordsXY(xStartPos + xOffset, yPos);
                    if (!riverEastWest)
                    {
                        pos = TileCoordsXY(pos.y, pos.x);
                    }

                    if (pos.x < 0 || pos.x >= heightMap.width || pos.y < 0 || pos.y >= heightMap.height)
                    {
                        // We might meander back to a valid position later,
                        // so we're only breaking out of the inner loop.
                        break;
                    }

                    if (riverbankWidth > 0 && xOffset < riverbankWidth)
                    {
                        // Western riverbank (high to low)
                        auto bankPos = riverbankWidth - xOffset;
                        auto bankHeight = heightMap[pos] * bankPos / riverbankWidth;
                        heightMap[pos] = std::max<uint8_t>(riverbedHeight, bankHeight);
                    }
                    else if (riverbankWidth > 0 && xOffset > easternBankOffset)
                    {
                        // Eastern riverbank (low to high)
                        auto bankPos = xOffset - easternBankOffset;
                        auto bankHeight = heightMap[pos] * bankPos / riverbankWidth;
                        heightMap[pos] = std::max<uint8_t>(riverbedHeight, bankHeight);
                    }
                    else
                    {
                        // Simply carve out the river
                        heightMap[pos] = riverbedHeight;
                    }
                }

                // Let the river meander slightly
                const auto meanderRate = settings.riverMeanderRate;
                if (meanderRate > 0 && yPos % 4 == 0)
                {
                    const auto halfMeanderRate = meanderRate / 2;

                    int8_t meanderOffset = ScenarioRandMax(meanderRate) - halfMeanderRate;
                    xStartPos += meanderOffset;

                    // Adjust bank width slightly as well
                    if (settings.riverbankWidth > 0 && halfMeanderRate != 0)
                    {
                        riverbankWidth += meanderOffset / halfMeanderRate;
                        easternBankOffset += meanderOffset / halfMeanderRate;
                        totalRiverWidth += meanderOffset / halfMeanderRate * 2;
                    }
                }
            }
        }
    }
} // namespace OpenRCT2::World::MapGenerator
