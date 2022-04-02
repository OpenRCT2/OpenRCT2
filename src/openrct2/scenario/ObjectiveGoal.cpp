/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ObjectiveGoal.h"

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
#include "../profiling/Profiling.h"
#include "../rct1/RCT1.h"
#include "../rct12/RCT12.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
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

#pragma region FunctionsDeclarations
bool ValueSignThresholdCondition(uint32_t value, uint32_t threshold, Sign sign);
std::string ValueThresholdGoalToString(
    Sign sign, GoalType goalType, uint64_t value, rct_string_id goal, rct_string_id restriction, rct_string_id fewer,
    bool currency = false);
bool CheckRides(
    uint8_t rideType, uint16_t minNum, uint16_t maxNum, uint16_t minLen = 0, uint16_t maxLen = 0, uint16_t minEx = 0,
    uint16_t maxEx = 0, uint16_t minIn = 0, uint16_t maxIn = 0, uint16_t minNau = 0, uint16_t maxNau = 0, bool unique = false,
    bool mustBeFinished = false);
static std::pair<rct_string_id, Formatter> GetFullRideName(
    const uint8_t _baseRideType, const ObjectEntryIndex _entryIndex, Formatter ft);
static rct_string_id GetShortRideName(const uint8_t _baseRideType, const ObjectEntryIndex _entryIndex);
std::string NumRidesString(
    uint8_t minNumRidesGoal, uint8_t maxNumRidesGoal, bool mustBeUniqueTypes, rct_string_id rideTypePlural,
    rct_string_id rideType, bool finishedExistingRides);
std::string AddRideStat(
    uint16_t minVal, uint16_t maxVal, rct_string_id valueType, bool* first, bool isLength = false, bool isStat = false,
    bool isSpeed = false, bool singular = false);
#pragma endregion

#pragma region Commonfunctions

/// <summary>
/// Check if a value is above, or below a threshold, based on a sign
/// </summary>
/// <param name="value"></param>
/// <param name="threshold"></param>
/// <param name="sign"></param>
/// <returns></returns>
bool ValueSignThresholdCondition(uint32_t value, uint32_t threshold, Sign sign)
{
    switch (sign)
    {
        case Sign::SmallerThan:
            return (value < threshold);
        case Sign::BiggerThan:
        default:
            return (value >= threshold);
    }
}

/// <summary>
/// Form a string for objectives involving a single value that must be above or below a threshold, possibly as restriction.
/// </summary>
/// <param name="sign"></param>
/// <param name="goalType"></param>
/// <param name="value"></param>
/// <param name="goal">string to use if it's a goal</param>
/// <param name="restriction">string to use if it's a restriction</param>
/// <param name="fewer">The rctstringid for which word fits the topic</param>
/// <param name="currency">Is this a currency value</param>
/// <returns></returns>
std::string ValueThresholdGoalToString(
    Sign sign, GoalType goalType, uint64_t value, rct_string_id goal, rct_string_id restriction, rct_string_id fewer,
    bool currency)
{
    auto ft = Formatter();
    utf8 buffer[512];

    if (sign == Sign::BiggerThan)
    {
        if (goalType == GoalType::Goal)
        {
            ft.Add<rct_string_id>(STR_AT_LEAST);
            if (!currency)
                ft.Add<uint32_t>(value);
            else
                ft.Add<money32>(value);
            format_string(buffer, 512, goal, ft.Data());
        }
        else
        {
            ft.Add<rct_string_id>(fewer);
            if (!currency)
                ft.Add<uint32_t>(value);
            else
                ft.Add<money32>(value);
            format_string(buffer, 512, restriction, ft.Data());
        }
    }
    else
    {
        if (goalType == GoalType::Goal)
        {
            ft.Add<rct_string_id>(STR_AT_MOST);
            if (!currency)
                ft.Add<uint32_t>(value);
            else
                ft.Add<money32>(value);
            format_string(buffer, 512, goal, ft.Data());
        }
        else
        {
            ft.Add<rct_string_id>(STR_MORE);
            if (!currency)
                ft.Add<uint32_t>(value);
            else
                ft.Add<money32>(value);
            format_string(buffer, 512, restriction, ft.Data());
        }
    }
    return std::string(buffer);
}

/// <summary>
/// Common function for all the num rides goals to do the check
/// </summary>
/// <param name="rideType">Category, gentle, thrill etc.</param>
/// <param name="minNum"></param>
/// <param name="maxNum"></param>
/// <param name="minLen"></param>
/// <param name="maxLen"></param>
/// <param name="minEx"></param>
/// <param name="maxEx"></param>
/// <param name="minIn"></param>
/// <param name="maxIn"></param>
/// <param name="minNau"></param>
/// <param name="maxNau"></param>
/// <param name="unique"></param>
/// <param name="mustBeFinished">Check for finished pre-existing rides</param>
/// <returns></returns>
bool CheckRides(
    uint8_t rideType, uint16_t minNum, uint16_t maxNum, uint16_t minLen, uint16_t maxLen, uint16_t minEx, uint16_t maxEx,
    uint16_t minIn, uint16_t maxIn, uint16_t minNau, uint16_t maxNau, bool unique, bool mustBeFinished)
{
    OpenRCT2::BitSet<MAX_RIDE_OBJECTS> type_already_counted;
    auto count = 0;

    for (const auto& ride : GetRideManager())
    {
        auto rideEntry = ride.GetRideEntry();
        if (rideEntry != nullptr)
        {
            if (ride.status != RideStatus::Open || ride.subtype == OBJECT_ENTRY_INDEX_NULL)
                continue;

            if (ride_entry_has_category(rideEntry, rideType))
            {
                if (!type_already_counted[ride.subtype] && (ride.GetTotalLength() >> 16) >= minLen
                    && (maxLen == 0 || (ride.GetTotalLength() >> 16) <= maxLen) && ride.excitement >= minEx
                    && (maxEx == 0 || ride.excitement <= maxEx) && ride.intensity >= minIn
                    && (maxIn == 0 || ride.intensity <= maxIn) && ride.nausea >= minNau
                    && (maxNau == 0 || ride.nausea <= maxNau)
                    && (!mustBeFinished || (ride.lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)))

                {
                    if (unique)
                        type_already_counted[ride.subtype] = true;
                    count++;
                }
            }
        }
    }
    if (count < minNum || (maxNum != 0 && count > maxNum))
        return false;
    return true;
}

