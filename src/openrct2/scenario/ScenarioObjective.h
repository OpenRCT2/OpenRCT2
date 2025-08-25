/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Money.hpp"
#include "../localisation/StringIdType.h"
#include "../ride/RideRatings.h"

namespace OpenRCT2
{
    struct GameState_t;
}

namespace OpenRCT2::Scenario
{
    enum class ObjectiveType : uint8_t
    {
        none,
        guestsBy,
        parkValueBy,
        haveFun,
        buildTheBest,
        tenRollercoasters,
        guestsAndRating,
        monthlyRideIncome,
        tenRollercoastersLength,
        finishFiveRollercoasters,
        repayLoanAndParkValue,
        monthlyFoodIncome,

        count
    };

    bool ObjectiveNeedsMoney(const ObjectiveType objective);

    enum class ObjectiveStatus : uint8_t
    {
        Undecided,
        Success,
        Failure,
    };

    struct Objective
    {
        ObjectiveType Type;
        uint8_t Year;
        union
        {
            uint16_t NumGuests;
            StringId RideId;
            uint16_t MinimumLength; // For the "Build 10 coasters of minimum length" objective.
        };
        union
        {
            money64 Currency;
            RideRating_t MinimumExcitement; // For the "Finish 5 coaster with a minimum excitement rating" objective.
        };

        bool NeedsMoney() const
        {
            return ObjectiveNeedsMoney(Type);
        }

        bool IsValid(bool useMoney, bool canAskMoneyForRides) const
        {
            const bool objectiveAllowedByMoneyUsage = useMoney || !NeedsMoney();
            // This objective can only work if the player can ask money for rides.
            const bool objectiveAllowedByPaymentSettings = (Type != ObjectiveType::monthlyRideIncome) || canAskMoneyForRides;
            return objectiveAllowedByMoneyUsage && objectiveAllowedByPaymentSettings;
        }

        ObjectiveStatus Check(GameState_t& gameState) const;

    private:
        ObjectiveStatus CheckGuestsBy() const;
        ObjectiveStatus CheckParkValueBy() const;
        ObjectiveStatus Check10RollerCoasters() const;
        ObjectiveStatus CheckGuestsAndRating() const;
        ObjectiveStatus CheckMonthlyRideIncome() const;
        ObjectiveStatus Check10RollerCoastersLength() const;
        ObjectiveStatus CheckFinish5RollerCoasters() const;
        ObjectiveStatus CheckRepayLoanAndParkValue() const;
        ObjectiveStatus CheckMonthlyFoodIncome() const;
    };
} // namespace OpenRCT2::Scenario
