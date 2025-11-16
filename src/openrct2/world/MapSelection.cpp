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

static MapSelectFlags _previousMapSelectFlags;
static CoordsXY _previousMapSelectPositionA;
static CoordsXY _previousMapSelectPositionB;
static CoordsXYZ _previousMapSelectArrowPosition;
static uint8_t _previousMapSelectArrowDirection;

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

namespace OpenRCT2::MapSelection
{

    void Invalidate()
    {
        if (!_previousMapSelectFlags.has(MapSelectFlag::enable) && gMapSelectFlags.has(MapSelectFlag::enable))
        {
            MapInvalidateRegion(gMapSelectPositionA, gMapSelectPositionB);
        }
        else if (_previousMapSelectFlags.has(MapSelectFlag::enable) && !gMapSelectFlags.has(MapSelectFlag::enable))
        {
            MapInvalidateRegion(_previousMapSelectPositionA, _previousMapSelectPositionB);
        }
        else if (
            gMapSelectFlags.has(MapSelectFlag::enable)
            && (_previousMapSelectPositionA != gMapSelectPositionA || _previousMapSelectPositionB != gMapSelectPositionB))
        {
            MapInvalidateRegion(_previousMapSelectPositionA, _previousMapSelectPositionB);
            MapInvalidateRegion(gMapSelectPositionA, gMapSelectPositionB);
        }

        if (!_previousMapSelectFlags.has(MapSelectFlag::enableArrow) && gMapSelectFlags.has(MapSelectFlag::enableArrow))
        {
            MapInvalidateTile({ gMapSelectArrowPosition, gMapSelectArrowPosition.z });
        }
        else if (_previousMapSelectFlags.has(MapSelectFlag::enableArrow) && !gMapSelectFlags.has(MapSelectFlag::enableArrow))
        {
            MapInvalidateTile({ _previousMapSelectArrowPosition, _previousMapSelectArrowPosition.z });
        }
        else if (gMapSelectFlags.has(MapSelectFlag::enableArrow) && _previousMapSelectArrowPosition != gMapSelectArrowPosition)
        {
            MapInvalidateTile({ _previousMapSelectArrowPosition, _previousMapSelectArrowPosition.z });
            MapInvalidateTile({ gMapSelectArrowPosition, gMapSelectArrowPosition.z });
        }
        else if (_previousMapSelectArrowDirection != gMapSelectArrowDirection)
        {
            MapInvalidateTile({ gMapSelectArrowPosition, gMapSelectArrowPosition.z });
        }

        _previousMapSelectFlags = gMapSelectFlags;
        _previousMapSelectPositionA = gMapSelectPositionA;
        _previousMapSelectPositionB = gMapSelectPositionB;
        _previousMapSelectArrowPosition = gMapSelectArrowPosition;
        _previousMapSelectArrowDirection = gMapSelectArrowDirection;
    }
} // namespace OpenRCT2::MapSelection
