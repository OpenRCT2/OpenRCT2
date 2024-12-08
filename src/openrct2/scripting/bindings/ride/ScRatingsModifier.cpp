/*****************************************************************************
 * Copyright (c) 2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScRatingsModifier.hpp"

    #include "../../../Context.h"
    #include "../../../ride/RideData.h"
    #include "../../Duktape.hpp"
    #include "../../ScriptEngine.h"
    #include "../object/ScObject.hpp"

namespace OpenRCT2::Scripting
{
    ScRatingsModifier::ScRatingsModifier(RatingsModifier ratingsModifier)
        : _ratingsModifier(ratingsModifier)
    {
    }

    uint8_t ScRatingsModifier::type_get() const
    {
        return EnumValue(_ratingsModifier.type);
    }

    int32_t ScRatingsModifier::threshold_get() const
    {
        return _ratingsModifier.threshold;
    }

    int32_t ScRatingsModifier::excitement_get() const
    {
        return _ratingsModifier.excitement;
    }

    int32_t ScRatingsModifier::intensity_get() const
    {
        return _ratingsModifier.intensity;
    }

    int32_t ScRatingsModifier::nausea_get() const
    {
        return _ratingsModifier.nausea;
    }

    void ScRatingsModifier::Register(duk_context* ctx)
    {
        dukglue_register_property(ctx, &ScRatingsModifier::type_get, nullptr, "type");
        dukglue_register_property(ctx, &ScRatingsModifier::threshold_get, nullptr, "threshold");
        dukglue_register_property(ctx, &ScRatingsModifier::excitement_get, nullptr, "excitement");
        dukglue_register_property(ctx, &ScRatingsModifier::intensity_get, nullptr, "intensity");
        dukglue_register_property(ctx, &ScRatingsModifier::nausea_get, nullptr, "nausea");
    }
} // namespace OpenRCT2::Scripting

#endif // ENABLE_SCRIPTING
