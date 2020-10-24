/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Sprite.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(BalloonPressAction, GAME_COMMAND_BALLOON_PRESS, GameActionResult)
{
    uint16_t _spriteIndex = SPRITE_INDEX_NULL;

public:
    BalloonPressAction() = default;
    BalloonPressAction(uint16_t spriteIndex)
        : _spriteIndex(spriteIndex)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_spriteIndex);
    }

    GameActionResult::Ptr Query() const override
    {
        auto balloon = TryGetEntity<Balloon>(_spriteIndex);
        if (balloon == nullptr)
        {
            log_error("Tried getting invalid sprite for balloon: %u", _spriteIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }
        return MakeResult();
    }

    GameActionResult::Ptr Execute() const override
    {
        auto balloon = TryGetEntity<Balloon>(_spriteIndex);
        if (balloon == nullptr)
        {
            log_error("Tried getting invalid sprite for balloon: %u", _spriteIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        balloon->Press();

        return MakeResult();
    }
};
