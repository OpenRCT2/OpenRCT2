/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../ride/TrackDesign.h"

#include <vector>

interface IStream;
struct Ride;

/**
 * Class to export RollerCoaster Tycoon 2 track designs (*.TD6).
 */
class T6Exporter final
{
public:
    T6Exporter();

    bool Save(ride_id_t rideId);
    bool SaveTrack(const utf8* path);
    bool SaveTrack(IStream* stream);

private:
    TrackDesign _trackDesign;
    Direction _saveDirection;
    rct_string_id CreateTrackDesign(Ride& ride);
    rct_string_id CreateTrackDesignTrack(Ride& ride);
    rct_string_id CreateTrackDesignMaze(Ride& ride);
    rct_string_id CreateTrackDesignScenery(Ride& ride);
    CoordsXYE MazeGetFirstElement(Ride& ride);
};
