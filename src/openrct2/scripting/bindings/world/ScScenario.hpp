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
    #include "../../../world/Park.h"
    #include "../../Duktape.hpp"
    #include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    static const DukEnumMap<uint32_t> ScenarioObjectiveTypeMap(
        {
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
            return std::string(ScenarioObjectiveTypeMap[getGameState().scenarioObjective.Type]);
        }

        void type_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().scenarioObjective.Type = ScenarioObjectiveTypeMap[value];
        }

        uint16_t guests_get()
        {
            auto& gameState = getGameState();
            if (gameState.scenarioObjective.Type == OBJECTIVE_GUESTS_BY
                || gameState.scenarioObjective.Type == OBJECTIVE_GUESTS_AND_RATING)
            {
                return gameState.scenarioObjective.NumGuests;
            }
            return 0;
        }

        void guests_set(uint16_t value)
        {
            ThrowIfGameStateNotMutable();
            auto& gameState = getGameState();
            if (gameState.scenarioObjective.Type == OBJECTIVE_GUESTS_BY
                || gameState.scenarioObjective.Type == OBJECTIVE_GUESTS_AND_RATING)
            {
                gameState.scenarioObjective.NumGuests = value;
            }
        }

        uint8_t year_get()
        {
            const auto& gameState = getGameState();
            if (gameState.scenarioObjective.Type == OBJECTIVE_GUESTS_BY
                || gameState.scenarioObjective.Type == OBJECTIVE_PARK_VALUE_BY)
            {
                return gameState.scenarioObjective.Year;
            }
            return 0;
        }

        void year_set(uint8_t value)
        {
            auto& gameState = getGameState();
            ThrowIfGameStateNotMutable();
            if (gameState.scenarioObjective.Type == OBJECTIVE_GUESTS_BY
                || gameState.scenarioObjective.Type == OBJECTIVE_PARK_VALUE_BY)
            {
                gameState.scenarioObjective.Year = value;
            }
        }

        uint16_t length_get()
        {
            const auto& gameState = getGameState();
            if (gameState.scenarioObjective.Type == OBJECTIVE_10_ROLLERCOASTERS_LENGTH)
            {
                return gameState.scenarioObjective.NumGuests;
            }
            return 0;
        }

        void length_set(uint16_t value)
        {
            ThrowIfGameStateNotMutable();
            auto& gameState = getGameState();
            if (gameState.scenarioObjective.Type == OBJECTIVE_10_ROLLERCOASTERS_LENGTH)
            {
                gameState.scenarioObjective.NumGuests = value;
            }
        }

        money64 excitement_get()
        {
            const auto& gameState = getGameState();
            if (gameState.scenarioObjective.Type == OBJECTIVE_FINISH_5_ROLLERCOASTERS)
            {
                return gameState.scenarioObjective.Currency;
            }
            return 0;
        }

        void excitement_set(money64 value)
        {
            ThrowIfGameStateNotMutable();
            auto& gameState = getGameState();
            if (gameState.scenarioObjective.Type == OBJECTIVE_FINISH_5_ROLLERCOASTERS)
            {
                gameState.scenarioObjective.Currency = value;
            }
        }

        money64 parkValue_get()
        {
            const auto& gameState = getGameState();
            if (gameState.scenarioObjective.Type == OBJECTIVE_PARK_VALUE_BY
                || gameState.scenarioObjective.Type == OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE)
            {
                return gameState.scenarioObjective.Currency;
            }
            return 0;
        }

        void parkValue_set(money64 value)
        {
            ThrowIfGameStateNotMutable();
            auto& gameState = getGameState();
            if (gameState.scenarioObjective.Type == OBJECTIVE_PARK_VALUE_BY
                || gameState.scenarioObjective.Type == OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE)
            {
                gameState.scenarioObjective.Currency = value;
            }
        }

        money64 monthlyIncome_get()
        {
            const auto& gameState = getGameState();
            if (gameState.scenarioObjective.Type == OBJECTIVE_MONTHLY_RIDE_INCOME
                || gameState.scenarioObjective.Type == OBJECTIVE_MONTHLY_FOOD_INCOME)
            {
                return gameState.scenarioObjective.Currency;
            }
            return 0;
        }

        void monthlyIncome_set(money64 value)
        {
            ThrowIfGameStateNotMutable();
            auto& gameState = getGameState();
            if (gameState.scenarioObjective.Type == OBJECTIVE_PARK_VALUE_BY
                || gameState.scenarioObjective.Type == OBJECTIVE_REPAY_LOAN_AND_PARK_VALUE)
            {
                gameState.scenarioObjective.Currency = value;
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
            return getGameState().scenarioName;
        }

        void name_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().scenarioName = value;
        }

        std::string details_get()
        {
            return getGameState().scenarioDetails;
        }

        void details_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().scenarioDetails = value;
        }

        std::string completedBy_get()
        {
            return getGameState().scenarioCompletedBy;
        }

        void completedBy_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().scenarioCompletedBy = value;
        }

        std::string filename_get()
        {
            return getGameState().scenarioFileName;
        }

        void filename_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().scenarioFileName = value;
        }

        std::shared_ptr<ScScenarioObjective> objective_get() const
        {
            return std::make_shared<ScScenarioObjective>();
        }

        uint16_t parkRatingWarningDays_get() const
        {
            return getGameState().scenarioParkRatingWarningDays;
        }

        void parkRatingWarningDays_set(uint16_t value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().scenarioParkRatingWarningDays = value;
        }

        DukValue completedCompanyValue_get() const
        {
            const auto& gameState = getGameState();
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            if (gameState.scenarioCompletedCompanyValue == kMoney64Undefined
                || gameState.scenarioCompletedCompanyValue == kCompanyValueOnFailedObjective)
            {
                return ToDuk(ctx, nullptr);
            }
            return ToDuk(ctx, gameState.scenarioCompletedCompanyValue);
        }
        void completedCompanyValue_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().scenarioCompletedCompanyValue = value;
        }

        std::string status_get() const
        {
            const auto& gameState = getGameState();
            if (gameState.scenarioCompletedCompanyValue == kMoney64Undefined)
                return "inProgress";
            if (gameState.scenarioCompletedCompanyValue == kCompanyValueOnFailedObjective)
                return "failed";
            return "completed";
        }
        void status_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            auto& gameState = getGameState();
            if (value == "inProgress")
                gameState.scenarioCompletedCompanyValue = kMoney64Undefined;
            else if (value == "failed")
                gameState.scenarioCompletedCompanyValue = kCompanyValueOnFailedObjective;
            else if (value == "completed")
                gameState.scenarioCompletedCompanyValue = gameState.companyValue;
        }

        money64 companyValueRecord_get() const
        {
            return getGameState().scenarioCompanyValueRecord;
        }
        void companyValueRecord_set(money64 value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().scenarioCompanyValueRecord = value;
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
