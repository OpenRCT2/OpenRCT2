/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "../../../Context.h"
    #include "../../../GameState.h"
    #include "../../../core/StringTypes.h"
    #include "../../../scenario/Scenario.h"
    #include "../../../world/Park.h"
    #include "../../Duktape.hpp"
    #include "../../ScriptEngine.h"

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
            return std::string(ScenarioObjectiveTypeMap[GetGameState().ScenarioObjective.Type]);
        }

        void type_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().ScenarioObjective.Type = ScenarioObjectiveTypeMap[value];
        }

        uint16_t guests_get()
        {
            auto& gameState = GetGameState();
            if (gameState.ScenarioObjective.Type == OBJECTIVE_GUESTS_BY
                || gameState.ScenarioObjective.Type == OBJECTIVE_GUESTS_AND_RATING)
            {
                return gameState.ScenarioObjective.NumGuests;
            }
            return 0;
        }

        void guests_set(uint16_t value)
        {
            ThrowIfGameStateNotMutable();
            auto& gameState = GetGameState();
            if (gameState.ScenarioObjective.Type == OBJECTIVE_GUESTS_BY
                || gameState.ScenarioObjective.Type == OBJECTIVE_GUESTS_AND_RATING)
            {
                gameState.ScenarioObjective.NumGuests = value;
            }
        }

        uint8_t year_get()
        {
            const auto& gameState = GetGameState();
            if (gameState.ScenarioObjective.Type == OBJECTIVE_GUESTS_BY
                || gameState.ScenarioObjective.Type == OBJECTIVE_PARK_VALUE_BY)
            {
                return gameState.ScenarioObjective.Year;
            }
            return 0;
        }

        void year_set(uint8_t value)
        {
            auto& gameState = GetGameState();
            ThrowIfGameStateNotMutable();
            if (gameState.ScenarioObjective.Type == OBJECTIVE_GUESTS_BY
                || gameState.ScenarioObjective.Type == OBJECTIVE_PARK_VALUE_BY)
            {
                gameState.ScenarioObjective.Year = value;
            }
        }

        uint16_t length_get()
        {
            const auto& gameState = GetGameState();
            if (gameState.ScenarioObjective.Type == OBJECTIVE_10_ROLLERCOASTERS_LENGTH)
            {
                return gameState.ScenarioObjective.NumGuests;
            }
            return 0;
        }

        void length_set(uint16_t value)
        {
            ThrowIfGameStateNotMutable();
            auto& gameState = GetGameState();
            if (gameState.ScenarioObjective.Type == OBJECTIVE_10_ROLLERCOASTERS_LENGTH)
            {
                gameState.ScenarioObjective.NumGuests = value;
            }
        }

        money64 excitement_get()
        {
            const auto& gameState = GetGameState();
            if (gameState.ScenarioObjective.Type == OBJECTIVE_FINISH_5_ROLLERCOASTERS)
            {
                return gameState.ScenarioObjective.Currency;
            }
            return 0;
        }

        void excitement_set(money64 value)
        {
            ThrowIfGameStateNotMutable();
            auto& gameState = GetGameState();
            if (gameState.ScenarioObjective.Type == OBJECTIVE_FINISH_5_ROLLERCOASTERS)
            {
                gameState.ScenarioObjective.Currency = value;
            }
        }

        money64 parkValue_get()
        {
            const auto& gameState = GetGameState();
            if (gameState.ScenarioObjective.Type == OBJECTIVE_PARK_VALUE_BY
                || gameState.ScenarioObjective.Type == OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE)
            {
                return gameState.ScenarioObjective.Currency;
            }
            return 0;
        }

        void parkValue_set(money64 value)
        {
            ThrowIfGameStateNotMutable();
            auto& gameState = GetGameState();
            if (gameState.ScenarioObjective.Type == OBJECTIVE_PARK_VALUE_BY
                || gameState.ScenarioObjective.Type == OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE)
            {
                gameState.ScenarioObjective.Currency = value;
            }
        }

        money64 monthlyIncome_get()
        {
            const auto& gameState = GetGameState();
            if (gameState.ScenarioObjective.Type == OBJECTIVE_MONTHLY_RIDE_INCOME
                || gameState.ScenarioObjective.Type == OBJECTIVE_MONTHLY_FOOD_INCOME)
            {
                return gameState.ScenarioObjective.Currency;
            }
            return 0;
        }

        void monthlyIncome_set(money64 value)
        {
            ThrowIfGameStateNotMutable();
            auto& gameState = GetGameState();
            if (gameState.ScenarioObjective.Type == OBJECTIVE_PARK_VALUE_BY
                || gameState.ScenarioObjective.Type == OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE)
            {
                gameState.ScenarioObjective.Currency = value;
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
            return GetGameState().ScenarioName;
        }

        void name_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().ScenarioName = value;
        }

        std::string details_get()
        {
            return GetGameState().ScenarioDetails;
        }

        void details_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().ScenarioDetails = value;
        }

        std::string completedBy_get()
        {
            return GetGameState().ScenarioCompletedBy;
        }

        void completedBy_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().ScenarioCompletedBy = value;
        }

        std::string filename_get()
        {
            return GetGameState().ScenarioFileName;
        }

        void filename_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().ScenarioFileName = value;
        }

        std::shared_ptr<ScScenarioObjective> objective_get() const
        {
            return std::make_shared<ScScenarioObjective>();
        }

        uint16_t parkRatingWarningDays_get() const
        {
            return GetGameState().ScenarioParkRatingWarningDays;
        }

        void parkRatingWarningDays_set(uint16_t value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().ScenarioParkRatingWarningDays = value;
        }

        DukValue completedCompanyValue_get() const
        {
            const auto& gameState = GetGameState();
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            if (gameState.ScenarioCompletedCompanyValue == kMoney64Undefined
                || gameState.ScenarioCompletedCompanyValue == kCompanyValueOnFailedObjective)
            {
                return ToDuk(ctx, nullptr);
            }
            return ToDuk(ctx, gameState.ScenarioCompletedCompanyValue);
        }
        void completedCompanyValue_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().ScenarioCompletedCompanyValue = value;
        }

        std::string status_get() const
        {
            const auto& gameState = GetGameState();
            if (gameState.ScenarioCompletedCompanyValue == kMoney64Undefined)
                return "inProgress";
            if (gameState.ScenarioCompletedCompanyValue == kCompanyValueOnFailedObjective)
                return "failed";
            return "completed";
        }
        void status_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            auto& gameState = GetGameState();
            if (value == "inProgress")
                gameState.ScenarioCompletedCompanyValue = kMoney64Undefined;
            else if (value == "failed")
                gameState.ScenarioCompletedCompanyValue = kCompanyValueOnFailedObjective;
            else if (value == "completed")
                gameState.ScenarioCompletedCompanyValue = gameState.CompanyValue;
        }

        money64 companyValueRecord_get() const
        {
            return GetGameState().ScenarioCompanyValueRecord;
        }
        void companyValueRecord_set(money64 value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().ScenarioCompanyValueRecord = value;
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
