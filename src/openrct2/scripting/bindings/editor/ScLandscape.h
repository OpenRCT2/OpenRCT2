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

    #include "../../Duktape.hpp"
    #include "../../ScriptEngine.h"
    #include "ScNoiseFn.h"

namespace OpenRCT2::Scripting
{

    class ScLandscape
    {
    private:
        std::shared_ptr<ScNoiseFn> getSimplexNoiseFn(DukValue maybeSeed);
        std::shared_ptr<ScNoiseFn> getSimplexFbmNoiseFn(
            float frequency, int32_t octaves, float lacunarity, float persistence, DukValue maybeSeed);

    public:
        static void Register(duk_context* ctx);
    };

} // namespace OpenRCT2::Scripting

#endif
