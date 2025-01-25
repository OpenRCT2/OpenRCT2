/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ScenarioSetSettingAction.h"

#include "../Diagnostic.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../entity/Peep.h"
#include "../management/Finance.h"
#include "../scenario/Scenario.h"
#include "../ui/WindowManager.h"
#include "../world/Park.h"

using namespace OpenRCT2;

void ScenarioSetSettingAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_setting) << DS_TAG(_value);
}

void ScenarioSetSettingAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("setting", _setting);
    visitor.Visit("value", _value);
}

GameActions::Result ScenarioSetSettingAction::Query() const
{
    if (_setting >= ScenarioSetSetting::Count)
    {
        LOG_ERROR("Invalid scenario setting: %u", _setting);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
    }

    return GameActions::Result();
}

GameActions::Result ScenarioSetSettingAction::Execute() const
{
    auto& gameState = GetGameState();
    auto* windowMgr = Ui::GetWindowManager();

    switch (_setting)
    {
        case ScenarioSetSetting::NoMoney:
            if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
            {
                if (_value != 0)
                {
                    gameState.Park.Flags |= PARK_FLAGS_NO_MONEY;
                }
                else
                {
                    gameState.Park.Flags &= ~PARK_FLAGS_NO_MONEY;
                }
            }
            else
            {
                if (_value != 0)
                {
                    gameState.Park.Flags |= PARK_FLAGS_NO_MONEY;
                }
                else
                {
                    gameState.Park.Flags &= ~PARK_FLAGS_NO_MONEY;
                }
                // Invalidate all windows that have anything to do with finance
                windowMgr->InvalidateByClass(WindowClass::Ride);
                windowMgr->InvalidateByClass(WindowClass::Peep);
                windowMgr->InvalidateByClass(WindowClass::ParkInformation);
                windowMgr->InvalidateByClass(WindowClass::Finances);
                windowMgr->InvalidateByClass(WindowClass::BottomToolbar);
                windowMgr->InvalidateByClass(WindowClass::TopToolbar);
            }
            break;
        case ScenarioSetSetting::InitialCash:
            gameState.InitialCash = std::clamp<money64>(_value, 0.00_GBP, 1000000.00_GBP);
            gameState.Cash = gameState.InitialCash;
            windowMgr->InvalidateByClass(WindowClass::Finances);
            windowMgr->InvalidateByClass(WindowClass::BottomToolbar);
            break;
        case ScenarioSetSetting::InitialLoan:
            gameState.BankLoan = std::clamp<money64>(_value, 0.00_GBP, 5000000.00_GBP);
            gameState.MaxBankLoan = std::max(gameState.BankLoan, gameState.MaxBankLoan);
            windowMgr->InvalidateByClass(WindowClass::Finances);
            break;
        case ScenarioSetSetting::MaximumLoanSize:
            gameState.MaxBankLoan = std::clamp<money64>(_value, 0.00_GBP, 5000000.00_GBP);
            gameState.BankLoan = std::min(gameState.BankLoan, gameState.MaxBankLoan);
            windowMgr->InvalidateByClass(WindowClass::Finances);
            break;
        case ScenarioSetSetting::AnnualInterestRate:
            gameState.BankLoanInterestRate = std::clamp<uint8_t>(_value, 0, MaxBankLoanInterestRate);
            windowMgr->InvalidateByClass(WindowClass::Finances);
            break;
        case ScenarioSetSetting::ForbidMarketingCampaigns:
            if (_value != 0)
            {
                gameState.Park.Flags |= PARK_FLAGS_FORBID_MARKETING_CAMPAIGN;
            }
            else
            {
                gameState.Park.Flags &= ~PARK_FLAGS_FORBID_MARKETING_CAMPAIGN;
            }
            break;
        case ScenarioSetSetting::AverageCashPerGuest:
            gameState.GuestInitialCash = std::clamp<money64>(_value, 0.00_GBP, 1000.00_GBP);
            break;
        case ScenarioSetSetting::GuestInitialHappiness:
            gameState.GuestInitialHappiness = std::clamp<uint8_t>(_value, 40, 250);
            break;
        case ScenarioSetSetting::GuestInitialHunger:
            gameState.GuestInitialHunger = std::clamp<uint8_t>(_value, 40, 250);
            break;
        case ScenarioSetSetting::GuestInitialThirst:
            gameState.GuestInitialThirst = std::clamp<uint8_t>(_value, 40, 250);
            break;
        case ScenarioSetSetting::GuestsPreferLessIntenseRides:
            if (_value != 0)
            {
                gameState.Park.Flags |= PARK_FLAGS_PREF_LESS_INTENSE_RIDES;
            }
            else
            {
                gameState.Park.Flags &= ~PARK_FLAGS_PREF_LESS_INTENSE_RIDES;
            }
            break;
        case ScenarioSetSetting::GuestsPreferMoreIntenseRides:
            if (_value != 0)
            {
                gameState.Park.Flags |= PARK_FLAGS_PREF_MORE_INTENSE_RIDES;
            }
            else
            {
                gameState.Park.Flags &= ~PARK_FLAGS_PREF_MORE_INTENSE_RIDES;
            }
            break;
        case ScenarioSetSetting::CostToBuyLand:
            gameState.LandPrice = std::clamp<money64>(_value, 5.00_GBP, 200.00_GBP);
            break;
        case ScenarioSetSetting::CostToBuyConstructionRights:
            gameState.ConstructionRightsPrice = std::clamp<money64>(_value, 5.00_GBP, 200.00_GBP);
            break;
        case ScenarioSetSetting::ParkChargeMethod:
            if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
            {
                if (_value == 0)
                {
                    gameState.Park.Flags |= PARK_FLAGS_PARK_FREE_ENTRY;
                    gameState.Park.Flags &= ~PARK_FLAGS_UNLOCK_ALL_PRICES;
                    gameState.Park.EntranceFee = 0.00_GBP;
                }
                else if (_value == 1)
                {
                    gameState.Park.Flags &= ~PARK_FLAGS_PARK_FREE_ENTRY;
                    gameState.Park.Flags &= ~PARK_FLAGS_UNLOCK_ALL_PRICES;
                    gameState.Park.EntranceFee = 10.00_GBP;
                }
                else
                {
                    gameState.Park.Flags |= PARK_FLAGS_PARK_FREE_ENTRY;
                    gameState.Park.Flags |= PARK_FLAGS_UNLOCK_ALL_PRICES;
                    gameState.Park.EntranceFee = 10.00_GBP;
                }
            }
            else
            {
                if (_value == 0)
                {
                    gameState.Park.Flags |= PARK_FLAGS_PARK_FREE_ENTRY;
                    gameState.Park.Flags &= ~PARK_FLAGS_UNLOCK_ALL_PRICES;
                }
                else if (_value == 1)
                {
                    gameState.Park.Flags &= ~PARK_FLAGS_PARK_FREE_ENTRY;
                    gameState.Park.Flags &= ~PARK_FLAGS_UNLOCK_ALL_PRICES;
                }
                else
                {
                    gameState.Park.Flags |= PARK_FLAGS_PARK_FREE_ENTRY;
                    gameState.Park.Flags |= PARK_FLAGS_UNLOCK_ALL_PRICES;
                }
                windowMgr->InvalidateByClass(WindowClass::ParkInformation);
                windowMgr->InvalidateByClass(WindowClass::Ride);
            }
            break;
        case ScenarioSetSetting::ParkChargeEntryFee:
            gameState.Park.EntranceFee = std::clamp<money64>(_value, 0.00_GBP, kMaxEntranceFee);
            windowMgr->InvalidateByClass(WindowClass::ParkInformation);
            break;
        case ScenarioSetSetting::ForbidTreeRemoval:
            if (_value != 0)
            {
                gameState.Park.Flags |= PARK_FLAGS_FORBID_TREE_REMOVAL;
            }
            else
            {
                gameState.Park.Flags &= ~PARK_FLAGS_FORBID_TREE_REMOVAL;
            }
            break;
        case ScenarioSetSetting::ForbidLandscapeChanges:
            if (_value != 0)
            {
                gameState.Park.Flags |= PARK_FLAGS_FORBID_LANDSCAPE_CHANGES;
            }
            else
            {
                gameState.Park.Flags &= ~PARK_FLAGS_FORBID_LANDSCAPE_CHANGES;
            }
            break;
        case ScenarioSetSetting::ForbidHighConstruction:
            if (_value != 0)
            {
                gameState.Park.Flags |= PARK_FLAGS_FORBID_HIGH_CONSTRUCTION;
            }
            else
            {
                gameState.Park.Flags &= ~PARK_FLAGS_FORBID_HIGH_CONSTRUCTION;
            }
            break;
        case ScenarioSetSetting::ParkRatingHigherDifficultyLevel:
            if (_value != 0)
            {
                gameState.Park.Flags |= PARK_FLAGS_DIFFICULT_PARK_RATING;
            }
            else
            {
                gameState.Park.Flags &= ~PARK_FLAGS_DIFFICULT_PARK_RATING;
            }
            break;
        case ScenarioSetSetting::GuestGenerationHigherDifficultyLevel:
            if (_value != 0)
            {
                gameState.Park.Flags |= PARK_FLAGS_DIFFICULT_GUEST_GENERATION;
            }
            else
            {
                gameState.Park.Flags &= ~PARK_FLAGS_DIFFICULT_GUEST_GENERATION;
            }
            break;
        case ScenarioSetSetting::AllowEarlyCompletion:
            gAllowEarlyCompletionInNetworkPlay = _value;
            break;
        case ScenarioSetSetting::UseRCT1Interest:
        {
            if (_value != 0)
            {
                gameState.Park.Flags |= PARK_FLAGS_RCT1_INTEREST;
            }
            else
            {
                gameState.Park.Flags &= ~PARK_FLAGS_RCT1_INTEREST;
            }
            break;
        }
        default:
            LOG_ERROR("Invalid scenario setting %u", _setting);
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
    }
    windowMgr->InvalidateByClass(WindowClass::EditorScenarioOptions);
    return GameActions::Result();
}
