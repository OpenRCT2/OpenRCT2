/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GameSetSpeedAction.h"

#include "../config/Config.h"

GameSetSpeedAction::GameSetSpeedAction(int32_t speed)
    : _speed(speed)
{
}

void GameSetSpeedAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("speed", _speed);
}

uint16_t GameSetSpeedAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void GameSetSpeedAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_speed);
}

GameActions::Result GameSetSpeedAction::Query() const
{
    GameActions::Result res = GameActions::Result();

    if (!GameSetSpeedAction::IsValidSpeed(_speed))
    {
        LOG_WARNING("Invalid game command for speed %u", _speed);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    return res;
}

GameActions::Result GameSetSpeedAction::Execute() const
{
    GameActions::Result res = GameActions::Result();

    if (!IsValidSpeed(_speed))
    {
        LOG_WARNING("Invalid game command for speed %u", _speed);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    gGameSpeed = _speed;
    WindowInvalidateByClass(WindowClass::TopToolbar);

    return res;
}

bool GameSetSpeedAction::IsValidSpeed(int32_t speed) const
{
    return (speed >= 1 && speed <= 4) || (gConfigGeneral.DebuggingTools && speed == 8);
}
