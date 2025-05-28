/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
    void ConvertFromTD46Flags(TrackDesignTrackElement& target, uint8_t flags)
    {
        target.brakeBoosterSpeed = kRCT2DefaultBlockBrakeSpeed;
        if (TrackTypeIsStation(target.type))
        {
            auto stationIndex = flags & EnumValue(TD46Flags::StationId);
            target.stationIndex = StationIndex::FromUnderlying(stationIndex);
        }
        else
        {
            auto speedOrSeatRotation = flags & EnumValue(TD46Flags::SpeedOrSeatRotation);
            if (TrackTypeHasSpeedSetting(target.type) && target.type != TrackElemType::BlockBrakes)
            {
                target.brakeBoosterSpeed = speedOrSeatRotation << 1;
            }
            else
            {
                target.seatRotation = speedOrSeatRotation;
            }
        }

        target.colourScheme = (flags & EnumValue(TD46Flags::ColourScheme)) >> 4;
        if (flags & EnumValue(TD46Flags::IsInverted))
            target.SetFlag(TrackDesignTrackElementFlag::isInverted);
        if (flags & EnumValue(TD46Flags::HasChain))
            target.SetFlag(TrackDesignTrackElementFlag::hasChain);
    }

    uint8_t ConvertToTD46Flags(const TrackDesignTrackElement& source)
    {
        uint8_t trackFlags = 0;
        if (TrackTypeIsStation(source.type))
        {
            trackFlags = (source.stationIndex.ToUnderlying() & EnumValue(TD46Flags::StationId));
        }
        else if (TrackTypeHasSpeedSetting(source.type) && source.type != TrackElemType::BlockBrakes)
        {
            trackFlags = (source.brakeBoosterSpeed >> 1);
        }
        else
        {
            trackFlags = source.seatRotation;
        }

        trackFlags |= source.colourScheme << 4;

        if (source.HasFlag(TrackDesignTrackElementFlag::hasChain))
            trackFlags |= EnumValue(TD46Flags::HasChain);
        if (source.HasFlag(TrackDesignTrackElementFlag::isInverted))
            trackFlags |= EnumValue(TD46Flags::IsInverted);

        return trackFlags;
    }

    void ImportMazeElement(TrackDesign& td, const TD46MazeElement& td46MazeElement)
    {
        if (td46MazeElement.IsEntrance() || td46MazeElement.IsExit())
        {
            TrackDesignEntranceElement element{};
            element.location = TileCoordsXYZD(td46MazeElement.x, td46MazeElement.y, 0, td46MazeElement.Direction);
            element.isExit = td46MazeElement.IsExit();
            td.entranceElements.push_back(element);
        }
        else
        {
            TrackDesignMazeElement mazeElement{};
            mazeElement.location.x = td46MazeElement.x;
            mazeElement.location.y = td46MazeElement.y;
            mazeElement.mazeEntry = td46MazeElement.MazeEntry;
            td.mazeElements.push_back(mazeElement);
        }
    }
} // namespace OpenRCT2::RCT12
