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

    #include "../../../world/map_generator/SimplexNoise.h"
    #include "../../ScriptEngine.h"
    #include "ScNoiseFn.hpp"

namespace OpenRCT2::Scripting
{
    class ScLandscape;
    extern ScLandscape gScLandscape;

    class ScLandscape final : public ScBase
    {
    private:
        static JSValue getSimplexNoiseFn(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            if (argc > 0 && JS_IsNumber(argv[0]))
            {
                uint32_t seed = JSToUint(ctx, argv[0]);
                return gScNoiseFn.New(ctx, new World::MapGenerator::SimplexNoise(seed));
            }
            return gScNoiseFn.New(ctx, new World::MapGenerator::SimplexNoise());
        }

        static JSValue getSimplexFbmNoiseFn(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            float frequency = static_cast<float>(JSToFloat64(ctx, argv[0]));
            int32_t octaves = JSToInt(ctx, argv[1]);
            float lacunarity = static_cast<float>(JSToFloat64(ctx, argv[2]));
            float persistence = static_cast<float>(JSToFloat64(ctx, argv[3]));

            if (argc > 4 && JS_IsNumber(argv[4]))
            {
                uint32_t seed = JSToUint(ctx, argv[4]);
                return gScNoiseFn.New(
                    ctx, new World::MapGenerator::SimplexFbmNoise(seed, frequency, octaves, lacunarity, persistence));
            }
            return gScNoiseFn.New(ctx, new World::MapGenerator::SimplexFbmNoise(frequency, octaves, lacunarity, persistence));
        }

    public:
        void Register(JSContext* ctx)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CFUNC_DEF("getSimplexNoiseFn", 0, getSimplexNoiseFn),
                JS_CFUNC_DEF("getSimplexFbmNoiseFn", 4, getSimplexFbmNoiseFn),
            };
            RegisterBase(ctx, "Landscape", nullptr, funcs);
        }

        JSValue New(JSContext* ctx)
        {
            return MakeWithOpaque(ctx, nullptr);
        }
    };

} // namespace OpenRCT2::Scripting

#endif
