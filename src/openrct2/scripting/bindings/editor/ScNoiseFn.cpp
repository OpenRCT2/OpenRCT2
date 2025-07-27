/*****************************************************************************
 * Copyright (c) 2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScNoiseFn.h"

namespace OpenRCT2::Scripting
{
    float ScNoiseFn::generate(float x, float y)
    {
        return _noise->Generate(x, y);
    }

    void ScNoiseFn::Register(duk_context* ctx)
    {
        dukglue_register_method(ctx, &ScNoiseFn::generate, "generate");
    }
} // namespace OpenRCT2::Scripting

#endif
