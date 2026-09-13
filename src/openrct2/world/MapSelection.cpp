/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

static std::vector<CoordsXY> _mapSelectionTiles;
static bool _mapSelectionTilesInvalidate = false;

static MapSelectFlags _previousMapSelectFlags;
static MapSelectType _previousMapSelectType;
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
    void clearSelectedTiles()
    {
        for (const CoordsXY& coords : _mapSelectionTiles)
        {
            MapInvalidateTileFull(coords);
        }
        _mapSelectionTiles.clear();
        _mapSelectionTilesInvalidate = false;
    }

    void addSelectedTile(const CoordsXY& coords)
    {
        _mapSelectionTiles.push_back(coords);
        _mapSelectionTilesInvalidate = true;
    }

    const std::vector<CoordsXY>& getSelectedTiles()
    {
        return _mapSelectionTiles;
    }

    void invalidate()
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
        else if (_previousMapSelectType != gMapSelectType)
        {
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

        if (_previousMapSelectFlags.has(MapSelectFlag::enableConstruct) != gMapSelectFlags.has(MapSelectFlag::enableConstruct)
            || _mapSelectionTilesInvalidate)
        {
            for (const CoordsXY& coords : _mapSelectionTiles)
            {
                MapInvalidateTileFull(coords);
            }
            _mapSelectionTilesInvalidate = false;
        }

        _previousMapSelectFlags = gMapSelectFlags;
        _previousMapSelectType = gMapSelectType;
        _previousMapSelectPositionA = gMapSelectPositionA;
        _previousMapSelectPositionB = gMapSelectPositionB;
        _previousMapSelectArrowPosition = gMapSelectArrowPosition;
        _previousMapSelectArrowDirection = gMapSelectArrowDirection;
    }
} // namespace OpenRCT2::MapSelection