/// <summary>
/// Get a full ride name + vehicle type based on the base-ride-type and entry-index
/// </summary>
/// <param name="_baseRideType"></param>
/// <param name="_entryIndex"></param>
/// <param name="ft"></param>
/// <returns></returns>
static std::pair<rct_string_id, Formatter> GetFullRideName(
    const uint8_t _baseRideType, const ObjectEntryIndex _entryIndex, Formatter ft)
{
    rct_string_id drawString;
    rct_string_id stringId = STR_NONE;
    rct_ride_entry* rideEntry = get_ride_entry(_entryIndex);
    if (rideEntry != nullptr)
    {
        stringId = rideEntry->naming.Name;
    }

    if (!GetRideTypeDescriptor(_baseRideType).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
    {
        drawString = STR_STRINGID_COLON_STRINGID;
        rct_string_id rideTypeName = get_ride_naming(_baseRideType, get_ride_entry(_entryIndex)).Name;
        ft.Add<rct_string_id>(rideTypeName);
        ft.Add<rct_string_id>(stringId);
    }
    else
    {
        drawString = STR_STRINGID;
        ft.Add<rct_string_id>(stringId);
    }

    return std::make_pair(drawString, ft);
}

/// <summary>
/// Get the short ridename without vehicles, but with the difference between flat-ride varients
/// </summary>
/// <param name="_baseRideType"></param>
/// <param name="_entryIndex"></param>
/// <returns></returns>
static rct_string_id GetShortRideName(const uint8_t _baseRideType, const ObjectEntryIndex _entryIndex)
{
    rct_ride_entry* rideEntry;
    rideEntry = get_ride_entry(_entryIndex);
    if (!GetRideTypeDescriptor(_baseRideType).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
        return get_ride_naming(_baseRideType, get_ride_entry(_entryIndex)).Name;
    else if (rideEntry != nullptr)
        return rideEntry->naming.Name;
    else
        return GetRideTypeDescriptor(_baseRideType).Naming.Name;
}

/// <summary>
/// Common function to create the string for num rides goals.
/// </summary>
/// <param name="minNumRidesGoal"></param>
/// <param name="maxNumRidesGoal"></param>
/// <param name="mustBeUniqueTypes"></param>
/// <param name="rideTypePlural"></param>
/// <param name="rideType"></param>
/// <param name="finishedExistingRides"></param>
/// <returns></returns>
std::string NumRidesString(
    uint8_t minNumRidesGoal, uint8_t maxNumRidesGoal, bool mustBeUniqueTypes, rct_string_id rideTypePlural,
    rct_string_id rideType, bool finishedExistingRides)
{
    auto ft = Formatter();
    utf8 buffer[512];

    if (minNumRidesGoal == 0)
    {
        ft.Add<rct_string_id>(STR_AT_MOST);
        ft.Add<uint32_t>(maxNumRidesGoal);
    }
    else if (minNumRidesGoal > 0 && maxNumRidesGoal == 0)
    {
        ft.Add<rct_string_id>(STR_AT_LEAST);
        ft.Add<uint32_t>(minNumRidesGoal);
    }
    else if (minNumRidesGoal == maxNumRidesGoal)
    {
        ft.Add<uint32_t>(maxNumRidesGoal);
        if (maxNumRidesGoal == 1 && !finishedExistingRides)
            ft.Add<rct_string_id>(rideType);
        else
            ft.Add<rct_string_id>(rideTypePlural);
        if (!finishedExistingRides)
        {
            format_string(buffer, 512, STR_EXACTLY_X_RIDES_OF_CATEGORY_GOAL, ft.Data());
        }
        else
        {
            format_string(buffer, 512, STR_FINISH_EXACTLY_X_RIDES_OF_CATEGORY_GOAL, ft.Data());
        }
        return std::string(buffer);
    }
    if (!(minNumRidesGoal != 0 && maxNumRidesGoal != 0))
    {
        if (minNumRidesGoal == 1 && maxNumRidesGoal == 0 || minNumRidesGoal == 0 && maxNumRidesGoal == 1)
        {
            if (!finishedExistingRides)
            {
                ft.Add<rct_string_id>(STR_SPACE);
                ft.Add<rct_string_id>(rideType);
                format_string(buffer, 512, STR_RIDE_NUM_ABOVEBELOW_GOAL_DESCRIPTION, ft.Data());
            }
            else
            {
                ft.Add<rct_string_id>(rideType);
                format_string(buffer, 512, STR_FINISH_RIDES_ABOVEBELOW_GOAL_DESCRIPTION, ft.Data());
            }
        }
        else
        {
            if (!finishedExistingRides)
            {
                if (mustBeUniqueTypes)
                    ft.Add<rct_string_id>(STR_DIFFERENT);
                else
                    ft.Add<rct_string_id>(STR_SPACE);
                ft.Add<rct_string_id>(rideTypePlural);
                format_string(buffer, 512, STR_RIDE_NUM_ABOVEBELOW_GOAL_DESCRIPTION, ft.Data());
            }
            else
            {
                ft.Add<rct_string_id>(rideTypePlural);
                format_string(buffer, 512, STR_FINISH_RIDES_ABOVEBELOW_GOAL_DESCRIPTION, ft.Data());
            }
        }
    }
    else
    {
        ft.Add<uint32_t>(minNumRidesGoal);
        ft.Add<uint32_t>(maxNumRidesGoal);
        if (mustBeUniqueTypes && !finishedExistingRides)
            ft.Add<rct_string_id>(STR_DIFFERENT);
        else if (!mustBeUniqueTypes && !finishedExistingRides)
            ft.Add<rct_string_id>(STR_SPACE);
        ft.Add<rct_string_id>(rideTypePlural);
        if (!finishedExistingRides)
            format_string(buffer, 512, STR_RIDE_NUM_BETWEEN_GOAL, ft.Data());
        else
            format_string(buffer, 512, STR_FINISH_RIDES_BETWEEN_GOAL_DESCRIPTION, ft.Data());
    }
    return std::string(buffer);
}

/// <summary>
/// Common function for ride goals to add a string for the limits of a stat
/// </summary>
/// <param name="minVal"></param>
/// <param name="maxVal"></param>
/// <param name="valueType"></param>
/// <param name="first"></param>
/// <param name="isLength"></param>
/// <param name="isStat"></param>
/// <param name="isSpeed"></param>
/// <param name="singular"></param>
/// <returns></returns>
std::string AddRideStat(
    uint16_t minVal, uint16_t maxVal, rct_string_id valueType, bool* first, bool isLength, bool isStat, bool isSpeed,
    bool singular)
{
    if (minVal == 0 && maxVal == 0)
    {
        return std::string();
    }
    auto ft = Formatter();
    utf8 buffer[512];

    ft.Add<rct_string_id>(valueType);

    if (minVal == 0)
    {
        ft.Add<rct_string_id>(STR_AT_MOST);
        if (isLength)
            ft.Add<rct_string_id>(STR_RIDE_LENGTH_ENTRY);
        else if (isStat)
            ft.Add<rct_string_id>(STR_EX_IN_NAU_RATING);
        else if (isSpeed)
            ft.Add<rct_string_id>(STR_RIDE_VELOCITY);
        else
            ft.Add<rct_string_id>(STR_FORMAT_INTEGER);
        ft.Add<uint32_t>(maxVal);
    }
    else if (minVal > 0 && maxVal == 0)
    {
        ft.Add<rct_string_id>(STR_AT_LEAST);
        if (isLength)
            ft.Add<rct_string_id>(STR_RIDE_LENGTH_ENTRY);
        else if (isStat)
            ft.Add<rct_string_id>(STR_EX_IN_NAU_RATING);
        else if (isSpeed)
            ft.Add<rct_string_id>(STR_RIDE_VELOCITY);
        else
            ft.Add<rct_string_id>(STR_FORMAT_INTEGER);
        ft.Add<uint32_t>(minVal);
    }
    else if (minVal == maxVal)
    {
        ft.Add<rct_string_id>(STR_EXACTLY);
        if (isLength)
            ft.Add<rct_string_id>(STR_RIDE_LENGTH_ENTRY);
        else if (isStat)
            ft.Add<rct_string_id>(STR_EX_IN_NAU_RATING);
        else if (isSpeed)
            ft.Add<rct_string_id>(STR_RIDE_VELOCITY);
        else
            ft.Add<rct_string_id>(STR_FORMAT_INTEGER);
        ft.Add<uint32_t>(minVal);
    }
    if (!(minVal != 0 && maxVal != 0) || (minVal == maxVal))
    {
        if (*first)
        {
            if (singular)
                format_string(buffer, 512, STR_BUILD_RIDE_STAT_ABOVEBELOW, ft.Data());
            else
                format_string(buffer, 512, STR_RIDE_NUM_ATTRIBUTE_ABOVEBELOW, ft.Data());
        }
        else
            format_string(buffer, 512, STR_RIDE_NUM_ATTRIBUTE_ABOVEBELOW_2, ft.Data());
    }
    else
    {
        rct_string_id format;

        if (isLength)
        {
            ft.Add<rct_string_id>(STR_RIDE_LENGTH_ENTRY);
            ft.Add<uint16_t>(minVal);
            ft.Add<rct_string_id>(STR_RIDE_LENGTH_ENTRY);
            ft.Add<uint16_t>(maxVal);
        }
        else
        {
            if (isStat)
                format = STR_EX_IN_NAU_RATING;
            else if (isSpeed)
                format = STR_RIDE_VELOCITY;
            else
                format = STR_FORMAT_INTEGER;

            ft.Add<rct_string_id>(format);
            ft.Add<uint32_t>(minVal);
            ft.Add<rct_string_id>(format);
            ft.Add<uint32_t>(maxVal);
        }
        if (*first)
            if (singular)
                format_string(buffer, 512, STR_BUILD_RIDE_STAT_BETWEEN, ft.Data());
            else
                format_string(buffer, 512, STR_RIDE_NUM_ATTRIBUTE_BETWEEN, ft.Data());
        else
            format_string(buffer, 512, STR_RIDE_NUM_ATTRIBUTE_BETWEEN_2, ft.Data());
    }
    *first = false;
    return std::string(buffer);
}

#pragma endregion

#pragma region ObjectiveGoal
ObjectiveGoal::ObjectiveGoal(
    GoalID _id, GoalType _type, Sign _sign, bool _usesMoney, uint16_t _warningDayIndex, uint16_t _leeWayPeriod)
    : goalID(_id)
    , goalType(_type)
    , sign(_sign)
    , warningDaysIndex(_warningDayIndex)
    , warningWeeksPeriod(_leeWayPeriod)
    , trueOnLastCheck(false)
    , usesMoney(_usesMoney)
{
    if (goalType == GoalType::Restriction || goalType == GoalType::Hybrid)
    {
        gScenarioObjectiveWarningDays.push_back(0);
        warningDaysIndex = static_cast<uint16_t>(gScenarioObjectiveWarningDays.size()) - 1;
    }
};

void ObjectiveGoal::SetGoalType(GoalType _goalType)
{
    goalType = _goalType;
    redoToString = true;
    if (goalType == GoalType::Restriction && warningDaysIndex == 0) // default is 0, yes, but that'll cause only 1 redudent
                                                                    // copy.
    {
        gScenarioObjectiveWarningDays.push_back(0);
        warningDaysIndex = static_cast<uint16_t>(gScenarioObjectiveWarningDays.size()) - 1;
    }

    gScenarioObjective.CalculateAllowParkOpening();
}

bool ObjectiveGoal::CheckGoalRestrictionWarningDays(
    bool test, rct_string_id goalAsWord, uint64_t value, rct_string_id valueStringFormat)
{
    if (gDateMonthsElapsed < 1)
    {
        return false;
    }
    uint16_t* warningDays = &gScenarioObjectiveWarningDays[warningDaysIndex];

    auto ft = Formatter();
    ft.Add<rct_string_id>(goalAsWord);
    if (test)
    {
        countingDown = true;
        (*warningDays)++;
        if (*warningDays == warningWeeksPeriod * 7)
        {
            News::AddItemToQueue(News::ItemType::Graph, STR_PARK_HAS_BEEN_CLOSED_DOWN, 0, {});
            gParkFlags &= ~PARK_FLAGS_PARK_OPEN;
            gGuestInitialHappiness = 50;
            return true;
        }
        else if (*warningDays == 1)
        {
            if (gConfigNotifications.park_objective_warnings)
            {
                if (sign == Sign::BiggerThan)
                {
                    ft.Add<rct_string_id>(STR_DROPPED_BELOW);
                    ft.Add<rct_string_id>(valueStringFormat);
                    if (valueStringFormat == STR_CURRENCY_FORMAT)
                        ft.Add<money64>(value);
                    else
                        ft.Add<uint32_t>(value);
                    ft.Add<rct_string_id>(STR_RAISED);
                    ft.Add<rct_string_id>(goalAsWord);
                    ft.Add<uint16_t>(warningWeeksPeriod);
                }
                else if (sign == Sign::SmallerThan)
                {
                    ft.Add<rct_string_id>(STR_RISEN_ABOVE);
                    ft.Add<rct_string_id>(valueStringFormat);
                    if (valueStringFormat == STR_CURRENCY_FORMAT)
                        ft.Add<money64>(value);
                    else
                        ft.Add<uint32_t>(value);
                    ft.Add<rct_string_id>(STR_LOWERED);
                    ft.Add<rct_string_id>(goalAsWord);
                    ft.Add<uint16_t>(warningWeeksPeriod);
                }
                News::AddItemToQueue(News::ItemType::Graph, STR_GENERIC_VALUE_TOO_SIGN_FIRST_WARNING, 0, ft);
            }
        }
        else if (*warningDays == (warningWeeksPeriod - 2) * 7)
        {
            if (gConfigNotifications.park_objective_warnings)
            {
                if (sign == Sign::BiggerThan)
                {
                    ft.Add<rct_string_id>(STR_BELOW);
                    ft.Add<rct_string_id>(valueStringFormat);
                    if (valueStringFormat == STR_CURRENCY_FORMAT)
                        ft.Add<money64>(value);
                    else
                        ft.Add<uint32_t>(value);
                    ft.Add<rct_string_id>(STR_RAISE);
                    ft.Add<rct_string_id>(goalAsWord);
                }
                else if (sign == Sign::SmallerThan)
                {
                    ft.Add<rct_string_id>(STR_ABOVE);
                    ft.Add<rct_string_id>(valueStringFormat);
                    if (valueStringFormat == STR_CURRENCY_FORMAT)
                        ft.Add<money64>(value);
                    else
                        ft.Add<uint32_t>(value);
                    ft.Add<rct_string_id>(STR_LOWER);
                    ft.Add<rct_string_id>(goalAsWord);
                }
                News::AddItemToQueue(News::ItemType::Graph, STR_GENERIC_VALUE_TOO_SIGN_SEMILAST_WARNING, 0, ft);
            }
        }
        else if (*warningDays == (warningWeeksPeriod - 1) * 7)
        {
            if (gConfigNotifications.park_objective_warnings)
            {
                if (sign == Sign::BiggerThan)
                {
                    ft.Add<rct_string_id>(STR_BELOW);
                    ft.Add<rct_string_id>(valueStringFormat);
                    if (valueStringFormat == STR_CURRENCY_FORMAT)
                        ft.Add<money64>(value);
                    else
                        ft.Add<uint32_t>(value);
                    ft.Add<rct_string_id>(STR_RAISE);
                    ft.Add<rct_string_id>(goalAsWord);
                }
                else if (sign == Sign::SmallerThan)
                {
                    ft.Add<rct_string_id>(STR_ABOVE);
                    ft.Add<rct_string_id>(valueStringFormat);
                    if (valueStringFormat == STR_CURRENCY_FORMAT)
                        ft.Add<money64>(value);
                    else
                        ft.Add<uint32_t>(value);
                    ft.Add<rct_string_id>(STR_LOWER);
                    ft.Add<rct_string_id>(goalAsWord);
                }
                News::AddItemToQueue(News::ItemType::Graph, STR_GENERIC_VALUE_TOO_SIGN_LAST_WARNING, 0, ft);
            }
        }
        else if (*warningDays % 7 == 0)
        {
            if (gConfigNotifications.park_objective_warnings)
            {
                if (sign == Sign::BiggerThan)
                {
                    ft.Add<rct_string_id>(STR_BELOW);
                    ft.Add<rct_string_id>(valueStringFormat);
                    if (valueStringFormat == STR_CURRENCY_FORMAT)
                        ft.Add<money64>(value);
                    else
                        ft.Add<uint32_t>(value);
                    ft.Add<uint32_t>(warningWeeksPeriod - (*warningDays / 7));
                    ft.Add<rct_string_id>(STR_RAISE);
                    ft.Add<rct_string_id>(goalAsWord);
                }
                else if (sign == Sign::SmallerThan)
                {
                    ft.Add<rct_string_id>(STR_ABOVE);
                    ft.Add<rct_string_id>(valueStringFormat);
                    if (valueStringFormat == STR_CURRENCY_FORMAT)
                        ft.Add<money64>(value);
                    else
                        ft.Add<uint32_t>(value);
                    ft.Add<uint32_t>(warningWeeksPeriod - (*warningDays / 7));
                    ft.Add<rct_string_id>(STR_LOWER);
                    ft.Add<rct_string_id>(goalAsWord);
                }
                News::AddItemToQueue(News::ItemType::Graph, STR_GENERIC_VALUE_TOO_SIGN_NORMAL_WARNING, 0, ft);
            }
        }
    }
    else if (gScenarioCompletedCompanyValue != COMPANY_VALUE_ON_FAILED_OBJECTIVE)
    {
        countingDown = false;
        *warningDays = 0;
    }

    return false;
}

bool ObjectiveGoal::CheckSpecialRequirements(rct_string_id& error) const
{
    bool okay = !usesMoney
        || !(
            ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && (gParkFlags & PARK_FLAGS_NO_MONEY_SCENARIO))
            || (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && (gParkFlags & PARK_FLAGS_NO_MONEY)));
    if (!okay)
        error = STR_ERROR_OBJECTIVE_REQUIRES_MONEY;
    return okay;
}

bool ObjectiveGoal::CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const
{
    if (_otherGoal->GetGoalID() != goalID)
        return true;
    error = STR_ERROR_OBJECTIVE_CANNOT_ADD_TWICE;
    return false;
}

#pragma endregion

#pragma region DummyGoal
std::string ObjectiveDummyGoal::ToString()
{
    if (!redoToString)
        return longText;
    utf8 buffer[512];
    format_string(buffer, 512, STR_DUMMY_GOAL_SHORT, {});
    longText = std::string(buffer);
    redoToString = false;
    return longText;
}
#pragma endregion

#pragma region GuestNumGoal
bool ObjectiveGuestNumGoal::CheckCondition()
{
    trueOnLastCheck = ValueSignThresholdCondition(gNumGuestsInPark, guestNumGoal, sign);
    if (goalType == GoalType::Restriction)
        trueOnLastCheck = CheckGoalRestrictionWarningDays(!trueOnLastCheck, STR_GUEST_NUMBER, guestNumGoal);
    return trueOnLastCheck;
}
std::string ObjectiveGuestNumGoal::ToString()
{
    if (!redoToString)
        return longText;
    longText = ValueThresholdGoalToString(
        sign, goalType, guestNumGoal, STR_GUEST_NUM_GOAL_DESCRIPTION, STR_GUEST_NUM_RESTRICTION_DESCRIPTION, STR_FEWER);
    redoToString = false;
    return longText;
}

bool ObjectiveGuestNumGoal::CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const
{
    if (_otherGoal->GetGoalID() != goalID)
        return true;
    auto other = std::static_pointer_cast<ObjectiveGuestNumGoal>(_otherGoal);
    error = STR_ERROR_OBJECTIVE_VALUE_RANGE_CONFLICT;
    if (sign == Sign::BiggerThan && other->GetSign() == Sign::SmallerThan && guestNumGoal > other->GetGuestNumGoal())
        return false;
    if (sign == Sign::SmallerThan && other->GetSign() == Sign::BiggerThan && guestNumGoal < other->GetGuestNumGoal())
        return false;
    error = STR_NONE;
    return true;
}
#pragma endregion

#pragma region ParkValueGoal
bool ObjectiveParkValueGoal::CheckCondition()
{
    money32 objectiveParkValue = parkValueGoal;
    money32 parkValue = gParkValue;
    trueOnLastCheck = ValueSignThresholdCondition(parkValue, objectiveParkValue, sign);
    if (goalType == GoalType::Restriction)
        trueOnLastCheck = CheckGoalRestrictionWarningDays(
            !trueOnLastCheck, STR_PARK_VALUE, objectiveParkValue, STR_CURRENCY_FORMAT);
    return trueOnLastCheck;
}

std::string ObjectiveParkValueGoal::ToString()
{
    if (!redoToString)
        return longText;
    longText = ValueThresholdGoalToString(
        sign, goalType, parkValueGoal, STR_PARK_VALUE_GOAL_DESCRIPTION, STR_PARK_VALUE_RESTRICTION_DESCRIPTION, STR_LESS);
    redoToString = false;
    return longText;
}

bool ObjectiveParkValueGoal::CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const
{
    if (_otherGoal->GetGoalID() != goalID)
        return true;
    auto other = std::static_pointer_cast<ObjectiveParkValueGoal>(_otherGoal);
    error = STR_ERROR_OBJECTIVE_VALUE_RANGE_CONFLICT;
    if (sign == Sign::BiggerThan && other->GetSign() == Sign::SmallerThan && parkValueGoal > other->GetParkValueGoal())
        return false;
    if (sign == Sign::SmallerThan && other->GetSign() == Sign::BiggerThan && parkValueGoal < other->GetParkValueGoal())
        return false;
    error = STR_NONE;
    return true;
}

#pragma endregion

#pragma region ParkRatingGoal
bool ObjectiveParkRatingGoal::CheckCondition()
{
    trueOnLastCheck = ValueSignThresholdCondition(gParkRating, parkRatingGoal, sign);
    if (goalType == GoalType::Restriction)
        trueOnLastCheck = CheckGoalRestrictionWarningDays(!trueOnLastCheck, STR_PARK_RATING, parkRatingGoal);
    return trueOnLastCheck;
}

std::string ObjectiveParkRatingGoal::ToString()
{
    if (!redoToString)
        return longText;
    longText = ValueThresholdGoalToString(
        sign, goalType, parkRatingGoal, STR_PARK_RATING_GOAL_DESCRIPTION, STR_PARK_RATING_RESTRICTION_DESCRIPTION, STR_LESS);
    redoToString = false;
    return longText;
}

bool ObjectiveParkRatingGoal::CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const
{
    if (_otherGoal->GetGoalID() != goalID)
        return true;
    auto other = std::static_pointer_cast<ObjectiveParkRatingGoal>(_otherGoal);

    error = STR_ERROR_OBJECTIVE_VALUE_RANGE_CONFLICT;
    if (sign == Sign::BiggerThan && other->GetSign() == Sign::SmallerThan && parkRatingGoal > other->GetParkRatingGoal())
        return false;
    if (sign == Sign::SmallerThan && other->GetSign() == Sign::BiggerThan && parkRatingGoal < other->GetParkRatingGoal())
        return false;
    error = STR_NONE;
    return true;
}
#pragma endregion

#pragma region ParkSizeGoal
bool ObjectiveParkSizeGoal::CheckCondition()
{
    trueOnLastCheck = ValueSignThresholdCondition(gParkSize, parkSizeGoal, sign);
    if (goalType == GoalType::Goal)
        return trueOnLastCheck;
    else
    {
        trueOnLastCheck = !trueOnLastCheck;
        if (trueOnLastCheck)
        {
            News::AddItemToQueue(News::ItemType::Graph, STR_PARK_HAS_BEEN_CLOSED_DOWN, 0, {});
            gParkFlags &= ~PARK_FLAGS_PARK_OPEN;
            gGuestInitialHappiness = 50;
        }
        return trueOnLastCheck;
    }
}

bool ObjectiveParkSizeGoal::CheckSpecialRequirements(rct_string_id& error) const
{
    uint32_t size = park_calculate_size();
    map_count_remaining_land_rights();
    uint32_t remaining = gLandRemainingOwnershipSales + gLandRemainingConstructionSales;
    switch (sign)
    {
        case Sign::BiggerThan:
            if (size + remaining < parkSizeGoal)
            {
                error = STR_ERROR_OBJECTIVE_PARK_TOO_SMALL;
                return false;
            }
            break;
        case Sign::SmallerThan:
            if (size > parkSizeGoal)
            {
                error = STR_ERROR_OBJECTIVE_PARK_TOO_BIG;
                return false;
            }
            break;
        default:
            break;
    }
    return true && ObjectiveGoal::CheckSpecialRequirements(error);
}

std::string ObjectiveParkSizeGoal::ToString()
{
    if (!redoToString)
        return longText;

    auto ft = Formatter();
    utf8 buffer[512];

    if (sign == Sign::BiggerThan)
        ft.Add<rct_string_id>(STR_AT_LEAST);
    else if (sign == Sign::SmallerThan)
        ft.Add<rct_string_id>(STR_AT_MOST);

    if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
    {
        ft.Add<uint32_t>(squaredmetres_to_squaredfeet(parkSizeGoal * 10));
        format_string(buffer, 512, STR_PARK_SIZE_GOAL_DESCRIPTION_IMPERIAL, ft.Data());
    }
    else
    {
        ft.Add<uint32_t>(parkSizeGoal * 10);
        format_string(buffer, 512, STR_PARK_SIZE_GOAL_DESCRIPTION_METRIC, ft.Data());
    }

    longText = std::string(buffer);
    redoToString = false;
    return longText;
}

bool ObjectiveParkSizeGoal::CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const
{
    if (_otherGoal->GetGoalID() != goalID)
        return true;
    auto other = std::static_pointer_cast<ObjectiveParkSizeGoal>(_otherGoal);
    error = STR_ERROR_OBJECTIVE_VALUE_RANGE_CONFLICT;
    if (sign == Sign::BiggerThan && other->GetSign() == Sign::SmallerThan && parkSizeGoal > other->GetParkSizeGoal())
        return false;
    if (sign == Sign::SmallerThan && other->GetSign() == Sign::BiggerThan && parkSizeGoal < other->GetParkSizeGoal())
        return false;
    error = STR_NONE;
    return true;
}
#pragma endregion

#pragma region ProfitGoals
#pragma region ProfitGoal
bool ObjectiveProfitGoal::CheckCondition()
{
    money32 lastMonthIncome = gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::FoodDrinkSales)]
        + gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::FoodDrinkStock)]
        + gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::ShopSales)]
        + gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::ShopStock)]
        + gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::ParkEntranceTickets)]
        + gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::ParkRideTickets)];
    trueOnLastCheck = ValueSignThresholdCondition(lastMonthIncome, profitGoal, sign);
    if (goalType == GoalType::Restriction)
        trueOnLastCheck = CheckGoalRestrictionWarningDays(!trueOnLastCheck, STR_PROFIT, profitGoal, STR_CURRENCY_FORMAT);
    return trueOnLastCheck;
}

