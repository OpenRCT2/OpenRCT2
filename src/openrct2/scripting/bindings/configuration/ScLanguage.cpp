/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "ScLanguage.hpp"
#    include "../../../config/Config.h"
#    include "../../Duktape.hpp"

namespace OpenRCT2::Scripting
{
    ScLanguage::ScLanguage()
    {
    }

    int32_t ScLanguage::id_get() const
    {
        return gConfigGeneral.Language;
    }

    void ScLanguage::id_set(int32_t value)
    {
        if (value >= 0 && value < LANGUAGE_COUNT)
        {
            gConfigGeneral.Language = value;
        }
    }

    std::string ScLanguage::code_get() const
    {
        return LanguagesDescriptors[id_get()].locale;
    }

    std::string ScLanguage::englishName_get() const
    {
        return LanguagesDescriptors[id_get()].english_name;
    }

    std::string ScLanguage::nativeName_get() const
    {
        return LanguagesDescriptors[id_get()].native_name;
    }

    int32_t ScLanguage::fallback_get() const
    {
        return LanguagesDescriptors[id_get()].fallback;
    }

    bool ScLanguage::isRtl_get() const
    {
        return LanguagesDescriptors[id_get()].isRtl;
    }

    void ScLanguage::Register(duk_context* ctx)
    {
        dukglue_register_property(ctx, &ScLanguage::id_get, id_set, "id");
        dukglue_register_property(ctx, &ScLanguage::code_get, nullptr, "code");
        dukglue_register_property(ctx, &ScLanguage::englishName_get, nullptr, "englishName");
        dukglue_register_property(ctx, &ScLanguage::nativeName_get, nullptr, "nativeName");
        dukglue_register_property(ctx, &ScLanguage::fallback_get, nullptr, "fallback");
        dukglue_register_property(ctx, &ScLanguage::isRtl_get, nullptr, "isRtl");
    }
} // namespace OpenRCT2::Scripting

#endif
