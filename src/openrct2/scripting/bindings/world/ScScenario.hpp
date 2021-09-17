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

#    include "../../../Context.h"
#    include "../../../GameState.h"
#    include "../../../common.h"
#    include "../../../core/String.hpp"
#    include "../../../scenario/Scenario.h"
#    include "../../../world/Park.h"
#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"

#    include <algorithm>

namespace OpenRCT2::Scripting
{
    static const DukEnumMap<uint32_t> ScenarioObjectiveTypeMap({
        { "none", OBJECTIVE_NONE },
        { "guestsBy", OBJECTIVE_GUESTS_BY },
        { "parkValueBy", OBJECTIVE_PARK_VALUE_BY },
        { "haveFun", OBJECTIVE_HAVE_FUN },
        { "buildTheBest", OBJECTIVE_BUILD_THE_BEST },
        { "10Rollercoasters", OBJECTIVE_10_ROLLERCOASTERS },
        { "guestsAndRating", OBJECTIVE_GUESTS_AND_RATING },
        { "monthlyRideIncome", OBJECTIVE_MONTHLY_RIDE_INCOME },
        { "10RollercoastersLength", OBJECTIVE_10_ROLLERCOASTERS_LENGTH },
        { "finish5Rollercoasters", OBJECTIVE_FINISH_5_ROLLERCOASTERS },
        { "repayLoanAndParkValue", OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE },
        { "monthlyFoodIncome", OBJECTIVE_MONTHLY_FOOD_INCOME },
    });

    class ScScenarioObjective
    {
    private:
        std::string type_get()
        {
            return std::string(ScenarioObjectiveTypeMap[gScenarioObjective.Type]);
        }

        void type_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            gScenarioObjective.Type = ScenarioObjectiveTypeMap[value];
        }

        uint16_t guests_get()
        {
            if (gScenarioObjective.Type == OBJECTIVE_GUESTS_BY || gScenarioObjective.Type == OBJECTIVE_GUESTS_AND_RATING)
            {
                return gScenarioObjective.NumGuests;
            }
            return 0;
        }

        void guests_set(uint16_t value)
        {
            ThrowIfGameStateNotMutable();
            if (gScenarioObjective.Type == OBJECTIVE_GUESTS_BY || gScenarioObjective.Type == OBJECTIVE_GUESTS_AND_RATING)
            {
                gScenarioObjective.NumGuests = value;
            }
        }

        uint8_t year_get()
        {
            if (gScenarioObjective.Type == OBJECTIVE_GUESTS_BY || gScenarioObjective.Type == OBJECTIVE_PARK_VALUE_BY)
            {
                return gScenarioObjective.Year;
            }
            return 0;
        }

