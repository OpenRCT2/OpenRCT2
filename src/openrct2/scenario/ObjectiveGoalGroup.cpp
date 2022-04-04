/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ObjectiveGoalGroup.h"

#include "../network/network.h"
#include "../util/Util.h"
#include "../world/Park.h"
#include "ScenarioRepository.h"

/// <summary>
/// Completely check the group to see if the objective is succes, failure, or undecided
/// </summary>
/// <returns></returns>
ObjectiveStatus ObjectiveGoalGroup::CheckTotalGroup() const
{
    // is true if at the end Goal goals and Hybrid goals passed their condition check and if the date check is true
    bool positive = true;

    // is true if any of restriction goals have passed the condition check, or if the date check is a deadline, and is true
    bool negative = false;

    switch (groupType)
    {
        case GoalGroupType::AfterPeriod:
        case GoalGroupType::AfterDate:
            positive = (gDateMonthsElapsed > monthGoal);
            if (AllowEarlyCompletion() && !disAllowEarlyCompletion)
                positive = true;
            break;
        case GoalGroupType::Dateless:
            positive = true;
            break;
        case GoalGroupType::BeforeDate:
        case GoalGroupType::DuringPeriod:
            negative = (gDateMonthsElapsed >= monthGoal);
            positive = true;
            break;

        case GoalGroupType::AtEndOfPeriod:
        case GoalGroupType::AtDate:
        default:
            negative = (gDateMonthsElapsed >= monthGoal);
            positive = (gDateMonthsElapsed == monthGoal);
            if (gDateMonthsElapsed < monthGoal && ((AllowEarlyCompletion() && !disAllowEarlyCompletion)))
                positive = true;
            break;
    }
    // Needs to be checked regardless, as trueOnLastCheck needs to be updated
    for (auto goal : goals)
    {
        if (goal->GetUsesMoney() && (gParkFlags & PARK_FLAGS_NO_MONEY))
            continue;
        if (goal->GetGoalType() == GoalType::Goal || goal->GetGoalType() == GoalType::Hybrid)
            positive &= goal->CheckCondition();
        else
            negative |= goal->CheckCondition();
        if (goal->GetGoalType() == GoalType::Restriction || goal->GetGoalType() == GoalType::Hybrid)
            positive &= !goal->GetCountingDown();
    }

    if (positive) // Dategoal must check positive before negative, as both turn true on the same day.
        return ObjectiveStatus::Success;
    if (negative)
        return ObjectiveStatus::Failure;

    return ObjectiveStatus::Undecided;
}

/// <summary>
/// Add a goal
/// </summary>
/// <param name="_newGoal"></param>
/// <param name="_skipCheck">Whether to check for incompetabilities</param>
/// <returns></returns>
rct_string_id ObjectiveGoalGroup::AddGoal(ObjectiveGoalPtr _newGoal, bool _skipCheck)
{
    if (_skipCheck)
    {
        goals.emplace_back(_newGoal);
        return STR_NONE;
    }

    gScenarioObjective.LegacyType = OBJECTIVE_MODULAR_SYSTEM_V1;
    rct_string_id error;
    bool add = _newGoal->CheckSpecialRequirements(error);
    if (!add)
        return error;
    for (auto goal : goals)
    {
        add &= _newGoal->CheckConflictingGoal(goal, error);
    }
    if (add)
    {
        goals.emplace_back(_newGoal);
        return STR_NONE;
    }
    return error;
}

/// <summary>
/// Replace a goal at index
/// </summary>
/// <param name="_newGoal"></param>
/// <param name="_index"></param>
/// <returns></returns>
rct_string_id ObjectiveGoalGroup::EditGoal(ObjectiveGoalPtr _newGoal, uint32_t _index)
{
    rct_string_id error;
    bool edit = _newGoal->CheckSpecialRequirements(error);

    if (!edit)
        return error;

    for (uint32_t i = 0; i < goals.size(); ++i)
    {
        if (i != _index) // don't check against self
            edit &= _newGoal->CheckConflictingGoal(goals[i], error);
        if (!edit)
            return error;
    }
    if (edit)
    {
        goals[_index] = _newGoal;
        gScenarioObjective.CalculateAllowParkOpening();
        return STR_NONE;
    }
    return error;
}

/// <summary>
/// Delete a goal at index
/// </summary>
/// <param name="_index"></param>
void ObjectiveGoalGroup::DeleteGoal(uint32_t _index)
{
    gScenarioObjective.LegacyType = OBJECTIVE_MODULAR_SYSTEM_V1;
    goals.erase(goals.begin() + _index);
}

void ObjectiveGoalGroup::Start()
{
    switch (groupType)
    {
        case GoalGroupType::AtDate:
        case GoalGroupType::BeforeDate:
            monthGoal = MONTH_COUNT * (yearDate - 1) + monthDate;
            SetDate(monthGoal);
            break;
        case GoalGroupType::AtEndOfPeriod:
        case GoalGroupType::DuringPeriod:
            monthGoal = gDateMonthsElapsed + MONTH_COUNT * yearPeriod + monthPeriod;
            SetDate(monthGoal);
            break;
        default:
            break;
    }
}
