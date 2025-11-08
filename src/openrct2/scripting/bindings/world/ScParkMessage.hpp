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

    #include "../../../Context.h"
    #include "../../../management/NewsItem.h"
    #include "../../ScriptEngine.h"

    #include <string>

namespace OpenRCT2::Scripting
{
    static constexpr const char* ParkMessageTypeStrings[] = {
        "attraction", "peep_on_attraction", "peep", "money", "blank", "research", "guests", "award", "chart", "campaign",
    };

    inline News::ItemType GetParkMessageType(const std::string& key)
    {
        // Get the first ItemType that appears in ParkMessageTypeStrings that isn't Null
        auto firstType = static_cast<uint8_t>(News::ItemType::ride);

        auto begin = std::begin(ParkMessageTypeStrings);
        auto end = std::end(ParkMessageTypeStrings);

        auto it = std::find(begin, end, key);
        return it != end ? static_cast<News::ItemType>(firstType + std::distance(begin, it)) : News::ItemType::blank;
    }

    inline std::string GetParkMessageType(News::ItemType type)
    {
        // Decrement 1 as ParkMessageTypeStrings doesn't contain the null type
        auto scriptType = static_cast<size_t>(type) - 1;

        if (scriptType < std::size(ParkMessageTypeStrings))
        {
            return ParkMessageTypeStrings[scriptType];
        }
        return {};
    }

    inline News::Item NewsItemFromJS(JSContext* ctx, JSValue value)
    {
        News::Item result{};
        result.type = GetParkMessageType(JSToStdString(ctx, value, "type"));
        result.assoc = JSToUint(ctx, value, "subject");
        result.ticks = JSToUint(ctx, value, "tickCount");
        result.monthYear = JSToUint(ctx, value, "month");
        result.day = JSToUint(ctx, value, "day");
        result.text = JSToStdString(ctx, value, "text");
        return result;
    }

    class ScParkMessage;
    extern ScParkMessage gScParkMessage;
    class ScParkMessage : public ScBase
    {
    private:
        static News::Item* GetMessage(JSValue thisVal);

        static JSValue isArchived_get(JSContext* ctx, JSValue thisVal);

        static JSValue month_get(JSContext* ctx, JSValue thisVal);
        static JSValue month_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue day_get(JSContext* ctx, JSValue thisVal);
        static JSValue day_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue tickCount_get(JSContext* ctx, JSValue thisVal);
        static JSValue tickCount_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue type_get(JSContext* ctx, JSValue thisVal);
        static JSValue type_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue subject_get(JSContext* ctx, JSValue thisVal);
        static JSValue subject_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue text_get(JSContext* ctx, JSValue thisVal);
        static JSValue text_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue remove(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

    public:
        JSValue New(JSContext* ctx, size_t index);

        void Register(JSContext* ctx);

    private:
        void Finalize(JSRuntime* rt, JSValue thisVal);
    };

} // namespace OpenRCT2::Scripting

#endif