std::string ObjectiveProfitGoal::ToString()
{
    if (!redoToString)
        return longText;
    longText = ValueThresholdGoalToString(
        sign, goalType, profitGoal, STR_PROFIT_GOAL_DESCRIPTION, STR_PROFIT_RESTRICTION_DESCRIPTION, STR_LESS, true);
    redoToString = false;
    return longText;
}

bool ObjectiveProfitGoal::CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const
{
    // This will not pick on up on two goals combining a lower limit to overcome totalincome's upper limit or similar
    // situations.
    switch (_otherGoal->GetGoalID())
    {
        case GoalID::ProfitGoal:
        case GoalID::FoodProfitGoal:
        case GoalID::MerchandiseProfitGoal:
        case GoalID::ParkEntryProfitGoal:
        case GoalID::RideTicketProfitGoal:
        case GoalID::StallProfitGoal:
            break;
        default:
            return true;
    }

    auto other = std::static_pointer_cast<ObjectiveProfitGoal>(_otherGoal);
    if ((goalID == GoalID::ProfitGoal) // total income cannot be lower than any subincome's lowest
        && (other->GetGoalID() != GoalID::ProfitGoal))
    {
        if (sign == Sign::SmallerThan && other->GetSign() == Sign::BiggerThan && profitGoal < other->GetProfitGoal())
        {
            error = STR_ERROR_OBJECTIVE_PROFIT_TOTAL_LOWER_THAN_SUB;
            return false;
        }
    }
    if ((goalID != GoalID::ProfitGoal) // same as above, but goals reversed
        && (other->GetGoalID() == GoalID::ProfitGoal))
    {
        if (other->GetSign() == Sign::SmallerThan && sign == Sign::BiggerThan && other->GetProfitGoal() < profitGoal)
        {
            error = STR_ERROR_OBJECTIVE_PROFIT_TOTAL_LOWER_THAN_SUB;
            return false;
        }
    }

    if (goalID == other->GetGoalID()) // Can't have two income goals of the same type have conflicts
    {
        if (sign == Sign::BiggerThan && other->GetSign() == Sign::SmallerThan && profitGoal > other->GetProfitGoal())
        {
            error = STR_ERROR_OBJECTIVE_VALUE_RANGE_CONFLICT;
            return false;
        }
        if (sign == Sign::SmallerThan && other->GetSign() == Sign::BiggerThan && profitGoal < other->GetProfitGoal())
        {
            error = STR_ERROR_OBJECTIVE_VALUE_RANGE_CONFLICT;
            return false;
        }
    }

    if ((goalID == GoalID::StallProfitGoal
         && (other->GetGoalID() == GoalID::FoodProfitGoal
             || other->GetGoalID()
                 == GoalID::MerchandiseProfitGoal)) // can't have merch or food income conflict with stall income
        || (other->GetGoalID() == GoalID::StallProfitGoal
            && (goalID == GoalID::FoodProfitGoal || goalID == GoalID::MerchandiseProfitGoal)))
    {
        if (sign == Sign::BiggerThan && other->GetSign() == Sign::SmallerThan && profitGoal > other->GetProfitGoal())
        {
            error = STR_ERROR_OBJECTIVE_PROFIT_STALL_LOWER_THAN_FOODMERCH;
            return false;
        }
        if (sign == Sign::SmallerThan && other->GetSign() == Sign::BiggerThan && profitGoal < other->GetProfitGoal())
        {
            error = STR_ERROR_OBJECTIVE_PROFIT_STALL_LOWER_THAN_FOODMERCH;
            return false;
        }
    }
    return true;
}

