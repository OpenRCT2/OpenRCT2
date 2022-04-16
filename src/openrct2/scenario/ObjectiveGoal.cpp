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

bool RideStatRequirement::Check(const Ride& ride)
{
    uint32_t value;
    switch (requirement)
    {
        case RideRequirement::AirTime:
            value = (ride.total_air_time * 3);
            break;
        case RideRequirement::AverageSpeed:
            value = (ride.average_speed * 9) >> 18;
            break;
        case RideRequirement::HighestSpeed:
            value = (ride.max_speed * 9) >> 18;
            break;
        case RideRequirement::CustomersOnRide:
            value = ride.cur_num_customers;
            break;
        case RideRequirement::CustomersPerHour:
            value = ride_customers_per_hour(&ride);
            break;
        case RideRequirement::QueueTime:
            value = ride.GetMaxQueueTime();
            break;
        case RideRequirement::ExcitementRating:
            value = ride.excitement;
            break;
        case RideRequirement::IntensityRating:
            value = ride.intensity;
            break;
        case RideRequirement::NauseaRating:
            value = ride.nausea;
            break;
        case RideRequirement::HighestDrop:
            value = (ride.highest_drop_height * 3) / 4;
            break;
        case RideRequirement::NumberOfDrops:
            value = ride.drops;
            break;
        case RideRequirement::NumberOfInversions:
            value = ride.inversions;
            break;
        case RideRequirement::Holes:
            value = ride.holes;
            break;
        case RideRequirement::Length:
            value = ride.GetTotalLength() >> 16;
            break;
        case RideRequirement::RideTime:
            value = ride.GetTotalTime();
            break;
        case RideRequirement::MaxLatGs:
            value = ride.max_lateral_g;
            break;
        case RideRequirement::MaxNegativeVgs:
            value = ride.max_negative_vertical_g;
            break;
        case RideRequirement::MaxPositiveVGs:
            value = ride.max_positive_vertical_g;
            break;
        default:
            value = 0;
            break;
    }

    return (value >= minValue && (value <= maxValue || maxValue == 0));
}

#pragma region ObjectiveGoal
ObjectiveGoal::ObjectiveGoal(GoalID _id, GoalType _type, bool _usesMoney, uint16_t _warningDayIndex, uint16_t _leeWayPeriod)
    : goalID(_id)
    , goalType(_type)
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

