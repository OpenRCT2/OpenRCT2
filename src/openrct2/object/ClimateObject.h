/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Climate.h"
#include "Object.h"

struct IReadObjectContext;

class ClimateObject final : public Object
{
private:
    Climate _climate;

public:
    static constexpr ObjectType kObjectType = ObjectType::climate;

    void ReadJson(IReadObjectContext* context, json_t& root) override;
    void Load() override;
    void Unload() override;
};
