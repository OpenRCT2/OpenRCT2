/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

namespace OpenRCT2::Park
{
    struct ParkData;
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

    bool ObjectiveNeedsMoney(ObjectiveType objective);

    enum class ObjectiveStatus : uint8_t
    {
        undecided,
        success,
        failure,
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

        // TODO: make park-specific
        static const int kLowParkRatingThreshold = 700;

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

        ObjectiveStatus Check(const Park::ParkData& park, const GameState_t& gameState) const;
        void onFailure(GameState_t& gameState) const;

    private:
        ObjectiveStatus CheckGuestsBy(const Park::ParkData& park, const GameState_t& gameState) const;
        ObjectiveStatus CheckParkValueBy(const Park::ParkData& park, const GameState_t& gameState) const;
        ObjectiveStatus Check10RollerCoasters(const Park::ParkData& park, const GameState_t& gameState) const;
        ObjectiveStatus CheckGuestsAndRating(const Park::ParkData& park, const GameState_t& gameState) const;
        ObjectiveStatus CheckMonthlyRideIncome(const Park::ParkData& park, const GameState_t& gameState) const;
        ObjectiveStatus Check10RollerCoastersLength(const Park::ParkData& park, const GameState_t& gameState) const;
        ObjectiveStatus CheckFinish5RollerCoasters(const Park::ParkData& park, const GameState_t& gameState) const;
        ObjectiveStatus CheckRepayLoanAndParkValue(const Park::ParkData& park, const GameState_t& gameState) const;
        ObjectiveStatus CheckMonthlyFoodIncome(const Park::ParkData& park, const GameState_t& gameState) const;
    };
} // namespace OpenRCT2::Scenario
