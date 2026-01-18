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

    #include "../../../Context.h"
    #include "../../ScriptEngine.h"
    #include "ScAward.hpp"
    #include "ScParkMessage.hpp"
    #include "ScResearch.hpp"

    #include <vector>

namespace OpenRCT2::Scripting
{
    class ScPark;
    extern ScPark gScPark;

    class ScPark final : public ScBase
    {
    private:
        static JSValue cash_get(JSContext* ctx, JSValue thisVal);
        static JSValue cash_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue rating_get(JSContext* ctx, JSValue thisVal);
        static JSValue rating_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue bankLoan_get(JSContext* ctx, JSValue thisVal);
        static JSValue bankLoan_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue maxBankLoan_get(JSContext* ctx, JSValue thisVal);
        static JSValue maxBankLoan_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue entranceFee_get(JSContext* ctx, JSValue thisVal);
        static JSValue entranceFee_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue guests_get(JSContext* ctx, JSValue thisVal);

        static JSValue suggestedGuestMaximum_get(JSContext* ctx, JSValue thisVal);

        static JSValue guestGenerationProbability_get(JSContext* ctx, JSValue thisVal);

        static JSValue generateGuest(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue guestInitialCash_get(JSContext* ctx, JSValue thisVal);

        static JSValue guestInitialHappiness_get(JSContext* ctx, JSValue thisVal);

        static JSValue guestInitialHunger_get(JSContext* ctx, JSValue thisVal);

        static JSValue guestInitialThirst_get(JSContext* ctx, JSValue thisVal);

        static JSValue value_get(JSContext* ctx, JSValue thisVal);
        static JSValue value_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue companyValue_get(JSContext* ctx, JSValue thisVal);
        static JSValue companyValue_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue totalRideValueForMoney_get(JSContext* ctx, JSValue thisVal);

        static JSValue totalAdmissions_get(JSContext* ctx, JSValue thisVal);
        static JSValue totalAdmissions_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue totalIncomeFromAdmissions_get(JSContext* ctx, JSValue thisVal);
        static JSValue totalIncomeFromAdmissions_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue landPrice_get(JSContext* ctx, JSValue thisVal);
        static JSValue landPrice_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue constructionRightsPrice_get(JSContext* ctx, JSValue thisVal);
        static JSValue constructionRightsPrice_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue casualtyPenalty_get(JSContext* ctx, JSValue thisVal);
        static JSValue casualtyPenalty_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue parkSize_get(JSContext* ctx, JSValue thisVal);

        static JSValue name_get(JSContext* ctx, JSValue thisVal);
        static JSValue name_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue getFlag(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue setFlag(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue research_get(JSContext* ctx, JSValue thisVal);

        static JSValue messages_get(JSContext* ctx, JSValue thisVal);

        static JSValue messages_set(JSContext* ctx, JSValue thisVal, JSValue value);

        static JSValue postMessage(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue getMonthlyExpenditure(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue awards_get(JSContext* ctx, JSValue thisVal);

        static JSValue clearAwards(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue grantAward(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

    public:
        JSValue New(JSContext* ctx);

        void Register(JSContext* ctx);
    };
} // namespace OpenRCT2::Scripting

#endif
