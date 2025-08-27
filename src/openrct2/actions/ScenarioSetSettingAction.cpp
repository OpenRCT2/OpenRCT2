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

namespace OpenRCT2::GameActions
{
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
        auto& gameState = getGameState();
        auto* windowMgr = Ui::GetWindowManager();

        switch (_setting)
        {
            case ScenarioSetSetting::NoMoney:
                if (gLegacyScene == LegacyScene::scenarioEditor)
                {
                    if (_value != 0)
                    {
                        gameState.park.flags |= PARK_FLAGS_NO_MONEY;
                    }
                    else
                    {
                        gameState.park.flags &= ~PARK_FLAGS_NO_MONEY;
                    }
                }
                else
                {
                    if (_value != 0)
                    {
                        gameState.park.flags |= PARK_FLAGS_NO_MONEY;
                    }
                    else
                    {
                        gameState.park.flags &= ~PARK_FLAGS_NO_MONEY;
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
                gameState.scenarioOptions.initialCash = std::clamp<money64>(_value, 0.00_GBP, 1000000.00_GBP);
                gameState.park.cash = gameState.scenarioOptions.initialCash;
                windowMgr->InvalidateByClass(WindowClass::Finances);
                windowMgr->InvalidateByClass(WindowClass::BottomToolbar);
                break;
            case ScenarioSetSetting::InitialLoan:
                gameState.park.bankLoan = std::clamp<money64>(_value, 0.00_GBP, 5000000.00_GBP);
                gameState.park.maxBankLoan = std::max(gameState.park.bankLoan, gameState.park.maxBankLoan);
                windowMgr->InvalidateByClass(WindowClass::Finances);
                break;
            case ScenarioSetSetting::MaximumLoanSize:
                gameState.park.maxBankLoan = std::clamp<money64>(_value, 0.00_GBP, 5000000.00_GBP);
                gameState.park.bankLoan = std::min(gameState.park.bankLoan, gameState.park.maxBankLoan);
                windowMgr->InvalidateByClass(WindowClass::Finances);
                break;
            case ScenarioSetSetting::AnnualInterestRate:
                gameState.park.bankLoanInterestRate = std::clamp<uint8_t>(_value, 0, kMaxBankLoanInterestRate);
                windowMgr->InvalidateByClass(WindowClass::Finances);
                break;
            case ScenarioSetSetting::ForbidMarketingCampaigns:
                if (_value != 0)
                {
                    gameState.park.flags |= PARK_FLAGS_FORBID_MARKETING_CAMPAIGN;
                }
                else
                {
                    gameState.park.flags &= ~PARK_FLAGS_FORBID_MARKETING_CAMPAIGN;
                }
                break;
            case ScenarioSetSetting::AverageCashPerGuest:
                gameState.scenarioOptions.guestInitialCash = std::clamp<money64>(_value, 0.00_GBP, 1000.00_GBP);
                break;
            case ScenarioSetSetting::GuestInitialHappiness:
                gameState.scenarioOptions.guestInitialHappiness = std::clamp<uint8_t>(_value, 40, 250);
                break;
            case ScenarioSetSetting::GuestInitialHunger:
                gameState.scenarioOptions.guestInitialHunger = std::clamp<uint8_t>(_value, 40, 250);
                break;
            case ScenarioSetSetting::GuestInitialThirst:
                gameState.scenarioOptions.guestInitialThirst = std::clamp<uint8_t>(_value, 40, 250);
                break;
            case ScenarioSetSetting::GuestsPreferLessIntenseRides:
                if (_value != 0)
                {
                    gameState.park.flags |= PARK_FLAGS_PREF_LESS_INTENSE_RIDES;
                }
                else
                {
                    gameState.park.flags &= ~PARK_FLAGS_PREF_LESS_INTENSE_RIDES;
                }
                break;
            case ScenarioSetSetting::GuestsPreferMoreIntenseRides:
                if (_value != 0)
                {
                    gameState.park.flags |= PARK_FLAGS_PREF_MORE_INTENSE_RIDES;
                }
                else
                {
                    gameState.park.flags &= ~PARK_FLAGS_PREF_MORE_INTENSE_RIDES;
                }
                break;
            case ScenarioSetSetting::CostToBuyLand:
                gameState.scenarioOptions.landPrice = std::clamp<money64>(_value, 5.00_GBP, 200.00_GBP);
                break;
            case ScenarioSetSetting::CostToBuyConstructionRights:
                gameState.scenarioOptions.constructionRightsPrice = std::clamp<money64>(_value, 5.00_GBP, 200.00_GBP);
                break;
            case ScenarioSetSetting::ParkChargeMethod:
                if (gLegacyScene == LegacyScene::scenarioEditor)
                {
                    if (_value == 0)
                    {
                        gameState.park.flags |= PARK_FLAGS_PARK_FREE_ENTRY;
                        gameState.park.flags &= ~PARK_FLAGS_UNLOCK_ALL_PRICES;
                        gameState.park.entranceFee = 0.00_GBP;
                    }
                    else if (_value == 1)
                    {
                        gameState.park.flags &= ~PARK_FLAGS_PARK_FREE_ENTRY;
                        gameState.park.flags &= ~PARK_FLAGS_UNLOCK_ALL_PRICES;
                        gameState.park.entranceFee = 10.00_GBP;
                    }
                    else
                    {
                        gameState.park.flags |= PARK_FLAGS_PARK_FREE_ENTRY;
                        gameState.park.flags |= PARK_FLAGS_UNLOCK_ALL_PRICES;
                        gameState.park.entranceFee = 10.00_GBP;
                    }
                }
                else
                {
                    if (_value == 0)
                    {
                        gameState.park.flags |= PARK_FLAGS_PARK_FREE_ENTRY;
                        gameState.park.flags &= ~PARK_FLAGS_UNLOCK_ALL_PRICES;
                    }
                    else if (_value == 1)
                    {
                        gameState.park.flags &= ~PARK_FLAGS_PARK_FREE_ENTRY;
                        gameState.park.flags &= ~PARK_FLAGS_UNLOCK_ALL_PRICES;
                    }
                    else
                    {
                        gameState.park.flags |= PARK_FLAGS_PARK_FREE_ENTRY;
                        gameState.park.flags |= PARK_FLAGS_UNLOCK_ALL_PRICES;
                    }
                    windowMgr->InvalidateByClass(WindowClass::ParkInformation);
                    windowMgr->InvalidateByClass(WindowClass::Ride);
                }
                break;
            case ScenarioSetSetting::ParkChargeEntryFee:
                gameState.park.entranceFee = std::clamp<money64>(_value, 0.00_GBP, kMaxEntranceFee);
                windowMgr->InvalidateByClass(WindowClass::ParkInformation);
                break;
            case ScenarioSetSetting::ForbidTreeRemoval:
                if (_value != 0)
                {
                    gameState.park.flags |= PARK_FLAGS_FORBID_TREE_REMOVAL;
                }
                else
                {
                    gameState.park.flags &= ~PARK_FLAGS_FORBID_TREE_REMOVAL;
                }
                break;
            case ScenarioSetSetting::ForbidLandscapeChanges:
                if (_value != 0)
                {
                    gameState.park.flags |= PARK_FLAGS_FORBID_LANDSCAPE_CHANGES;
                }
                else
                {
                    gameState.park.flags &= ~PARK_FLAGS_FORBID_LANDSCAPE_CHANGES;
                }
                break;
            case ScenarioSetSetting::ForbidHighConstruction:
                if (_value != 0)
                {
                    gameState.park.flags |= PARK_FLAGS_FORBID_HIGH_CONSTRUCTION;
                }
                else
                {
                    gameState.park.flags &= ~PARK_FLAGS_FORBID_HIGH_CONSTRUCTION;
                }
                break;
            case ScenarioSetSetting::ParkRatingHigherDifficultyLevel:
                if (_value != 0)
                {
                    gameState.park.flags |= PARK_FLAGS_DIFFICULT_PARK_RATING;
                }
                else
                {
                    gameState.park.flags &= ~PARK_FLAGS_DIFFICULT_PARK_RATING;
                }
                break;
            case ScenarioSetSetting::GuestGenerationHigherDifficultyLevel:
                if (_value != 0)
                {
                    gameState.park.flags |= PARK_FLAGS_DIFFICULT_GUEST_GENERATION;
                }
                else
                {
                    gameState.park.flags &= ~PARK_FLAGS_DIFFICULT_GUEST_GENERATION;
                }
                break;
            case ScenarioSetSetting::AllowEarlyCompletion:
                gAllowEarlyCompletionInNetworkPlay = _value;
                break;
            case ScenarioSetSetting::UseRCT1Interest:
            {
                if (_value != 0)
                {
                    gameState.park.flags |= PARK_FLAGS_RCT1_INTEREST;
                }
                else
                {
                    gameState.park.flags &= ~PARK_FLAGS_RCT1_INTEREST;
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
} // namespace OpenRCT2::GameActions
