/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ScenarioObjective.h"

#include "../GameState.h"
#include "../config/Config.h"
#include "../core/UnitConversion.h"
#include "../object/ObjectLimits.h"
#include "../ride/RideManager.hpp"
#include "Scenario.h"

namespace OpenRCT2::Scenario
{
    ObjectiveStatus Objective::CheckGuestsBy() const
    {
        auto parkRating = getGameState().park.rating;
        int32_t currentMonthYear = GetDate().GetMonthsElapsed();

        if (currentMonthYear == MONTH_COUNT * Year || AllowEarlyCompletion())
        {
            if (parkRating >= 600 && getGameState().park.numGuestsInPark >= NumGuests)
            {
                return ObjectiveStatus::Success;
            }

            if (currentMonthYear == MONTH_COUNT * Year)
            {
                return ObjectiveStatus::Failure;
            }
        }

        return ObjectiveStatus::Undecided;
    }

    ObjectiveStatus Objective::CheckParkValueBy() const
    {
        int32_t currentMonthYear = GetDate().GetMonthsElapsed();
        money64 objectiveParkValue = Currency;
        money64 parkValue = getGameState().park.value;

        if (currentMonthYear == MONTH_COUNT * Year || AllowEarlyCompletion())
        {
            if (parkValue >= objectiveParkValue)
            {
                return ObjectiveStatus::Success;
            }

            if (currentMonthYear == MONTH_COUNT * Year)
            {
                return ObjectiveStatus::Failure;
            }
        }

        return ObjectiveStatus::Undecided;
    }

    /**
     * Checks if there are 10 rollercoasters of different subtype with
     * excitement >= 600 .
     * rct2:
     **/
    ObjectiveStatus Objective::Check10RollerCoasters() const
    {
        auto rcs = 0;
        BitSet<kMaxRideObjects> type_already_counted;
        for (const auto& ride : GetRideManager())
        {
            if (ride.status == RideStatus::open && ride.ratings.excitement >= RideRating::make(6, 00)
                && ride.subtype != kObjectEntryIndexNull)
            {
                auto rideEntry = ride.getRideEntry();
                if (rideEntry != nullptr)
                {
                    if (RideEntryHasCategory(*rideEntry, RideCategory::rollerCoaster) && !type_already_counted[ride.subtype])
                    {
                        type_already_counted[ride.subtype] = true;
                        rcs++;
                    }
                }
            }
        }
        if (rcs >= 10)
        {
            return ObjectiveStatus::Success;
        }

        return ObjectiveStatus::Undecided;
    }

    /**
     *
     *  rct2: 0x0066A13C
     */
    ObjectiveStatus Objective::CheckGuestsAndRating() const
    {
        auto& gameState = getGameState();
        if (gameState.park.rating < 700 && GetDate().GetMonthsElapsed() >= 1)
        {
            gameState.scenarioParkRatingWarningDays++;
            if (gameState.scenarioParkRatingWarningDays == 1)
            {
                if (Config::Get().notifications.ParkRatingWarnings)
                {
                    News::AddItemToQueue(News::ItemType::graph, STR_PARK_RATING_WARNING_4_WEEKS_REMAINING, 0, {});
                }
            }
            else if (gameState.scenarioParkRatingWarningDays == 8)
            {
                if (Config::Get().notifications.ParkRatingWarnings)
                {
                    News::AddItemToQueue(News::ItemType::graph, STR_PARK_RATING_WARNING_3_WEEKS_REMAINING, 0, {});
                }
            }
            else if (gameState.scenarioParkRatingWarningDays == 15)
            {
                if (Config::Get().notifications.ParkRatingWarnings)
                {
                    News::AddItemToQueue(News::ItemType::graph, STR_PARK_RATING_WARNING_2_WEEKS_REMAINING, 0, {});
                }
            }
            else if (gameState.scenarioParkRatingWarningDays == 22)
            {
                if (Config::Get().notifications.ParkRatingWarnings)
                {
                    News::AddItemToQueue(News::ItemType::graph, STR_PARK_RATING_WARNING_1_WEEK_REMAINING, 0, {});
                }
            }
            else if (gameState.scenarioParkRatingWarningDays == 29)
            {
                News::AddItemToQueue(News::ItemType::graph, STR_PARK_HAS_BEEN_CLOSED_DOWN, 0, {});
                gameState.park.flags &= ~PARK_FLAGS_PARK_OPEN;
                gameState.scenarioOptions.guestInitialHappiness = 50;
                return ObjectiveStatus::Failure;
            }
        }
        else if (gameState.scenarioCompletedCompanyValue != kCompanyValueOnFailedObjective)
        {
            gameState.scenarioParkRatingWarningDays = 0;
        }

        if (gameState.park.rating >= 700)
            if (gameState.park.numGuestsInPark >= NumGuests)
                return ObjectiveStatus::Success;

        return ObjectiveStatus::Undecided;
    }

