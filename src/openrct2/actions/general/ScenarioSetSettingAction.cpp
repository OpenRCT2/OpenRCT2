/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ScenarioSetSettingAction.h"

#include "../../Diagnostic.h"
#include "../../GameState.h"
#include "../../OpenRCT2.h"
#include "../../management/Finance.h"
#include "../../scenario/Scenario.h"
#include "../../ui/WindowManager.h"

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

    Result ScenarioSetSettingAction::Query(GameState_t& gameState, Park::ParkData& park) const
    {
        if (_setting >= ScenarioSetSetting::count)
        {
            LOG_ERROR("Invalid scenario setting: %u", _setting);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        return Result();
    }

    Result ScenarioSetSettingAction::Execute(GameState_t& gameState, Park::ParkData& park) const
    {
        auto* windowMgr = Ui::GetWindowManager();

        switch (_setting)
        {
            case ScenarioSetSetting::noMoney:
                park.flags.set(ParkFlag::noMoney, _value != 0);
                if (gLegacyScene != LegacyScene::scenarioEditor)
                {
                    // Invalidate all windows that have anything to do with finance
                    windowMgr->InvalidateByClass(WindowClass::ride);
                    windowMgr->InvalidateByClass(WindowClass::peep);
                    windowMgr->InvalidateByClass(WindowClass::parkInformation);
                    windowMgr->InvalidateByClass(WindowClass::finances);
                    windowMgr->InvalidateByClass(WindowClass::parkInfoPanel);
                    windowMgr->InvalidateByClass(WindowClass::topToolbar);
                }
                break;
            case ScenarioSetSetting::initialCash:
                gameState.scenarioOptions.initialCash = std::clamp<money64>(_value, 0.00_GBP, 1000000.00_GBP);
                park.cash = gameState.scenarioOptions.initialCash;
                windowMgr->InvalidateByClass(WindowClass::finances);
                windowMgr->InvalidateByClass(WindowClass::parkInfoPanel);
                break;
            case ScenarioSetSetting::initialLoan:
                park.bankLoan = std::clamp<money64>(_value, 0.00_GBP, 5000000.00_GBP);
                park.maxBankLoan = std::max(park.bankLoan, park.maxBankLoan);
                windowMgr->InvalidateByClass(WindowClass::finances);
                break;
            case ScenarioSetSetting::maximumLoanSize:
                park.maxBankLoan = std::clamp<money64>(_value, 0.00_GBP, 5000000.00_GBP);
                park.bankLoan = std::min(park.bankLoan, park.maxBankLoan);
                windowMgr->InvalidateByClass(WindowClass::finances);
                break;
            case ScenarioSetSetting::annualInterestRate:
                park.bankLoanInterestRate = std::clamp<uint8_t>(_value, 0, kMaxBankLoanInterestRate);
                windowMgr->InvalidateByClass(WindowClass::finances);
                break;
            case ScenarioSetSetting::forbidMarketingCampaigns:
                park.flags.set(ParkFlag::forbidMarketingCampaigns, _value != 0);
                break;
            case ScenarioSetSetting::averageCashPerGuest:
                gameState.scenarioOptions.guestInitialCash = std::clamp<money64>(_value, 0.00_GBP, 1000.00_GBP);
                break;
            case ScenarioSetSetting::guestInitialHappiness:
                gameState.scenarioOptions.guestInitialHappiness = std::clamp<uint8_t>(_value, 40, 250);
                break;
            case ScenarioSetSetting::guestInitialHunger:
                gameState.scenarioOptions.guestInitialHunger = std::clamp<uint8_t>(_value, 40, 250);
                break;
            case ScenarioSetSetting::guestInitialThirst:
                gameState.scenarioOptions.guestInitialThirst = std::clamp<uint8_t>(_value, 40, 250);
                break;
            case ScenarioSetSetting::guestsPreferLessIntenseRides:
                park.flags.set(ParkFlag::guestPreferLessIntenseRides, _value != 0);
                break;
            case ScenarioSetSetting::guestsPreferMoreIntenseRides:
                park.flags.set(ParkFlag::guestPreferMoreIntenseRides, _value != 0);
                break;
            case ScenarioSetSetting::costToBuyLand:
                gameState.scenarioOptions.landPrice = std::clamp<money64>(_value, 5.00_GBP, 200.00_GBP);
                break;
            case ScenarioSetSetting::costToBuyConstructionRights:
                gameState.scenarioOptions.constructionRightsPrice = std::clamp<money64>(_value, 5.00_GBP, 200.00_GBP);
                break;
            case ScenarioSetSetting::parkChargeMethod:
                if (gLegacyScene == LegacyScene::scenarioEditor)
                {
                    if (_value == 0)
                    {
                        park.flags.set(ParkFlag::freeEntry);
                        park.flags.unset(ParkFlag::unlockAllPrices);
                        park.entranceFee = 0.00_GBP;
                    }
                    else if (_value == 1)
                    {
                        park.flags.unset(ParkFlag::freeEntry);
                        park.flags.unset(ParkFlag::unlockAllPrices);
                        park.entranceFee = 10.00_GBP;
                    }
                    else
                    {
                        park.flags.set(ParkFlag::freeEntry);
                        park.flags.set(ParkFlag::unlockAllPrices);
                        park.entranceFee = 10.00_GBP;
                    }
                }
                else
                {
                    if (_value == 0)
                    {
                        park.flags.set(ParkFlag::freeEntry);
                        park.flags.unset(ParkFlag::unlockAllPrices);
                    }
                    else if (_value == 1)
                    {
                        park.flags.unset(ParkFlag::freeEntry);
                        park.flags.unset(ParkFlag::unlockAllPrices);
                    }
                    else
                    {
                        park.flags.set(ParkFlag::freeEntry);
                        park.flags.set(ParkFlag::unlockAllPrices);
                    }
                    windowMgr->InvalidateByClass(WindowClass::parkInformation);
                    windowMgr->InvalidateByClass(WindowClass::ride);
                }
                break;
            case ScenarioSetSetting::parkChargeEntryFee:
                park.entranceFee = std::clamp<money64>(_value, 0.00_GBP, kMaxEntranceFee);
                windowMgr->InvalidateByClass(WindowClass::parkInformation);
                break;
            case ScenarioSetSetting::forbidTreeRemoval:
                park.flags.set(ParkFlag::forbidTreeRemoval, _value != 0);
                break;
            case ScenarioSetSetting::forbidLandscapeChanges:
                park.flags.set(ParkFlag::forbidLandscapeChanges, _value != 0);
                break;
            case ScenarioSetSetting::forbidHighConstruction:
                park.flags.set(ParkFlag::forbidHighConstruction, _value != 0);
                break;
            case ScenarioSetSetting::parkRatingHigherDifficultyLevel:
                park.flags.set(ParkFlag::difficultParkRating, _value != 0);
                break;
            case ScenarioSetSetting::guestGenerationHigherDifficultyLevel:
                park.flags.set(ParkFlag::difficultGuestGeneration, _value != 0);
                break;
            case ScenarioSetSetting::allowEarlyCompletion:
                gAllowEarlyCompletionInNetworkPlay = _value;
                break;
            case ScenarioSetSetting::useRCT1Interest:
            {
                park.flags.set(ParkFlag::rct1Interest, _value != 0);
                break;
            }
            case ScenarioSetSetting::transportRideNavigation:
                park.flags.set(ParkFlag::transportRideNavigation, _value != 0);
                windowMgr->InvalidateByClass(WindowClass::options);
                break;
            default:
                LOG_ERROR("Invalid scenario setting %u", _setting);
                return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
        }
        windowMgr->InvalidateByClass(WindowClass::editorScenarioOptions);
        return Result();
    }
} // namespace OpenRCT2::GameActions
