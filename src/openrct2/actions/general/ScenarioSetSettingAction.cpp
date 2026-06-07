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
#include "../../entity/Peep.h"
#include "../../management/Finance.h"
#include "../../scenario/Scenario.h"
#include "../../ui/WindowManager.h"
#include "../../world/Park.h"

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
                if (gLegacyScene == LegacyScene::scenarioEditor)
                {
                    if (_value != 0)
                    {
                        park.flags |= PARK_FLAGS_NO_MONEY;
                    }
                    else
                    {
                        park.flags &= ~PARK_FLAGS_NO_MONEY;
                    }
                }
                else
                {
                    if (_value != 0)
                    {
                        park.flags |= PARK_FLAGS_NO_MONEY;
                    }
                    else
                    {
                        park.flags &= ~PARK_FLAGS_NO_MONEY;
                    }
                    // Invalidate all windows that have anything to do with finance
                    windowMgr->InvalidateByClass(WindowClass::ride);
                    windowMgr->InvalidateByClass(WindowClass::peep);
                    windowMgr->InvalidateByClass(WindowClass::parkInformation);
                    windowMgr->InvalidateByClass(WindowClass::finances);
                    windowMgr->InvalidateByClass(WindowClass::bottomToolbar);
                    windowMgr->InvalidateByClass(WindowClass::topToolbar);
                }
                break;
            case ScenarioSetSetting::initialCash:
                gameState.scenarioOptions.initialCash = std::clamp<money64>(_value, 0.00_GBP, 1000000.00_GBP);
                park.cash = gameState.scenarioOptions.initialCash;
                windowMgr->InvalidateByClass(WindowClass::finances);
                windowMgr->InvalidateByClass(WindowClass::bottomToolbar);
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
                if (_value != 0)
                {
                    park.flags |= PARK_FLAGS_FORBID_MARKETING_CAMPAIGN;
                }
                else
                {
                    park.flags &= ~PARK_FLAGS_FORBID_MARKETING_CAMPAIGN;
                }
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
                if (_value != 0)
                {
                    park.flags |= PARK_FLAGS_PREF_LESS_INTENSE_RIDES;
                }
                else
                {
                    park.flags &= ~PARK_FLAGS_PREF_LESS_INTENSE_RIDES;
                }
                break;
            case ScenarioSetSetting::guestsPreferMoreIntenseRides:
                if (_value != 0)
                {
                    park.flags |= PARK_FLAGS_PREF_MORE_INTENSE_RIDES;
                }
                else
                {
                    park.flags &= ~PARK_FLAGS_PREF_MORE_INTENSE_RIDES;
                }
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
                        park.flags |= PARK_FLAGS_PARK_FREE_ENTRY;
                        park.flags &= ~PARK_FLAGS_UNLOCK_ALL_PRICES;
                        park.entranceFee = 0.00_GBP;
                    }
                    else if (_value == 1)
                    {
                        park.flags &= ~PARK_FLAGS_PARK_FREE_ENTRY;
                        park.flags &= ~PARK_FLAGS_UNLOCK_ALL_PRICES;
                        park.entranceFee = 10.00_GBP;
                    }
                    else
                    {
                        park.flags |= PARK_FLAGS_PARK_FREE_ENTRY;
                        park.flags |= PARK_FLAGS_UNLOCK_ALL_PRICES;
                        park.entranceFee = 10.00_GBP;
                    }
                }
                else
                {
                    if (_value == 0)
                    {
                        park.flags |= PARK_FLAGS_PARK_FREE_ENTRY;
                        park.flags &= ~PARK_FLAGS_UNLOCK_ALL_PRICES;
                    }
                    else if (_value == 1)
                    {
                        park.flags &= ~PARK_FLAGS_PARK_FREE_ENTRY;
                        park.flags &= ~PARK_FLAGS_UNLOCK_ALL_PRICES;
                    }
                    else
                    {
                        park.flags |= PARK_FLAGS_PARK_FREE_ENTRY;
                        park.flags |= PARK_FLAGS_UNLOCK_ALL_PRICES;
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
                if (_value != 0)
                {
                    park.flags |= PARK_FLAGS_FORBID_TREE_REMOVAL;
                }
                else
                {
                    park.flags &= ~PARK_FLAGS_FORBID_TREE_REMOVAL;
                }
                break;
            case ScenarioSetSetting::forbidLandscapeChanges:
                if (_value != 0)
                {
                    park.flags |= PARK_FLAGS_FORBID_LANDSCAPE_CHANGES;
                }
                else
                {
                    park.flags &= ~PARK_FLAGS_FORBID_LANDSCAPE_CHANGES;
                }
                break;
            case ScenarioSetSetting::forbidHighConstruction:
                if (_value != 0)
                {
                    park.flags |= PARK_FLAGS_FORBID_HIGH_CONSTRUCTION;
                }
                else
                {
                    park.flags &= ~PARK_FLAGS_FORBID_HIGH_CONSTRUCTION;
                }
                break;
            case ScenarioSetSetting::parkRatingHigherDifficultyLevel:
                if (_value != 0)
                {
                    park.flags |= PARK_FLAGS_DIFFICULT_PARK_RATING;
                }
                else
                {
                    park.flags &= ~PARK_FLAGS_DIFFICULT_PARK_RATING;
                }
                break;
            case ScenarioSetSetting::guestGenerationHigherDifficultyLevel:
                if (_value != 0)
                {
                    park.flags |= PARK_FLAGS_DIFFICULT_GUEST_GENERATION;
                }
                else
                {
                    park.flags &= ~PARK_FLAGS_DIFFICULT_GUEST_GENERATION;
                }
                break;
            case ScenarioSetSetting::allowEarlyCompletion:
                gAllowEarlyCompletionInNetworkPlay = _value;
                break;
            case ScenarioSetSetting::useRCT1Interest:
            {
                if (_value != 0)
                {
                    park.flags |= PARK_FLAGS_RCT1_INTEREST;
                }
                else
                {
                    park.flags &= ~PARK_FLAGS_RCT1_INTEREST;
                }
                break;
            }
            default:
                LOG_ERROR("Invalid scenario setting %u", _setting);
                return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
        }
        windowMgr->InvalidateByClass(WindowClass::editorScenarioOptions);
        return Result();
    }
} // namespace OpenRCT2::GameActions
