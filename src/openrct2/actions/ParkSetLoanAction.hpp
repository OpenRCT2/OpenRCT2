#pragma region Copyright (c) 2014-2018 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#pragma once

#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"
#include "GameAction.h"

struct ParkSetLoanAction : public GameActionBase<GAME_COMMAND_SET_CURRENT_LOAN, GameActionResult>
{
private:
    money32 _value;

public:
    ParkSetLoanAction() {}
    ParkSetLoanAction(money32 value)
        : _value(value)
    {
    }

    uint16 GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);
        stream << _value;
    }

    GameActionResult::Ptr Query() const override
    {
        auto  currentLoan = gBankLoan;
        auto loanDifference = currentLoan - _value;
        if (_value > currentLoan)
        {
            if (_value > gMaxBankLoan)
            {
                return MakeResult(GA_ERROR::DISALLOWED, STR_CANT_BORROW_ANY_MORE_MONEY, STR_BANK_REFUSES_TO_INCREASE_LOAN);
            }
        }
        else
        {
            if (loanDifference > gCash)
            {
                return MakeResult(GA_ERROR::INSUFFICIENT_FUNDS, STR_CANT_PAY_BACK_LOAN, STR_NOT_ENOUGH_CASH_AVAILABLE);
            }
        }
        return MakeResult();
    }

    GameActionResult::Ptr Execute() const override
    {
        gCash -= (gBankLoan - _value);
        gBankLoan = _value;

        auto windowManager = OpenRCT2::GetContext()->GetUiContext()->GetWindowManager();
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_UPDATE_CASH));
        return MakeResult();
    }
};