        void year_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            if (gScenarioObjective.Type == OBJECTIVE_GUESTS_BY || gScenarioObjective.Type == OBJECTIVE_PARK_VALUE_BY)
            {
                gScenarioObjective.Year = value;
            }
        }

        uint16_t length_get()
        {
            if (gScenarioObjective.Type == OBJECTIVE_10_ROLLERCOASTERS_LENGTH)
            {
                return gScenarioObjective.NumGuests;
            }
            return 0;
        }

        void length_set(uint16_t value)
        {
            ThrowIfGameStateNotMutable();
            if (gScenarioObjective.Type == OBJECTIVE_10_ROLLERCOASTERS_LENGTH)
            {
                gScenarioObjective.NumGuests = value;
            }
        }

        money64 excitement_get()
        {
            if (gScenarioObjective.Type == OBJECTIVE_FINISH_5_ROLLERCOASTERS)
            {
                return gScenarioObjective.Currency;
            }
            return 0;
        }

        void excitement_set(money64 value)
        {
            ThrowIfGameStateNotMutable();
            if (gScenarioObjective.Type == OBJECTIVE_FINISH_5_ROLLERCOASTERS)
            {
                gScenarioObjective.Currency = value;
            }
        }

        money64 parkValue_get()
        {
            if (gScenarioObjective.Type == OBJECTIVE_PARK_VALUE_BY
                || gScenarioObjective.Type == OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE)
            {
                return gScenarioObjective.Currency;
            }
            return 0;
        }

        void parkValue_set(money64 value)
        {
            ThrowIfGameStateNotMutable();
            if (gScenarioObjective.Type == OBJECTIVE_PARK_VALUE_BY
                || gScenarioObjective.Type == OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE)
            {
                gScenarioObjective.Currency = value;
            }
        }

        money64 monthlyIncome_get()
        {
            if (gScenarioObjective.Type == OBJECTIVE_MONTHLY_RIDE_INCOME
                || gScenarioObjective.Type == OBJECTIVE_MONTHLY_FOOD_INCOME)
            {
                return gScenarioObjective.Currency;
            }
            return 0;
        }

        void monthlyIncome_set(money64 value)
        {
            ThrowIfGameStateNotMutable();
            if (gScenarioObjective.Type == OBJECTIVE_PARK_VALUE_BY
                || gScenarioObjective.Type == OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE)
            {
                gScenarioObjective.Currency = value;
            }
        }

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScScenarioObjective::type_get, &ScScenarioObjective::type_set, "type");
            dukglue_register_property(ctx, &ScScenarioObjective::guests_get, &ScScenarioObjective::guests_set, "guests");
            dukglue_register_property(ctx, &ScScenarioObjective::year_get, &ScScenarioObjective::year_set, "year");
            dukglue_register_property(
                ctx, &ScScenarioObjective::excitement_get, &ScScenarioObjective::excitement_set, "excitement");
            dukglue_register_property(
                ctx, &ScScenarioObjective::monthlyIncome_get, &ScScenarioObjective::monthlyIncome_set, "monthlyIncome");
            dukglue_register_property(
                ctx, &ScScenarioObjective::parkValue_get, &ScScenarioObjective::parkValue_set, "parkValue");
        }
    };

    class ScScenario
    {
    public:
        std::string name_get()
        {
            return gScenarioName;
        }

        void name_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            gScenarioName = value;
        }

        std::string details_get()
        {
            return gScenarioDetails;
        }

        void details_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            gScenarioDetails = value;
        }

        std::string completedBy_get()
        {
            return gScenarioCompletedBy;
        }

        void completedBy_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            gScenarioCompletedBy = value;
        }

        std::string filename_get()
        {
            return gScenarioFileName;
        }

        void filename_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            gScenarioFileName = value;
        }

        std::shared_ptr<ScScenarioObjective> objective_get() const
        {
            return std::make_shared<ScScenarioObjective>();
        }

        uint16_t parkRatingWarningDays_get() const
        {
            return gScenarioParkRatingWarningDays;
        }

        void parkRatingWarningDays_set(uint16_t value)
        {
            ThrowIfGameStateNotMutable();
            gScenarioParkRatingWarningDays = value;
        }

        DukValue completedCompanyValue_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            if (gScenarioCompletedCompanyValue == MONEY64_UNDEFINED
                || gScenarioCompletedCompanyValue == COMPANY_VALUE_ON_FAILED_OBJECTIVE)
            {
                return ToDuk(ctx, nullptr);
            }
            return ToDuk(ctx, gScenarioCompletedCompanyValue);
        }
        void completedCompanyValue_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            gScenarioCompletedCompanyValue = value;
        }

        std::string status_get() const
        {
            if (gScenarioCompletedCompanyValue == MONEY64_UNDEFINED)
                return "inProgress";
            if (gScenarioCompletedCompanyValue == COMPANY_VALUE_ON_FAILED_OBJECTIVE)
                return "failed";
            return "completed";
        }
        void status_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            if (value == "inProgress")
                gScenarioCompletedCompanyValue = MONEY64_UNDEFINED;
            else if (value == "failed")
                gScenarioCompletedCompanyValue = COMPANY_VALUE_ON_FAILED_OBJECTIVE;
            else if (value == "completed")
                gScenarioCompletedCompanyValue = gCompanyValue;
        }

        money64 companyValueRecord_get() const
        {
            return gScenarioCompanyValueRecord;
        }
        void companyValueRecord_set(money64 value)
        {
            ThrowIfGameStateNotMutable();
            gScenarioCompanyValueRecord = value;
        }

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScScenario::name_get, &ScScenario::name_set, "name");
            dukglue_register_property(ctx, &ScScenario::details_get, &ScScenario::details_set, "details");
            dukglue_register_property(ctx, &ScScenario::completedBy_get, &ScScenario::completedBy_set, "completedBy");
            dukglue_register_property(ctx, &ScScenario::filename_get, &ScScenario::filename_set, "filename");
            dukglue_register_property(
                ctx, &ScScenario::parkRatingWarningDays_get, &ScScenario::parkRatingWarningDays_set, "parkRatingWarningDays");
            dukglue_register_property(ctx, &ScScenario::objective_get, nullptr, "objective");
            dukglue_register_property(ctx, &ScScenario::status_get, &ScScenario::status_set, "status");
            dukglue_register_property(
                ctx, &ScScenario::completedCompanyValue_get, &ScScenario::completedCompanyValue_set, "completedCompanyValue");
            dukglue_register_property(
                ctx, &ScScenario::companyValueRecord_get, &ScScenario::companyValueRecord_set, "companyValueRecord");
        }
    };
} // namespace OpenRCT2::Scripting

#endif
