/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ParkSetLoanAction.h"

#include "../Context.h"
#include "../GameState.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"

namespace OpenRCT2::GameActions
{
    ParkSetLoanAction::ParkSetLoanAction(money64 value)
        : _value(value)
    {
    }

    void ParkSetLoanAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("value", _value);
    }

    uint16_t ParkSetLoanAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void ParkSetLoanAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_value);
    }

    Result ParkSetLoanAction::Query(GameState_t& gameState) const
    {
        auto& park = gameState.park;
        if (_value > park.bankLoan && _value > park.maxBankLoan)
        {
            return Result(Status::disallowed, STR_CANT_BORROW_ANY_MORE_MONEY, STR_BANK_REFUSES_TO_INCREASE_LOAN);
        }
        if (_value < park.bankLoan && _value < 0.00_GBP)
        {
            return Result(Status::invalidParameters, STR_CANT_PAY_BACK_LOAN, STR_LOAN_CANT_BE_NEGATIVE);
        }
        // The “isPayingBack” check is needed to allow increasing the loan when the player is in debt.
        const auto isPayingBack = park.bankLoan > _value;
        const auto amountToPayBack = park.bankLoan - _value;
        if (isPayingBack && amountToPayBack > park.cash)
        {
            return Result(Status::insufficientFunds, STR_CANT_PAY_BACK_LOAN, STR_NOT_ENOUGH_CASH_AVAILABLE);
        }
        return Result();
    }

    Result ParkSetLoanAction::Execute(GameState_t& gameState) const
    {
        auto& park = gameState.park;

        park.cash -= (park.bankLoan - _value);
        park.bankLoan = _value;

        auto windowManager = Ui::GetWindowManager();
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_UPDATE_CASH));
        return Result();
    }
} // namespace OpenRCT2::GameActions
