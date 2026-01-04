/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../drawing/ImageImporter.h"

#include <cstdint>
#include <optional>
#include <string_view>

struct G1Element;
using utf8 = char;
using u8string_view = std::basic_string_view<utf8>;

namespace OpenRCT2::CommandLine::Sprite
{
    using namespace OpenRCT2::Drawing;
    bool SpriteImageExport(const G1Element& spriteElement, u8string_view outPath);
    std::optional<Image> SpriteImageLoad(u8string_view path, ImageImportMeta meta);
    std::string PopStr(std::ostringstream& oss);

    int32_t append(const char** argv, int32_t argc, ImportMode spriteMode);
    int32_t build(const char** argv, int32_t argc, ImportMode spriteMode);
    int32_t combine(const char** argv, int32_t argc);
    int32_t create(const char** argv, int32_t argc);
    int32_t details(const char** argv, int32_t argc);
    int32_t exportAll(const char** argv, int32_t argc);
    int32_t exportObject(const char** argv, int32_t argc);
    int32_t exportSingle(const char** argv, int32_t argc);
} // namespace OpenRCT2::CommandLine::Sprite
