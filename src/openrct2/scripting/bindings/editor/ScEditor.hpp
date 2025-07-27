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

    #include "../../../Context.h"
    #include "../../../OpenRCT2.h"
    #include "../../ScriptEngine.h"
    #include "ScLandscape.hpp"

namespace OpenRCT2::Scripting
{
    class ScEditor;
    extern ScEditor gScEditor;

    class ScEditor final : public ScBase
    {
    private:
        static void ThrowIfEditorIsNotActive(JSContext* ctx)
        {
            if (gLegacyScene != LegacyScene::scenarioEditor)
            {
                JS_ThrowPlainError(ctx, "Scenario editor not active.");
            }
        }

        static JSValue landscape_get(JSContext* ctx, JSValue)
        {
            ThrowIfEditorIsNotActive(ctx);
            return gScLandscape.New(ctx);
        }

    public:
        void Register(JSContext* ctx)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("landscape", landscape_get, nullptr),
            };
            RegisterBase(ctx, "Editor", nullptr, funcs);
        }

        JSValue New(JSContext* ctx)
        {
            return MakeWithOpaque(ctx, nullptr);
        }
    };

} // namespace OpenRCT2::Scripting

#endif
