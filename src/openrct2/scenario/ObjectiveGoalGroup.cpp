/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ObjectiveGoalGroup.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../FileClassifier.h"
#include "../Game.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../ParkImporter.h"
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../core/BitSet.hpp"
#include "../core/Guard.hpp"
#include "../core/Random.hpp"
#include "../entity/Duck.h"
#include "../entity/Guest.h"
#include "../entity/Staff.h"
#include "../interface/Viewport.h"
#include "../management/Award.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../management/Research.h"
#include "../network/network.h"
#include "../object/Object.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../platform/platform.h"
#include "../profiling/Profiling.h"
#include "../rct1/RCT1.h"
#include "../rct12/RCT12.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"
#include "../util/SawyerCoding.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Climate.h"
#include "../world/Entrance.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Water.h"
#include "ScenarioRepository.h"
#include "ScenarioSources.h"

#include <algorithm>
#include <openrct2/ride/TrackData.h>

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
        case GoalGroupType::Permanent:
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
    for (uint32_t i = 0; i < goals.size(); ++i)
    {
        add &= _newGoal->CheckConflictingGoal(goals[i], error);
    }

    if (groupType == GoalGroupType::Permanent) // If this is permanent goals, it must check against every phased goal
    {
        for (auto group : gScenarioObjective.PhasedGoals)
        {
            for (auto goal : group.goals)
            {
                add &= _newGoal->CheckConflictingGoal(goal, error);
                if (!add)
                    return error;
            }
        }
    }
    else // If this is in a phased goal, it only needs to check against the permanent goals.
    {
        for (auto goal : gScenarioObjective.PermanentGoals.goals)
        {
            add &= _newGoal->CheckConflictingGoal(goal, error);
            if (!add)
                return error;
        }
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
    if (groupType == GoalGroupType::Permanent) // If this is permanent goals, it must check against every phased goal
    {
        for (auto group : gScenarioObjective.PhasedGoals)
        {
            for (auto goal : group.goals)
            {
                edit &= _newGoal->CheckConflictingGoal(goal, error);
                if (!edit)
                    return error;
            }
        }
    }
    else // If this is in a phased goal, it only needs to check against the permanent goals.
    {
        for (auto goal : gScenarioObjective.PermanentGoals.goals)
        {
            edit &= _newGoal->CheckConflictingGoal(goal, error);
            if (!edit)
                return error;
        }
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
    }
}

/// <summary>
/// Create a string of how the group's deadline must be displayed
/// </summary>
/// <param name="_editorView"></param>
/// <returns></returns>
std::string ObjectiveGoalGroup::ToString(bool _editorView) const
{
    auto ft = Formatter();
    utf8 buffer[512];
    if (_editorView
        && (groupType == GoalGroupType::AtEndOfPeriod || groupType == GoalGroupType::DuringPeriod
            || groupType == GoalGroupType::AfterPeriod))
    {
        ft.Add<uint32_t>(yearPeriod);
        ft.Add<uint32_t>(monthPeriod);
        if (groupType == GoalGroupType::AtEndOfPeriod)
        {
            if (!disAllowEarlyCompletion && AllowEarlyCompletion())
                format_string(buffer, 512, STR_DURING_PERIOD_FULL, ft.Data());
            else
                format_string(buffer, 512, STR_AT_END_OF_PERIOD_FULL, ft.Data());
        }
        else if (groupType == GoalGroupType::DuringPeriod)
            format_string(buffer, 512, STR_DURING_PERIOD_FULL, ft.Data());
        else if (groupType == GoalGroupType::AfterPeriod)
            format_string(buffer, 512, STR_AFTER_PERIOD_FULL, ft.Data());
    }
    else
    {
        ft.Add<rct_string_id>(DateGameMonthNames[monthDate - 1]);
        ft.Add<uint32_t>(yearDate);
        if (groupType == GoalGroupType::AtDate || groupType == GoalGroupType::AtEndOfPeriod)
        {
            if (!disAllowEarlyCompletion && AllowEarlyCompletion())
                format_string(buffer, 512, STR_OBJECTIVE_BEFORE_DATE, ft.Data());
            else
                format_string(buffer, 512, STR_OBJECTIVE_AT_DATE, ft.Data());
        }
        else if (groupType == GoalGroupType::BeforeDate || groupType == GoalGroupType::DuringPeriod)
            format_string(buffer, 512, STR_OBJECTIVE_BEFORE_DATE, ft.Data());
        else if (groupType == GoalGroupType::AfterDate || groupType == GoalGroupType::AfterPeriod)
            format_string(buffer, 512, STR_OBJECTIVE_AFTER_DATE, ft.Data());
        else if (groupType == GoalGroupType::Dateless)
            format_string(buffer, 512, STR_NO_DEADLINE, ft.Data());
    }

    return std::string(buffer);
}
