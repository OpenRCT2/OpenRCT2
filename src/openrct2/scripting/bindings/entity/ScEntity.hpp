/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "../../../Context.h"
    #include "../../../GameState.h"
    #include "../../../entity/EntityRegistry.h"
    #include "../../../entity/Peep.h"
    #include "../../ScriptEngine.h"

    #include <string_view>
    #include <unordered_map>

namespace OpenRCT2::Scripting
{
    class ScEntity;
    extern ScEntity gScEntity;

    class ScEntity : public ScBase
    {
    private:
        static JSValue id_get(JSContext* ctx, JSValue thisVal);
        static JSValue type_get(JSContext* ctx, JSValue thisVal);

        // x getter and setter
        static JSValue x_get(JSContext* ctx, JSValue thisVal);
        static JSValue x_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        // y getter and setter
        static JSValue y_get(JSContext* ctx, JSValue thisVal);
        static JSValue y_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        // z getter and setter
        static JSValue z_get(JSContext* ctx, JSValue thisVal);
        static JSValue z_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue remove(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

    protected:
        static EntityId GetEntityId(JSValue thisVal);
        static EntityBase* GetEntity(JSValue thisVal);

    public:
        JSValue NewInstance(JSContext* ctx, EntityId entityId);
        static JSValue New(JSContext* ctx, EntityId entityId);

        void Register(JSContext* ctx);

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal);
    };

} // namespace OpenRCT2::Scripting

#endif
