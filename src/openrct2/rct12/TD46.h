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

namespace OpenRCT2
{
    enum class TrackElemType : uint16_t;

#pragma pack(push, 1)
    struct TD7TrackElement
    {
        TrackElemType type; // 0x00
        uint8_t flags;      // 0x02
    };
#pragma pack(pop)
    static_assert(sizeof(TD7TrackElement) == 0x03);
} // namespace OpenRCT2

namespace OpenRCT2::RCT12
{
    enum class TrackElemType : uint8_t;

    enum class TD46Version : uint8_t
    {
        td4,
        td4AA,
        td6,
        td7,
        unknown
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

    /* Track Element entry  size: 0x02 */
    struct TD46TrackElement
    {
        TrackElemType type; // 0x00
        uint8_t flags;      // 0x01
    };
    static_assert(sizeof(TD46TrackElement) == 0x02);
#pragma pack(pop)

    void convertFromTD46Flags(TrackDesignTrackElement& target, uint8_t flags, TD46Version version);
    uint8_t convertToTD46Flags(const TrackDesignTrackElement& source, TD46Version version);
    void importMazeElement(TrackDesign& td, const TD46MazeElement& td46MazeElement);
} // namespace OpenRCT2::RCT12
