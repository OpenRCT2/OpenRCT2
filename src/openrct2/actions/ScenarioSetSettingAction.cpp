/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ScenarioSetSettingAction.h"

#include "../OpenRCT2.h"
#include "../entity/Peep.h"
#include "../interface/Window.h"
#include "../management/Finance.h"
#include "../scenario/Scenario.h"
#include "../util/Util.h"
#include "../world/Park.h"

#include <algorithm>
namespace OpenRCT2
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
            LOG_ERROR("Invalid setting: %u", _setting);
            return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
        }

        return GameActions::Result();
    }

    GameActions::Result ScenarioSetSettingAction::Execute() const
    {
        switch (_setting)
        {
            case ScenarioSetSetting::NoMoney:
                if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
                {
                    if (_value != 0)
                    {
                        gParkFlags |= PARK_FLAGS_NO_MONEY;
                    }
                    else
                    {
                        gParkFlags &= ~PARK_FLAGS_NO_MONEY;
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
                    WindowInvalidateByClass(WindowClass::Ride);
                    WindowInvalidateByClass(WindowClass::Peep);
                    WindowInvalidateByClass(WindowClass::ParkInformation);
                    WindowInvalidateByClass(WindowClass::Finances);
                    WindowInvalidateByClass(WindowClass::BottomToolbar);
                    WindowInvalidateByClass(WindowClass::TopToolbar);
                }
                break;
            case ScenarioSetSetting::InitialCash:
                gInitialCash = std::clamp<money64>(_value, 0.00_GBP, 1000000.00_GBP);
                gCash = gInitialCash;
                WindowInvalidateByClass(WindowClass::Finances);
                WindowInvalidateByClass(WindowClass::BottomToolbar);
                break;
            case ScenarioSetSetting::InitialLoan:
                gBankLoan = std::clamp<money64>(_value, 0.00_GBP, 5000000.00_GBP);
                gMaxBankLoan = std::max(gBankLoan, gMaxBankLoan);
                WindowInvalidateByClass(WindowClass::Finances);
                break;
            case ScenarioSetSetting::MaximumLoanSize:
                gMaxBankLoan = std::clamp<money64>(_value, 0.00_GBP, 5000000.00_GBP);
                gBankLoan = std::min(gBankLoan, gMaxBankLoan);
                WindowInvalidateByClass(WindowClass::Finances);
                break;
            case ScenarioSetSetting::AnnualInterestRate:
                gBankLoanInterestRate = std::clamp<uint8_t>(_value, 0, MaxBankLoanInterestRate);
                WindowInvalidateByClass(WindowClass::Finances);
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
                gGuestInitialCash = std::clamp<money64>(_value, 0.00_GBP, 1000.00_GBP);
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
                gLandPrice = std::clamp<money32>(_value, 5.00_GBP, 200.00_GBP);
                break;
            case ScenarioSetSetting::CostToBuyConstructionRights:
                gConstructionRightsPrice = std::clamp<money32>(_value, 5.00_GBP, 200.00_GBP);
                break;
            case ScenarioSetSetting::ParkChargeMethod:
                if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
                {
                    if (_value == 0)
                    {
                        gParkFlags |= PARK_FLAGS_PARK_FREE_ENTRY;
                        gParkFlags &= ~PARK_FLAGS_UNLOCK_ALL_PRICES;
                        gParkEntranceFee = 0.00_GBP;
                    }
                    else if (_value == 1)
                    {
                        gParkFlags &= ~PARK_FLAGS_PARK_FREE_ENTRY;
                        gParkFlags &= ~PARK_FLAGS_UNLOCK_ALL_PRICES;
                        gParkEntranceFee = 10.00_GBP;
                    }
                    else
                    {
                        gParkFlags |= PARK_FLAGS_PARK_FREE_ENTRY;
                        gParkFlags |= PARK_FLAGS_UNLOCK_ALL_PRICES;
                        gParkEntranceFee = 10.00_GBP;
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
                    WindowInvalidateByClass(WindowClass::ParkInformation);
                    WindowInvalidateByClass(WindowClass::Ride);
                }
                break;
            case ScenarioSetSetting::ParkChargeEntryFee:
                gParkEntranceFee = std::clamp<money32>(_value, 0.00_GBP, MAX_ENTRANCE_FEE);
                WindowInvalidateByClass(WindowClass::ParkInformation);
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
            case ScenarioSetSetting::AllowEarlyCompletion:
                gAllowEarlyCompletionInNetworkPlay = _value;
                break;
            default:
                LOG_ERROR("Invalid setting: %u", _setting);
                return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
        }
        WindowInvalidateByClass(WindowClass::EditorScenarioOptions);
        return GameActions::Result();
    }
} // namespace OpenRCT2
