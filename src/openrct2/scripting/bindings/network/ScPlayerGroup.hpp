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

namespace OpenRCT2::Scripting
{
    class ScPlayerGroup;
    extern ScPlayerGroup gScPlayerGroup;

    class ScPlayerGroup : public ScBase
    {
    private:
        static int32_t GetGroupId(JSValue thisVal);

        static JSValue id_get(JSContext* ctx, JSValue thisVal);

        static JSValue name_get(JSContext* ctx, JSValue thisVal);
        static JSValue name_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue permissions_get(JSContext* ctx, JSValue thisVal);
        static JSValue permissions_set(JSContext* ctx, JSValue thisVal, JSValue value);

    public:
        JSValue New(JSContext* ctx, int32_t id);

        void Register(JSContext* ctx);

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal);
    };

} // namespace OpenRCT2::Scripting

#endif
