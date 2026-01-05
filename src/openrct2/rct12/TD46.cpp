/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TD46.h"

#include "../ride/Track.h"
#include "../ride/TrackDesign.h"

namespace OpenRCT2::RCT12
{
    void convertFromTD46Flags(TrackDesignTrackElement& target, uint8_t flags)
    {
        target.brakeBoosterSpeed = kRCT2DefaultBlockBrakeSpeed;
        if (::TrackTypeIsStation(target.type))
        {
            auto stationIndex = flags & EnumValue(TD46Flags::stationId);
            target.stationIndex = StationIndex::FromUnderlying(stationIndex);
        }
        else
        {
            auto speedOrSeatRotation = flags & EnumValue(TD46Flags::speedOrSeatRotation);
            if (::TrackTypeHasSpeedSetting(target.type) && target.type != OpenRCT2::TrackElemType::blockBrakes)
            {
                target.brakeBoosterSpeed = speedOrSeatRotation << 1;
            }
            else
            {
                target.seatRotation = speedOrSeatRotation;
            }
        }

        target.colourScheme = (flags & EnumValue(TD46Flags::colourScheme)) >> 4;
        if (flags & EnumValue(TD46Flags::isInverted))
            target.SetFlag(TrackDesignTrackElementFlag::isInverted);
        if (flags & EnumValue(TD46Flags::hasChain))
            target.SetFlag(TrackDesignTrackElementFlag::hasChain);
    }

    uint8_t convertToTD46Flags(const TrackDesignTrackElement& source)
    {
        uint8_t trackFlags = 0;
        if (::TrackTypeIsStation(source.type))
        {
            trackFlags = (source.stationIndex.ToUnderlying() & EnumValue(TD46Flags::stationId));
        }
        else if (::TrackTypeHasSpeedSetting(source.type) && source.type != OpenRCT2::TrackElemType::blockBrakes)
        {
            trackFlags = (source.brakeBoosterSpeed >> 1);
        }
        else
        {
            trackFlags = source.seatRotation;
        }

        trackFlags |= source.colourScheme << 4;

        if (source.HasFlag(TrackDesignTrackElementFlag::hasChain))
            trackFlags |= EnumValue(TD46Flags::hasChain);
        if (source.HasFlag(TrackDesignTrackElementFlag::isInverted))
            trackFlags |= EnumValue(TD46Flags::isInverted);

        return trackFlags;
    }

    void importMazeElement(TrackDesign& td, const TD46MazeElement& td46MazeElement)
    {
        if (td46MazeElement.isEntrance() || td46MazeElement.isExit())
        {
            TrackDesignEntranceElement element{};
            element.location = TileCoordsXYZD(td46MazeElement.x, td46MazeElement.y, 0, td46MazeElement.direction);
            element.isExit = td46MazeElement.isExit();
            td.entranceElements.push_back(element);
        }
        else
        {
            TrackDesignMazeElement mazeElement{};
            mazeElement.location.x = td46MazeElement.x;
            mazeElement.location.y = td46MazeElement.y;
            mazeElement.mazeEntry = td46MazeElement.mazeEntry;
            td.mazeElements.push_back(mazeElement);
        }
    }
} // namespace OpenRCT2::RCT12
