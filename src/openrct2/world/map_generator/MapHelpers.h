/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Location.hpp"
#include "BaseMap.hpp"
#include "MapGen.h"

#include <cstdint>
#include <functional>
#include <optional>

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

    template<class... Arrays>
    consteval auto concat(Arrays... arrays)
    {
        return std::apply([](auto... args) { return std::array{ args... }; }, std::tuple_cat(arrays...));
    }

    static constexpr std::array kNeighbourOffsetsCardinal = {
        TileCoordsXY{ -1, 0 },
        TileCoordsXY{ 1, 0 },
        TileCoordsXY{ 0, -1 },
        TileCoordsXY{ 0, 1 },
    };

    static constexpr std::array kNeighbourOffsetsOrdinal = {
        TileCoordsXY{ 1, 1 },
        TileCoordsXY{ 1, -1 },
        TileCoordsXY{ -1, 1 },
        TileCoordsXY{ -1, -1 }
    };

    static constexpr std::array kNeighbourOffsets = concat(kNeighbourOffsetsCardinal, kNeighbourOffsetsOrdinal);

    // TODO deduplicate smoothing functions
    using SmoothFunction = std::function<int32_t(TileCoordsXY, std::optional<RiverMap>&)>;

    int32_t smoothTileSlopeStrong(TileCoordsXY tileCoords, std::optional<RiverMap>& catchment);
    int32_t smoothTileSlopeWeak(TileCoordsXY tileCoords, std::optional<RiverMap>& catchment);

    void smoothBox(HeightMap& heightMap, int32_t iterations);
    void smoothGaussian(HeightMap& heightMap, float sigma);
    void sharpen(HeightMap& heightMap, int32_t iterations);
    void smoothBilateral(HeightMap& heightMap, float sigmaScale, float sigmaIntensity);

    void applyHeightMapTransform(MapGenCtx& context);
    void applyTileSlopeSmooth(MapGenCtx& context);
} // namespace OpenRCT2::World::MapGenerator
