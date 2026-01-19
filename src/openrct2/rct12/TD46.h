/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/EnumUtils.hpp"

#include <cstdint>

struct TrackDesign;
struct TrackDesignTrackElement;

namespace OpenRCT2::RCT12
{
    enum class TD46Version : uint8_t
    {
        td4,
        td4AA,
        td6,
        unknown
    };

    enum class TD46Flags : uint8_t
    {
        stationId = 0b00000011,
        speedOrSeatRotation = 0b00001111,
        colourScheme = 0b00110000,
        isInverted = 0b01000000,
        hasChain = 0b10000000,
    };

#pragma pack(push, 1)
    enum class TD46MazeElementType : uint8_t
    {
        entrance = (1 << 3),
        exit = (1 << 7)
    };

    /* Maze Element entry   size: 0x04 */
    struct TD46MazeElement
    {
        union
        {
            uint32_t all;
            struct
            {
                int8_t x;
                int8_t y;
                union
                {
                    uint16_t mazeEntry;
                    struct
                    {
                        uint8_t direction;
                        uint8_t type;
                    };
                };
            };
        };

        constexpr bool isEntrance() const
        {
            return type == EnumValue(TD46MazeElementType::entrance);
        }

        constexpr bool isExit() const
        {
            return type == EnumValue(TD46MazeElementType::exit);
        }
    };
    static_assert(sizeof(TD46MazeElement) == 0x04);
#pragma pack(pop)

    void convertFromTD46Flags(TrackDesignTrackElement& target, uint8_t flags);
    uint8_t convertToTD46Flags(const TrackDesignTrackElement& source);
    void importMazeElement(TrackDesign& td, const TD46MazeElement& td46MazeElement);
} // namespace OpenRCT2::RCT12
