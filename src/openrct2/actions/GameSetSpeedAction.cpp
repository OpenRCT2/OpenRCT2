/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GameSetSpeedAction.h"

#include "../Diagnostic.h"
#include "../config/Config.h"
#include "../ui/WindowManager.h"

namespace OpenRCT2::GameActions
{
    GameSetSpeedAction::GameSetSpeedAction(uint8_t speed)
        : _speed(speed)
    {
    }

    void GameSetSpeedAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("speed", _speed);
    }

    uint16_t GameSetSpeedAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused | Flags::IgnoreForReplays;
    }

    void GameSetSpeedAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_speed);
    }

    Result GameSetSpeedAction::Query(GameState_t& gameState) const
    {
        Result res = Result();

        if (!IsValidSpeed(_speed))
        {
            LOG_ERROR("Invalid speed %u", _speed);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        return res;
    }

    Result GameSetSpeedAction::Execute(GameState_t& gameState) const
    {
        Result res = Result();

        if (!IsValidSpeed(_speed))
        {
            LOG_ERROR("Invalid speed %u", _speed);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        gGameSpeed = _speed;

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByClass(WindowClass::topToolbar);

        return res;
    }

    bool GameSetSpeedAction::IsValidSpeed(uint8_t speed) const
    {
        return speed >= 1 && (speed <= 4 || (Config::Get().general.debuggingTools && speed <= 8));
    }
} // namespace OpenRCT2::GameActions
