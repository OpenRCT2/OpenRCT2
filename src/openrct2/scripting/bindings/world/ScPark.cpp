/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "ScPark.hpp"

#    include "../../../Context.h"
#    include "../../../GameState.h"
#    include "../../../common.h"
#    include "../../../core/String.hpp"
#    include "../../../entity/Guest.h"
#    include "../../../management/Finance.h"
#    include "../../../management/NewsItem.h"
#    include "../../../windows/Intent.h"
#    include "../../../world/Park.h"
#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"
#    include "ScParkMessage.hpp"

#    include <algorithm>

namespace OpenRCT2::Scripting
{
    static const DukEnumMap<uint64_t> ParkFlagMap({
        { "open", PARK_FLAGS_PARK_OPEN },
        { "scenarioCompleteNameInput", PARK_FLAGS_SCENARIO_COMPLETE_NAME_INPUT },
        { "forbidLandscapeChanges", PARK_FLAGS_FORBID_LANDSCAPE_CHANGES },
        { "forbidTreeRemoval", PARK_FLAGS_FORBID_TREE_REMOVAL },
        { "forbidHighConstruction", PARK_FLAGS_FORBID_HIGH_CONSTRUCTION },
        { "preferLessIntenseRides", PARK_FLAGS_PREF_LESS_INTENSE_RIDES },
        { "forbidMarketingCampaigns", PARK_FLAGS_FORBID_MARKETING_CAMPAIGN },
        { "preferMoreIntenseRides", PARK_FLAGS_PREF_MORE_INTENSE_RIDES },
        { "noMoney", PARK_FLAGS_NO_MONEY },
        { "difficultGuestGeneration", PARK_FLAGS_DIFFICULT_GUEST_GENERATION },
        { "freeParkEntry", PARK_FLAGS_PARK_FREE_ENTRY },
        { "difficultParkRating", PARK_FLAGS_DIFFICULT_PARK_RATING },
        { "unlockAllPrices", PARK_FLAGS_UNLOCK_ALL_PRICES },
    });

    money64 ScPark::cash_get() const
    {
        return gCash;
    }
    void ScPark::cash_set(money64 value)
    {
        ThrowIfGameStateNotMutable();

        if (gCash != value)
        {
            gCash = value;
            auto intent = Intent(INTENT_ACTION_UPDATE_CASH);
            context_broadcast_intent(&intent);
        }
    }

    int32_t ScPark::rating_get() const
    {
        return gParkRating;
    }
    void ScPark::rating_set(int32_t value)
    {
        ThrowIfGameStateNotMutable();

        auto valueClamped = std::min(std::max(0, value), 999);
        if (gParkRating != valueClamped)
        {
            gParkRating = std::min(std::max(0, value), 999);
            auto intent = Intent(INTENT_ACTION_UPDATE_PARK_RATING);
            context_broadcast_intent(&intent);
        }
    }

    money64 ScPark::bankLoan_get() const
    {
        return gBankLoan;
    }
    void ScPark::bankLoan_set(money64 value)
    {
        ThrowIfGameStateNotMutable();

        if (gBankLoan != value)
        {
            gBankLoan = value;
            auto intent = Intent(INTENT_ACTION_UPDATE_CASH);
            context_broadcast_intent(&intent);
        }
    }

    money64 ScPark::maxBankLoan_get() const
    {
        return gMaxBankLoan;
    }
    void ScPark::maxBankLoan_set(money64 value)
    {
        ThrowIfGameStateNotMutable();

        if (gMaxBankLoan != value)
        {
            gMaxBankLoan = value;
            auto intent = Intent(INTENT_ACTION_UPDATE_CASH);
            context_broadcast_intent(&intent);
        }
    }

    money16 ScPark::entranceFee_get() const
    {
        return gParkEntranceFee;
    }
    void ScPark::entranceFee_set(money16 value)
    {
        ThrowIfGameStateNotMutable();

        if (gParkEntranceFee != value)
        {
            gParkEntranceFee = value;
            window_invalidate_by_class(WC_PARK_INFORMATION);
        }
    }

