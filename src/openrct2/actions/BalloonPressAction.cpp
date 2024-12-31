/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "BalloonPressAction.h"

#include "../Diagnostic.h"
#include "../entity/Balloon.h"
#include "../entity/EntityRegistry.h"
#include "GameAction.h"

using namespace OpenRCT2;

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
        LOG_ERROR("Balloon not found for spriteIndex %u", _spriteIndex);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_BALLOON_NOT_FOUND);
    }
    return GameActions::Result();
}

GameActions::Result BalloonPressAction::Execute() const
{
    auto balloon = TryGetEntity<Balloon>(_spriteIndex);
    if (balloon == nullptr)
    {
        LOG_ERROR("Balloon not found for spriteIndex %u", _spriteIndex);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_BALLOON_NOT_FOUND);
    }

    balloon->Press();

    return GameActions::Result();
}
