/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/FlagHolder.hpp"
#include "Location.hpp"

#include <vector>

enum class MapSelectFlag : uint8_t
{
    enable,
    enableConstruct,
    enableArrow,
    green,
};
using MapSelectFlags = FlagHolder<uint8_t, MapSelectFlag>;

enum class MapSelectType : uint8_t
{
    corner0,
    corner1,
    corner2,
    corner3,
    full,
    fullWater,
    fullLandRights,
    quarter0,
    quarter1,
    quarter2,
    quarter3,
    edge0,
    edge1,
    edge2,
    edge3,
};
constexpr MapSelectType getMapSelectCorner(Direction direction)
{
    return static_cast<MapSelectType>(EnumValue(MapSelectType::corner0) + direction);
}
constexpr MapSelectType getMapSelectQuarter(Direction direction)
{
    return static_cast<MapSelectType>(EnumValue(MapSelectType::quarter0) + direction);
}
constexpr MapSelectType getMapSelectEdge(Direction direction)
{
    return static_cast<MapSelectType>(EnumValue(MapSelectType::edge0) + direction);
}

extern MapSelectFlags gMapSelectFlags;
extern MapSelectType gMapSelectType;
extern CoordsXY gMapSelectPositionA;
extern CoordsXY gMapSelectPositionB;
extern CoordsXYZ gMapSelectArrowPosition;
extern uint8_t gMapSelectArrowDirection;

MapRange getMapSelectRange();
void setMapSelectRange(const MapRange& range);
void setMapSelectRange(CoordsXY coords);

namespace OpenRCT2::MapSelection
{
    void clearSelectedTiles();
    void addSelectedTile(const CoordsXY& coords);
    const std::vector<CoordsXY>& getSelectedTiles();

    void invalidate();
} // namespace OpenRCT2::MapSelection