#pragma endregion

#pragma region RideTicketProfitGoal
bool ObjectiveRideTicketProfitGoal::CheckCondition()
{
    money32 lastMonthIncome = gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::ParkRideTickets)];
    trueOnLastCheck = ValueSignThresholdCondition(lastMonthIncome, profitGoal, sign);
    if (goalType == GoalType::Restriction)
        trueOnLastCheck = CheckGoalRestrictionWarningDays(
            !trueOnLastCheck, STR_PROFIT_RIDE_TICKET, profitGoal, STR_CURRENCY_FORMAT);
    return trueOnLastCheck;
}
std::string ObjectiveRideTicketProfitGoal::ToString()
{
    if (!redoToString)
        return longText;
    longText = ValueThresholdGoalToString(
        sign, goalType, profitGoal, STR_RIDE_TICKET_PROFIT_GOAL_DESCRIPTION, STR_RIDE_TICKET_PROFIT_RESTRICTION_DESCRIPTION,
        STR_LESS, true);
    redoToString = false;
    return longText;
}

bool ObjectiveRideTicketProfitGoal::CheckSpecialRequirements(rct_string_id& error) const
{
    bool okay = park_ride_prices_unlocked();
    if (!okay)
        error = STR_ERROR_OBJECTIVE_REQUIRES_RIDE_PRICES;
    okay &= ObjectiveGoal::CheckSpecialRequirements(error);
    return okay;
}
#pragma endregion

#pragma region ParkEntryProfitGoal
bool ObjectiveParkEntryProfitGoal::CheckCondition()
{
    money32 lastMonthIncome = gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::ParkEntranceTickets)];
    trueOnLastCheck = ValueSignThresholdCondition(lastMonthIncome, profitGoal, sign);
    if (goalType == GoalType::Restriction)
        trueOnLastCheck = CheckGoalRestrictionWarningDays(
            !trueOnLastCheck, STR_PROFIT_PARK_ENTRY, profitGoal, STR_CURRENCY_FORMAT);
    return trueOnLastCheck;
}

bool ObjectiveParkEntryProfitGoal::CheckSpecialRequirements(rct_string_id& error) const
{
    bool okay = park_entry_price_unlocked();
    if (!okay)
        error = STR_ERROR_OBJECTIVE_REQUIRES_PARK_ENTRY_PRICES;
    okay &= ObjectiveGoal::CheckSpecialRequirements(error);
    return okay;
}
std::string ObjectiveParkEntryProfitGoal::ToString()
{
    if (!redoToString)
        return longText;
    longText = ValueThresholdGoalToString(
        sign, goalType, profitGoal, STR_PARK_ENTRY_PROFIT_GOAL_DESCRIPTION, STR_PARK_ENTRY_PROFIT_RESTRICTION_DESCRIPTION,
        STR_LESS, true);
    redoToString = false;
    return longText;
}
#pragma endregion

#pragma region StallProfitGoal
bool ObjectiveStallProfitGoal::CheckCondition()
{
    money32 lastMonthIncome = gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::FoodDrinkSales)]
        + gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::FoodDrinkStock)]
        + gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::ShopSales)]
        + gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::ShopStock)];
    trueOnLastCheck = ValueSignThresholdCondition(lastMonthIncome, profitGoal, sign);
    if (goalType == GoalType::Restriction)
        trueOnLastCheck = CheckGoalRestrictionWarningDays(!trueOnLastCheck, STR_PROFIT_STALL, profitGoal, STR_CURRENCY_FORMAT);
    return trueOnLastCheck;
}

std::string ObjectiveStallProfitGoal::ToString()
{
    if (!redoToString)
        return longText;
    longText = ValueThresholdGoalToString(
        sign, goalType, profitGoal, STR_STALL_PROFIT_GOAL_DESCRIPTION, STR_STALL_PROFIT_RESTRICTION_DESCRIPTION, STR_LESS,
        true);
    redoToString = false;
    return longText;
}
#pragma endregion

#pragma region FoodProfitGoal
bool ObjectiveFoodProfitGoal::CheckCondition()
{
    money32 lastMonthIncome = gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::FoodDrinkSales)]
        + gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::FoodDrinkStock)];
    trueOnLastCheck = ValueSignThresholdCondition(lastMonthIncome, profitGoal, sign);
    if (goalType == GoalType::Restriction)
        trueOnLastCheck = CheckGoalRestrictionWarningDays(!trueOnLastCheck, STR_PROFIT_FOOD, profitGoal, STR_CURRENCY_FORMAT);
    return trueOnLastCheck;
}
std::string ObjectiveFoodProfitGoal::ToString()
{
    if (!redoToString)
        return longText;
    longText = ValueThresholdGoalToString(
        sign, goalType, profitGoal, STR_FOOD_PROFIT_GOAL_DESCRIPTION, STR_FOOD_PROFIT_RESTRICTION_DESCRIPTION, STR_LESS, true);
    redoToString = false;
    return longText;
}
#pragma endregion

