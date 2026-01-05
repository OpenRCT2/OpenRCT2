/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Range.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace OpenRCT2
{
    class ResourceTable
    {
    protected:
        enum class SourceKind
        {
            none,
            data,
            objData,
            gx,
            g1,
            csg,
            png,
        };

        struct SourceInfo
        {
            SourceKind Kind{};
            std::string Path;
            std::optional<Range<int32_t>> SourceRange;
        };

        static Range<int32_t> ParseRange(std::string_view s);
        static SourceInfo ParseSource(std::string_view source);
    };
} // namespace OpenRCT2