#pragma region ValueGoal
bool ObjectiveValueGoal::CheckGoalRestrictionWarningDays(
    bool _tooLow, bool _tooHigh, rct_string_id _goalAsWord, rct_string_id _valueStringFormat)
{
    if (gDateMonthsElapsed < 1)
    {
        return false;
    }
    uint16_t* warningDays = &gScenarioObjectiveWarningDays[warningDaysIndex];

    auto ft = Formatter();
    ft.Add<rct_string_id>(_goalAsWord);
    if (_tooLow || _tooHigh)
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
            if (gConfigNotifications.park_rating_warnings)
            {
                if (_tooLow)
                {
                    ft.Add<rct_string_id>(STR_DROPPED_BELOW);
                    ft.Add<rct_string_id>(_valueStringFormat);
                    if (_valueStringFormat == STR_CURRENCY_FORMAT)
                        ft.Add<money64>(minValue);
                    else
                        ft.Add<uint32_t>(minValue);
                    ft.Add<rct_string_id>(STR_RAISED);
                    ft.Add<rct_string_id>(_goalAsWord);
                    ft.Add<uint16_t>(warningWeeksPeriod);
                }
                else if (_tooHigh)
                {
                    ft.Add<rct_string_id>(STR_RISEN_ABOVE);
                    ft.Add<rct_string_id>(_valueStringFormat);
                    if (_valueStringFormat == STR_CURRENCY_FORMAT)
                        ft.Add<money64>(maxValue);
                    else
                        ft.Add<uint32_t>(maxValue);
                    ft.Add<rct_string_id>(STR_LOWERED);
                    ft.Add<rct_string_id>(_goalAsWord);
                    ft.Add<uint16_t>(warningWeeksPeriod);
                }
                News::AddItemToQueue(News::ItemType::Graph, STR_GENERIC_VALUE_TOO_SIGN_FIRST_WARNING, 0, ft);
            }
        }
        else if (*warningDays == (warningWeeksPeriod - 2) * 7)
        {
            if (gConfigNotifications.park_rating_warnings)
            {
                if (_tooLow)
                {
                    ft.Add<rct_string_id>(STR_BELOW);
                    ft.Add<rct_string_id>(_valueStringFormat);
                    if (_valueStringFormat == STR_CURRENCY_FORMAT)
                        ft.Add<money64>(minValue);
                    else
                        ft.Add<uint32_t>(minValue);
                    ft.Add<rct_string_id>(STR_RAISE);
                    ft.Add<rct_string_id>(_goalAsWord);
                }
                else if (_tooHigh)
                {
                    ft.Add<rct_string_id>(STR_ABOVE);
                    ft.Add<rct_string_id>(_valueStringFormat);
                    if (_valueStringFormat == STR_CURRENCY_FORMAT)
                        ft.Add<money64>(maxValue);
                    else
                        ft.Add<uint32_t>(maxValue);
                    ft.Add<rct_string_id>(STR_LOWER);
                    ft.Add<rct_string_id>(_goalAsWord);
                }
                News::AddItemToQueue(News::ItemType::Graph, STR_GENERIC_VALUE_TOO_SIGN_SEMILAST_WARNING, 0, ft);
            }
        }
        else if (*warningDays == (warningWeeksPeriod - 1) * 7)
        {
            if (gConfigNotifications.park_rating_warnings)
            {
                if (_tooLow)
                {
                    ft.Add<rct_string_id>(STR_BELOW);
                    ft.Add<rct_string_id>(_valueStringFormat);
                    if (_valueStringFormat == STR_CURRENCY_FORMAT)
                        ft.Add<money64>(minValue);
                    else
                        ft.Add<uint32_t>(minValue);
                    ft.Add<rct_string_id>(STR_RAISE);
                    ft.Add<rct_string_id>(_goalAsWord);
                }
                else if (_tooHigh)
                {
                    ft.Add<rct_string_id>(STR_ABOVE);
                    ft.Add<rct_string_id>(_valueStringFormat);
                    if (_valueStringFormat == STR_CURRENCY_FORMAT)
                        ft.Add<money64>(maxValue);
                    else
                        ft.Add<uint32_t>(maxValue);
                    ft.Add<rct_string_id>(STR_LOWER);
                    ft.Add<rct_string_id>(_goalAsWord);
                }
                News::AddItemToQueue(News::ItemType::Graph, STR_GENERIC_VALUE_TOO_SIGN_LAST_WARNING, 0, ft);
            }
        }
        else if (*warningDays % 7 == 0)
        {
            if (gConfigNotifications.park_rating_warnings)
            {
                if (_tooLow)
                {
                    ft.Add<rct_string_id>(STR_BELOW);
                    ft.Add<rct_string_id>(_valueStringFormat);
                    if (_valueStringFormat == STR_CURRENCY_FORMAT)
                        ft.Add<money64>(minValue);
                    else
                        ft.Add<uint32_t>(minValue);
                    ft.Add<uint32_t>(warningWeeksPeriod - (*warningDays / 7));
                    ft.Add<rct_string_id>(STR_RAISE);
                    ft.Add<rct_string_id>(_goalAsWord);
                }
                else if (_tooHigh)
                {
                    ft.Add<rct_string_id>(STR_ABOVE);
                    ft.Add<rct_string_id>(_valueStringFormat);
                    if (_valueStringFormat == STR_CURRENCY_FORMAT)
                        ft.Add<money64>(maxValue);
                    else
                        ft.Add<uint32_t>(maxValue);
                    ft.Add<uint32_t>(warningWeeksPeriod - (*warningDays / 7));
                    ft.Add<rct_string_id>(STR_LOWER);
                    ft.Add<rct_string_id>(_goalAsWord);
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

bool ObjectiveValueGoal::CheckValues(uint32_t _value, rct_string_id _goalAsWord, rct_string_id _valueStringFormat)
{
    trueOnLastCheck = _value >= minValue && (_value <= maxValue || maxValue == 0);
    if (goalType == GoalType::Restriction)
        trueOnLastCheck = CheckGoalRestrictionWarningDays(
            _value < minValue, (_value > maxValue && maxValue != 0), _goalAsWord, _valueStringFormat);
    return trueOnLastCheck;
}

bool ObjectiveValueGoal::CheckSpecialRequirements(rct_string_id& error) const
{
    if (minValue > maxValue && maxValue != 0)
    {
        error = STR_ERROR_OBJECTIVE_MAX_STAT_BELOW_MIN;
        return false;
    }
    return true;
}

#pragma endregion

#pragma region GuestNumGoal
bool ObjectiveGuestNumGoal::CheckCondition()
{
    return CheckValues(gNumGuestsInPark, STR_GUEST_NUMBER);
}
#pragma endregion

#pragma region ParkValueGoal
bool ObjectiveParkValueGoal::CheckCondition()
{
    return CheckValues(gParkValue, STR_PARK_VALUE, STR_CURRENCY_FORMAT);
}
#pragma endregion

#pragma region ParkRatingGoal
bool ObjectiveParkRatingGoal::CheckCondition()
{
    return CheckValues(gParkRating, STR_PARK_RATING);
}
#pragma endregion

#pragma region ProfitGoal
bool ObjectiveProfitGoal::CheckCondition()
{
    money32 lastMonthIncome = 0;

    if (profitTypeFlags & static_cast<uint8_t>(ProfitTypeFlags::ParkEntry))
    {
        lastMonthIncome += gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::ParkEntranceTickets)];
    }
    if (profitTypeFlags & static_cast<uint8_t>(ProfitTypeFlags::RideTickets))
    {
        lastMonthIncome += gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::ParkRideTickets)];
    }
    if (profitTypeFlags & static_cast<uint8_t>(ProfitTypeFlags::Food))
    {
        lastMonthIncome += gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::FoodDrinkSales)]
            + gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::FoodDrinkStock)];
    }
    if (profitTypeFlags & static_cast<uint8_t>(ProfitTypeFlags::Merch))
    {
        lastMonthIncome += gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::ShopSales)]
            + gExpenditureTable[1][static_cast<int32_t>(ExpenditureType::ShopStock)];
    }

    return CheckValues(lastMonthIncome, STR_PROFIT /*TODO this needs to be changed to reflect the flags*/, STR_CURRENCY_FORMAT);
}

