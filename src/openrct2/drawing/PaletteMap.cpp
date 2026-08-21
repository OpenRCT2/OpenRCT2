/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PaletteMap.h"

#include "../core/EnumUtils.hpp"

#include <cassert>

namespace OpenRCT2::Drawing
{
    static auto _defaultPaletteMapping = []() {
        std::array<PaletteIndex, 256> res;
        for (size_t i = 0; i < std::size(res); i++)
        {
            res[i] = static_cast<PaletteIndex>(i);
        }
        return res;
    }();

    PaletteMap PaletteMap::GetDefault()
    {
        return PaletteMap(_defaultPaletteMapping);
    }

    PaletteIndex& PaletteMap::operator[](size_t index)
    {
        return _data[index];
    }

    PaletteIndex PaletteMap::operator[](size_t index) const
    {
        return _data[index];
    }

    PaletteIndex PaletteMap::Blend(PaletteIndex src, PaletteIndex dst) const
    {
        const auto srcValue = EnumValue(src);
        const auto dstValue = EnumValue(dst);
#ifdef _DEBUG
        // src = 0 would be transparent so there is no blend palette for that, hence (src - 1)
        assert(src != PaletteIndex::transparent);
        assert(static_cast<size_t>(srcValue - 1) < _numMaps);
        assert(static_cast<size_t>(dstValue) < _mapLength);
#endif
        auto idx = ((srcValue - 1) * 256) + dstValue;
        return _data[idx];
    }

    void PaletteMap::Copy(PaletteIndex dstIndex, const PaletteMap& src, PaletteIndex srcIndex, size_t length)
    {
        auto srcOffset = EnumValue(srcIndex);
        auto dstOffset = EnumValue(dstIndex);
        auto maxLength = std::min(_data.size() - srcOffset, _data.size() - dstOffset);
        assert(length <= maxLength);
        auto copyLength = std::min(length, maxLength);
        std::copy(src._data.begin() + srcOffset, src._data.begin() + srcOffset + copyLength, _data.begin() + dstOffset);
    }
} // namespace OpenRCT2::Drawing
