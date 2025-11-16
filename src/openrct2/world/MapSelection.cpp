/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MapSelection.h"

#include "../interface/Viewport.h"
#include "Map.h"

MapSelectFlags gMapSelectFlags;
MapSelectType gMapSelectType;
CoordsXY gMapSelectPositionA;
CoordsXY gMapSelectPositionB;
CoordsXYZ gMapSelectArrowPosition;
uint8_t gMapSelectArrowDirection;

std::vector<CoordsXY> gMapSelectionTiles;

MapRange getMapSelectRange()
{
    return MapRange(gMapSelectPositionA, gMapSelectPositionB);
}

void setMapSelectRange(const MapRange& range)
{
    const auto normalised = range.Normalise();
    gMapSelectPositionA = normalised.Point1;
    gMapSelectPositionB = normalised.Point2;
}

void setMapSelectRange(const CoordsXY coords)
{
    setMapSelectRange({ coords, coords });
}
