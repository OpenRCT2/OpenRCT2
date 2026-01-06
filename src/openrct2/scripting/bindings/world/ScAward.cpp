/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScAward.hpp"

    #include "../../../GameState.h"
    #include "../../../localisation/Formatter.h"
    #include "../../../localisation/Formatting.h"
    #include "../../../management/Award.h"
    #include "../../../windows/Intent.h"
    #include "../../Duktape.hpp"

namespace OpenRCT2::Scripting
{
    ScAward::ScAward(size_t index)
        : _index(index)
    {
    }

    void ScAward::Register(duk_context* ctx)
    {
        dukglue_register_property(ctx, &ScAward::type_get, nullptr, "type");
        dukglue_register_property(ctx, &ScAward::text_get, nullptr, "text");
        dukglue_register_property(ctx, &ScAward::positive_get, nullptr, "positive");
        dukglue_register_property(ctx, &ScAward::imageId_get, nullptr, "imageId");
        dukglue_register_property(ctx, &ScAward::monthsRemaining_get, nullptr, "monthsRemaining");
    }

    Award* ScAward::GetAward() const
    {
        return &getGameState().park.currentAwards[_index];
    }

    std::string ScAward::type_get() const
    {
        auto award = GetAward();
        if (award == nullptr)
            return {};

        return AwardTypeToString(award->Type).value_or(std::string());
    }

    std::string ScAward::text_get() const
    {
        auto award = GetAward();
        if (award == nullptr)
            return {};

        Formatter ft{};
        ft.Add<StringId>(AwardGetText(award->Type));
        return FormatStringIDLegacy(STR_STRINGID, ft.Data());
    }

    uint16_t ScAward::monthsRemaining_get() const
    {
        auto award = GetAward();
        if (award == nullptr)
            return {};

        return award->Time;
    }

    bool ScAward::positive_get() const
    {
        auto award = GetAward();
        if (award == nullptr)
            return {};

        return AwardIsPositive(award->Type);
    }

    uint32_t ScAward::imageId_get() const
    {
        auto award = GetAward();
        if (award == nullptr)
            return {};

        return AwardGetSprite(award->Type);
    }

} // namespace OpenRCT2::Scripting

#endif
