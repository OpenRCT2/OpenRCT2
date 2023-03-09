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
    class ScLanguage
    {
    public:
        ScLanguage();

        int32_t id_get() const;
        void id_set(int32_t value);

        std::string code_get() const;
        std::string englishName_get() const;
        std::string nativeName_get() const;
        int32_t fallback_get() const;
        bool isRtl_get() const;

        static void Register(duk_context* ctx);
    };

} // namespace OpenRCT2::Scripting

#endif
