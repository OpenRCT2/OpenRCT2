/*****************************************************************************
 * Copyright (c) 2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "../../../world/map_generator/Noise.h"
    #include "../../Duktape.hpp"
    #include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    class ScNoiseFn
    {
    private:
        std::unique_ptr<World::MapGenerator::Noise> _noise;

    public:
        explicit ScNoiseFn(std::unique_ptr<World::MapGenerator::Noise> noise)
            : _noise(std::move(noise))
        {
        }
        static void Register(duk_context* ctx);
        float generate(float x, float y);
    };
} // namespace OpenRCT2::Scripting
#endif
