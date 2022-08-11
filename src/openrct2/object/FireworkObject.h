/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/IStream.hpp"
#include "Object.h"

struct Firework
{
    uint32_t Image;
    StringId Name;
    bool UseRemap1;
    bool UseRemap2;
    bool UseRemap3;

    uint8_t Width;
    uint8_t Height;
    std::string SoundId;
};


class FireworkObject final : public Object
{
private:
    Firework _firework;

public:
    void ReadJson(IReadObjectContext* context, json_t& root) override;
    void Load() override;
    void Unload() override;
};
