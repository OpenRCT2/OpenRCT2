/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

namespace OpenRCT2::Entity::Yaw
{
    enum class SpritePrecision : uint8_t
    {
        None = 0,
        Sprites1,
        Sprites2,
        Sprites4,
        Sprites8,
        Sprites16,
        Sprites32,
        Sprites64
    };

    // Sprites32 represents the precision of the base rotation precision. Base rotation is the precision of
    // EntityBase.sprite_direction
    constexpr const int32_t BaseRotation = 32;

    // The first value represents None, the last value represents 64 which has not yet been implemented
    constexpr const uint8_t PrecisionOffset[] = { 5, 5, 4, 3, 2, 1, 0, 0 };

    [[nodiscard]] constexpr int32_t Add(int32_t yaw1, int32_t yaw2)
    {
        return (yaw1 + yaw2) & 0x1F;
    }
    [[nodiscard]] constexpr uint8_t YawFrom4(int32_t rotation)
    {
        return rotation << 3;
    }
    [[nodiscard]] constexpr int32_t YawTo4(int32_t yaw)
    {
        return yaw >> 3;
    }
    [[nodiscard]] constexpr int32_t YawTo8(int32_t yaw)
    {
        return yaw >> 2;
    }
    [[nodiscard]] constexpr int32_t YawTo16(int32_t yaw)
    {
        return yaw >> 1;
    }
    [[nodiscard]] constexpr int32_t YawTo32(int32_t yaw)
    {
        return yaw;
    }

    [[nodiscard]] constexpr int32_t YawToPrecision(int32_t yaw, SpritePrecision precision)
    {
        return yaw >> PrecisionOffset[static_cast<uint8_t>(precision)];
    }

    [[nodiscard]] constexpr uint8_t NumSpritesPrecision(SpritePrecision precision)
    {
        return (1 << static_cast<uint8_t>(precision)) >> 1;
    }
} // namespace OpenRCT2::Entity::Yaw