    uint32_t ScPark::guests_get() const
    {
        return gNumGuestsInPark;
    }

    uint32_t ScPark::suggestedGuestMaximum_get() const
    {
        return _suggestedGuestMaximum;
    }

    int32_t ScPark::guestGenerationProbability_get() const
    {
        return _guestGenerationProbability;
    }

    money16 ScPark::guestInitialCash_get() const
    {
        return gGuestInitialCash;
    }

    uint8_t ScPark::guestInitialHappiness_get() const
    {
        return gGuestInitialHappiness;
    }

    uint8_t ScPark::guestInitialHunger_get() const
    {
        return gGuestInitialHunger;
    }

    uint8_t ScPark::guestInitialThirst_get() const
    {
        return gGuestInitialThirst;
    }

    money64 ScPark::value_get() const
    {
        return gParkValue;
    }
    void ScPark::value_set(money64 value)
    {
        ThrowIfGameStateNotMutable();

        if (gParkValue != value)
        {
            gParkValue = value;
            auto intent = Intent(INTENT_ACTION_UPDATE_CASH);
            context_broadcast_intent(&intent);
        }
    }

    money64 ScPark::companyValue_get() const
    {
        return gCompanyValue;
    }
    void ScPark::companyValue_set(money64 value)
    {
        ThrowIfGameStateNotMutable();

        if (gCompanyValue != value)
        {
            gCompanyValue = value;
            auto intent = Intent(INTENT_ACTION_UPDATE_CASH);
            context_broadcast_intent(&intent);
        }
    }

    money16 ScPark::totalRideValueForMoney_get() const
    {
        return gTotalRideValueForMoney;
    }

    uint32_t ScPark::totalAdmissions_get() const
    {
        return gTotalAdmissions;
    }
    void ScPark::totalAdmissions_set(uint32_t value)
    {
        ThrowIfGameStateNotMutable();

        if (gTotalAdmissions != value)
        {
            gTotalAdmissions = value;
            window_invalidate_by_class(WC_PARK_INFORMATION);
        }
    }

    money64 ScPark::totalIncomeFromAdmissions_get() const
    {
        return gTotalIncomeFromAdmissions;
    }
    void ScPark::totalIncomeFromAdmissions_set(money64 value)
    {
        ThrowIfGameStateNotMutable();

        if (gTotalIncomeFromAdmissions != value)
        {
            gTotalIncomeFromAdmissions = value;
            window_invalidate_by_class(WC_PARK_INFORMATION);
        }
    }

    money32 ScPark::landPrice_get() const
    {
        return gLandPrice;
    }
    void ScPark::landPrice_set(money32 value)
    {
        ThrowIfGameStateNotMutable();
        gLandPrice = value;
    }

    money32 ScPark::constructionRightsPrice_get() const
    {
        return gConstructionRightsPrice;
    }
    void ScPark::constructionRightsPrice_set(money32 value)
    {
        ThrowIfGameStateNotMutable();
        gConstructionRightsPrice = value;
    }

    int16_t ScPark::casualtyPenalty_get() const
    {
        return gParkRatingCasualtyPenalty;
    }
    void ScPark::casualtyPenalty_set(int16_t value)
    {
        ThrowIfGameStateNotMutable();
        gParkRatingCasualtyPenalty = value;
    }

    uint16_t ScPark::parkSize_get() const
    {
        return gParkSize;
    }

    std::string ScPark::name_get() const
    {
        return GetContext()->GetGameState()->GetPark().Name;
    }
    void ScPark::name_set(std::string value)
    {
        ThrowIfGameStateNotMutable();

        auto& park = GetContext()->GetGameState()->GetPark();
        if (park.Name != value)
        {
            park.Name = std::move(value);
            gfx_invalidate_screen();
        }
    }

    bool ScPark::getFlag(const std::string& key) const
    {
        auto mask = ParkFlagMap[key];
        return (gParkFlags & mask) != 0;
    }

