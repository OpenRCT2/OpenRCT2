/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../util/Hash.hpp"
#include "../Location.hpp"
#include "BaseMap.hpp"
#include "MapGen.h"

#include <cstdint>
#include <functional>

struct TileCoordsXY;

namespace OpenRCT2::World::MapGenerator
{
    enum
    {
        SLOPE_S_THRESHOLD_FLAGS = (1 << 0),
        SLOPE_W_THRESHOLD_FLAGS = (1 << 1),
        SLOPE_N_THRESHOLD_FLAGS = (1 << 2),
        SLOPE_E_THRESHOLD_FLAGS = (1 << 3)
    };

    // map coordinates and orientation, rotated 45deg counter clockwise in game when compass points north
    //    +x                   -x, -y
    //      +-----+-----+-----+
    //      |  W  | NW  |  N  |
    //      +-----+-----+-----+
    //      | SW  |  *  | NE  |
    //      +-----+-----+-----+
    //      |  S  | SE  |  E  |
    //      +-----+-----+-----+
    // +x,+y                    +y

    static constexpr TileCoordsXY kNeighbourOffsetNE{ -1, 0};
    static constexpr TileCoordsXY kNeighbourOffsetSW{ 1, 0};
    static constexpr TileCoordsXY kNeighbourOffsetNW{ 0, -1};
    static constexpr TileCoordsXY kNeighbourOffsetSE{ 0, 1};

    static constexpr TileCoordsXY kNeighbourOffsetS{ 1, 1 };
    static constexpr TileCoordsXY kNeighbourOffsetW{ 1, -1 };
    static constexpr TileCoordsXY kNeighbourOffsetE{ -1, 1 };
    static constexpr TileCoordsXY kNeighbourOffsetN{ -1, -1 };

    static constexpr std::array kNeighbourOffsetsOrdinal = {
        kNeighbourOffsetNE,
        kNeighbourOffsetSW,
        kNeighbourOffsetNW,
        kNeighbourOffsetSE,
    };

    static constexpr std::array kNeighbourOffsetsCardinal = {
        kNeighbourOffsetS,
        kNeighbourOffsetW,
        kNeighbourOffsetE,
        kNeighbourOffsetN,
    };

    static constexpr std::array kNeighbourOffsets = {
        kNeighbourOffsetNE,
        kNeighbourOffsetSW,
        kNeighbourOffsetNW,
        kNeighbourOffsetSE,
        kNeighbourOffsetS,
        kNeighbourOffsetW,
        kNeighbourOffsetE,
        kNeighbourOffsetN,
    };

    struct TileCoordsXYHash
    {
        size_t operator()(const TileCoordsXY& pos) const noexcept
        {
            using namespace Util::Hash;
            size_t hash = 0;
            update(hash, pos.x);
            update(hash, pos.y);
            return hash;
        }
    };

    using TileCoordsXYSet = std::unordered_set<TileCoordsXY, TileCoordsXYHash>;

    // TODO deduplicate smoothing functions
    using SmoothFunction = std::function<int32_t(TileCoordsXY, const MapGenCtx&)>;

    int32_t smoothTileSlopeStrong(TileCoordsXY tileCoords, const MapGenCtx& context);
    int32_t smoothTileSlopeWeak(TileCoordsXY tileCoords, const MapGenCtx& context);

    void smoothBox(HeightMap& heightMap, int32_t iterations);
    void smoothGaussian(HeightMap& heightMap, float sigma);
    void sharpen(HeightMap& heightMap, int32_t iterations);
    void smoothBilateral(HeightMap& heightMap, float sigmaScale, float sigmaIntensity);

    void applyHeightMapTransform(MapGenCtx& context);
    void applyTileSlopeSmooth(MapGenCtx& context);

    TileCoordsXY getWorldCoordsOffset(const Settings& settings);
    TileCoordsXY worldCoordsToGenCoords(const MapGenCtx& context, const TileCoordsXY& worldCoords);
    TileCoordsXY genCoordsToWorldCoords(const MapGenCtx& context, const TileCoordsXY& genCoords);
    bool isInWorldMap(const MapGenCtx& context, const TileCoordsXY& genCoords);

    uint8_t quantizeHeight(float height);
} // namespace OpenRCT2::World::MapGenerator