#pragma region MerchandiseProfitGoal
bool ObjectiveMerchandiseProfitGoal::CheckCondition()
{
    money32 lastMonthIncome = gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::ShopSales)]
        + gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::ShopStock)];
    trueOnLastCheck = ValueSignThresholdCondition(lastMonthIncome, profitGoal, sign);
    if (goalType == GoalType::Restriction)
        trueOnLastCheck = CheckGoalRestrictionWarningDays(!trueOnLastCheck, STR_PROFIT_MERCH, profitGoal, STR_CURRENCY_FORMAT);
    return trueOnLastCheck;
}
std::string ObjectiveMerchandiseProfitGoal::ToString()
{
    if (!redoToString)
        return longText;
    longText = ValueThresholdGoalToString(
        sign, goalType, profitGoal, STR_MERCH_PROFIT_GOAL_DESCRIPTION, STR_MERCH_PROFIT_RESTRICTION_DESCRIPTION, STR_LESS,
        true);
    redoToString = false;
    return longText;
}
#pragma endregion

#pragma endregion

#pragma region RepayLoanGoal
bool ObjectiveRepayLoanGoal::CheckCondition()
{
    trueOnLastCheck = (gBankLoan <= 0);
    return trueOnLastCheck;
}

std::string ObjectiveRepayLoanGoal::ToString()
{
    if (!redoToString)
        return longText;
    utf8 buffer[512];
    format_string(buffer, 512, STR_REPAY_LOAN_GOAL_DESCRIPTION, {});
    longText = std::string(buffer);
    redoToString = false;
    return longText;
}
#pragma endregion

#pragma region NoDebtGoal
bool ObjectiveNoDebtGoal::CheckCondition()
{
    uint16_t* warningDays = &gScenarioObjectiveWarningDays[warningDaysIndex];
    if (gCash < 0)
    {
        countingDown = true;
        (*warningDays)++;
        if (*warningDays == warningWeeksPeriod * 7)
        {
            News::AddItemToQueue(News::ItemType::Graph, STR_PARK_HAS_BEEN_CLOSED_DOWN, 0, {});
            gParkFlags &= ~PARK_FLAGS_PARK_OPEN;
            gGuestInitialHappiness = 50;
            trueOnLastCheck = true;
        }
        else // warn EVERY day, this is imminent.
        {
            auto ft = Formatter();
            ft.Add<uint32_t>(7 - (*warningDays));
            if (gConfigNotifications.park_objective_warnings)
            {
                News::AddItemToQueue(News::ItemType::Graph, STR_DEBT_WARNING, 0, ft);
            }
        }
    }
    else if (gScenarioCompletedCompanyValue != COMPANY_VALUE_ON_FAILED_OBJECTIVE)
    {
        *warningDays = 0;
        countingDown = false;
    }

    return trueOnLastCheck;
}

std::string ObjectiveNoDebtGoal::ToString()
{
    if (!redoToString)
        return longText;
    utf8 buffer[512];
    format_string(buffer, 512, STR_NO_DEBT_GOAL_DESCRIPTION, {});
    longText = std::string(buffer);
    redoToString = false;
    return longText;
}
#pragma endregion

#pragma region NoDeathsGoal
bool ObjectiveNoDeathsGoal::CheckCondition()
{
    trueOnLastCheck = (gParkRatingCasualtyPenalty > 0);
    return trueOnLastCheck;
}
std::string ObjectiveNoDeathsGoal::ToString()
{
    if (!redoToString)
        return longText;
    utf8 buffer[512];
    format_string(buffer, 512, STR_NO_GUEST_DIE_GOAL_DESCRIPTION, {});
    longText = std::string(buffer);
    redoToString = false;
    return longText;
}
#pragma endregion

#pragma region NoDuplicateRidesGoal
bool ObjectiveNoDuplicateRidesGoal::CheckCondition()
{
    OpenRCT2::BitSet<MAX_RIDE_OBJECTS> type_already_counted;

    trueOnLastCheck = false;
    countingDown = false;
    for (const auto& ride : GetRideManager())
    {
        auto rideEntry = ride.GetRideEntry();
        if (rideEntry != nullptr)
        {
            if (ride.status != RideStatus::Open || ride.subtype == OBJECT_ENTRY_INDEX_NULL)
                continue;
            if (category == 7 || (category == 6 && !ride_entry_has_category(rideEntry, RIDE_CATEGORY_SHOP))
                || ride_entry_has_category(rideEntry, category))
            {
                if (type_already_counted[ride.subtype])
                {
                    trueOnLastCheck = true;
                    countingDown = true;
                    break;
                }
                type_already_counted[ride.subtype] = true;
            }
        }
    }
    return trueOnLastCheck;
}

bool ObjectiveNoDuplicateRidesGoal::CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const
{
    if (_otherGoal->GetGoalID() == goalID)
    {
        auto other = std::static_pointer_cast<ObjectiveNoDuplicateRidesGoal>(_otherGoal);
        if (other->GetCategory() == category)
        {
            error = STR_ERROR_OBJECTIVE_NO_DUPLICATE_RIDES_CATEGORY_SELECTED;
            return false;
        }
        if (other->GetCategory() == 6 || other->GetCategory() == 7 || category == 6 || category == 7)
        {
            error = STR_ERROR_OBJECTIVE_NO_DUPLICATE_RIDES_ALL_RIDES_SELECTED;
            return false;
        }
    }
    return true;
}

std::string ObjectiveNoDuplicateRidesGoal::ToString()
{
    if (!redoToString)
        return longText;
    utf8 buffer[512];
    auto ft = Formatter();
    switch (category)
    {
        case RIDE_CATEGORY_TRANSPORT:
            ft.Add<rct_string_id>(STR_RESEARCH_CATEGORY_TRANSPORT);
            format_string(buffer, 512, STR_NO_DUPLICATE_RIDES_DESCRIPTION_SUBTYPE, ft.Data());
            break;
        case RIDE_CATEGORY_GENTLE:
            ft.Add<rct_string_id>(STR_RESEARCH_CATEGORY_GENTLE);
            format_string(buffer, 512, STR_NO_DUPLICATE_RIDES_DESCRIPTION_SUBTYPE, ft.Data());
            break;
        case RIDE_CATEGORY_ROLLERCOASTER:
            ft.Add<rct_string_id>(STR_RESEARCH_CATEGORY_ROLLERCOASTER);
            format_string(buffer, 512, STR_NO_DUPLICATE_RIDES_DESCRIPTION_SUBTYPE, ft.Data());
            break;
        case RIDE_CATEGORY_THRILL:
            ft.Add<rct_string_id>(STR_RESEARCH_CATEGORY_THRILL);
            format_string(buffer, 512, STR_NO_DUPLICATE_RIDES_DESCRIPTION_SUBTYPE, ft.Data());
            break;
        case RIDE_CATEGORY_WATER:
            ft.Add<rct_string_id>(STR_RESEARCH_CATEGORY_WATER);
            format_string(buffer, 512, STR_NO_DUPLICATE_RIDES_DESCRIPTION_SUBTYPE, ft.Data());
            break;
        case RIDE_CATEGORY_SHOP:
            ft.Add<rct_string_id>(STR_RESEARCH_CATEGORY_SHOP);
            format_string(buffer, 512, STR_NO_DUPLICATE_RIDES_DESCRIPTION_SUBTYPE, ft.Data());
            break;
        case 6: // all but shops
            format_string(buffer, 512, STR_NO_DUPLICATE_RIDES_DESCRIPTION, {});
            break;
        case 7: // all
            format_string(buffer, 512, STR_NO_DUPLICATE_RIDES_DESCRIPTION_AND_STALLS, {});
            break;
    }
    longText = std::string(buffer);
    redoToString = false;
    return longText;
}
#pragma endregion

#pragma region NoExtremeRidesGoal
bool ObjectiveNoExtremeRidesGoal::CheckCondition()
{
    uint16_t* warningDays = &gScenarioObjectiveWarningDays[warningDaysIndex];

    bool tooIntense = false;
    for (const auto& ride : GetRideManager())
    {
        auto rideEntry = ride.GetRideEntry();
        if (rideEntry != nullptr)
        {
            if (ride.subtype == OBJECT_ENTRY_INDEX_NULL)
                continue;

            if (ride.intensity >= RIDE_RATING(10, 00))
            {
                tooIntense = true;
                break;
            }
        }
    }
    if (tooIntense)
    {
        countingDown = true;
        if (*warningDays % 7 == 0)
        {
            if (gConfigNotifications.park_objective_warnings)
            {
                News::AddItemToQueue(News::ItemType::Graph, STR_EXTREME_RIDES_WARNING, 0, {});
            }
        }
        return false;
    }
    else if (gScenarioCompletedCompanyValue != COMPANY_VALUE_ON_FAILED_OBJECTIVE)
    {
        (*warningDays) = 0;
        countingDown = false;
    }

    return true;
}

bool ObjectiveNoExtremeRidesGoal::CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const
{
    if (_otherGoal->GetGoalID() == goalID)
    {
        error = STR_ERROR_OBJECTIVE_CANNOT_ADD_TWICE;
        return false;
    }
    if (_otherGoal->GetGoalID() == GoalID::CoasterGoal
        && std::static_pointer_cast<ObjectiveCoasterGoal>(_otherGoal)->GetMinRideIntensityGoal() >= RIDE_RATING(10, 00))
    {
        error = STR_ERROR_OBJECTIVE_EXTREME_RIDES_FORBIDDEN_AND_NEEDED;
        return false;
    }
    if (_otherGoal->GetGoalID() == GoalID::SpecificTrackedRideGoal
        && std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(_otherGoal)->GetMinIntensityGoal() >= RIDE_RATING(10, 00))
    {
        error = STR_ERROR_OBJECTIVE_EXTREME_RIDES_FORBIDDEN_AND_NEEDED;
        return false;
    }
    return true;
}

std::string ObjectiveNoExtremeRidesGoal::ToString()
{
    if (!redoToString)
        return longText;
    utf8 buffer[512];
    format_string(buffer, 512, STR_NO_EXTREME_RIDES, {});
    longText = std::string(buffer);
    redoToString = false;
    return longText;
}
#pragma endregion

