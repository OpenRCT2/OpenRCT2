/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ParkSetLoanAction.h"

#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"

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
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void ParkSetLoanAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_value);
}

GameActions::Result ParkSetLoanAction::Query() const
{
    auto loanDifference = gBankLoan - _value;
    if (_value > gBankLoan && _value > gMaxBankLoan)
    {
        return GameActions::Result(
            GameActions::Status::Disallowed, STR_CANT_BORROW_ANY_MORE_MONEY, STR_BANK_REFUSES_TO_INCREASE_LOAN);
    }
    if (_value < gBankLoan && _value < 0.00_GBP)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_PAY_BACK_LOAN, STR_LOAN_CANT_BE_NEGATIVE);
    }
    if (loanDifference > gCash)
    {
        return GameActions::Result(
            GameActions::Status::InsufficientFunds, STR_CANT_PAY_BACK_LOAN, STR_NOT_ENOUGH_CASH_AVAILABLE);
    }
    return GameActions::Result();
}

GameActions::Result ParkSetLoanAction::Execute() const
{
    gCash -= (gBankLoan - _value);
    gBankLoan = _value;

    auto windowManager = OpenRCT2::GetContext()->GetUiContext()->GetWindowManager();
    windowManager->BroadcastIntent(Intent(INTENT_ACTION_UPDATE_CASH));
    return GameActions::Result();
}
