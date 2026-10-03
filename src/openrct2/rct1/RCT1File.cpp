/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RCT1File.h"

#include "../core/Numerics.hpp"

#include <stdexcept>

namespace OpenRCT2::RCT1
{
    RCT1VersionAndFileType detectFileType(const uint8_t* src, size_t length)
    {
        if (length < 4)
        {
            throw std::length_error("Stream is (nearly) empty!");
        }

        // Currently can't detect TD4, as the checksum is the same as SC4 (need alternative method)

        uint32_t checksum = *(reinterpret_cast<const uint32_t*>(&src[length - 4]));
        uint32_t actualChecksum = 0;
        for (size_t i = 0; i < length - 4; i++)
        {
            actualChecksum = (actualChecksum & 0xFFFFFF00) | (((actualChecksum & 0xFF) + static_cast<uint8_t>(src[i])) & 0xFF);
            actualChecksum = Numerics::rol32(actualChecksum, 3);
        }

        return detectRCT1Version(checksum - actualChecksum);
    }

    RCT1VersionAndFileType detectRCT1Version(int32_t gameVersion)
    {
        auto fileType = (gameVersion) > 0 ? RCT1FileType::sv4 : RCT1FileType::sc4;
        gameVersion = abs(gameVersion);

        if (gameVersion >= 108000 && gameVersion < 110000)
            return RCT1VersionAndFileType(RCT1Version::baseGame, fileType);
        if (gameVersion >= 110000 && gameVersion < 120000)
            return RCT1VersionAndFileType(RCT1Version::addedAttractions, fileType);
        if (gameVersion >= 120000 && gameVersion < 130000)
            return RCT1VersionAndFileType(RCT1Version::loopyLandscapes, fileType);
        // RCTOA Acres sets this, and possibly some other user-created scenarios as well
        if (gameVersion == 0)
            return RCT1VersionAndFileType(RCT1Version::loopyLandscapes, fileType);

        throw std::length_error("Unrecognised game version!");
    }
} // namespace OpenRCT2::RCT1
