/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../../Duktape.hpp"

namespace OpenRCT2::Scripting
{
    class ScLocalisation
    {
    public:
        ScLocalisation();

//        DukValue language_get() const;

        bool showHeightAsUnits_get() const;
        void showHeightAsUnits_set(bool value);

        std::string measurementFormat_get() const;
        void measurementFormat_set(std::string value);

        std::string temperatureFormat_get() const;
        void temperatureFormat_set(std::string value);

        std::string dateFormat_get() const;
        void dateFormat_set(std::string value);

        static void Register(duk_context* ctx);
    };

} // namespace OpenRCT2::Scripting

#endif
