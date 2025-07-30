/*****************************************************************************
 * Copyright (c) 2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScLandscape.h"

    #include "../../../world/map_generator/SimplexNoise.h"

namespace OpenRCT2::Scripting
{
    std::shared_ptr<ScNoiseFn> ScLandscape::getSimplexNoiseFn(DukValue maybeSeed)
    {
        if (maybeSeed.type() == DukValue::NUMBER)
        {
            return std::make_shared<ScNoiseFn>(std::make_unique<World::MapGenerator::SimplexNoise>(maybeSeed.as_uint()));
        }
        return std::make_shared<ScNoiseFn>(std::make_unique<World::MapGenerator::SimplexNoise>());
    }

    std::shared_ptr<ScNoiseFn> ScLandscape::getSimplexFbmNoiseFn(
        float frequency, int32_t octaves, float lacunarity, float persistence, DukValue maybeSeed)
    {
        if (maybeSeed.type() == DukValue::NUMBER)
        {
            return std::make_shared<ScNoiseFn>(std::make_unique<World::MapGenerator::SimplexFbmNoise>(
                maybeSeed.as_uint(), frequency, octaves, lacunarity, persistence));
        }
        return std::make_shared<ScNoiseFn>(
            std::make_unique<World::MapGenerator::SimplexFbmNoise>(frequency, octaves, lacunarity, persistence));
    }

    void ScLandscape::Register(duk_context* ctx)
    {
        dukglue_register_method(ctx, &ScLandscape::getSimplexNoiseFn, "getSimplexNoiseFn");
        dukglue_register_method(ctx, &ScLandscape::getSimplexFbmNoiseFn, "getSimplexFbmNoiseFn");
    }

} // namespace OpenRCT2::Scripting
#endif
