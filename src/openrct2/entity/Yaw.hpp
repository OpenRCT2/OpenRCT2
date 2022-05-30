/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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
    constexpr const int32_t BaseRotation = 32;

    // smallest precision is 4 frames
    constexpr const uint8_t PrecisionOffset[] = { 3, 2, 1, 0, 0, 0, 0, 0 };

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
    [[nodiscard]] constexpr int32_t YawToPrecision(int32_t yaw, uint8_t precision)
    {
        return yaw >> PrecisionOffset[precision];
    }
    [[nodiscard]] constexpr uint8_t NumSpritesPrecision(uint8_t precision)
    {
        return 4 << precision;
    }
} // namespace OpenRCT2::Entity::Yaw
