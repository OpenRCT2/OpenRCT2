/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "common.h"
#include "core/String.hpp"

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

enum class FILE_TYPE
{
    UNDEFINED,
    OBJECT,
    SAVED_GAME,
    SCENARIO,
    TRACK_DESIGN,
    PARK,
};

struct ClassifiedFileInfo
{
    FILE_TYPE Type = FILE_TYPE::UNDEFINED;
    uint32_t Version = 0;
};

#define FILE_TYPE_S4_CUTOFF 2
bool TryClassifyFile(const std::string& path, ClassifiedFileInfo* result);
bool TryClassifyFile(OpenRCT2::IStream* stream, ClassifiedFileInfo* result);

FileExtension get_file_extension_type(u8string_view path);