bool ObjectiveProfitGoal::CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const
{
    // This will not pick on up on two goals combining a lower limit to overcome totalincome's upper limit or similar
    // situations.
    if (_otherGoal->GetGoalID() == goalID)
        return true;

    auto other = std::static_pointer_cast<ObjectiveProfitGoal>(_otherGoal);

    if (other->GetProfitTypeFlags() == profitTypeFlags)
    {
        error = STR_ERROR_OBJECTIVE_CANNOT_ADD_TWICE;
        return false;
    }

    uint8_t mutalFlags = other->GetProfitTypeFlags() & profitTypeFlags;

    if (mutalFlags == 0) // The other goal and this one don't share any of the types of profit they check for
        return true;

    if (mutalFlags == profitTypeFlags) // this profitgoal's flags are entirely contained in the other
    {
        if (other->GetMaxValue() > minValue)
        {
            error = STR_ERROR_OBJECTIVE_PROFIT_TOTAL_LOWER_THAN_SUB;
            return false;
        }
    }
    else if (mutalFlags == other->GetProfitTypeFlags()) // the other's flags are entirely contained in this
    {
        if (maxValue < other->GetMinValue())
        {
            error = STR_ERROR_OBJECTIVE_PROFIT_TOTAL_LOWER_THAN_SUB;
            return false;
        }
    }
    // If program gets here there's 2 goals with some overlap, but not enough to definitely say it's impossible. However, it
    // might be impossible.
    return true;
}

#pragma endregion

