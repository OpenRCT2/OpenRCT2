/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

enum class ScenarioSetSetting : uint8_t
{
    NoMoney,
    InitialCash,
    InitialLoan,
    MaximumLoanSize,
    AnnualInterestRate,
    ForbidMarketingCampaigns,
    AverageCashPerGuest,
    GuestInitialHappiness,
    GuestInitialHunger,
    GuestInitialThirst,
    GuestsPreferLessIntenseRides,
    GuestsPreferMoreIntenseRides,
    CostToBuyLand,
    CostToBuyConstructionRights,
    ParkChargeMethod,
    ParkChargeEntryFee,
    ForbidTreeRemoval,
    ForbidLandscapeChanges,
    ForbidHighConstruction,
    ParkRatingHigherDifficultyLevel,
    GuestGenerationHigherDifficultyLevel,
    AllowEarlyCompletion,
    KeepCurrentWeather,
    Count
};

class ScenarioSetSettingAction final : public GameActionBase<GameCommand::EditScenarioOptions>
{
private:
    ScenarioSetSetting _setting{ ScenarioSetSetting::Count };
    uint32_t _value{};

public:
    ScenarioSetSettingAction() = default;
    ScenarioSetSettingAction(ScenarioSetSetting setting, uint32_t value)
        : _setting(setting)
        , _value(value)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
    }

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;
};
