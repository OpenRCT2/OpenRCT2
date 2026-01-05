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
    #include "../../../GameState.h"
    #include "../../../core/StringTypes.h"
    #include "../../../scenario/Scenario.h"
    #include "../../../world/Park.h"
    #include "../../Duktape.hpp"
    #include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    using namespace OpenRCT2::Scenario;

    static const DukEnumMap<ObjectiveType> ScenarioObjectiveTypeMap(
        {
            { "none", ObjectiveType::none },
            { "guestsBy", ObjectiveType::guestsBy },
            { "parkValueBy", ObjectiveType::parkValueBy },
            { "haveFun", ObjectiveType::haveFun },
            { "buildTheBest", ObjectiveType::buildTheBest },
            { "10Rollercoasters", ObjectiveType::tenRollercoasters },
            { "guestsAndRating", ObjectiveType::guestsAndRating },
            { "monthlyRideIncome", ObjectiveType::monthlyRideIncome },
            { "10RollercoastersLength", ObjectiveType::tenRollercoastersLength },
            { "finish5Rollercoasters", ObjectiveType::finishFiveRollercoasters },
            { "repayLoanAndParkValue", ObjectiveType::repayLoanAndParkValue },
            { "monthlyFoodIncome", ObjectiveType::monthlyFoodIncome },
        });

    class ScScenarioObjective
    {
    private:
        std::string type_get()
        {
            return std::string(ScenarioObjectiveTypeMap[getGameState().scenarioOptions.objective.Type]);
        }

        void type_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().scenarioOptions.objective.Type = ScenarioObjectiveTypeMap[value];
        }

        uint16_t guests_get()
        {
            auto& gameState = getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::guestsBy
                || gameState.scenarioOptions.objective.Type == ObjectiveType::guestsAndRating)
            {
                return gameState.scenarioOptions.objective.NumGuests;
            }
            return 0;
        }

        void guests_set(uint16_t value)
        {
            ThrowIfGameStateNotMutable();
            auto& gameState = getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::guestsBy
                || gameState.scenarioOptions.objective.Type == ObjectiveType::guestsAndRating)
            {
                gameState.scenarioOptions.objective.NumGuests = value;
            }
        }

        uint8_t year_get()
        {
            const auto& gameState = getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::guestsBy
                || gameState.scenarioOptions.objective.Type == ObjectiveType::parkValueBy)
            {
                return gameState.scenarioOptions.objective.Year;
            }
            return 0;
        }

        void year_set(uint8_t value)
        {
            auto& gameState = getGameState();
            ThrowIfGameStateNotMutable();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::guestsBy
                || gameState.scenarioOptions.objective.Type == ObjectiveType::parkValueBy)
            {
                gameState.scenarioOptions.objective.Year = value;
            }
        }

        uint16_t length_get()
        {
            const auto& gameState = getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::tenRollercoastersLength)
            {
                return gameState.scenarioOptions.objective.NumGuests;
            }
            return 0;
        }

        void length_set(uint16_t value)
        {
            ThrowIfGameStateNotMutable();
            auto& gameState = getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::tenRollercoastersLength)
            {
                gameState.scenarioOptions.objective.NumGuests = value;
            }
        }

        money64 excitement_get()
        {
            const auto& gameState = getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::finishFiveRollercoasters)
            {
                return gameState.scenarioOptions.objective.Currency;
            }
            return 0;
        }

        void excitement_set(money64 value)
        {
            ThrowIfGameStateNotMutable();
            auto& gameState = getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::finishFiveRollercoasters)
            {
                gameState.scenarioOptions.objective.Currency = value;
            }
        }

        money64 parkValue_get()
        {
            const auto& gameState = getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::parkValueBy
                || gameState.scenarioOptions.objective.Type == ObjectiveType::repayLoanAndParkValue)
            {
                return gameState.scenarioOptions.objective.Currency;
            }
            return 0;
        }

        void parkValue_set(money64 value)
        {
            ThrowIfGameStateNotMutable();
            auto& gameState = getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::parkValueBy
                || gameState.scenarioOptions.objective.Type == ObjectiveType::repayLoanAndParkValue)
            {
                gameState.scenarioOptions.objective.Currency = value;
            }
        }

        money64 monthlyIncome_get()
        {
            const auto& gameState = getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::monthlyRideIncome
                || gameState.scenarioOptions.objective.Type == ObjectiveType::monthlyFoodIncome)
            {
                return gameState.scenarioOptions.objective.Currency;
            }
            return 0;
        }

        void monthlyIncome_set(money64 value)
        {
            ThrowIfGameStateNotMutable();
            auto& gameState = getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::parkValueBy
                || gameState.scenarioOptions.objective.Type == ObjectiveType::repayLoanAndParkValue)
            {
                gameState.scenarioOptions.objective.Currency = value;
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
            return getGameState().scenarioOptions.name;
        }

        void name_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().scenarioOptions.name = value;
        }

        std::string details_get()
        {
            return getGameState().scenarioOptions.details;
        }

        void details_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().scenarioOptions.details = value;
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
                gameState.scenarioCompletedCompanyValue = gameState.park.companyValue;
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
