/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "core/StringTypes.h"

enum class FileExtension
{
    Unknown,
    DAT,
    SC4,
    SV4,
    TD4,
    SC6,
    SV6,
    TD6,
    PARK,
};

#include <string>

namespace OpenRCT2
{
    struct IStream;
}

enum class FileType
{
    undefined,
    object,
    savedGame,
    scenario,
    trackDesign,
    park,
};

struct ClassifiedFileInfo
{
    FileType Type = FileType::undefined;
    uint32_t Version = 0;
};

constexpr uint32_t kFileTypeS4Cutoff = 2;
bool TryClassifyFile(const std::string& path, ClassifiedFileInfo* result);
bool TryClassifyFile(OpenRCT2::IStream* stream, ClassifiedFileInfo* result);

FileExtension GetFileExtensionType(u8string_view path);
