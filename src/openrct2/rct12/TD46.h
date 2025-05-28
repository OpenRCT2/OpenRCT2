/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
    enum class RCT12TrackDesignVersion : uint8_t
    {
        TD4,
        TD4_AA,
        TD6,
        unknown
    };

    enum class TD46Flags : uint8_t
    {
        StationId = 0b00000011,
        SpeedOrSeatRotation = 0b00001111,
        ColourScheme = 0b00110000,
        IsInverted = 0b01000000,
        HasChain = 0b10000000,
    };

#pragma pack(push, 1)
    enum class TD46MazeElementType : uint8_t
    {
        Entrance = (1 << 3),
        Exit = (1 << 7)
    };

    /* Maze Element entry   size: 0x04 */
    struct TD46MazeElement
    {
        union
        {
            uint32_t All;
            struct
            {
                int8_t x;
                int8_t y;
                union
                {
                    uint16_t MazeEntry;
                    struct
                    {
                        uint8_t Direction;
                        uint8_t Type;
                    };
                };
            };
        };

        constexpr bool IsEntrance() const
        {
            return Type == EnumValue(TD46MazeElementType::Entrance);
        }

        constexpr bool IsExit() const
        {
            return Type == EnumValue(TD46MazeElementType::Exit);
        }
    };
    static_assert(sizeof(TD46MazeElement) == 0x04);
#pragma pack(pop)

    void ConvertFromTD46Flags(TrackDesignTrackElement& target, uint8_t flags);
    uint8_t ConvertToTD46Flags(const TrackDesignTrackElement& source);
    void ImportMazeElement(TrackDesign& td, const TD46MazeElement& td46MazeElement);
} // namespace OpenRCT2::RCT12
