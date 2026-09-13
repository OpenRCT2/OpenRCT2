/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../GameAction.hpp"

namespace OpenRCT2::GameActions
{
    enum class ScenarioSetSetting : uint8_t
    {
        noMoney,
        initialCash,
        initialLoan,
        maximumLoanSize,
        annualInterestRate,
        forbidMarketingCampaigns,
        averageCashPerGuest,
        guestInitialHappiness,
        guestInitialHunger,
        guestInitialThirst,
        guestsPreferLessIntenseRides,
        guestsPreferMoreIntenseRides,
        costToBuyLand,
        costToBuyConstructionRights,
        parkChargeMethod,
        parkChargeEntryFee,
        forbidTreeRemoval,
        forbidLandscapeChanges,
        forbidHighConstruction,
        parkRatingHigherDifficultyLevel,
        guestGenerationHigherDifficultyLevel,
        allowEarlyCompletion,
        useRCT1Interest,
        count
    };

    class ScenarioSetSettingAction final : public GameActionBase<GameCommand::EditScenarioOptions>
    {
    private:
        ScenarioSetSetting _setting{ ScenarioSetSetting::count };
        uint32_t _value{};

    public:
        ScenarioSetSettingAction() = default;
        ScenarioSetSettingAction(ScenarioSetSetting setting, uint32_t value)
            : _setting(setting)
            , _value(value)
        {
        }

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override
        {
            return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
        }

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState, Park::ParkData& park) const override;
        Result Execute(GameState_t& gameState, Park::ParkData& park) const override;
    };
} // namespace OpenRCT2::GameActions
