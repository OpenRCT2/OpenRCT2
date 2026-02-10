/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once
#include "../../core/StringTypes.h"

namespace OpenRCT2::Platform::OOBE::GOG
{
    struct InnoArchiveFile
    {
        u8string_view filename;
        uint32_t offset;
        uint32_t compressedSize;
        uint32_t unCompressedSize;
    };

    bool isRCT2BinFile(u8string_view binFile);
    bool isRCT1BinFile(u8string_view binFile);
    bool extractRCT2Files(u8string_view binFile, u8string_view targetFolder);
    bool extractRCT1Files(u8string_view binFile, u8string_view targetFolder);
} // namespace OpenRCT2::Platform::OOBE::GOG
