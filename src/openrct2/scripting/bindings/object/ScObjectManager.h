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
    #include "ScInstalledObject.hpp"
    #include "ScObject.hpp"

    #include <memory>

namespace OpenRCT2::Scripting
{
    class ScObjectManager;
    extern ScObjectManager gScObjectManager;

    class ScObjectManager final : public ScBase
    {
    public:
        void Register(JSContext* ctx);
        JSValue New(JSContext* ctx);

        static JSValue getObject(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
        static JSValue getAllObjects(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

    private:
        static JSValue installedObjects_get(JSContext* ctx, JSValue thisVal);
        static JSValue getInstalledObject(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue load(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
        static JSValue unload(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static void MarkAsResearched(const Object* object);
        static void RefreshResearchedItems();
        static JSValue CreateScObject(JSContext* ctx, ObjectType type, int32_t index);
    };
} // namespace OpenRCT2::Scripting

#endif
