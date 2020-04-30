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
#    include "../common.h"
#    include "../management/Finance.h"
#    include "../management/NewsItem.h"
#    include "../windows/Intent.h"
#    include "../world/Park.h"
#    include "Duktape.hpp"
#    include "ScriptEngine.h"

#    include <algorithm>

namespace OpenRCT2::Scripting
{
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

        void postMessage(DukValue message)
        {
            ThrowIfGameStateNotMutable();
            try
            {
                uint32_t assoc = std::numeric_limits<uint32_t>::max();
                uint8_t type = NEWS_ITEM_BLANK;
                std::string text;
                if (message.type() == DukValue::Type::STRING)
                {
                    text = language_convert_string(message.as_string());
                }
                else
                {
                    type = GetParkMessageType(message["type"].as_string());
                    text = language_convert_string(message["text"].as_string());
                    if (type == NEWS_ITEM_BLANK)
                    {
                        assoc = static_cast<uint32_t>(((COORDS_NULL & 0xFFFF) << 16) | (COORDS_NULL & 0xFFFF));
                    }

                    auto dukSubject = message["subject"];
                    if (dukSubject.type() == DukValue::Type::NUMBER)
                    {
                        assoc = static_cast<uint32_t>(dukSubject.as_int());
                    }
                }
                news_item_add_to_queue_raw(type, text.c_str(), assoc);
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
            dukglue_register_method(ctx, &ScPark::postMessage, "postMessage");
        }

    private:
        static uint8_t GetParkMessageType(const std::string& key)
        {
            static auto keys = { "attraction", "peep_on_attraction", "peep", "money", "blank", "research", "guests", "award",
                                 "chart" };
            uint8_t i = 0;
            for (const auto& k : keys)
            {
                if (k == key)
                {
                    return NEWS_ITEM_RIDE + i;
                }
                i++;
            }
            return NEWS_ITEM_BLANK;
        }
    };
} // namespace OpenRCT2::Scripting

#endif