    void ScPark::setFlag(const std::string& key, bool value)
    {
        ThrowIfGameStateNotMutable();
        auto mask = ParkFlagMap[key];
        if (value)
            gParkFlags |= mask;
        else
            gParkFlags &= ~mask;
        gfx_invalidate_screen();
    }

    std::vector<std::shared_ptr<ScParkMessage>> ScPark::messages_get() const
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

    void ScPark::messages_set(const std::vector<DukValue>& value)
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

    void ScPark::postMessage(DukValue message)
    {
        ThrowIfGameStateNotMutable();
        try
        {
            uint32_t assoc = std::numeric_limits<uint32_t>::max();
            auto type = News::ItemType::Blank;
            std::string text;
            if (message.type() == DukValue::Type::STRING)
            {
                text = message.as_string();
            }
            else
            {
                type = GetParkMessageType(message["type"].as_string());
                text = message["text"].as_string();
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

    void ScPark::Register(duk_context* ctx)
    {
        dukglue_register_property(ctx, &ScPark::cash_get, &ScPark::cash_set, "cash");
        dukglue_register_property(ctx, &ScPark::rating_get, &ScPark::rating_set, "rating");
        dukglue_register_property(ctx, &ScPark::bankLoan_get, &ScPark::bankLoan_set, "bankLoan");
        dukglue_register_property(ctx, &ScPark::maxBankLoan_get, &ScPark::maxBankLoan_set, "maxBankLoan");
        dukglue_register_property(ctx, &ScPark::entranceFee_get, &ScPark::entranceFee_set, "entranceFee");
        dukglue_register_property(ctx, &ScPark::guests_get, nullptr, "guests");
        dukglue_register_property(ctx, &ScPark::suggestedGuestMaximum_get, nullptr, "suggestedGuestMaximum");
        dukglue_register_property(ctx, &ScPark::guestGenerationProbability_get, nullptr, "guestGenerationProbability");
        dukglue_register_property(ctx, &ScPark::guestInitialCash_get, nullptr, "guestInitialCash");
        dukglue_register_property(ctx, &ScPark::guestInitialHappiness_get, nullptr, "guestInitialHappiness");
        dukglue_register_property(ctx, &ScPark::guestInitialHunger_get, nullptr, "guestInitialHunger");
        dukglue_register_property(ctx, &ScPark::guestInitialThirst_get, nullptr, "guestInitialThirst");
        dukglue_register_property(ctx, &ScPark::value_get, &ScPark::value_set, "value");
        dukglue_register_property(ctx, &ScPark::companyValue_get, &ScPark::companyValue_set, "companyValue");
        dukglue_register_property(ctx, &ScPark::totalRideValueForMoney_get, nullptr, "totalRideValueForMoney");
        dukglue_register_property(ctx, &ScPark::totalAdmissions_get, &ScPark::totalAdmissions_set, "totalAdmissions");
        dukglue_register_property(
            ctx, &ScPark::totalIncomeFromAdmissions_get, &ScPark::totalIncomeFromAdmissions_set, "totalIncomeFromAdmissions");
        dukglue_register_property(ctx, &ScPark::landPrice_get, &ScPark::landPrice_set, "landPrice");
        dukglue_register_property(
            ctx, &ScPark::constructionRightsPrice_get, &ScPark::constructionRightsPrice_set, "constructionRightsPrice");
        dukglue_register_property(ctx, &ScPark::parkSize_get, nullptr, "parkSize");
        dukglue_register_property(ctx, &ScPark::name_get, &ScPark::name_set, "name");
        dukglue_register_property(ctx, &ScPark::messages_get, &ScPark::messages_set, "messages");
        dukglue_register_property(ctx, &ScPark::casualtyPenalty_get, &ScPark::casualtyPenalty_set, "casualtyPenalty");
        dukglue_register_method(ctx, &ScPark::getFlag, "getFlag");
        dukglue_register_method(ctx, &ScPark::setFlag, "setFlag");
        dukglue_register_method(ctx, &ScPark::postMessage, "postMessage");
    }

} // namespace OpenRCT2::Scripting

#endif
