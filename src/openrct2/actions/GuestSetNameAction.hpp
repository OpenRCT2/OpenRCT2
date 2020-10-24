/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Cheats.h"
#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../windows/Intent.h"
#include "../world/Park.h"
#include "../world/Sprite.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(GuestSetNameAction, GAME_COMMAND_SET_GUEST_NAME, GameActionResult)
{
private:
    uint16_t _spriteIndex;
    std::string _name;

public:
    GuestSetNameAction()
    {
    }
    GuestSetNameAction(uint16_t spriteIndex, const std::string& name)
        : _spriteIndex(spriteIndex)
        , _name(name)
    {
    }

    void AcceptParameters(GameActionParameterVisitor & visitor) override
    {
        visitor.Visit("peep", _spriteIndex);
        visitor.Visit("name", _name);
    }

    uint16_t GetSpriteIndex() const
    {
        return _spriteIndex;
    }

    std::string GetGuestName() const
    {
        return _name;
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_spriteIndex) << DS_TAG(_name);
    }

    GameActionResult::Ptr Query() const override
    {
        if (_spriteIndex >= MAX_SPRITES)
        {
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_CANT_NAME_GUEST, STR_NONE);
        }

        auto guest = TryGetEntity<Guest>(_spriteIndex);
        if (guest == nullptr)
        {
            log_warning("Invalid game command for sprite %u", _spriteIndex);
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_CANT_NAME_GUEST, STR_NONE);
        }

        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        auto guest = TryGetEntity<Guest>(_spriteIndex);
        if (guest == nullptr)
        {
            log_warning("Invalid game command for sprite %u", _spriteIndex);
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_CANT_NAME_GUEST, STR_NONE);
        }

        auto curName = guest->GetName();
        if (curName == _name)
        {
            return std::make_unique<GameActionResult>(GA_ERROR::OK, STR_NONE);
        }

        if (!guest->SetName(_name))
        {
            return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_CANT_NAME_GUEST, STR_NONE);
        }

        // Easter egg functions are for guests only
        guest->HandleEasterEggName();

        gfx_invalidate_screen();

        auto intent = Intent(INTENT_ACTION_REFRESH_GUEST_LIST);
        context_broadcast_intent(&intent);

        auto res = std::make_unique<GameActionResult>();
        res->Position.x = guest->x;
        res->Position.y = guest->y;
        res->Position.z = guest->z;
        return res;
    }
};