#pragma region CompleteResearchGoal
bool ObjectiveCompleteResearchGoal::CheckCondition()
{
    trueOnLastCheck = (gResearchItemsUninvented.size() == 0);
    if (!gScenarioObjective.PhasedGoals.empty()
        && gScenarioObjective.PhasedGoals[gScenarioObjective.PhasedGoalIndex].groupType != GoalGroupType::Dateless
        && gScenarioObjective.PhasedGoals[gScenarioObjective.PhasedGoalIndex].groupType != GoalGroupType::AfterDate
        && gScenarioObjective.PhasedGoals[gScenarioObjective.PhasedGoalIndex].groupType != GoalGroupType::AfterPeriod)
    {
        for (auto permGoal :
             gScenarioObjective.PermanentGoals.goals) // look through permananent goals to make sure this goal isn't in there
        {
            if (permGoal->GetGoalID()
                == goalID) // if it is in there, no need to worry about sending warnings about remaining time.
            {              // Only one CompleteResearch goal allowed in total, so checking ID should be fine.
                return trueOnLastCheck;
            }
        }
        uint16_t* warningDays = &gScenarioObjectiveWarningDays[warningDaysIndex];

        float timeRemaining = gScenarioObjective.PhasedGoals[gScenarioObjective.PhasedGoalIndex].GetDate() - gDateMonthsElapsed;
        float researchRemaining = gResearchItemsUninvented.size();

        researchRemaining *= static_cast<float>(_researchRate[3])
            / (static_cast<float>(_researchRate[gResearchFundingLevel] + 0.01));
        countingDown = false;
        if (timeRemaining < researchRemaining)
        {
            countingDown = true;
            if ((*warningDays) % 7 == 0)
            {
                News::AddItemToQueue(News::ItemType::Graph, STR_TOO_LOW_RESEARCH_LEVEL_WARNING, 0, {});
            }
            (*warningDays)++;
        }
    }
    return trueOnLastCheck;
}

bool ObjectiveCompleteResearchGoal::CheckSpecialRequirements(rct_string_id& error) const
{
    // TODO somehow get the time, then see if there's anough time to get all the research
    bool okay = gResearchItemsUninvented.size() > 0;
    if (!okay)
    {
        error = STR_ERROR_OBJECTIVE_NO_RESEARCH;
        return false;
    }
    return ObjectiveGoal::CheckSpecialRequirements(error);
}

std::string ObjectiveCompleteResearchGoal::ToString()
{
    if (!redoToString)
        return longText;
    utf8 buffer[512];
    format_string(buffer, 512, STR_COMPLETE_RESEARCH_GOAL_DESCRIPTION, {});
    longText = std::string(buffer);
    redoToString = false;
    return longText;
}
#pragma endregion

#pragma region ResearchRideGoal
bool ObjectiveResearchRide::CheckCondition()
{
    for (auto item : gResearchItemsInvented)
    {
        if (item.type == Research::EntryType::Ride && item.baseRideType == baseRideType && item.entryIndex == entryIndex)
        {
            trueOnLastCheck = true;
            return true;
        }
    }
    trueOnLastCheck = false;
    return false;
}

bool ObjectiveResearchRide::CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const
{
    if (_otherGoal->GetGoalID() == goalID)
    {
        if (std::static_pointer_cast<ObjectiveResearchRide>(_otherGoal)->GetBaseRideType() == baseRideType
            && std::static_pointer_cast<ObjectiveResearchRide>(_otherGoal)->GetEntryIndex() == entryIndex)
        {
            error = STR_ERROR_OBJECTIVE_DUPLICATE_RESEARCH_RIDE;
            return false;
        }
    }
    return true;
}

std::string ObjectiveResearchRide::ToString()
{
    if (!redoToString)
        return longText;
    utf8 buffer[512];
    auto ft = Formatter();
    ft.Add<rct_string_id>(STR_RESEARCH_A_RIDE_FULL);
    auto [drawString, ft2] = GetFullRideName(baseRideType, entryIndex, ft);
    format_string(buffer, 512, drawString, ft2.Data());
    longText = std::string(buffer);
    redoToString = false;
    return longText;
}
#pragma endregion

#pragma region BuildRideGoal
bool ObjectiveSpecificRideGoal::CheckCondition()
{
    if (buildBestGoal)
    {
        trueOnLastCheck = false;
        return trueOnLastCheck;
    }

    int count = 0;
    for (const auto& ride : GetRideManager())
    {
        auto rideEntry = ride.GetRideEntry();
        if (rideEntry != nullptr)
        {
            if (ride.status != RideStatus::Open || ride.subtype == OBJECT_ENTRY_INDEX_NULL)
                continue;
            if (ride.type == baseRideType
                && (ride.subtype == entryIndex
                    || !GetRideTypeDescriptor(baseRideType).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY)))
            {
                trueOnLastCheck = true;
                count++;
            }
        }
    }
    if ((mustBeOnlySuchRide && count > 1))
    {
        trueOnLastCheck = false;
    }
    return trueOnLastCheck;
}

bool ObjectiveSpecificRideGoal::CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const
{
    if (_otherGoal->GetGoalID() == goalID)
    {
        if (std::static_pointer_cast<ObjectiveSpecificRideGoal>(_otherGoal)->GetBaseRideType() == baseRideType
            && std::static_pointer_cast<ObjectiveSpecificRideGoal>(_otherGoal)->GetEntryIndex() == entryIndex)
        {
            error = STR_ERROR_OBJECTIVE_DUPLICATE_BUILD_RIDE;
            return false;
        }
    }
    return true;
}

std::string ObjectiveSpecificRideGoal::ToString()
{
    if (!redoToString)
        return longText;
    auto ft = Formatter();
    utf8 buffer[512];

    rct_string_id rideTypeString = GetShortRideName(baseRideType, entryIndex);

    ft.Add<rct_string_id>(rideTypeString);
    if (buildBestGoal)
        format_string(buffer, 512, STR_BUILD_THE_BEST_DESCRIPTION, ft.Data());
    else if (mustBeOnlySuchRide)
        format_string(buffer, 512, STR_BUILD_UNIQUE_RIDE_GOAL_DESCRIPTION, ft.Data());
    else
        format_string(buffer, 512, STR_BUILD_RIDE_GOAL_DESCRIPTION, ft.Data());
    longText = std::string(buffer);
    redoToString = false;
    return longText;
}
#pragma endregion

#pragma region BuildTrackedRideGoal
bool ObjectiveSpecificTrackedRideGoal::CheckCondition()
{
    int count = 0;
    for (const auto& ride : GetRideManager())
    {
        auto rideEntry = ride.GetRideEntry();
        if (rideEntry != nullptr)
        {
            if (ride.status != RideStatus::Open || ride.subtype == OBJECT_ENTRY_INDEX_NULL)
                continue;
            if (ride.type == baseRideType
                && (ride.subtype == entryIndex
                    || !GetRideTypeDescriptor(baseRideType).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY)))
            { // for debugging
                /* trueOnLastCheck = (((ride.max_speed * 9) >> 18) >= minSpeedGoal);
                trueOnLastCheck &= (maxSpeedGoal == 0 || ((ride.max_speed * 9) >> 18) <= maxSpeedGoal);
                trueOnLastCheck &= ((ride.drops & 0x3F) >= minDropCountGoal);
                trueOnLastCheck &= (maxDropCountGoal == 0 || (ride.drops & 0x3F) <= maxDropCountGoal);
                trueOnLastCheck &= (ride.highest_drop_height >= minDropHeightGoal);
                trueOnLastCheck &= (maxDropHeightGoal == 0 || ride.highest_drop_height <= maxDropHeightGoal);
                trueOnLastCheck &= ((ride.GetTotalLength() >> 16) >= minLengthGoal);
                trueOnLastCheck &= (maxLengthGoal == 0 || (ride.GetTotalLength() >> 16) <= maxLengthGoal);
                trueOnLastCheck &= (ride.excitement >= minExcitementGoal);
                trueOnLastCheck &= (maxExcitementGoal == 0 || ride.excitement <= maxExcitementGoal);
                trueOnLastCheck &= (ride.intensity >= minIntensityGoal);
                trueOnLastCheck &= (maxIntensityGoal == 0 || ride.intensity <= maxIntensityGoal);
                trueOnLastCheck &= (ride.nausea >= minNauseaGoal);
                trueOnLastCheck &= (maxNauseaGoal == 0 || ride.nausea <= maxNauseaGoal);
                trueOnLastCheck &= (ride.inversions >= minNumInversions);
                trueOnLastCheck &= (maxNumInversions == 0 || ride.inversions <= maxNumInversions);*/

                if ((((ride.max_speed * 9) >> 18) >= minSpeedGoal)
                    && (maxSpeedGoal == 0 || ((ride.max_speed * 9) >> 18) <= maxSpeedGoal)
                    && ((ride.drops & 0x3F) >= minDropCountGoal)
                    && (maxDropCountGoal == 0 || (ride.drops & 0x3F) <= maxDropCountGoal)
                    && (ride.highest_drop_height >= minDropHeightGoal)
                    && (maxDropHeightGoal == 0 || ride.highest_drop_height <= maxDropHeightGoal)
                    && ((ride.GetTotalLength() >> 16) >= minLengthGoal)
                    && (maxLengthGoal == 0 || (ride.GetTotalLength() >> 16) <= maxLengthGoal)
                    && (ride.excitement >= minExcitementGoal)
                    && (maxExcitementGoal == 0 || ride.excitement <= maxExcitementGoal) && (ride.intensity >= minIntensityGoal)
                    && (maxIntensityGoal == 0 || ride.intensity <= maxIntensityGoal) && (ride.nausea >= minNauseaGoal)
                    && (maxNauseaGoal == 0 || ride.nausea <= maxNauseaGoal) && (ride.inversions >= minNumInversions)
                    && (maxNumInversions == 0 || ride.inversions <= maxNumInversions))
                {
                    trueOnLastCheck = true;
                    for (auto piece : trackPiecesRequired)
                    {
                        trueOnLastCheck &= ride.HasElement(piece, true);
                    }
                }
                count++;
            }
        }
    }
    if (mustBeOnlySuchRide && count > 1)
    {
        trueOnLastCheck = false;
    }
    return trueOnLastCheck;
}

bool ObjectiveSpecificTrackedRideGoal::CheckSpecialRequirements(rct_string_id& error) const
{
    bool okay = (minDropCountGoal <= maxDropCountGoal || maxDropCountGoal == 0)
        && (minDropHeightGoal <= maxDropHeightGoal || maxDropHeightGoal == 0)
        && (minLengthGoal <= maxLengthGoal || maxLengthGoal == 0)
        && (minExcitementGoal <= maxExcitementGoal || maxExcitementGoal == 0)
        && (minIntensityGoal <= maxIntensityGoal || maxIntensityGoal == 0)
        && (minNauseaGoal <= maxNauseaGoal || maxNauseaGoal == 0) && (minSpeedGoal <= maxSpeedGoal || maxSpeedGoal == 0)
        && (minNumInversions <= maxNumInversions || maxNumInversions == 0);
    if (!okay)
        error = STR_ERROR_OBJECTIVE_MAX_STAT_BELOW_MIN;
    return okay;
}

