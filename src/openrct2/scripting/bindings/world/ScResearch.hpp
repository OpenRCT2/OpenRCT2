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

    #include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    class ScResearch;
    extern ScResearch gScResearch;

    class ScResearch final : public ScBase
    {
    public:
        void Register(JSContext* ctx);
        JSValue New(JSContext* ctx);

    private:
        static JSValue funding_get(JSContext* ctx, JSValue thisVal);
        static JSValue funding_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue priorities_get(JSContext* ctx, JSValue thisVal);
        static JSValue priorities_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue stage_get(JSContext* ctx, JSValue thisVal);
        static JSValue stage_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue progress_get(JSContext* ctx, JSValue thisVal);
        static JSValue progress_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue expectedMonth_get(JSContext* ctx, JSValue thisVal);
        static JSValue expectedDay_get(JSContext* ctx, JSValue thisVal);
        static JSValue lastResearchedItem_get(JSContext* ctx, JSValue thisVal);
        static JSValue expectedItem_get(JSContext* ctx, JSValue thisVal);

        static JSValue inventedItems_get(JSContext* ctx, JSValue thisVal);
        static JSValue inventedItems_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue uninventedItems_get(JSContext* ctx, JSValue thisVal);
        static JSValue uninventedItems_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue isObjectResearched(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static std::vector<ResearchItem> ConvertResearchList(JSContext* ctx, JSValue value);
    };

} // namespace OpenRCT2::Scripting

#endif
