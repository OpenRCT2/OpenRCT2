/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../Context.h"
#    include "../GameState.h"
#    include "../common.h"
#    include "../core/String.hpp"
#    include "../management/Finance.h"
#    include "../management/NewsItem.h"
#    include "../windows/Intent.h"
#    include "../world/Park.h"
#    include "Duktape.hpp"
#    include "ScriptEngine.h"

#    include <algorithm>

namespace OpenRCT2::Scripting
{
    static constexpr const char* ParkMessageTypeStrings[] = {
        "attraction", "peep_on_attraction", "peep", "money", "blank", "research", "guests", "award", "chart",
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

        auto text = language_convert_string(value["text"].as_string());
        String::Set(result.Text, sizeof(result.Text), text.c_str());
        return result;
    }

    class ScParkMessage
    {
    private:
        size_t _index{};

    public:
        ScParkMessage(size_t index)
            : _index(index)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScParkMessage::isArchived_get, nullptr, "isArchived");
            dukglue_register_property(ctx, &ScParkMessage::month_get, &ScParkMessage::month_set, "month");
            dukglue_register_property(ctx, &ScParkMessage::day_get, &ScParkMessage::day_set, "day");
            dukglue_register_property(ctx, &ScParkMessage::tickCount_get, &ScParkMessage::tickCount_set, "tickCount");
            dukglue_register_property(ctx, &ScParkMessage::type_get, &ScParkMessage::type_set, "type");
            dukglue_register_property(ctx, &ScParkMessage::subject_get, &ScParkMessage::subject_set, "subject");
            dukglue_register_property(ctx, &ScParkMessage::text_get, &ScParkMessage::text_set, "text");
            dukglue_register_method(ctx, &ScParkMessage::remove, "remove");
        }

    private:
        News::Item* GetMessage() const
        {
            return &gNewsItems[_index];
        }

        bool isArchived_get() const
        {
            return _index >= News::ItemHistoryStart;
        }

        uint16_t month_get() const
        {
            auto msg = GetMessage();
            if (msg != nullptr)
            {
                return msg->MonthYear;
            }
            return 0;
        }

        void month_set(uint16_t value)
        {
            ThrowIfGameStateNotMutable();
            auto msg = GetMessage();
            if (msg != nullptr)
            {
                msg->MonthYear = value;
            }
        }

        uint8_t day_get() const
        {
            auto msg = GetMessage();
            if (msg != nullptr)
            {
                return msg->Day;
            }
            return 0;
        }

        void day_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto msg = GetMessage();
            if (msg != nullptr)
            {
                msg->Day = value;
            }
        }

        uint16_t tickCount_get() const
        {
            auto msg = GetMessage();
            if (msg != nullptr)
            {
                return msg->Ticks;
            }
            return 0;
        }

        void tickCount_set(uint16_t value)
        {
            ThrowIfGameStateNotMutable();
            auto msg = GetMessage();
            if (msg != nullptr)
            {
                msg->Ticks = value;
            }
        }

        std::string type_get() const
        {
            auto msg = GetMessage();
            if (msg != nullptr)
            {
                return GetParkMessageType(msg->Type);
            }
            return {};
        }

        void type_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            auto msg = GetMessage();
            if (msg != nullptr)
            {
                msg->Type = GetParkMessageType(value);
            }
        }

        uint32_t subject_get() const
        {
            auto msg = GetMessage();
            if (msg != nullptr)
            {
                return msg->Assoc;
            }
            return 0;
        }

        void subject_set(uint32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto msg = GetMessage();
            if (msg != nullptr)
            {
                msg->Assoc = value;
            }
        }

        std::string text_get() const
        {
            auto msg = GetMessage();
            if (msg != nullptr)
            {
                return language_convert_string_to_tokens(msg->Text);
            }
            return 0;
        }

        void text_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            auto msg = GetMessage();
            if (msg != nullptr)
            {
                auto text = language_convert_string(value);
                String::Set(msg->Text, sizeof(msg->Text), text.c_str());
            }
        }

        void remove()
        {
            News::RemoveItem(static_cast<int32_t>(_index));
        }
    };

    class ScPark
    {
    public:
        money32 cash_get() const
        {
            return gCash;
        }
        void cash_set(money32 value)
        {
            ThrowIfGameStateNotMutable();
            gCash = value;
            auto intent = Intent(INTENT_ACTION_UPDATE_CASH);
            context_broadcast_intent(&intent);
        }

        int32_t rating_get() const
        {
            return gParkRating;
        }
        void rating_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            gParkRating = std::min(std::max(0, value), 999);
            auto intent = Intent(INTENT_ACTION_UPDATE_PARK_RATING);
            context_broadcast_intent(&intent);
        }

        money32 bankLoan_get() const
        {
            return gBankLoan;
        }
        void bankLoan_set(money32 value)
        {
            ThrowIfGameStateNotMutable();
            gBankLoan = value;
            auto intent = Intent(INTENT_ACTION_UPDATE_CASH);
            context_broadcast_intent(&intent);
        }

        money32 maxBankLoan_get() const
        {
            return gMaxBankLoan;
        }
        void maxBankLoan_set(money32 value)
        {
            ThrowIfGameStateNotMutable();
            gMaxBankLoan = value;
            auto intent = Intent(INTENT_ACTION_UPDATE_CASH);
            context_broadcast_intent(&intent);
        }

        money16 entranceFee_get() const
        {
            return gParkEntranceFee;
        }
        void entranceFee_set(money16 value)
        {
            ThrowIfGameStateNotMutable();
            gParkEntranceFee = value;
        }

        std::string name_get() const
        {
            return GetContext()->GetGameState()->GetPark().Name;
        }
        void name_set(std::string value)
        {
            ThrowIfGameStateNotMutable();
            GetContext()->GetGameState()->GetPark().Name = value;
        }

        std::vector<std::shared_ptr<ScParkMessage>> messages_get() const
        {
            std::vector<std::shared_ptr<ScParkMessage>> result;
            for (size_t i = 0, newsSize = gNewsItems.GetRecent().size(); i < newsSize; i++)
            {
                result.push_back(std::make_shared<ScParkMessage>(i));
            }
            for (size_t i = 0, newsSize = gNewsItems.GetArchived().size(); i < newsSize; i++)
            {
                result.push_back(std::make_shared<ScParkMessage>(i + News::ItemHistoryStart));
            }
            return result;
        }

        void messages_set(const std::vector<DukValue>& value)
        {
            int32_t index = 0;
            int32_t archiveIndex = News::ItemHistoryStart;
            for (const auto& item : value)
            {
                auto isArchived = item["isArchived"].as_bool();
                auto newsItem = FromDuk<News::Item>(item);
                if (isArchived)
                {
                    if (archiveIndex < News::MaxItems)
                    {
                        gNewsItems[archiveIndex] = newsItem;
                        archiveIndex++;
                    }
                }
                else
                {
                    if (index < News::ItemHistoryStart)
                    {
                        gNewsItems[index] = newsItem;
                        index++;
                    }
                }
            }

            // End the lists by setting next item to null
            if (index < News::ItemHistoryStart)
            {
                gNewsItems[index].Type = News::ItemType::Null;
            }
            if (archiveIndex < News::MaxItems)
            {
                gNewsItems[archiveIndex].Type = News::ItemType::Null;
            }
        }

        void postMessage(DukValue message)
        {
            ThrowIfGameStateNotMutable();
            try
            {
                uint32_t assoc = std::numeric_limits<uint32_t>::max();
                auto type = News::ItemType::Blank;
                std::string text;
                if (message.type() == DukValue::Type::STRING)
                {
                    text = language_convert_string(message.as_string());
                }
                else
                {
                    type = GetParkMessageType(message["type"].as_string());
                    text = language_convert_string(message["text"].as_string());
                    if (type == News::ItemType::Blank)
                    {
                        assoc = static_cast<uint32_t>(((COORDS_NULL & 0xFFFF) << 16) | (COORDS_NULL & 0xFFFF));
                    }

                    auto dukSubject = message["subject"];
                    if (dukSubject.type() == DukValue::Type::NUMBER)
                    {
                        assoc = static_cast<uint32_t>(dukSubject.as_int());
                    }
                }
                News::AddItemToQueue(type, text.c_str(), assoc);
            }
            catch (const DukException&)
            {
                duk_error(message.context(), DUK_ERR_ERROR, "Invalid message argument.");
            }
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScPark::cash_get, &ScPark::cash_set, "cash");
            dukglue_register_property(ctx, &ScPark::rating_get, &ScPark::rating_set, "rating");
            dukglue_register_property(ctx, &ScPark::bankLoan_get, &ScPark::bankLoan_set, "bankLoan");
            dukglue_register_property(ctx, &ScPark::maxBankLoan_get, &ScPark::maxBankLoan_set, "maxBankLoan");
            dukglue_register_property(ctx, &ScPark::entranceFee_get, &ScPark::entranceFee_set, "entranceFee");
            dukglue_register_property(ctx, &ScPark::name_get, &ScPark::name_set, "name");
            dukglue_register_property(ctx, &ScPark::messages_get, &ScPark::messages_set, "messages");
            dukglue_register_method(ctx, &ScPark::postMessage, "postMessage");
        }
    };
} // namespace OpenRCT2::Scripting

#endif