bool ObjectiveSpecificTrackedRideGoal::CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const
{
    if (_otherGoal->GetGoalID() == goalID)
    {
        if (std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(_otherGoal)->GetBaseRideType() == baseRideType
            && std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(_otherGoal)->GetEntryIndex() == entryIndex)
        {
            if (mustBeOnlySuchRide)
            {
                error = STR_ERROR_OBJECTIVE_DUPLICATE_BUILD_TRACKED_RIDE_2;
                return false;
            }
            else if (std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(_otherGoal)->GetMustBeOnlySuchRide())
            {
                error = STR_ERROR_OBJECTIVE_DUPLICATE_BUILD_TRACKED_RIDE_1;
                return false;
            }
        }
    }
    if (_otherGoal->GetGoalID() == GoalID::NoExtremeRides && minIntensityGoal > RIDE_RATING(10, 00))
    {
        error = STR_ERROR_OBJECTIVE_EXTREME_RIDES_FORBIDDEN_AND_NEEDED;
        return false;
    }
    return true;
}

std::string ObjectiveSpecificTrackedRideGoal::ToString()
{
    if (!redoToString)
        return longText;
    bool first = true;
    auto ft = Formatter();
    utf8 buffer[512];

    rct_string_id rideTypeString = GetShortRideName(baseRideType, entryIndex);
    ft.Add<rct_string_id>(rideTypeString);
    if (mustBeOnlySuchRide)
        format_string(buffer, 512, STR_BUILD_UNIQUE_RIDE_GOAL_DESCRIPTION, ft.Data());
    else
        format_string(buffer, 512, STR_BUILD_RIDE_GOAL_DESCRIPTION, ft.Data());
    std::string s = std::string(buffer)
        + AddRideStat(minLengthGoal, maxLengthGoal, STR_A_LENGTH, &first, true, false, false, true);
    s += AddRideStat(minSpeedGoal, maxSpeedGoal, STR_A_MAX_SPEED, &first, false, false, true, true);
    s += AddRideStat(minNumInversions, maxNumInversions, STR_NUM_INVERSIONS, &first, false, false, false, true);
    s += AddRideStat(minDropCountGoal, maxDropCountGoal, STR_NUM_DROPS, &first, false, false, false, true);
    s += AddRideStat(minDropHeightGoal, maxDropHeightGoal, STR_A_DROP_HEIGHT, &first, true, false, false, true);
    s += AddRideStat(minExcitementGoal, maxExcitementGoal, STR_AN_EXCITEMENT_RATING, &first, false, true, false, true);
    s += AddRideStat(minIntensityGoal, maxIntensityGoal, STR_AN_INTENSITY_RATING, &first, false, true, false, true);
    s += AddRideStat(minNauseaGoal, maxNauseaGoal, STR_A_NAUSEA_RATING, &first, false, true, false, true);
    if (!trackPiecesRequired.empty())
    {
        format_string(buffer, 512, STR_CONTAINS_TRACK_ELEMENT, {});

        s += std::string(buffer);
        for (size_t i = 0; i < trackPiecesRequired.size(); ++i)
        {
            format_string(buffer, 512, RideConfigurationStringIds[trackPiecesRequired[i]], {});
            s += std::string(buffer);
            if (i < trackPiecesRequired.size() - 2 && trackPiecesRequired.size() > 1)
            {
                s += ", ";
            }
            if (i == trackPiecesRequired.size() - 2)
            {
                format_string(buffer, 512, STR_AND, {});
                s += std::string(buffer);
            }
        }
    }
    longText = s;
    redoToString = false;
    return longText;
}
#pragma endregion

#pragma region NumRideGoals

#pragma region TransportRidesGoal

bool ObjectiveTransportRidesGoal::CheckCondition()
{
    trueOnLastCheck = CheckRides(
        RIDE_CATEGORY_TRANSPORT, minNumRidesGoal, maxNumRidesGoal, minRideLengthGoal, maxRideLengthGoal, 0, 0, 0, 0,
        mustBeUniqueTypes, finishedExistingRides);
    return trueOnLastCheck;
}

bool ObjectiveTransportRidesGoal::CheckSpecialRequirements(rct_string_id& error) const
{
    bool okay = (minRideLengthGoal <= maxRideLengthGoal || maxRideLengthGoal == 0);
    if (!okay)
        error = STR_ERROR_OBJECTIVE_MAX_STAT_BELOW_MIN;
    return (okay && ObjectiveGentleRidesGoal::CheckSpecialRequirements(error));
}

std::string ObjectiveTransportRidesGoal::ToString()
{
    if (!redoToString)
        return longText;
    bool first = true;
    longText = NumRidesString(
                   minNumRidesGoal, maxNumRidesGoal, mustBeUniqueTypes, STR_TRANSPORT_RIDES_TIP,
                   STR_RESEARCH_CATEGORY_TRANSPORT, finishedExistingRides)
        + AddRideStat(
                   minRideLengthGoal, maxRideLengthGoal, STR_A_LENGTH, &first, true, false, false,
                   (minNumRidesGoal == maxNumRidesGoal && minNumRidesGoal == 1));
    return longText;
}

#pragma endregion

#pragma region GentleRidesGoal

bool ObjectiveGentleRidesGoal::CheckCondition()
{
    trueOnLastCheck = CheckRides(
        RIDE_CATEGORY_GENTLE, minNumRidesGoal, maxNumRidesGoal, 0, 0, 0, 0, 0, 0, mustBeUniqueTypes, finishedExistingRides);
    return trueOnLastCheck;
}

std::string ObjectiveGentleRidesGoal::ToString()
{
    if (!redoToString)
        return longText;
    longText = NumRidesString(
        minNumRidesGoal, maxNumRidesGoal, mustBeUniqueTypes, STR_GENTLE_RIDES_TIP, STR_RESEARCH_CATEGORY_GENTLE,
        finishedExistingRides);
    redoToString = false;
    return longText;
}

bool ObjectiveGentleRidesGoal::CheckSpecialRequirements(rct_string_id& error) const
{
    int count = 0;
    bool okay = true;
    int category = RIDE_CATEGORY_GENTLE;
    switch (goalID)
    {
        case GoalID::TransportRidesGoal:
            category = RIDE_CATEGORY_TRANSPORT;
            break;
        case GoalID::WaterRidesGoal:
            category = RIDE_CATEGORY_WATER;
            break;
        case GoalID::CoasterGoal:
            category = RIDE_CATEGORY_ROLLERCOASTER;
            break;
        default:
            break;
    }
    okay &= (minNumRidesGoal <= maxNumRidesGoal || maxNumRidesGoal == 0);
    if (!okay)
        error = STR_ERROR_OBJECTIVE_MAX_STAT_BELOW_MIN;
    if (!finishedExistingRides)
    {
        // are there enough types available in case of mustbeunique, or at least 1 otherwise
        for (auto item : gResearchItemsUninvented)
        {
            if (item.type == Research::EntryType::Ride && GetRideTypeDescriptor(item.baseRideType).Category == category)
                count++;
        }
        for (auto item : gResearchItemsInvented)
        {
            if (item.type == Research::EntryType::Ride && GetRideTypeDescriptor(item.baseRideType).Category == category)
                count++;
        }
        okay &= ((count > 1 && !mustBeUniqueTypes) || count > minNumRidesGoal);
        if (!okay)
            error = STR_ERROR_OBJECTIVE_INSUFFICIENT_RIDE_TYPES_AVAILABLE;
    }
    else
    {
        // are there enough (partially) built rides available?
        for (auto const& ride : GetRideManager())
        {
            const auto* rideEntry = ride.GetRideEntry();
            if (rideEntry != nullptr && ride_entry_has_category(rideEntry, category)
                && GetRideTypeDescriptor(ride.type).HasFlag(
                    RIDE_TYPE_FLAG_HAS_TRACK)                              // only tracked gentle rides can be finished
                && (ride.lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE)) // Up to the player to select which rides must be
                                                                           // destructable beforehand
            {
                count++;
            }
        }
        okay &= (count >= minNumRidesGoal);
        okay &= (count >= maxNumRidesGoal || maxNumRidesGoal == 0); // Pointless to ask to finish up to 10 rides, if there are
                                                                    // only 5. Even if the min was possible, makes no sense.
        if (!okay)
            error = STR_ERROR_OBJECTIVE_NOT_ENOUGH_PREBUILT_RIDES;
    }
    return okay && ObjectiveGoal::CheckSpecialRequirements(error);
}

bool ObjectiveGentleRidesGoal::CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const
{
    if (_otherGoal->GetGoalID() != goalID)
        return true;
    auto other = std::static_pointer_cast<ObjectiveGentleRidesGoal>(_otherGoal);

    if (finishedExistingRides == other->GetFinishedExistingRides())
    {
        error = STR_ERROR_OBJECTIVE_CANNOT_ADD_TWICE_EXCEPT_FINISHED;
        return false;
    }
    return true;
}

#pragma endregion

#pragma region CoasterGoal

bool ObjectiveCoasterGoal::CheckCondition()
{
    trueOnLastCheck = CheckRides(
        RIDE_CATEGORY_ROLLERCOASTER, minNumRidesGoal, maxNumRidesGoal, minRideLengthGoal, maxRideLengthGoal,
        minRideExcitementGoal, maxRideExcitementGoal, minRideIntensityGoal, maxRideIntensityGoal, minRideNauseaGoal,
        maxRideNauseaGoal, mustBeUniqueTypes, finishedExistingRides);
    return trueOnLastCheck;
}

bool ObjectiveCoasterGoal::CheckSpecialRequirements(rct_string_id& error) const
{
    bool okay = ((minRideIntensityGoal <= maxRideIntensityGoal) || maxRideIntensityGoal == 0)
        && ((minRideNauseaGoal <= maxRideNauseaGoal) || maxRideNauseaGoal == 0);
    if (!okay)
        error = STR_ERROR_OBJECTIVE_MAX_STAT_BELOW_MIN;
    return (okay && ObjectiveWaterRidesGoal::CheckSpecialRequirements(error));
}

