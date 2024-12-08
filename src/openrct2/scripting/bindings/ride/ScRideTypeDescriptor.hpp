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
    #include "ScRideRatingsDescriptor.hpp"

namespace OpenRCT2::Scripting
{
    class ScRideTypeDescriptor
    {
    private:
        RideTypeDescriptor _rideTypeDescriptor;

    public:
        ScRideTypeDescriptor(RideTypeDescriptor rideTypeDescriptor);

    private:
        uint8_t category_get() const;
        uint16_t startTrackPiece_get() const;
        uint8_t defaultMode_get() const;
        uint8_t availableBreakdowns_get() const;
        uint8_t maxMass_get() const;
        uint8_t bonusValue_get() const;
        std::string name_get() const;

        std::shared_ptr<ScRideRatingsDescriptor> ratingsData_get();

    public:
        static void Register(duk_context* ctx);
    };

} // namespace OpenRCT2::Scripting

#endif // ENABLE_SCRIPTING
