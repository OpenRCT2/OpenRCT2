/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstddef>
#include <cstdint>

namespace OpenRCT2::RCT1
{
    enum class RCT1Version : uint8_t
    {
        baseGame,
        addedAttractions,
        loopyLandscapes,
    };
    enum class RCT1FileType : uint8_t
    {
        td4,
        sv4,
        sc4,
    };
    struct RCT1VersionAndFileType
    {
        RCT1Version version;
        RCT1FileType fileType;
    };

    RCT1VersionAndFileType detectFileType(const uint8_t* src, size_t length);
    RCT1VersionAndFileType detectRCT1Version(int32_t gameVersion);
} // namespace OpenRCT2::RCT1
