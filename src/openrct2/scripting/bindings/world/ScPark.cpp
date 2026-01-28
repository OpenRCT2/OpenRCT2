/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScPark.hpp"

    #include "../../../Context.h"
    #include "../../../Date.h"
    #include "../../../GameState.h"
    #include "../../../core/String.hpp"
    #include "../../../drawing/Drawing.h"
    #include "../../../entity/Guest.h"
    #include "../../../management/Finance.h"
    #include "../../../management/NewsItem.h"
    #include "../../../ui/WindowManager.h"
    #include "../../../windows/Intent.h"
    #include "../../../world/Park.h"
    #include "../../Duktape.hpp"
    #include "../../ScriptEngine.h"
    #include "../entity/ScGuest.hpp"
    #include "ScParkMessage.hpp"

namespace OpenRCT2::Scripting
{
    static const DukEnumMap<uint64_t> ParkFlagMap(
        {
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

    ScPark::ScPark(duk_context* ctx)
        : _context(ctx)
    {
    }

    money64 ScPark::cash_get() const
    {
        const auto& gameState = getGameState();
        return getPlayerPark(gameState).cash;
    }
    void ScPark::cash_set(money64 value)
    {
        ThrowIfGameStateNotMutable();

        auto& gameState = getGameState();
        auto& park = getPlayerPark(gameState);
        if (park.cash != value)
        {
            park.cash = value;
            auto intent = Intent(INTENT_ACTION_UPDATE_CASH);
            ContextBroadcastIntent(&intent);
        }
    }

    int32_t ScPark::rating_get() const
    {
        const auto& gameState = getGameState();
        return getPlayerPark(gameState).rating;
    }
    void ScPark::rating_set(int32_t value)
    {
        ThrowIfGameStateNotMutable();

        auto valueClamped = std::min(std::max(0, value), 999);

        auto& gameState = getGameState();
        auto& park = getPlayerPark(gameState);
        if (park.rating != valueClamped)
        {
            park.rating = std::min(std::max(0, value), 999);
            auto intent = Intent(INTENT_ACTION_UPDATE_PARK_RATING);
            ContextBroadcastIntent(&intent);
        }
    }

    money64 ScPark::bankLoan_get() const
    {
        const auto& gameState = getGameState();
        return getPlayerPark(gameState).bankLoan;
    }
    void ScPark::bankLoan_set(money64 value)
    {
        ThrowIfGameStateNotMutable();

        auto& gameState = getGameState();
        auto& park = getPlayerPark(gameState);
        if (park.bankLoan != value)
        {
            park.bankLoan = value;
            auto intent = Intent(INTENT_ACTION_UPDATE_CASH);
            ContextBroadcastIntent(&intent);
        }
    }

    money64 ScPark::maxBankLoan_get() const
    {
        const auto& gameState = getGameState();
        return getPlayerPark(gameState).maxBankLoan;
    }
    void ScPark::maxBankLoan_set(money64 value)
    {
        ThrowIfGameStateNotMutable();

        auto& gameState = getGameState();
        auto& park = getPlayerPark(gameState);
        if (park.maxBankLoan != value)
        {
            park.maxBankLoan = value;
            auto intent = Intent(INTENT_ACTION_UPDATE_CASH);
            ContextBroadcastIntent(&intent);
        }
    }

    money64 ScPark::entranceFee_get() const
    {
        const auto& gameState = getGameState();
        return getPlayerPark(gameState).entranceFee;
    }
    void ScPark::entranceFee_set(money64 value)
    {
        ThrowIfGameStateNotMutable();

        auto& gameState = getGameState();
        auto& park = getPlayerPark(gameState);
        if (park.entranceFee != value)
        {
            park.entranceFee = value;
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->InvalidateByClass(WindowClass::parkInformation);
        }
    }

    uint32_t ScPark::guests_get() const
    {
        const auto& gameState = getGameState();
        return getPlayerPark(gameState).numGuestsInPark;
    }

    uint32_t ScPark::suggestedGuestMaximum_get() const
    {
        const auto& gameState = getGameState();
        return getPlayerPark(gameState).suggestedGuestMaximum;
    }

    int32_t ScPark::guestGenerationProbability_get() const
    {
        const auto& gameState = getGameState();
        return getPlayerPark(gameState).guestGenerationProbability;
    }

    DukValue ScPark::generateGuest()
    {
        ThrowIfGameStateNotMutable();
        auto guest = Park::GenerateGuest();
        return GetObjectAsDukValue(_context, std::make_shared<ScGuest>(guest->Id));
    }

    money64 ScPark::guestInitialCash_get() const
    {
        return getGameState().scenarioOptions.guestInitialCash;
    }

    uint8_t ScPark::guestInitialHappiness_get() const
    {
        return getGameState().scenarioOptions.guestInitialHappiness;
    }

    uint8_t ScPark::guestInitialHunger_get() const
    {
        return getGameState().scenarioOptions.guestInitialHunger;
    }

    uint8_t ScPark::guestInitialThirst_get() const
    {
        return getGameState().scenarioOptions.guestInitialThirst;
    }

    money64 ScPark::value_get() const
    {
        const auto& gameState = getGameState();
        return getPlayerPark(gameState).value;
    }
    void ScPark::value_set(money64 value)
    {
        ThrowIfGameStateNotMutable();

        auto& gameState = getGameState();
        auto& park = getPlayerPark(gameState);
        if (park.value != value)
        {
            park.value = value;
            auto intent = Intent(INTENT_ACTION_UPDATE_CASH);
            ContextBroadcastIntent(&intent);
        }
    }

    money64 ScPark::companyValue_get() const
    {
        const auto& gameState = getGameState();
        return getPlayerPark(gameState).companyValue;
    }
    void ScPark::companyValue_set(money64 value)
    {
        ThrowIfGameStateNotMutable();

        auto& gameState = getGameState();
        auto& park = getPlayerPark(gameState);
        if (park.companyValue != value)
        {
            park.companyValue = value;
            auto intent = Intent(INTENT_ACTION_UPDATE_CASH);
            ContextBroadcastIntent(&intent);
        }
    }

    money64 ScPark::totalRideValueForMoney_get() const
    {
        const auto& gameState = getGameState();
        return getPlayerPark(gameState).totalRideValueForMoney;
    }

    uint32_t ScPark::totalAdmissions_get() const
    {
        const auto& gameState = getGameState();
        return getPlayerPark(gameState).totalAdmissions;
    }
    void ScPark::totalAdmissions_set(uint32_t value)
    {
        ThrowIfGameStateNotMutable();

        auto& gameState = getGameState();
        auto& park = getPlayerPark(gameState);
        if (park.totalAdmissions != value)
        {
            park.totalAdmissions = value;
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->InvalidateByClass(WindowClass::parkInformation);
        }
    }

    money64 ScPark::totalIncomeFromAdmissions_get() const
    {
        const auto& gameState = getGameState();
        return getPlayerPark(gameState).totalIncomeFromAdmissions;
    }
    void ScPark::totalIncomeFromAdmissions_set(money64 value)
    {
        ThrowIfGameStateNotMutable();

        auto& gameState = getGameState();
        auto& park = getPlayerPark(gameState);
        if (park.totalIncomeFromAdmissions != value)
        {
            park.totalIncomeFromAdmissions = value;
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->InvalidateByClass(WindowClass::parkInformation);
        }
    }

    money64 ScPark::landPrice_get() const
    {
        return getGameState().scenarioOptions.landPrice;
    }
    void ScPark::landPrice_set(money64 value)
    {
        ThrowIfGameStateNotMutable();
        getGameState().scenarioOptions.landPrice = value;
    }

    money64 ScPark::constructionRightsPrice_get() const
    {
        return getGameState().scenarioOptions.constructionRightsPrice;
    }
    void ScPark::constructionRightsPrice_set(money64 value)
    {
        ThrowIfGameStateNotMutable();
        getGameState().scenarioOptions.constructionRightsPrice = value;
    }

    int16_t ScPark::casualtyPenalty_get() const
    {
        const auto& gameState = getGameState();
        return getPlayerPark(gameState).ratingCasualtyPenalty;
    }
    void ScPark::casualtyPenalty_set(int16_t value)
    {
        ThrowIfGameStateNotMutable();

        auto& gameState = getGameState();
        getPlayerPark(gameState).ratingCasualtyPenalty = value;
    }

    uint16_t ScPark::parkSize_get() const
    {
        const auto& gameState = getGameState();
        return getPlayerPark(gameState).size;
    }

    std::string ScPark::name_get() const
    {
        const auto& gameState = getGameState();
        return getPlayerPark(gameState).name;
    }
    void ScPark::name_set(std::string value)
    {
        ThrowIfGameStateNotMutable();

        auto& gameState = getGameState();
        auto& park = getPlayerPark(gameState);
        if (park.name != value)
        {
            park.name = std::move(value);
            GfxInvalidateScreen();
        }
    }

    bool ScPark::getFlag(const std::string& key) const
    {
        auto mask = ParkFlagMap[key];

        const auto& gameState = getGameState();
        return (getPlayerPark(gameState).flags & mask) != 0;
    }

    void ScPark::setFlag(const std::string& key, bool value)
    {
        ThrowIfGameStateNotMutable();
        auto mask = ParkFlagMap[key];

        auto& gameState = getGameState();
        auto& park = getPlayerPark(gameState);
        if (value)
            park.flags |= mask;
        else
            park.flags &= ~mask;

        GfxInvalidateScreen();
    }

    std::shared_ptr<ScResearch> ScPark::research_get() const
    {
        return std::make_shared<ScResearch>(_context);
    }

    std::vector<std::shared_ptr<ScParkMessage>> ScPark::messages_get() const
    {
        std::vector<std::shared_ptr<ScParkMessage>> result;
        auto& gameState = getGameState();
        for (size_t i = 0, newsSize = gameState.newsItems.GetRecent().size(); i < newsSize; i++)
        {
            result.push_back(std::make_shared<ScParkMessage>(i));
        }
        for (size_t i = 0, newsSize = gameState.newsItems.GetArchived().size(); i < newsSize; i++)
        {
            result.push_back(std::make_shared<ScParkMessage>(i + News::ItemHistoryStart));
        }
        return result;
    }

    void ScPark::messages_set(const std::vector<DukValue>& value)
    {
        int32_t index = 0;
        int32_t archiveIndex = News::ItemHistoryStart;
        auto& gameState = getGameState();
        for (const auto& item : value)
        {
            auto isArchived = item["isArchived"].as_bool();
            auto newsItem = FromDuk<News::Item>(item);
            if (isArchived)
            {
                if (archiveIndex < News::MaxItems)
                {
                    gameState.newsItems[archiveIndex] = newsItem;
                    archiveIndex++;
                }
            }
            else
            {
                if (index < News::ItemHistoryStart)
                {
                    gameState.newsItems[index] = newsItem;
                    index++;
                }
            }
        }

        // End the lists by setting next item to null
        if (index < News::ItemHistoryStart)
        {
            gameState.newsItems[index].type = News::ItemType::null;
        }
        if (archiveIndex < News::MaxItems)
        {
            gameState.newsItems[archiveIndex].type = News::ItemType::null;
        }
    }

    void ScPark::postMessage(DukValue message)
    {
        ThrowIfGameStateNotMutable();
        try
        {
            uint32_t assoc = std::numeric_limits<uint32_t>::max();
            auto type = News::ItemType::blank;
            std::string text;
            if (message.type() == DukValue::Type::STRING)
            {
                text = message.as_string();
            }
            else
            {
                type = GetParkMessageType(message["type"].as_string());
                text = message["text"].as_string();
                if (type == News::ItemType::blank)
                {
                    assoc = static_cast<uint32_t>(((kCoordsNull & 0xFFFF) << 16) | (kCoordsNull & 0xFFFF));
                }

                auto dukSubject = message["subject"];
                if (dukSubject.type() == DukValue::Type::NUMBER)
                {
                    assoc = static_cast<uint32_t>(dukSubject.as_uint());
                }
            }
            News::AddItemToQueue(type, text.c_str(), assoc);
        }
        catch (const DukException&)
        {
            duk_error(message.context(), DUK_ERR_ERROR, "Invalid message argument.");
        }
    }

    std::vector<int32_t> ScPark::getMonthlyExpenditure(const std::string& expenditureType) const
    {
        auto recordedMonths = std::clamp(
            GetDate().GetMonthsElapsed() + 1, static_cast<uint32_t>(0), static_cast<uint32_t>(kExpenditureTableMonthCount));
        std::vector<int32_t> result(recordedMonths, 0);
        auto type = ScriptEngine::StringToExpenditureType(expenditureType);
        if (type != ExpenditureType::count)
        {
            const auto& gameState = getGameState();
            for (size_t i = 0; i < recordedMonths; ++i)
            {
                result[i] = getPlayerPark(gameState).expenditureTable[i][EnumValue(type)];
            }
        }
        return result;
    }

    std::vector<std::shared_ptr<ScAward>> ScPark::awards_get() const
    {
        std::vector<std::shared_ptr<ScAward>> result;

        const auto& gameState = getGameState();
        for (size_t i = 0; i < getPlayerPark(gameState).currentAwards.size(); i++)
        {
            result.push_back(std::make_shared<ScAward>(i));
        }

        return result;
    }

    void ScPark::clearAwards() const
    {
        ThrowIfGameStateNotMutable();
        AwardReset();
    }

    void ScPark::grantAward(const std::string& awardType) const
    {
        ThrowIfGameStateNotMutable();
        auto optType = StringToAwardType(awardType);
        if (optType.has_value())
        {
            AwardGrant(optType.value());
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
        dukglue_register_method(ctx, &ScPark::generateGuest, "generateGuest");
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
        dukglue_register_property(ctx, &ScPark::research_get, nullptr, "research");
        dukglue_register_property(ctx, &ScPark::messages_get, &ScPark::messages_set, "messages");
        dukglue_register_property(ctx, &ScPark::casualtyPenalty_get, &ScPark::casualtyPenalty_set, "casualtyPenalty");
        dukglue_register_method(ctx, &ScPark::getFlag, "getFlag");
        dukglue_register_method(ctx, &ScPark::setFlag, "setFlag");
        dukglue_register_method(ctx, &ScPark::postMessage, "postMessage");
        dukglue_register_method(ctx, &ScPark::getMonthlyExpenditure, "getMonthlyExpenditure");
        dukglue_register_property(ctx, &ScPark::awards_get, nullptr, "awards");
        dukglue_register_method(ctx, &ScPark::clearAwards, "clearAwards");
        dukglue_register_method(ctx, &ScPark::grantAward, "grantAward");
    }

} // namespace OpenRCT2::Scripting

#endif