#pragma region RepayLoanGoal
bool ObjectiveRepayLoanGoal::CheckCondition()
{
    trueOnLastCheck = (gBankLoan <= 0);
    return trueOnLastCheck;
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
#pragma endregion

#pragma region RidesGoal
bool ObjectiveRidesGoal::CheckCondition()
{
    OpenRCT2::BitSet<MAX_RIDE_OBJECTS> type_already_counted;
    auto count = 0;

    for (const auto& ride : GetRideManager())
    {
        auto rideEntry = ride.GetRideEntry();
        if (rideEntry == nullptr)
            continue;
        if (ride.status != RideStatus::Open || ride.subtype == OBJECT_ENTRY_INDEX_NULL)
            continue;
        if (!ride_entry_has_category(rideEntry, rideCategory))
            continue;

        if (!type_already_counted[ride.subtype]
            && (!finishedExistingRides || (ride.lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)))

        {
            bool okay = true;
            for (auto req : requirements)
            {
                okay &= req.Check(ride);
            }
            if (!okay)
                continue;

            if (mustBeUniqueTypes)
                type_already_counted[ride.subtype] = true;
            count++;
        }
    }
    if (count < minNumber || (maxNumber != 0 && count > maxNumber))
        return false;
    return true;
}

bool ObjectiveRidesGoal::CheckSpecialRequirements(rct_string_id& error) const
{
    int count = 0;
    bool okay = true;
    okay &= (minNumber <= maxNumber || maxNumber == 0);
    if (!okay)
        error = STR_ERROR_OBJECTIVE_MAX_STAT_BELOW_MIN;
    if (!finishedExistingRides)
    {
        // are there enough types available in case of mustbeunique, or at least 1 otherwise
        for (auto item : gResearchItemsUninvented)
        {
            if (item.type == Research::EntryType::Ride && GetRideTypeDescriptor(item.baseRideType).Category == rideCategory)
                count++;
        }
        for (auto item : gResearchItemsInvented)
        {
            if (item.type == Research::EntryType::Ride && GetRideTypeDescriptor(item.baseRideType).Category == rideCategory)
                count++;
        }
        okay &= ((count > 1 && !mustBeUniqueTypes) || count > maxNumber);
        if (!okay)
            error = STR_ERROR_OBJECTIVE_INSUFFICIENT_RIDE_TYPES_AVAILABLE;
    }
    else
    {
        // are there enough (partially) built rides available?
        for (auto const& ride : GetRideManager())
        {
            const auto* rideEntry = ride.GetRideEntry();
            if (rideEntry != nullptr && ride_entry_has_category(rideEntry, rideCategory)
                && GetRideTypeDescriptor(ride.type).HasFlag(
                    RIDE_TYPE_FLAG_HAS_TRACK)                              // only tracked gentle rides can be finished
                && (ride.lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE)) // Up to the player to select which rides must be
                                                                           // destructable beforehand
            {
                count++;
            }
        }
        okay &= (count >= minNumber);
        okay &= (count >= maxNumber || maxNumber == 0); // Pointless to ask to finish up to 10 rides, if there are
                                                        // only 5. Even if the min was possible, makes no sense.
        if (!okay)
            error = STR_ERROR_OBJECTIVE_NOT_ENOUGH_PREBUILT_RIDES;
    }
    return okay && ObjectiveGoal::CheckSpecialRequirements(error);
}

bool ObjectiveRidesGoal::CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const
{
    if (_otherGoal->GetGoalID() != goalID)
    {
        return true;
    }
    auto other = std::static_pointer_cast<ObjectiveRidesGoal>(_otherGoal);
    if (other->GetRideCategory() != rideCategory)
    {
        return true;
    }
    if (finishedExistingRides == other->GetFinishedExistingRides())
    {
        error = STR_ERROR_OBJECTIVE_CANNOT_ADD_TWICE_EXCEPT_FINISHED;
        return false;
    }
    return true;
}
#pragma endregion

#pragma region RideStatRequirementsList
bool RideRequirementsList::AddRequirement(RideStatRequirement _req, rct_string_id& error)
{
    if (_req.maxValue >= _req.minValue || _req.maxValue == 0)
    {
        for (auto req : requirements)
        {
            if (req.requirement == _req.requirement)
            {
                // error = TODO
                return false;
            }
        }

        requirements.emplace_back(_req);
        SetRedoToString();
        return true;
    }
    else
    {
        error = STR_ERROR_OBJECTIVE_MAX_STAT_BELOW_MIN;
        return false;
    }
}

bool RideRequirementsList::RemoveRequirement(uint8_t _position)
{
    if (requirements.size() <= _position)
        return false;
    requirements.erase(requirements.begin() + _position);
    SetRedoToString();
    return true;
}

bool RideRequirementsList::ReplaceRequirement(RideStatRequirement _req, uint8_t _position, rct_string_id& error)
{
    if (_req.maxValue >= _req.minValue || _req.maxValue == 0)
    {
        for (int i = 0; i < requirements.size(); ++i)
        {
            if (requirements[i].requirement == _req.requirement && i != _position)
                return false;
        }

        requirements[_position] = _req;
        SetRedoToString();
        return true;
    }
    else
    {
        error = STR_ERROR_OBJECTIVE_MAX_STAT_BELOW_MIN;
        return false;
    }
}
#pragma endregion
