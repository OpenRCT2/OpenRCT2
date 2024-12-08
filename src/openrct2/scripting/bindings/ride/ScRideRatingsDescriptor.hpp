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
    #include "ScRatingsModifier.hpp"

namespace OpenRCT2::Scripting
{
    class ScRideRatingsDescriptor
    {
    private:
        RideRatingsDescriptor _rideRatingsDescriptor;

    public:
        ScRideRatingsDescriptor(RideRatingsDescriptor rideRatingsDescriptor);

    private:
        uint8_t type_get() const;
        uint8_t unreliability_get() const;
        int8_t rideShelter_get() const;
        bool relaxRequirementsIfInversions_get() const;
        std::vector<std::shared_ptr<ScRatingsModifier>> modifiers_get() const;

    public:
        static void Register(duk_context* ctx);
    };

} // namespace OpenRCT2::Scripting

#endif // ENABLE_SCRIPTING
