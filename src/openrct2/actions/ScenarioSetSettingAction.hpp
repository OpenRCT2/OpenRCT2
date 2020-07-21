/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../OpenRCT2.h"
#include "../interface/Window.h"
#include "../management/Finance.h"
#include "../peep/Peep.h"
#include "../world/Park.h"
#include "GameAction.h"

#include <algorithm>

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
    Count
};

DEFINE_GAME_ACTION(ScenarioSetSettingAction, GAME_COMMAND_EDIT_SCENARIO_OPTIONS, GameActionResult)
{
private:
    uint8_t _setting{ static_cast<uint8_t>(ScenarioSetSetting::Count) };
    uint32_t _value{ 0 };

public:
    ScenarioSetSettingAction()
    {
    }
    ScenarioSetSettingAction(ScenarioSetSetting setting, uint32_t value)
        : _setting(static_cast<uint8_t>(setting))
        , _value(value)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_setting) << DS_TAG(_value);
    }

    GameActionResult::Ptr Query() const override
    {
        if (_setting >= static_cast<uint8_t>(ScenarioSetSetting::Count))
        {
            log_error("Invalid setting: %u", _setting);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        return MakeResult();
    }

    GameActionResult::Ptr Execute() const override
    {
        switch (static_cast<ScenarioSetSetting>(_setting))
        {
            case ScenarioSetSetting::NoMoney:
                if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
                {
                    if (_value != 0)
                    {
                        gParkFlags |= PARK_FLAGS_NO_MONEY_SCENARIO;
                    }
                    else
                    {
                        gParkFlags &= ~PARK_FLAGS_NO_MONEY_SCENARIO;
                    }
                }
                else
                {
                    if (_value != 0)
                    {
                        gParkFlags |= PARK_FLAGS_NO_MONEY;
                    }
                    else
                    {
                        gParkFlags &= ~PARK_FLAGS_NO_MONEY;
                    }
                    // Invalidate all windows that have anything to do with finance
                    window_invalidate_by_class(WC_RIDE);
                    window_invalidate_by_class(WC_PEEP);
                    window_invalidate_by_class(WC_PARK_INFORMATION);
                    window_invalidate_by_class(WC_FINANCES);
                    window_invalidate_by_class(WC_BOTTOM_TOOLBAR);
                    window_invalidate_by_class(WC_TOP_TOOLBAR);
                }
                break;
            case ScenarioSetSetting::InitialCash:
                gInitialCash = std::clamp<money32>(_value, MONEY(0, 00), MONEY(1000000, 00));
                gCash = gInitialCash;
                window_invalidate_by_class(WC_FINANCES);
                window_invalidate_by_class(WC_BOTTOM_TOOLBAR);
                break;
            case ScenarioSetSetting::InitialLoan:
                gBankLoan = std::clamp<money32>(_value, MONEY(0, 00), MONEY(5000000, 00));
                gMaxBankLoan = std::max(gBankLoan, gMaxBankLoan);
                window_invalidate_by_class(WC_FINANCES);
                break;
            case ScenarioSetSetting::MaximumLoanSize:
                gMaxBankLoan = std::clamp<money32>(_value, MONEY(0, 00), MONEY(5000000, 00));
                gBankLoan = std::min(gBankLoan, gMaxBankLoan);
                window_invalidate_by_class(WC_FINANCES);
                break;
            case ScenarioSetSetting::AnnualInterestRate:
                gBankLoanInterestRate = std::clamp<uint8_t>(_value, 0, 80);
                window_invalidate_by_class(WC_FINANCES);
                break;
            case ScenarioSetSetting::ForbidMarketingCampaigns:
                if (_value != 0)
                {
                    gParkFlags |= PARK_FLAGS_FORBID_MARKETING_CAMPAIGN;
                }
                else
                {
                    gParkFlags &= ~PARK_FLAGS_FORBID_MARKETING_CAMPAIGN;
                }
                break;
            case ScenarioSetSetting::AverageCashPerGuest:
                gGuestInitialCash = std::clamp<money32>(_value, MONEY(0, 00), MONEY(1000, 00));
                break;
            case ScenarioSetSetting::GuestInitialHappiness:
                gGuestInitialHappiness = std::clamp<uint8_t>(_value, 40, 250);
                break;
            case ScenarioSetSetting::GuestInitialHunger:
                gGuestInitialHunger = std::clamp<uint8_t>(_value, 40, 250);
                break;
            case ScenarioSetSetting::GuestInitialThirst:
                gGuestInitialThirst = std::clamp<uint8_t>(_value, 40, 250);
                break;
            case ScenarioSetSetting::GuestsPreferLessIntenseRides:
                if (_value != 0)
                {
                    gParkFlags |= PARK_FLAGS_PREF_LESS_INTENSE_RIDES;
                }
                else
                {
                    gParkFlags &= ~PARK_FLAGS_PREF_LESS_INTENSE_RIDES;
                }
                break;
            case ScenarioSetSetting::GuestsPreferMoreIntenseRides:
                if (_value != 0)
                {
                    gParkFlags |= PARK_FLAGS_PREF_MORE_INTENSE_RIDES;
                }
                else
                {
                    gParkFlags &= ~PARK_FLAGS_PREF_MORE_INTENSE_RIDES;
                }
                break;
            case ScenarioSetSetting::CostToBuyLand:
                gLandPrice = std::clamp<money32>(_value, MONEY(5, 00), MONEY(200, 00));
                break;
            case ScenarioSetSetting::CostToBuyConstructionRights:
                gConstructionRightsPrice = std::clamp<money32>(_value, MONEY(5, 00), MONEY(200, 00));
                break;
            case ScenarioSetSetting::ParkChargeMethod:
                if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
                {
                    if (_value == 0)
                    {
                        gParkFlags |= PARK_FLAGS_PARK_FREE_ENTRY;
                        gParkFlags &= ~PARK_FLAGS_UNLOCK_ALL_PRICES;
                        gParkEntranceFee = MONEY(0, 00);
                    }
                    else if (_value == 1)
                    {
                        gParkFlags &= ~PARK_FLAGS_PARK_FREE_ENTRY;
                        gParkFlags &= ~PARK_FLAGS_UNLOCK_ALL_PRICES;
                        gParkEntranceFee = MONEY(10, 00);
                    }
                    else
                    {
                        gParkFlags |= PARK_FLAGS_PARK_FREE_ENTRY;
                        gParkFlags |= PARK_FLAGS_UNLOCK_ALL_PRICES;
                        gParkEntranceFee = MONEY(10, 00);
                    }
                }
                else
                {
                    if (_value == 0)
                    {
                        gParkFlags |= PARK_FLAGS_PARK_FREE_ENTRY;
                        gParkFlags &= ~PARK_FLAGS_UNLOCK_ALL_PRICES;
                    }
                    else if (_value == 1)
                    {
                        gParkFlags &= ~PARK_FLAGS_PARK_FREE_ENTRY;
                        gParkFlags &= ~PARK_FLAGS_UNLOCK_ALL_PRICES;
                    }
                    else
                    {
                        gParkFlags |= PARK_FLAGS_PARK_FREE_ENTRY;
                        gParkFlags |= PARK_FLAGS_UNLOCK_ALL_PRICES;
                    }
                    window_invalidate_by_class(WC_PARK_INFORMATION);
                    window_invalidate_by_class(WC_RIDE);
                }
                break;
            case ScenarioSetSetting::ParkChargeEntryFee:
                gParkEntranceFee = std::clamp<money32>(_value, MONEY(0, 00), MAX_ENTRANCE_FEE);
                window_invalidate_by_class(WC_PARK_INFORMATION);
                break;
            case ScenarioSetSetting::ForbidTreeRemoval:
                if (_value != 0)
                {
                    gParkFlags |= PARK_FLAGS_FORBID_TREE_REMOVAL;
                }
                else
                {
                    gParkFlags &= ~PARK_FLAGS_FORBID_TREE_REMOVAL;
                }
                break;
            case ScenarioSetSetting::ForbidLandscapeChanges:
                if (_value != 0)
                {
                    gParkFlags |= PARK_FLAGS_FORBID_LANDSCAPE_CHANGES;
                }
                else
                {
                    gParkFlags &= ~PARK_FLAGS_FORBID_LANDSCAPE_CHANGES;
                }
                break;
            case ScenarioSetSetting::ForbidHighConstruction:
                if (_value != 0)
                {
                    gParkFlags |= PARK_FLAGS_FORBID_HIGH_CONSTRUCTION;
                }
                else
                {
                    gParkFlags &= ~PARK_FLAGS_FORBID_HIGH_CONSTRUCTION;
                }
                break;
            case ScenarioSetSetting::ParkRatingHigherDifficultyLevel:
                if (_value != 0)
                {
                    gParkFlags |= PARK_FLAGS_DIFFICULT_PARK_RATING;
                }
                else
                {
                    gParkFlags &= ~PARK_FLAGS_DIFFICULT_PARK_RATING;
                }
                break;
            case ScenarioSetSetting::GuestGenerationHigherDifficultyLevel:
                if (_value != 0)
                {
                    gParkFlags |= PARK_FLAGS_DIFFICULT_GUEST_GENERATION;
                }
                else
                {
                    gParkFlags &= ~PARK_FLAGS_DIFFICULT_GUEST_GENERATION;
                }
                break;
            default:
                log_error("Invalid setting: %u", _setting);
                return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
                break;
        }
        window_invalidate_by_class(WC_EDITOR_SCENARIO_OPTIONS);
        return MakeResult();
    }
};
