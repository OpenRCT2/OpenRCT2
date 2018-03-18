#include <algorithm>
#include <dukglue/dukglue.h>
#include "../common.h"
#include "../Context.h"
#include "../management/Finance.h"
#include "../management/NewsItem.h"
#include "../windows/Intent.h"
#include "../world/Park.h"

namespace OpenRCT2::Scripting
{
    class ScPark
    {
    public:
        money32 cash_get() { return gCash; }
        void cash_set(money32 value)
        {
            gCash = value;
            auto intent = Intent(INTENT_ACTION_UPDATE_CASH);
            context_broadcast_intent(&intent);
        }

        sint32 rating_get() { return gParkRating; }
        void rating_set(sint32 value)
        {
            gParkRating = std::min(std::max(0, value), 999);
            auto intent = Intent(INTENT_ACTION_UPDATE_PARK_RATING);
            context_broadcast_intent(&intent);
        }

        money32 bankLoan_get() { return gBankLoan; }
        void bankLoan_set(money32 value)
        {
            gBankLoan = value;
            auto intent = Intent(INTENT_ACTION_UPDATE_CASH);
            context_broadcast_intent(&intent);
        }

        money32 maxBankLoan_get() { return gMaxBankLoan; }
        void maxBankLoan_set(money32 value)
        {
            gMaxBankLoan = value;
            auto intent = Intent(INTENT_ACTION_UPDATE_CASH);
            context_broadcast_intent(&intent);
        }

        void postMessage(DukValue message)
        {
            try
            {
                uint8 type = NEWS_ITEM_BLANK;
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
                news_item_add_to_queue_raw(type, text.c_str(), -1);
            }
            catch (const std::exception&)
            {
            }
        }

        static void Register(duk_context * ctx)
        {
            dukglue_register_property(ctx, &ScPark::cash_get, &ScPark::cash_set, "cash");
            dukglue_register_property(ctx, &ScPark::rating_get, &ScPark::rating_set, "rating");
            dukglue_register_property(ctx, &ScPark::bankLoan_get, &ScPark::bankLoan_set, "bankLoan");
            dukglue_register_property(ctx, &ScPark::maxBankLoan_get, &ScPark::maxBankLoan_set, "maxBankLoan");
            dukglue_register_method(ctx, &ScPark::postMessage, "postMessage");
        }

    private:
        uint8 GetParkMessageType(const std::string& key)
        {
            static constexpr auto keys = {
                "attraction",
                "peep_on_attraction",
                "peep",
                "money",
                "blank",
                "research",
                "guests",
                "award",
                "chart" };
            uint8 i = 0;
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
}
