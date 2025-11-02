/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "../../../Context.h"
    #include "../../../GameState.h"
    #include "../../../core/EnumMap.hpp"
    #include "../../../core/StringTypes.h"
    #include "../../../scenario/Scenario.h"
    #include "../../../world/Park.h"
    #include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    using namespace OpenRCT2::Scenario;

    static const EnumMap<ObjectiveType> ScenarioObjectiveTypeMap(
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

    class ScScenarioObjective;
    extern ScScenarioObjective gScScenarioObjective;
    class ScScenarioObjective final : public ScBase
    {
    private:
        static JSValue type_get(JSContext* ctx, JSValue)
        {
            return JSFromStdString(ctx, ScenarioObjectiveTypeMap[getGameState().scenarioOptions.objective.Type]);
        }

        static JSValue type_set(JSContext* ctx, JSValue, JSValue jsValue)
        {
            JS_UNPACK_STR(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().scenarioOptions.objective.Type = ScenarioObjectiveTypeMap[value];
            return JS_UNDEFINED;
        }

        static JSValue guests_get(JSContext* ctx, JSValue)
        {
            auto& gameState = getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::guestsBy
                || gameState.scenarioOptions.objective.Type == ObjectiveType::guestsAndRating)
            {
                return JS_NewUint32(ctx, gameState.scenarioOptions.objective.NumGuests);
            }
            return JS_NewUint32(ctx, 0);
        }

        static JSValue guests_set(JSContext* ctx, JSValue, JSValue jsValue)
        {
            JS_UNPACK_UINT32(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto& gameState = OpenRCT2::getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::guestsBy
                || gameState.scenarioOptions.objective.Type == ObjectiveType::guestsAndRating)
            {
                gameState.scenarioOptions.objective.NumGuests = value;
            }
            return JS_UNDEFINED;
        }

        static JSValue year_get(JSContext* ctx, JSValue)
        {
            const auto& gameState = getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::guestsBy
                || gameState.scenarioOptions.objective.Type == ObjectiveType::parkValueBy)
            {
                return JS_NewUint32(ctx, gameState.scenarioOptions.objective.Year);
            }
            return JS_NewUint32(ctx, 0);
        }

        static JSValue year_set(JSContext* ctx, JSValue, JSValue jsValue)
        {
            JS_UNPACK_UINT32(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto& gameState = OpenRCT2::getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::guestsBy
                || gameState.scenarioOptions.objective.Type == ObjectiveType::parkValueBy)
            {
                gameState.scenarioOptions.objective.Year = value;
            }
            return JS_UNDEFINED;
        }

        static JSValue length_get(JSContext* ctx, JSValue)
        {
            const auto& gameState = OpenRCT2::getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::tenRollercoastersLength)
            {
                return JS_NewUint32(ctx, gameState.scenarioOptions.objective.NumGuests);
            }
            return JS_NewUint32(ctx, 0);
        }

        static JSValue length_set(JSContext* ctx, JSValue, JSValue jsValue)
        {
            JS_UNPACK_UINT32(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto& gameState = OpenRCT2::getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::tenRollercoastersLength)
            {
                gameState.scenarioOptions.objective.NumGuests = value;
            }
            return JS_UNDEFINED;
        }

        static JSValue excitement_get(JSContext* ctx, JSValue)
        {
            const auto& gameState = getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::finishFiveRollercoasters)
            {
                return JS_NewInt64(ctx, gameState.scenarioOptions.objective.Currency);
            }
            return JS_NewInt64(ctx, 0);
        }

        static JSValue excitement_set(JSContext* ctx, JSValue, JSValue jsValue)
        {
            JS_UNPACK_INT64(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto& gameState = OpenRCT2::getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::finishFiveRollercoasters)
            {
                gameState.scenarioOptions.objective.Currency = value;
            }
            return JS_UNDEFINED;
        }

        static JSValue parkValue_get(JSContext* ctx, JSValue)
        {
            const auto& gameState = getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::parkValueBy
                || gameState.scenarioOptions.objective.Type == ObjectiveType::repayLoanAndParkValue)
            {
                return JS_NewInt64(ctx, gameState.scenarioOptions.objective.Currency);
            }
            return JS_NewInt64(ctx, 0);
        }

        static JSValue parkValue_set(JSContext* ctx, JSValue, JSValue jsValue)
        {
            JS_UNPACK_INT64(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto& gameState = OpenRCT2::getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::parkValueBy
                || gameState.scenarioOptions.objective.Type == ObjectiveType::repayLoanAndParkValue)
            {
                gameState.scenarioOptions.objective.Currency = value;
            }
            return JS_UNDEFINED;
        }

        static JSValue monthlyIncome_get(JSContext* ctx, JSValue)
        {
            const auto& gameState = getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::monthlyRideIncome
                || gameState.scenarioOptions.objective.Type == ObjectiveType::monthlyFoodIncome)
            {
                return JS_NewInt64(ctx, gameState.scenarioOptions.objective.Currency);
            }
            return JS_NewInt64(ctx, 0);
        }

        static JSValue monthlyIncome_set(JSContext* ctx, JSValue, JSValue jsValue)
        {
            JS_UNPACK_INT64(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto& gameState = OpenRCT2::getGameState();
            if (gameState.scenarioOptions.objective.Type == ObjectiveType::parkValueBy
                || gameState.scenarioOptions.objective.Type == ObjectiveType::repayLoanAndParkValue)
            {
                gameState.scenarioOptions.objective.Currency = value;
            }
            return JS_UNDEFINED;
        }

    public:
        JSValue New(JSContext* ctx)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("type", ScScenarioObjective::type_get, ScScenarioObjective::type_set),
                JS_CGETSET_DEF("guests", ScScenarioObjective::guests_get, ScScenarioObjective::guests_set),
                JS_CGETSET_DEF("year", ScScenarioObjective::year_get, ScScenarioObjective::year_set),
                JS_CGETSET_DEF("excitement", ScScenarioObjective::excitement_get, ScScenarioObjective::excitement_set),
                JS_CGETSET_DEF("monthlyIncome", ScScenarioObjective::monthlyIncome_get, ScScenarioObjective::monthlyIncome_set),
                JS_CGETSET_DEF("parkValue", ScScenarioObjective::parkValue_get, ScScenarioObjective::parkValue_set),
            };

            return MakeWithOpaque(ctx, funcs, nullptr);
        }

        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "ScenarioObjective");
        }
    };

    class ScScenario;
    extern ScScenario gScScenario;
    class ScScenario final : public ScBase
    {
    private:
        static JSValue name_get(JSContext* ctx, JSValue)
        {
            return JSFromStdString(ctx, getGameState().scenarioOptions.name);
        }

        static JSValue name_set(JSContext* ctx, JSValue, JSValue jsValue)
        {
            JS_UNPACK_STR(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().scenarioOptions.name = value;
            return JS_UNDEFINED;
        }

        static JSValue details_get(JSContext* ctx, JSValue)
        {
            return JSFromStdString(ctx, getGameState().scenarioOptions.details);
        }

        static JSValue details_set(JSContext* ctx, JSValue, JSValue jsValue)
        {
            JS_UNPACK_STR(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().scenarioOptions.details = value;
            return JS_UNDEFINED;
        }

        static JSValue completedBy_get(JSContext* ctx, JSValue)
        {
            return JSFromStdString(ctx, getGameState().scenarioCompletedBy);
        }

        static JSValue completedBy_set(JSContext* ctx, JSValue, JSValue jsValue)
        {
            JS_UNPACK_STR(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().scenarioCompletedBy = value;
            return JS_UNDEFINED;
        }

        static JSValue filename_get(JSContext* ctx, JSValue)
        {
            return JSFromStdString(ctx, getGameState().scenarioFileName);
        }

        static JSValue filename_set(JSContext* ctx, JSValue, JSValue jsValue)
        {
            JS_UNPACK_STR(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().scenarioFileName = value;
            return JS_UNDEFINED;
        }

        static JSValue objective_get(JSContext* ctx, JSValue)
        {
            return gScScenarioObjective.New(ctx);
        }

        static JSValue parkRatingWarningDays_get(JSContext* ctx, JSValue)
        {
            return JS_NewUint32(ctx, getGameState().scenarioParkRatingWarningDays);
        }

        static JSValue parkRatingWarningDays_set(JSContext* ctx, JSValue, JSValue jsValue)
        {
            JS_UNPACK_UINT32(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().scenarioParkRatingWarningDays = value;
            return JS_UNDEFINED;
        }

        static JSValue completedCompanyValue_get(JSContext* ctx, JSValue)
        {
            const auto& gameState = getGameState();
            if (gameState.scenarioCompletedCompanyValue == kMoney64Undefined
                || gameState.scenarioCompletedCompanyValue == kCompanyValueOnFailedObjective)
            {
                return JS_NULL;
            }
            return JS_NewInt64(ctx, gameState.scenarioCompletedCompanyValue);
        }
        static JSValue completedCompanyValue_set(JSContext* ctx, JSValue, JSValue jsValue)
        {
            JS_UNPACK_INT32(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().scenarioCompletedCompanyValue = value;
            return JS_UNDEFINED;
        }

        static JSValue status_get(JSContext* ctx, JSValue)
        {
            const auto& gameState = OpenRCT2::getGameState();
            if (gameState.scenarioCompletedCompanyValue == kMoney64Undefined)
                return JSFromStdString(ctx, "inProgress");
            if (gameState.scenarioCompletedCompanyValue == kCompanyValueOnFailedObjective)
                return JSFromStdString(ctx, "failed");
            return JSFromStdString(ctx, "completed");
        }
        static JSValue status_set(JSContext* ctx, JSValue, JSValue jsValue)
        {
            JS_UNPACK_STR(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto& gameState = getGameState();
            if (value == "inProgress")
                gameState.scenarioCompletedCompanyValue = kMoney64Undefined;
            else if (value == "failed")
                gameState.scenarioCompletedCompanyValue = kCompanyValueOnFailedObjective;
            else if (value == "completed")
                gameState.scenarioCompletedCompanyValue = gameState.park.companyValue;
            return JS_UNDEFINED;
        }

        static JSValue companyValueRecord_get(JSContext* ctx, JSValue)
        {
            return JS_NewInt64(ctx, OpenRCT2::getGameState().scenarioCompanyValueRecord);
        }
        static JSValue companyValueRecord_set(JSContext* ctx, JSValue, JSValue jsValue)
        {
            JS_UNPACK_MONEY64(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().scenarioCompanyValueRecord = value;
            return JS_UNDEFINED;
        }

    public:
        JSValue New(JSContext* ctx)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("name", ScScenario::name_get, ScScenario::name_set),
                JS_CGETSET_DEF("details", ScScenario::details_get, ScScenario::details_set),
                JS_CGETSET_DEF("completedBy", ScScenario::completedBy_get, ScScenario::completedBy_set),
                JS_CGETSET_DEF("filename", ScScenario::filename_get, ScScenario::filename_set),
                JS_CGETSET_DEF(
                    "parkRatingWarningDays", ScScenario::parkRatingWarningDays_get, ScScenario::parkRatingWarningDays_set),
                JS_CGETSET_DEF("objective", ScScenario::objective_get, nullptr),
                JS_CGETSET_DEF("status", ScScenario::status_get, ScScenario::status_set),
                JS_CGETSET_DEF(
                    "completedCompanyValue", ScScenario::completedCompanyValue_get, ScScenario::completedCompanyValue_set),
                JS_CGETSET_DEF("companyValueRecord", ScScenario::companyValueRecord_get, ScScenario::companyValueRecord_set),
            };

            return MakeWithOpaque(ctx, funcs, nullptr);
        }

        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "Scenario");
        }
    };
} // namespace OpenRCT2::Scripting

#endif