    ObjectiveStatus Objective::CheckMonthlyRideIncome() const
    {
        money64 lastMonthRideIncome = getGameState().park.expenditureTable[1][EnumValue(ExpenditureType::parkRideTickets)];
        if (lastMonthRideIncome >= Currency)
        {
            return ObjectiveStatus::Success;
        }

        return ObjectiveStatus::Undecided;
    }

    /**
     * Checks if there are 10 rollercoasters of different subtype with
     * excitement > 700 and a minimum length;
     *  rct2: 0x0066A6B5
     */
    ObjectiveStatus Objective::Check10RollerCoastersLength() const
    {
        BitSet<kMaxRideObjects> type_already_counted;
        auto rcs = 0;
        for (const auto& ride : GetRideManager())
        {
            if (ride.status == RideStatus::open && ride.ratings.excitement >= RideRating::make(7, 00)
                && ride.subtype != kObjectEntryIndexNull)
            {
                auto rideEntry = ride.getRideEntry();
                if (rideEntry != nullptr)
                {
                    if (RideEntryHasCategory(*rideEntry, RideCategory::rollerCoaster) && !type_already_counted[ride.subtype])
                    {
                        if (ToHumanReadableRideLength(ride.getTotalLength()) >= MinimumLength)
                        {
                            type_already_counted[ride.subtype] = true;
                            rcs++;
                        }
                    }
                }
            }
        }
        if (rcs >= 10)
        {
            return ObjectiveStatus::Success;
        }

        return ObjectiveStatus::Undecided;
    }

    ObjectiveStatus Objective::CheckFinish5RollerCoasters() const
    {
        // Originally, this did not check for null rides, neither did it check if
        // the rides are even rollercoasters, never mind the right rollercoasters to be finished.
        auto rcs = 0;
        for (const auto& ride : GetRideManager())
        {
            if (ride.status != RideStatus::closed && ride.ratings.excitement >= MinimumExcitement)
            {
                auto rideEntry = ride.getRideEntry();
                if (rideEntry != nullptr)
                {
                    if ((ride.lifecycleFlags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)
                        && RideEntryHasCategory(*rideEntry, RideCategory::rollerCoaster))
                    {
                        rcs++;
                    }
                }
            }
        }
        if (rcs >= 5)
        {
            return ObjectiveStatus::Success;
        }

        return ObjectiveStatus::Undecided;
    }

    ObjectiveStatus Objective::CheckRepayLoanAndParkValue() const
    {
        const auto& gameState = getGameState();
        money64 parkValue = gameState.park.value;
        money64 currentLoan = gameState.park.bankLoan;

        if (currentLoan <= 0 && parkValue >= Currency)
        {
            return ObjectiveStatus::Success;
        }

        return ObjectiveStatus::Undecided;
    }

    ObjectiveStatus Objective::CheckMonthlyFoodIncome() const
    {
        const auto* lastMonthExpenditure = getGameState().park.expenditureTable[1];
        auto lastMonthProfit = lastMonthExpenditure[EnumValue(ExpenditureType::shopSales)]
            + lastMonthExpenditure[EnumValue(ExpenditureType::shopStock)]
            + lastMonthExpenditure[EnumValue(ExpenditureType::foodDrinkSales)]
            + lastMonthExpenditure[EnumValue(ExpenditureType::foodDrinkStock)];

        if (lastMonthProfit >= Currency)
        {
            return ObjectiveStatus::Success;
        }

        return ObjectiveStatus::Undecided;
    }

    /**
     * Checks the win/lose conditions of the current objective.
     *  rct2: 0x0066A4B2
     */
    ObjectiveStatus Objective::Check(GameState_t& gameState) const
    {
        if (gameState.scenarioCompletedCompanyValue != kMoney64Undefined)
        {
            return ObjectiveStatus::Undecided;
        }

        switch (Type)
        {
            case ObjectiveType::guestsBy:
                return CheckGuestsBy();
            case ObjectiveType::parkValueBy:
                return CheckParkValueBy();
            case ObjectiveType::tenRollercoasters:
                return Check10RollerCoasters();
            case ObjectiveType::guestsAndRating:
                return CheckGuestsAndRating();
            case ObjectiveType::monthlyRideIncome:
                return CheckMonthlyRideIncome();
            case ObjectiveType::tenRollercoastersLength:
                return Check10RollerCoastersLength();
            case ObjectiveType::finishFiveRollercoasters:
                return CheckFinish5RollerCoasters();
            case ObjectiveType::repayLoanAndParkValue:
                return CheckRepayLoanAndParkValue();
            case ObjectiveType::monthlyFoodIncome:
                return CheckMonthlyFoodIncome();
            default:
                return ObjectiveStatus::Undecided;
        }
    }

    bool ObjectiveNeedsMoney(const ObjectiveType objective)
    {
        switch (objective)
        {
            case ObjectiveType::parkValueBy:
            case ObjectiveType::monthlyRideIncome:
            case ObjectiveType::repayLoanAndParkValue:
            case ObjectiveType::monthlyFoodIncome:
                return true;
            default:
                return false;
        }
    }
} // namespace OpenRCT2::Scenario
