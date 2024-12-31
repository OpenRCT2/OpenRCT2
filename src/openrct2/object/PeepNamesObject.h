/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/IStream.hpp"
#include "Object.h"

#include <string>
#include <vector>

class PeepNamesObject final : public Object
{
private:
    std::vector<std::string> _givenNames;
    std::vector<std::string> _surnames;

public:
    static constexpr ObjectType kObjectType = ObjectType::PeepNames;

    void ReadJson(IReadObjectContext* context, json_t& root) override;
    void Load() override;
    void Unload() override;

    std::string GetGivenNameAt(size_t index) const;
    std::string GetSurnameAt(size_t index) const;
};
