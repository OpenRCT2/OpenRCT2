/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "ObjectiveGoal.h"

class ObjectiveGoal;
using ObjectiveGoalPtr = std::shared_ptr<ObjectiveGoal>;

enum class GoalGroupType : uint8_t
{
    Dateless,
    BeforeDate,
    AtDate,
    AfterDate,
    DuringPeriod,
    AtEndOfPeriod,
    AfterPeriod,
    Permanent
};

enum class ObjectiveStatus : uint8_t
{
    Undecided,
    Success,
    Failure,
};

struct ObjectiveGoalGroup
{
    // stuff that needs to be saved:
    uint16_t monthGoal;           // total months, so for example: month 43
    bool disAllowEarlyCompletion; // forces to wait till date, regardless of ORCT setting
    std::string scenarioDetailsPhase;
    GoalGroupType groupType;
    std::vector<ObjectiveGoalPtr> goals;
    bool completed;
    uint16_t yearDate;
    uint8_t monthDate;
    uint16_t yearPeriod;
    uint8_t monthPeriod;
    // stuff that doesn't need to be saved:
    bool initialized;

public:
    // Do not call.
    ObjectiveGoalGroup()
        : groupType(GoalGroupType::Permanent)
        , initialized(false)
        , disAllowEarlyCompletion(false)
        , monthGoal(0)
        , scenarioDetailsPhase("")
        , completed(false)
        , yearDate(0)
        , monthDate(0)
        , yearPeriod(0)
        , monthPeriod(0)
    {
    }
    ObjectiveGoalGroup(GoalGroupType _type)
        : groupType(_type)
        , initialized(true)
        , disAllowEarlyCompletion(false)
        , monthGoal(0)
        , completed(false)
        , yearDate(0)
        , monthDate(0)
        , yearPeriod(0)
        , monthPeriod(0)
    {
    }
    // call for normal init with date
    ObjectiveGoalGroup(
        GoalGroupType _type, uint8_t _month, uint8_t _year, bool _allowEarlyCompletion, bool _disallowEarlyCompletion,
        std::string _scenarioDetailsPhase)
        : disAllowEarlyCompletion(_disallowEarlyCompletion)
        , groupType(_type)
        , initialized(true)
        , scenarioDetailsPhase(_scenarioDetailsPhase)
        , completed(false)
        , yearDate(0)
        , monthDate(0)
        , yearPeriod(0)
        , monthPeriod(0)
    {
        switch (_type)
        {
            case GoalGroupType::AtDate:
            case GoalGroupType::BeforeDate:
            case GoalGroupType::AfterDate:
                yearDate = _year;
                monthDate = _month;
                break;
            case GoalGroupType::AtEndOfPeriod:
            case GoalGroupType::DuringPeriod:
            case GoalGroupType::AfterPeriod:
                yearPeriod = _year;
                monthPeriod = _month;
                break;
        }
        monthGoal = MONTH_COUNT * (_year - 1) + _month;
        SetDate(monthGoal);
    }
    ObjectiveStatus CheckTotalGroup() const;
    rct_string_id AddGoal(ObjectiveGoalPtr _newGoal, bool _skipCheck = false);
    rct_string_id EditGoal(ObjectiveGoalPtr _newGoal, uint32_t _index);
    void DeleteGoal(uint32_t _index);
    bool Initialized() const
    {
        return initialized;
    }
    GoalGroupType GetGoalGroupType() const
    {
        return groupType;
    }
    uint16_t GetDate() const
    {
        return monthGoal;
    }
    void Start();
    void SetDate(uint32_t _totalMonths)
    {
        monthGoal = _totalMonths;
        monthDate = 1 + (_totalMonths - 1) % MONTH_COUNT;
        yearDate = 1 + (_totalMonths - 1) / MONTH_COUNT;
    }
    std::string GetDetails() const
    {
        return scenarioDetailsPhase;
    }
    std::string ToString(bool _editorView = false) const;
};
