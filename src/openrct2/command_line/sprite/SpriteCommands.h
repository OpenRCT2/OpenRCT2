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
#include "../../drawing/ImageImporter.h"
#include "../ExitCode.h"

#include <cstdint>
#include <optional>

namespace OpenRCT2
{
    struct G1Element;
}

namespace OpenRCT2::CommandLine::Sprite
{
    using namespace OpenRCT2::Drawing;
    bool SpriteImageExport(const G1Element& spriteElement, u8string_view outPath);
    std::optional<Image> SpriteImageLoad(u8string_view path, ImageImportMeta meta);
    std::string PopStr(std::ostringstream& oss);

    ExitCode append(const char** argv, int32_t argc, ImportMode spriteMode);
    ExitCode build(const char** argv, int32_t argc, ImportMode spriteMode);
    ExitCode combine(const char** argv, int32_t argc);
    ExitCode create(const char** argv, int32_t argc);
    ExitCode details(const char** argv, int32_t argc);
    ExitCode exportAll(const char** argv, int32_t argc);
    ExitCode exportObject(const char** argv, int32_t argc);
    ExitCode exportSingle(const char** argv, int32_t argc);
} // namespace OpenRCT2::CommandLine::Sprite
