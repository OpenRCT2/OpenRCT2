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
    #include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    class ScNoiseFn;
    extern ScNoiseFn gScNoiseFn;

    class ScNoiseFn final : public ScBase
    {
    private:
        static JSValue generate(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            float x = static_cast<float>(JSToFloat64(ctx, argv[0]));
            float y = static_cast<float>(JSToFloat64(ctx, argv[1]));

            auto fn = gScNoiseFn.GetOpaque<World::MapGenerator::Noise*>(thisVal);
            float value = fn->Generate(x, y);
            return JS_NewFloat64(ctx, value);
        }

    public:
        JSValue New(JSContext* ctx, World::MapGenerator::Noise* noiseFn)
        {
            return MakeWithOpaque(ctx, noiseFn);
        }

        void Register(JSContext* ctx)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CFUNC_DEF("generate", 2, generate),
            };
            RegisterBase(ctx, "NoiseFn", Finalize, funcs);
        }

        static void Finalize(JSRuntime* rt, JSValue thisVal)
        {
            auto fn = gScNoiseFn.GetOpaque<World::MapGenerator::Noise*>(thisVal);
            if (fn)
            {
                delete fn;
            }
        }
    };
} // namespace OpenRCT2::Scripting
#endif
