/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Context.h"
#include "../common.h"
#include "../management/Finance.h"
#include "../management/NewsItem.h"
#include "../windows/Intent.h"
#include "../world/Park.h"
#include "Duktape.hpp"
#include "ScriptEngine.h"

#include <algorithm>

namespace OpenRCT2::Scripting
{
    class ScPark
    {
    public:
        money32 cash_get()
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

        int32_t rating_get()
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

        money32 bankLoan_get()
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

        money32 maxBankLoan_get()
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
                uint8_t type = NEWS_ITEM_BLANK;
                std::string text;
                if (message.type() == DukValue::Type::STRING)
                {
                    text = message.as_string();
                }
                else
                {
                    type = GetParkMessageType(message["type"].as_string());
                    text = message["text"].as_string();
                }
                news_item_add_to_queue_raw(type, text.c_str(), static_cast<uint32_t>(-1));
            }
            catch (const std::exception&)
            {
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
        uint8_t GetParkMessageType(const std::string& key)
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
