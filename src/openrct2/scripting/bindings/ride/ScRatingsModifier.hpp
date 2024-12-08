/*****************************************************************************
 * Copyright (c) 2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "../../../Context.h"
    #include "../../../ride/RideData.h"
    #include "../../Duktape.hpp"
    #include "../../ScriptEngine.h"
    #include "../object/ScObject.hpp"

namespace OpenRCT2::Scripting
{
    class ScRatingsModifier
    {
    private:
        RatingsModifier _ratingsModifier;

    public:
        ScRatingsModifier(RatingsModifier ratingsModifier);

    private:
        uint8_t type_get() const;
        int32_t threshold_get() const;
        int32_t excitement_get() const;
        int32_t intensity_get() const;
        int32_t nausea_get() const;

    public:
        static void Register(duk_context* ctx);
    };

} // namespace OpenRCT2::Scripting

#endif // ENABLE_SCRIPTING
