/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "BalloonPressAction.h"

#include "../entity/Balloon.h"
#include "../entity/EntityRegistry.h"
#include "GameAction.h"

BalloonPressAction::BalloonPressAction(EntityId spriteIndex)
    : _spriteIndex(spriteIndex)
{
}

void BalloonPressAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("id", _spriteIndex);
}

uint16_t BalloonPressAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

void BalloonPressAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_spriteIndex);
}

GameActions::Result BalloonPressAction::Query() const
{
    auto balloon = TryGetEntity<Balloon>(_spriteIndex);
    if (balloon == nullptr)
    {
        log_error("Tried getting invalid sprite for balloon: %u", _spriteIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }
    return GameActions::Result();
}

GameActions::Result BalloonPressAction::Execute() const
{
    auto balloon = TryGetEntity<Balloon>(_spriteIndex);
    if (balloon == nullptr)
    {
        log_error("Tried getting invalid sprite for balloon: %u", _spriteIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    balloon->Press();

    return GameActions::Result();
}
