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
    #include "ScLandscape.h"

namespace OpenRCT2::Scripting
{
    class ScEditor
    {
    private:
        static bool EditorIsActive();
        static void ThrowIfEditorIsNotActive();
        std::shared_ptr<ScLandscape> getLandscape();

    public:
        static void Register(duk_context* ctx);
    };

} // namespace OpenRCT2::Scripting

#endif
