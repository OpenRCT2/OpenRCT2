/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "SettingsTypes.hpp"

#include <nlohmann/json.hpp>

namespace OpenRCT2::World::MapGenerator
{
    template<typename T, T low, T high, T step = 1>
    inline void to_json(nlohmann::json& j, const NumericSetting<T, low, high, step>& numericSetting)
    {
        j = numericSetting.get();
    }

    template<typename T, T low, T high, T step = 1>
    inline void from_json(const nlohmann::json& j, NumericSetting<T, low, high, step>& numericSetting)
    {
        numericSetting = j.get<T>();
    }

} // namespace OpenRCT2::World::MapGenerator
