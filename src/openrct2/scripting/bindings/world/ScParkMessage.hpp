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

#    include "../../../Context.h"
#    include "../../../common.h"
#    include "../../../management/NewsItem.h"
#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"

#    include <algorithm>
#    include <string>

namespace OpenRCT2::Scripting
{
    static constexpr const char* ParkMessageTypeStrings[] = {
        "attraction", "peep_on_attraction", "peep", "money", "blank", "research", "guests", "award", "chart", "campaign",
    };

    inline News::ItemType GetParkMessageType(const std::string& key)
    {
        // Get the first ItemType that appears in ParkMessageTypeStrings that isn't Null
        auto firstType = static_cast<uint8_t>(News::ItemType::Ride);

        auto begin = std::begin(ParkMessageTypeStrings);
        auto end = std::end(ParkMessageTypeStrings);

        auto it = std::find(begin, end, key);
        return it != end ? static_cast<News::ItemType>(firstType + std::distance(begin, it)) : News::ItemType::Blank;
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

    template<> inline News::Item FromDuk(const DukValue& value)
    {
        News::Item result{};
        result.Type = GetParkMessageType(value["type"].as_string());
        result.Assoc = value["subject"].as_int();
        result.Ticks = value["tickCount"].as_int();
        result.MonthYear = value["month"].as_int();
        result.Day = value["day"].as_int();
        result.Text = value["text"].as_string();
        return result;
    }

    class ScParkMessage
    {
    private:
        size_t _index{};

    public:
        ScParkMessage(size_t index);

        static void Register(duk_context* ctx);

    private:
        News::Item* GetMessage() const;

        bool isArchived_get() const;

        uint16_t month_get() const;
        void month_set(uint16_t value);

        uint8_t day_get() const;
        void day_set(uint8_t value);

        uint16_t tickCount_get() const;
        void tickCount_set(uint16_t value);

        std::string type_get() const;
        void type_set(const std::string& value);

        uint32_t subject_get() const;
        void subject_set(uint32_t value);

        std::string text_get() const;
        void text_set(const std::string& value);

        void remove();
    };

} // namespace OpenRCT2::Scripting

#endif