bool ObjectiveCoasterGoal::CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const
{
    if (_otherGoal->GetGoalID() != goalID)
    {
        if (_otherGoal->GetGoalID() == GoalID::NoExtremeRides && minRideIntensityGoal >= RIDE_RATING(10, 00))
        {
            error = STR_ERROR_OBJECTIVE_EXTREME_RIDES_FORBIDDEN_AND_NEEDED;
            return false;
        }
        return true;
    }
    auto other = std::static_pointer_cast<ObjectiveCoasterGoal>(_otherGoal);

    if (finishedExistingRides == other->GetFinishedExistingRides())
    {
        error = STR_ERROR_OBJECTIVE_CANNOT_ADD_TWICE_EXCEPT_FINISHED;
        return false;
    }
    return true;
}

std::string ObjectiveCoasterGoal::ToString()
{
    if (!redoToString)
        return longText;
    bool first = true;
    std::string s = NumRidesString(
                        minNumRidesGoal, maxNumRidesGoal, mustBeUniqueTypes, STR_ROLLER_COASTERS_TIP,
                        STR_RESEARCH_CATEGORY_ROLLERCOASTER, finishedExistingRides)
        + AddRideStat(minRideLengthGoal, maxRideLengthGoal, STR_A_LENGTH, &first, true, false, false,
                      (minNumRidesGoal == maxNumRidesGoal && minNumRidesGoal == 1));

    s += AddRideStat(
        minRideExcitementGoal, maxRideExcitementGoal, STR_AN_EXCITEMENT_RATING, &first, false, true, false,
        (minNumRidesGoal == maxNumRidesGoal && minNumRidesGoal == 1));
    s += AddRideStat(
        minRideIntensityGoal, maxRideIntensityGoal, STR_AN_INTENSITY_RATING, &first, false, true, false,
        (minNumRidesGoal == maxNumRidesGoal && minNumRidesGoal == 1));
    s += AddRideStat(
        minRideNauseaGoal, maxRideNauseaGoal, STR_A_NAUSEA_RATING, &first, false, true, false,
        (minNumRidesGoal == maxNumRidesGoal && minNumRidesGoal == 1));

    longText = s;
    redoToString = false;
    return longText;
}
#pragma endregion

#pragma region ThrillRidesGoal

bool ObjectiveThrillRidesGoal::CheckCondition()
{
    trueOnLastCheck = CheckRides(RIDE_CATEGORY_THRILL, minNumRidesGoal, maxNumRidesGoal, 0, 0, 0, 0, 0, 0, mustBeUniqueTypes);
    return trueOnLastCheck;
}

std::string ObjectiveThrillRidesGoal::ToString()
{
    if (!redoToString)
        return longText;
    longText = NumRidesString(
        minNumRidesGoal, maxNumRidesGoal, mustBeUniqueTypes, STR_THRILL_RIDES_TIP, STR_RESEARCH_CATEGORY_THRILL, false);
    redoToString = false;
    return longText;
}

bool ObjectiveThrillRidesGoal::CheckSpecialRequirements(rct_string_id& error) const
{
    int thrillCount = 0;
    for (auto item : gResearchItemsUninvented)
    {
        if (item.type == Research::EntryType::Ride && GetRideTypeDescriptor(item.baseRideType).Category == RIDE_CATEGORY_THRILL)
            thrillCount++;
    }
    for (auto item : gResearchItemsInvented)
    {
        if (item.type == Research::EntryType::Ride && GetRideTypeDescriptor(item.baseRideType).Category == RIDE_CATEGORY_THRILL)
            thrillCount++;
    }
    bool okay = (thrillCount > 1 || (mustBeUniqueTypes && thrillCount > minNumRidesGoal));
    if (!okay)
        error = STR_ERROR_OBJECTIVE_INSUFFICIENT_RIDE_TYPES_AVAILABLE;
    okay &= (minNumRidesGoal <= maxNumRidesGoal || maxNumRidesGoal == 0);
    if (!okay)
        error = STR_ERROR_OBJECTIVE_MAX_STAT_BELOW_MIN;

    return okay && ObjectiveGoal::CheckSpecialRequirements(error);
}

#pragma endregion

#pragma region WaterRidesGoal

bool ObjectiveWaterRidesGoal::CheckCondition()
{
    trueOnLastCheck = CheckRides(
        RIDE_CATEGORY_WATER, minNumRidesGoal, maxNumRidesGoal, minRideLengthGoal, maxRideLengthGoal, minRideExcitementGoal,
        maxRideExcitementGoal, 0, 0, mustBeUniqueTypes, finishedExistingRides);
    return trueOnLastCheck;
}

bool ObjectiveWaterRidesGoal::CheckSpecialRequirements(rct_string_id& error) const
{
    bool okay = (minRideExcitementGoal <= maxRideExcitementGoal || maxRideExcitementGoal == 0);
    if (!okay)
        error = STR_ERROR_OBJECTIVE_MAX_STAT_BELOW_MIN;
    return (okay && ObjectiveTransportRidesGoal::CheckSpecialRequirements(error));
}

std::string ObjectiveWaterRidesGoal::ToString()
{
    if (!redoToString)
        return longText;
    bool first = true;
    std::string s = NumRidesString(
                        minNumRidesGoal, maxNumRidesGoal, mustBeUniqueTypes, STR_WATER_RIDES_TIP, STR_RESEARCH_CATEGORY_WATER,
                        finishedExistingRides)
        + AddRideStat(minRideLengthGoal, maxRideLengthGoal, STR_A_LENGTH, &first, true, false, false,
                      (minNumRidesGoal == maxNumRidesGoal && minNumRidesGoal == 1));
    s += AddRideStat(
        minRideExcitementGoal, maxRideExcitementGoal, STR_AN_EXCITEMENT_RATING, &first, false, true, false,
        (minNumRidesGoal == maxNumRidesGoal && minNumRidesGoal == 1));
    longText = s;
    redoToString = false;
    return longText;
}
#pragma endregion

#pragma endregion

#pragma region AwardGoal
bool ObjectiveAwardGoal::CheckCondition()
{
    auto const& awards = GetAwards();
    if (trueOnLastCheck && atAnyTime)
        trueOnLastCheck = true;
    else
        trueOnLastCheck = false;

    for (auto award : awards)
    {
        if (award.Type == awardsGoal)
        {
            trueOnLastCheck = true;
        }
    }
    return trueOnLastCheck;
}

bool ObjectiveAwardGoal::CheckSpecialRequirements(rct_string_id& error) const
{
    if ((awardsGoal == AwardType::BestValue || awardsGoal == AwardType::WorstValue)
        && (((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && (gParkFlags & PARK_FLAGS_NO_MONEY_SCENARIO))
            || (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && (gParkFlags & PARK_FLAGS_NO_MONEY))))
    {
        error = STR_ERROR_OBJECTIVE_REQUIRES_MONEY;
        return false;
    }
    return true; // Not gonna check for if there are coasters/waterrides/toilets etc
}

bool ObjectiveAwardGoal::CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const
{
    if (_otherGoal->GetGoalID() == GoalID::NoNegativeAwardsGoal)
    {
        if (!award_is_positive(awardsGoal))
        {
            error = STR_ERROR_OBJECTIVE_NO_NEGATIVE_AND_NEGATIVE_AWARD;
            return false;
        }
    }
    return true; // Allow needing to have both best and worst park value, it's possible to screw up the park quickly enough to
                 // gain both
}

std::string ObjectiveAwardGoal::ToString()
{
    if (!redoToString)
        return longText;
    auto ft = Formatter();
    ft.Add<rct_string_id>(static_cast<uint32_t>(awardsGoal) + STR_AWARD_MOST_UNTIDY_COLOURLESS);
    utf8 buffer[512];
    if (atAnyTime)
        format_string(buffer, 512, STR_AWARD_GOAL_DESCRIPTION, ft.Data());
    else
        format_string(buffer, 512, STR_AWARD_GOAL_DESCRIPTION_RECENTLY, ft.Data());
    longText = std::string(buffer);
    redoToString = false;
    return longText;
}
#pragma endregion

#pragma region NumPositiveAwardsGoal
bool ObjectiveNumPositiveAwardsGoal::CheckCondition()
{
    auto const& awards = GetAwards();
    for (auto award : awards)
    {
        if (award_is_positive(award.Type))
        {
            for (auto awardGotten : typesAchieved)
            {
                if (award.Type == awardGotten)
                    continue;
            }
            typesAchieved.push_back(award.Type);
        }
    }
    if (typesAchieved.size() == numAwards)
        trueOnLastCheck = true;

    return trueOnLastCheck;
}

std::string ObjectiveNumPositiveAwardsGoal::ToString()
{
    if (!redoToString)
        return longText;
    auto ft = Formatter();
    if (sign == Sign::BiggerThan)
        ft.Add<rct_string_id>(STR_AT_LEAST);
    else if (sign == Sign::SmallerThan)
        ft.Add<rct_string_id>(STR_AT_MOST);

    ft.Add<rct_string_id>(numAwards);
    utf8 buffer[512];
    format_string(buffer, 512, STR_AWARD_NUM_GOAL_DESCRIPTION, ft.Data());
    longText = std::string(buffer);
    redoToString = false;
    return longText;
}
#pragma endregion

#pragma region NoNegativeAwardsGoal
bool ObjectiveNoNegativeAwardsGoal::CheckCondition()
{
    auto const& awards = GetAwards();
    for (auto award : awards)
    {
        if (!award_is_positive(award.Type))
            trueOnLastCheck = true;
    }
    return trueOnLastCheck;
}

bool ObjectiveNoNegativeAwardsGoal::CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const
{
    if (_otherGoal->GetGoalID() != goalID)
    {
        if (_otherGoal->GetGoalID() == GoalID::AwardGoal)
        {
            if (!award_is_positive(std::static_pointer_cast<ObjectiveAwardGoal>(_otherGoal)->GetAwardsGoal()))
            {
                error = STR_ERROR_OBJECTIVE_NO_NEGATIVE_AND_NEGATIVE_AWARD;
                return false;
            }
        }
        return true;
    }
    error = STR_ERROR_OBJECTIVE_CANNOT_ADD_TWICE;
    return false;
}

std::string ObjectiveNoNegativeAwardsGoal::ToString()
{
    if (!redoToString)
        return longText;
    utf8 buffer[512];
    format_string(buffer, 512, STR_NO_NEGATIVE_AWARDS_GOAL_DESCRIPTION, {});
    longText = std::string(buffer);
    redoToString = false;
    return longText;
}
#pragma endregion

#pragma region FunGoal
std::string ObjectiveFunGoal::ToString()
{
    if (!redoToString)
        return longText;
    utf8 buffer[512];
    format_string(buffer, 512, STR_HAVE_FUN_DESCRIPTION, {});
    longText = std::string(buffer);
    redoToString = false;
    return longText;
}
#pragma endregion
