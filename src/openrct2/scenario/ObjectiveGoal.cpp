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
bool CheckRides(
    uint8_t rideType, uint16_t minNum, uint16_t maxNum, uint16_t minLen = 0, uint16_t maxLen = 0, uint16_t minEx = 0,
    uint16_t maxEx = 0, uint16_t minIn = 0, uint16_t maxIn = 0, uint16_t minNau = 0, uint16_t maxNau = 0, bool unique = false,
    bool mustBeFinished = false);
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
            if (gConfigNotifications.park_rating_warnings)
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
            if (gConfigNotifications.park_rating_warnings)
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
            if (gConfigNotifications.park_rating_warnings)
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
            if (gConfigNotifications.park_rating_warnings)
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

#pragma region GuestNumGoal
bool ObjectiveGuestNumGoal::CheckCondition()
{
    trueOnLastCheck = ValueSignThresholdCondition(gNumGuestsInPark, guestNumGoal, sign);
    if (goalType == GoalType::Restriction)
        trueOnLastCheck = CheckGoalRestrictionWarningDays(!trueOnLastCheck, STR_GUEST_NUMBER, guestNumGoal);
    return trueOnLastCheck;
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

bool ObjectiveProfitGoal::CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const
{
    // This will not pick on up on two goals combining a lower limit to overcome totalincome's upper limit or similar
    // situations.
    switch (_otherGoal->GetGoalID())
    {
        case GoalID::ProfitGoal:
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

bool ObjectiveRideTicketProfitGoal::CheckSpecialRequirements(rct_string_id& error) const
{
    bool okay = park_ride_prices_unlocked();
    if (!okay)
        error = STR_ERROR_OBJECTIVE_REQUIRES_RIDE_PRICES;
    okay &= ObjectiveGoal::CheckSpecialRequirements(error);
    return okay;
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

#pragma endregion

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

#pragma endregion

#pragma region GentleRidesGoal

bool ObjectiveGentleRidesGoal::CheckCondition()
{
    trueOnLastCheck = CheckRides(
        RIDE_CATEGORY_GENTLE, minNumRidesGoal, maxNumRidesGoal, 0, 0, 0, 0, 0, 0, mustBeUniqueTypes, finishedExistingRides);
    return trueOnLastCheck;
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

#pragma endregion

#pragma region ThrillRidesGoal

bool ObjectiveThrillRidesGoal::CheckCondition()
{
    trueOnLastCheck = CheckRides(RIDE_CATEGORY_THRILL, minNumRidesGoal, maxNumRidesGoal, 0, 0, 0, 0, 0, 0, mustBeUniqueTypes);
    return trueOnLastCheck;
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

#pragma endregion

#pragma endregion
