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

struct TileCoordsXY;

namespace OpenRCT2::World::MapGenerator
{
    using SmoothFunction = std::function<int32_t(const MapGenContext&, TileCoordsXY)>;

    int32_t smoothTileSlopeStrong(const MapGenContext& ctx, TileCoordsXY tileCoords);
    int32_t smoothTileSlopeWeak(const MapGenContext& ctx, TileCoordsXY tileCoords);

    void smoothBox(HeightMap& heightMap, int32_t iterations);
    void smoothGaussian(HeightMap& heightMap, float sigma);
    void sharpen(HeightMap& heightMap, int32_t iterations);
    void smoothBilateral(HeightMap& heightMap, float sigmaSpace, float sigmaIntensity);

    void applyHeightMapTransform(MapGenContext& ctx);
    void applyTileSlopeSmooth(MapGenContext& ctx);

    TileCoordsXY getWorldCoordsOffset(const Settings& settings, int32_t overscanFactor);
    TileCoordsXY worldCoordsToGenCoords(const MapGenContext& ctx, const TileCoordsXY& worldCoords);
    TileCoordsXY genCoordsToWorldCoords(const MapGenContext& ctx, const TileCoordsXY& genCoords);
    bool isInWorldMap(const MapGenContext& ctx, const TileCoordsXY& genCoords);

    uint8_t quantizeHeight(float height);
} // namespace OpenRCT2::World::MapGenerator
