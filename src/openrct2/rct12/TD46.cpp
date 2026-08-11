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
    namespace TD46Flags
    {
        constexpr uint8_t stationIdMask = 0b00000011;
        constexpr uint8_t speedOrSeatRotationMask = 0b00001111;
        constexpr uint8_t colourSchemeMask = 0b00110000;
        constexpr uint8_t isInverted = 0b01000000;
        constexpr uint8_t hasChain = 0b10000000;
    } // namespace TD46Flags

    void convertFromTD46Flags(TrackDesignTrackElement& target, uint8_t flags, TD46Version version)
    {
        target.brakeBoosterSpeed = kRCT2DefaultBlockBrakeSpeed;
        if (trackTypeIsStation(target.type))
        {
            auto stationIndex = flags & TD46Flags::stationIdMask;
            target.stationIndex = StationIndex::FromUnderlying(stationIndex);
        }
        else
        {
            auto speedOrSeatRotation = flags & TD46Flags::speedOrSeatRotationMask;
            if (trackTypeHasSpeedSetting(target.type)
                && (target.type != OpenRCT2::TrackElemType::blockBrakes || version == TD46Version::td7))
            {
                target.brakeBoosterSpeed = speedOrSeatRotation << 1;
            }
            else
            {
                target.seatRotation = speedOrSeatRotation;
            }
        }

        target.colourScheme = (flags & TD46Flags::colourSchemeMask) >> 4;
        if (flags & TD46Flags::isInverted)
            target.flags.set(TrackDesignTrackElementFlag::isInverted);
        if (flags & TD46Flags::hasChain)
            target.flags.set(TrackDesignTrackElementFlag::hasChain);
    }

    uint8_t convertToTD46Flags(const TrackDesignTrackElement& source, TD46Version version)
    {
        uint8_t trackFlags = 0;
        if (trackTypeIsStation(source.type))
        {
            trackFlags = source.stationIndex.ToUnderlying() & TD46Flags::stationIdMask;
        }
        else if (
            trackTypeHasSpeedSetting(source.type)
            && (source.type != OpenRCT2::TrackElemType::blockBrakes || version == TD46Version::td7))
        {
            trackFlags = (source.brakeBoosterSpeed >> 1);
        }
        else
        {
            trackFlags = source.seatRotation;
        }

        trackFlags |= source.colourScheme << 4;

        if (source.flags.has(TrackDesignTrackElementFlag::hasChain))
            trackFlags |= TD46Flags::hasChain;
        if (source.flags.has(TrackDesignTrackElementFlag::isInverted))
            trackFlags |= TD46Flags::isInverted;

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
