/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING_REFACTOR

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
    #include "../../ScriptEngine.h"
    #include "../entity/ScGuest.hpp"
    #include "ScParkMessage.hpp"

namespace OpenRCT2::Scripting
{
    static const EnumMap<uint64_t> ParkFlagMap(
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

    JSValue ScPark::cash_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt64(ctx, getGameState().park.cash);
    }
    JSValue ScPark::cash_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT64(valueInt, ctx, value)
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto& gameState = getGameState();
        if (gameState.park.cash != valueInt)
        {
            gameState.park.cash = valueInt;
            auto intent = Intent(INTENT_ACTION_UPDATE_CASH);
            ContextBroadcastIntent(&intent);
        }
        return JS_UNDEFINED;
    }

    JSValue ScPark::rating_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt32(ctx, getGameState().park.rating);
    }
    JSValue ScPark::rating_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT32(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto valueClamped = std::min(std::max(0, valueInt), 999);
        auto& gameState = getGameState();
        if (gameState.park.rating != valueClamped)
        {
            gameState.park.rating = std::min(std::max(0, valueInt), 999);
            auto intent = Intent(INTENT_ACTION_UPDATE_PARK_RATING);
            ContextBroadcastIntent(&intent);
        }
        return JS_UNDEFINED;
    }

    JSValue ScPark::bankLoan_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt64(ctx, getGameState().park.bankLoan);
    }
    JSValue ScPark::bankLoan_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT64(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto& gameState = getGameState();

        if (gameState.park.bankLoan != valueInt)
        {
            gameState.park.bankLoan = valueInt;
            auto intent = Intent(INTENT_ACTION_UPDATE_CASH);
            ContextBroadcastIntent(&intent);
        }
        return JS_UNDEFINED;
    }

    JSValue ScPark::maxBankLoan_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt64(ctx, getGameState().park.maxBankLoan);
    }
    JSValue ScPark::maxBankLoan_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT64(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE()

        auto& gameState = getGameState();
        if (gameState.park.maxBankLoan != valueInt)
        {
            gameState.park.maxBankLoan = valueInt;
            auto intent = Intent(INTENT_ACTION_UPDATE_CASH);
            ContextBroadcastIntent(&intent);
        }
        return JS_UNDEFINED;
    }

    JSValue ScPark::entranceFee_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt64(ctx, getGameState().park.entranceFee);
    }
    JSValue ScPark::entranceFee_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT64(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto& gameState = getGameState();
        if (gameState.park.entranceFee != valueInt)
        {
            gameState.park.entranceFee = valueInt;
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->InvalidateByClass(WindowClass::parkInformation);
        }
        return JS_UNDEFINED;
    }

    JSValue ScPark::guests_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt64(ctx, getGameState().park.numGuestsInPark);
    }

    JSValue ScPark::suggestedGuestMaximum_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt64(ctx, getGameState().park.suggestedGuestMaximum);
    }

    JSValue ScPark::guestGenerationProbability_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt32(ctx, getGameState().park.guestGenerationProbability);
    }

    JSValue ScPark::generateGuest(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto guest = Park::GenerateGuest();
        return ScGuest::New(ctx, guest->Id);
    }

    JSValue ScPark::guestInitialCash_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt64(ctx, getGameState().scenarioOptions.guestInitialCash);
    }

    JSValue ScPark::guestInitialHappiness_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt32(ctx, getGameState().scenarioOptions.guestInitialHappiness);
    }

    JSValue ScPark::guestInitialHunger_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt32(ctx, getGameState().scenarioOptions.guestInitialHunger);
    }

    JSValue ScPark::guestInitialThirst_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt32(ctx, getGameState().scenarioOptions.guestInitialThirst);
    }

    JSValue ScPark::value_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt64(ctx, getGameState().park.value);
    }
    JSValue ScPark::value_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT64(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto& gameState = getGameState();
        if (gameState.park.value != valueInt)
        {
            gameState.park.value = valueInt;
            auto intent = Intent(INTENT_ACTION_UPDATE_CASH);
            ContextBroadcastIntent(&intent);
        }
        return JS_UNDEFINED;
    }

    JSValue ScPark::companyValue_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt64(ctx, getGameState().park.companyValue);
    }
    JSValue ScPark::companyValue_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT64(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto& gameState = getGameState();

        if (gameState.park.companyValue != valueInt)
        {
            gameState.park.companyValue = valueInt;
            auto intent = Intent(INTENT_ACTION_UPDATE_CASH);
            ContextBroadcastIntent(&intent);
        }
        return JS_UNDEFINED;
    }

    JSValue ScPark::totalRideValueForMoney_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt64(ctx, getGameState().park.totalRideValueForMoney);
    }

    JSValue ScPark::totalAdmissions_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt64(ctx, getGameState().park.totalAdmissions);
    }
    JSValue ScPark::totalAdmissions_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT64(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto& gameState = getGameState();

        if (gameState.park.totalAdmissions != static_cast<uint64_t>(valueInt))
        {
            gameState.park.totalAdmissions = static_cast<uint64_t>(valueInt);
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->InvalidateByClass(WindowClass::parkInformation);
        }
        return JS_UNDEFINED;
    }

    JSValue ScPark::totalIncomeFromAdmissions_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt64(ctx, getGameState().park.totalIncomeFromAdmissions);
    }
    JSValue ScPark::totalIncomeFromAdmissions_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT64(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto& gameState = getGameState();

        if (gameState.park.totalIncomeFromAdmissions != valueInt)
        {
            gameState.park.totalIncomeFromAdmissions = valueInt;
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->InvalidateByClass(WindowClass::parkInformation);
        }
        return JS_UNDEFINED;
    }

    JSValue ScPark::landPrice_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt64(ctx, getGameState().scenarioOptions.landPrice);
    }
    JSValue ScPark::landPrice_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT64(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        getGameState().scenarioOptions.landPrice = valueInt;
        return JS_UNDEFINED;
    }

    JSValue ScPark::constructionRightsPrice_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt64(ctx, getGameState().scenarioOptions.constructionRightsPrice);
    }
    JSValue ScPark::constructionRightsPrice_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT64(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        getGameState().scenarioOptions.constructionRightsPrice = valueInt;
        return JS_UNDEFINED;
    }

    JSValue ScPark::casualtyPenalty_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt32(ctx, getGameState().park.ratingCasualtyPenalty);
    }
    JSValue ScPark::casualtyPenalty_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT32(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        getGameState().park.ratingCasualtyPenalty = valueInt;
        return JS_UNDEFINED;
    }

    JSValue ScPark::parkSize_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt64(ctx, getGameState().park.size);
    }

    JSValue ScPark::name_get(JSContext* ctx, JSValue thisVal)
    {
        return JSFromStdString(ctx, getGameState().park.name);
    }
    JSValue ScPark::name_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_STR(valueStr, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto& park = getGameState().park;
        if (park.name != valueStr)
        {
            park.name = std::move(valueStr);
            GfxInvalidateScreen();
        }
        return JS_UNDEFINED;
    }

    JSValue ScPark::getFlag(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_STR(key, ctx, argv[0])
        auto mask = ParkFlagMap[key];
        return JS_NewBool(ctx, (getGameState().park.flags & mask) != 0);
    }

    JSValue ScPark::setFlag(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_STR(key, ctx, argv[0]);
        JS_UNPACK_BOOL(value, ctx, argv[1]);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto mask = ParkFlagMap[key];
        auto& gameState = getGameState();
        if (value)
            gameState.park.flags |= mask;
        else
            gameState.park.flags &= ~mask;
        GfxInvalidateScreen();
        return JS_UNDEFINED;
    }

    JSValue ScPark::research_get(JSContext* ctx, JSValue thisVal)
    {
        return gScResearch.New(ctx);
    }

    JSValue ScPark::messages_get(JSContext* ctx, JSValue thisVal)
    {
        JSValue result = JS_NewArray(ctx);
        int64_t resultIdx = 0;
        auto& gameState = getGameState();
        for (size_t i = 0, newsSize = gameState.newsItems.GetRecent().size(); i < newsSize; i++)
        {
            JS_SetPropertyInt64(ctx, result, resultIdx++, gScParkMessage.New(ctx, i));
        }
        for (size_t i = 0, newsSize = gameState.newsItems.GetArchived().size(); i < newsSize; i++)
        {
            auto offset = i + News::ItemHistoryStart;
            JS_SetPropertyInt64(ctx, result, resultIdx++, gScParkMessage.New(ctx, offset));
        }
        return result;
    }

    JSValue ScPark::messages_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        int32_t index = 0;
        int32_t archiveIndex = News::ItemHistoryStart;
        auto& gameState = getGameState();
        JSIterateArray(ctx, value, [&index, &archiveIndex, &gameState](JSContext* ctx, JSValue item) {
            auto isArchived = AsOrDefault(ctx, item, "isArchived", false);
            auto newsItem = NewsItemFromJS(ctx, item);
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
        });

        // End the lists by setting next item to null
        if (index < News::ItemHistoryStart)
        {
            gameState.newsItems[index].type = News::ItemType::null;
        }
        if (archiveIndex < News::MaxItems)
        {
            gameState.newsItems[archiveIndex].type = News::ItemType::null;
        }
        return JS_UNDEFINED;
    }

    JSValue ScPark::postMessage(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        uint32_t assoc = std::numeric_limits<uint32_t>::max();
        auto type = News::ItemType::blank;
        std::string text;
        if (JS_IsString(argv[0]))
        {
            text = JSToStdString(ctx, argv[0]);
        }
        else
        {
            auto textOption = JSToOptionalStdString(ctx, argv[0], "text");
            auto typeOption = JSToOptionalStdString(ctx, argv[0], "type");
            if (!textOption.has_value() || !typeOption.has_value())
            {
                return JS_ThrowPlainError(ctx, "Invalid message argument.");
            }

            type = GetParkMessageType(typeOption.value());
            text = textOption.value();
            if (type == News::ItemType::blank)
            {
                assoc = static_cast<uint32_t>(((kCoordsNull & 0xFFFF) << 16) | (kCoordsNull & 0xFFFF));
            }

            auto subject = JS_GetPropertyStr(ctx, argv[0], "subject");
            if (JS_IsNumber(subject))
            {
                assoc = JSToUint(ctx, subject);
            }
        }
        News::AddItemToQueue(type, text.c_str(), assoc);
        return JS_UNDEFINED;
    }

    JSValue ScPark::getMonthlyExpenditure(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_STR(expenditureType, ctx, argv[0]);

        auto recordedMonths = std::clamp(
            GetDate().GetMonthsElapsed() + 1, static_cast<uint32_t>(0), static_cast<uint32_t>(kExpenditureTableMonthCount));
        JSValue result = JS_NewArray(ctx);
        auto type = ScriptEngine::StringToExpenditureType(expenditureType);
        if (type != ExpenditureType::count)
        {
            auto& gameState = getGameState();
            for (size_t i = 0; i < recordedMonths; ++i)
            {
                JS_SetPropertyInt64(ctx, result, i, JS_NewInt64(ctx, gameState.park.expenditureTable[i][EnumValue(type)]));
            }
        }
        return result;
    }

    JSValue ScPark::awards_get(JSContext* ctx, JSValue thisVal)
    {
        JSValue result = JS_NewArray(ctx);

        auto& gameState = getGameState();
        for (size_t i = 0; i < gameState.park.currentAwards.size(); i++)
        {
            JS_SetPropertyInt64(ctx, result, i, gScAward.New(ctx, i));
        }

        return result;
    }

    JSValue ScPark::clearAwards(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        AwardReset();
        return JS_UNDEFINED;
    }

    JSValue ScPark::grantAward(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_STR(awardType, ctx, argv[0]);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto optType = StringToAwardType(awardType);
        if (optType.has_value())
        {
            AwardGrant(optType.value());
        }
        return JS_UNDEFINED;
    }

    JSValue ScPark::New(JSContext* ctx)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("cash", ScPark::cash_get, ScPark::cash_set),
            JS_CGETSET_DEF("rating", ScPark::rating_get, ScPark::rating_set),
            JS_CGETSET_DEF("bankLoan", ScPark::bankLoan_get, ScPark::bankLoan_set),
            JS_CGETSET_DEF("maxBankLoan", ScPark::maxBankLoan_get, ScPark::maxBankLoan_set),
            JS_CGETSET_DEF("entranceFee", ScPark::entranceFee_get, ScPark::entranceFee_set),
            JS_CGETSET_DEF("guests", ScPark::guests_get, nullptr),
            JS_CGETSET_DEF("suggestedGuestMaximum", ScPark::suggestedGuestMaximum_get, nullptr),
            JS_CGETSET_DEF("guestGenerationProbability", ScPark::guestGenerationProbability_get, nullptr),
            JS_CFUNC_DEF("generateGuest", 0, ScPark::generateGuest),
            JS_CGETSET_DEF("guestInitialCash", ScPark::guestInitialCash_get, nullptr),
            JS_CGETSET_DEF("guestInitialHappiness", ScPark::guestInitialHappiness_get, nullptr),
            JS_CGETSET_DEF("guestInitialHunger", ScPark::guestInitialHunger_get, nullptr),
            JS_CGETSET_DEF("guestInitialThirst", ScPark::guestInitialThirst_get, nullptr),
            JS_CGETSET_DEF("value", ScPark::value_get, ScPark::value_set),
            JS_CGETSET_DEF("companyValue", ScPark::companyValue_get, ScPark::companyValue_set),
            JS_CGETSET_DEF("totalRideValueForMoney", ScPark::totalRideValueForMoney_get, nullptr),
            JS_CGETSET_DEF("totalAdmissions", ScPark::totalAdmissions_get, ScPark::totalAdmissions_set),
            JS_CGETSET_DEF(
                "totalIncomeFromAdmissions", ScPark::totalIncomeFromAdmissions_get, ScPark::totalIncomeFromAdmissions_set),
            JS_CGETSET_DEF("landPrice", ScPark::landPrice_get, ScPark::landPrice_set),
            JS_CGETSET_DEF("constructionRightsPrice", ScPark::constructionRightsPrice_get, ScPark::constructionRightsPrice_set),
            JS_CGETSET_DEF("parkSize", ScPark::parkSize_get, nullptr),
            JS_CGETSET_DEF("name", ScPark::name_get, ScPark::name_set),
            JS_CGETSET_DEF("research", ScPark::research_get, nullptr),
            JS_CGETSET_DEF("messages", ScPark::messages_get, ScPark::messages_set),
            JS_CGETSET_DEF("casualtyPenalty", ScPark::casualtyPenalty_get, ScPark::casualtyPenalty_set),
            JS_CFUNC_DEF("getFlag", 1, ScPark::getFlag),
            JS_CFUNC_DEF("setFlag", 2, ScPark::setFlag),
            JS_CFUNC_DEF("postMessage", 1, ScPark::postMessage),
            JS_CFUNC_DEF("getMonthlyExpenditure", 1, ScPark::getMonthlyExpenditure),
            JS_CGETSET_DEF("awards", ScPark::awards_get, nullptr),
            JS_CFUNC_DEF("clearAwards", 0, ScPark::clearAwards),
            JS_CFUNC_DEF("grantAward", 1, ScPark::grantAward),
        };
        return MakeWithOpaque(ctx, funcs, nullptr);
    } // namespace OpenRCT2::Scripting

    void ScPark::Register(JSContext* ctx)
    {
        RegisterBaseStr(ctx, "Park");
    }
} // namespace OpenRCT2::Scripting

#endif
