/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include "../localisation/Formatter.h"
#include "../object/ObjectLimits.h"
#include "../ride/RideManager.hpp"
#include "Scenario.h"

namespace OpenRCT2::Scenario
{
    ObjectiveStatus Objective::CheckGuestsBy(Park::ParkData& park, GameState_t& gameState) const
    {
        auto parkRating = park.rating;
        int32_t currentMonthYear = GetDate().GetMonthsElapsed();

        if (currentMonthYear == MONTH_COUNT * Year || AllowEarlyCompletion())
        {
            if (parkRating >= 600 && park.numGuestsInPark >= NumGuests)
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

    ObjectiveStatus Objective::CheckParkValueBy(Park::ParkData& park, GameState_t& gameState) const
    {
        int32_t currentMonthYear = GetDate().GetMonthsElapsed();
        money64 objectiveParkValue = Currency;
        money64 parkValue = park.value;

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
    ObjectiveStatus Objective::Check10RollerCoasters(Park::ParkData& park, GameState_t& gameState) const
    {
        auto rcs = 0;
        BitSet<kMaxRideObjects> type_already_counted;
        for (const auto& ride : RideManager(gameState))
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
    ObjectiveStatus Objective::CheckGuestsAndRating(Park::ParkData& park, GameState_t& gameState) const
    {
        // TODO: make park-specific
        if (park.rating < 700 && GetDate().GetMonthsElapsed() >= 1)
        {
            gameState.scenarioParkRatingWarningDays++;
            if (gameState.scenarioParkRatingWarningDays == 1)
            {
                if (Config::Get().notifications.parkRatingWarnings)
                {
                    News::AddItemToQueue(News::ItemType::graph, STR_PARK_RATING_WARNING_4_WEEKS_REMAINING, 0, {});
                }
            }
            else if (gameState.scenarioParkRatingWarningDays == 8)
            {
                if (Config::Get().notifications.parkRatingWarnings)
                {
                    News::AddItemToQueue(News::ItemType::graph, STR_PARK_RATING_WARNING_3_WEEKS_REMAINING, 0, {});
                }
            }
            else if (gameState.scenarioParkRatingWarningDays == 15)
            {
                if (Config::Get().notifications.parkRatingWarnings)
                {
                    News::AddItemToQueue(News::ItemType::graph, STR_PARK_RATING_WARNING_2_WEEKS_REMAINING, 0, {});
                }
            }
            else if (gameState.scenarioParkRatingWarningDays == 22)
            {
                if (Config::Get().notifications.parkRatingWarnings)
                {
                    News::AddItemToQueue(News::ItemType::graph, STR_PARK_RATING_WARNING_1_WEEK_REMAINING, 0, {});
                }
            }
            else if (gameState.scenarioParkRatingWarningDays == 29)
            {
                News::AddItemToQueue(News::ItemType::graph, STR_PARK_HAS_BEEN_CLOSED_DOWN, 0, {});
                park.flags &= ~PARK_FLAGS_PARK_OPEN;
                gameState.scenarioOptions.guestInitialHappiness = 50;
                return ObjectiveStatus::Failure;
            }
        }
        else if (gameState.scenarioCompletedCompanyValue != kCompanyValueOnFailedObjective)
        {
            gameState.scenarioParkRatingWarningDays = 0;
        }

        if (park.rating >= 700)
            if (park.numGuestsInPark >= NumGuests)
                return ObjectiveStatus::Success;

        return ObjectiveStatus::Undecided;
    }

    ObjectiveStatus Objective::CheckMonthlyRideIncome(Park::ParkData& park, GameState_t& gameState) const
    {
        money64 lastMonthRideIncome = park.expenditureTable[1][EnumValue(ExpenditureType::parkRideTickets)];
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
    ObjectiveStatus Objective::Check10RollerCoastersLength(Park::ParkData& park, GameState_t& gameState) const
    {
        BitSet<kMaxRideObjects> type_already_counted;
        auto rcs = 0;
        for (const auto& ride : RideManager(gameState))
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

    ObjectiveStatus Objective::CheckFinish5RollerCoasters(Park::ParkData& park, GameState_t& gameState) const
    {
        // Originally, this did not check for null rides, neither did it check if
        // the rides are even rollercoasters, never mind the right rollercoasters to be finished.
        auto rcs = 0;
        for (const auto& ride : RideManager(gameState))
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

    ObjectiveStatus Objective::CheckRepayLoanAndParkValue(Park::ParkData& park, GameState_t& gameState) const
    {
        money64 parkValue = park.value;
        money64 currentLoan = park.bankLoan;

        if (currentLoan <= 0 && parkValue >= Currency)
        {
            return ObjectiveStatus::Success;
        }

        return ObjectiveStatus::Undecided;
    }

    ObjectiveStatus Objective::CheckMonthlyFoodIncome(Park::ParkData& park, GameState_t& gameState) const
    {
        const auto* lastMonthExpenditure = park.expenditureTable[1];
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
    ObjectiveStatus Objective::Check(Park::ParkData& park, GameState_t& gameState) const
    {
        if (gameState.scenarioCompletedCompanyValue != kMoney64Undefined)
        {
            return ObjectiveStatus::Undecided;
        }

        switch (Type)
        {
            case ObjectiveType::guestsBy:
                return CheckGuestsBy(park, gameState);
            case ObjectiveType::parkValueBy:
                return CheckParkValueBy(park, gameState);
            case ObjectiveType::tenRollercoasters:
                return Check10RollerCoasters(park, gameState);
            case ObjectiveType::guestsAndRating:
                return CheckGuestsAndRating(park, gameState);
            case ObjectiveType::monthlyRideIncome:
                return CheckMonthlyRideIncome(park, gameState);
            case ObjectiveType::tenRollercoastersLength:
                return Check10RollerCoastersLength(park, gameState);
            case ObjectiveType::finishFiveRollercoasters:
                return CheckFinish5RollerCoasters(park, gameState);
            case ObjectiveType::repayLoanAndParkValue:
                return CheckRepayLoanAndParkValue(park, gameState);
            case ObjectiveType::monthlyFoodIncome:
                return CheckMonthlyFoodIncome(park, gameState);
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
