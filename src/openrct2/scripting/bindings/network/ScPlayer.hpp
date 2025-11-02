/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "../../ScriptEngine.h"
    #include "quickjs.h"

namespace OpenRCT2::Scripting
{
    class ScPlayer;
    extern ScPlayer gScPlayer;

    class ScPlayer : public ScBase
    {
    private:
        static int32_t GetPlayerId(JSValue thisVal);

        static JSValue id_get(JSContext* ctx, JSValue thisVal);

        static JSValue name_get(JSContext* ctx, JSValue thisVal);

        static JSValue group_get(JSContext* ctx, JSValue thisVal);
        static JSValue group_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue ping_get(JSContext* ctx, JSValue thisVal);

        static JSValue commandsRan_get(JSContext* ctx, JSValue thisVal);

        static JSValue moneySpent_get(JSContext* ctx, JSValue thisVal);

        static JSValue ipAddress_get(JSContext* ctx, JSValue thisVal);

        static JSValue publicKeyHash_get(JSContext* ctx, JSValue thisVal);

    public:
        JSValue New(JSContext* ctx, int32_t id);

        void Register(JSContext* ctx);

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal);
    };

} // namespace OpenRCT2::Scripting

#endif
