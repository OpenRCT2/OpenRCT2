/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "ScAward.hpp"

    #include "../../../GameState.h"
    #include "../../../localisation/Formatter.h"
    #include "../../../localisation/Formatting.h"
    #include "../../../management/Award.h"
    #include "../../../windows/Intent.h"

namespace OpenRCT2::Scripting
{
    extern ScAward gScAward;

    using OpaqueAwardData = struct
    {
        size_t index;
    };

    JSValue ScAward::New(JSContext* ctx, size_t index)
    {
        return MakeWithOpaque(ctx, funcs, new OpaqueAwardData{ index });
    }

    void ScAward::Register(JSContext* ctx)
    {
        RegisterBaseStr(ctx, "Award", Finalize);
    }

    void ScAward::Finalize(JSRuntime* rt, JSValue thisVal)
    {
        OpaqueAwardData* data = gScAward.GetOpaque<OpaqueAwardData*>(thisVal);
        if (data)
            delete data;
    }

    Award* ScAward::GetAward(JSValue thisVal)
    {
        OpaqueAwardData* data = gScAward.GetOpaque<OpaqueAwardData*>(thisVal);
        return &getGameState().park.currentAwards[data->index];
    }

    JSValue ScAward::type_get(JSContext* ctx, JSValue thisVal)
    {
        auto award = GetAward(thisVal);
        if (award == nullptr)
            return JSFromStdString(ctx, {});

        return JSFromStdString(ctx, AwardTypeToString(award->Type).value_or(std::string()));
    }

    JSValue ScAward::text_get(JSContext* ctx, JSValue thisVal)
    {
        auto award = GetAward(thisVal);
        if (award == nullptr)
            return JSFromStdString(ctx, {});

        Formatter ft{};
        ft.Add<StringId>(AwardGetText(award->Type));
        return JSFromStdString(ctx, FormatStringIDLegacy(STR_STRINGID, ft.Data()));
    }

    JSValue ScAward::monthsRemaining_get(JSContext* ctx, JSValue thisVal)
    {
        auto award = GetAward(thisVal);
        if (award == nullptr)
            return JS_NewInt32(ctx, {});

        return JS_NewInt32(ctx, award->Time);
    }

    JSValue ScAward::positive_get(JSContext* ctx, JSValue thisVal)
    {
        auto award = GetAward(thisVal);
        if (award == nullptr)
            return JS_NewBool(ctx, {});

        return JS_NewBool(ctx, AwardIsPositive(award->Type));
    }

    JSValue ScAward::imageId_get(JSContext* ctx, JSValue thisVal)
    {
        auto award = GetAward(thisVal);
        if (award == nullptr)
            return JS_NewUint32(ctx, {});

        return JS_NewUint32(ctx, AwardGetSprite(award->Type));
    }

} // namespace OpenRCT2::Scripting

#endif
