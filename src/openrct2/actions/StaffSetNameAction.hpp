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
#include "../peep/Staff.h"
#include "../windows/Intent.h"
#include "../world/Park.h"
#include "../world/Sprite.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(StaffSetNameAction, GAME_COMMAND_SET_STAFF_NAME, GameActionResult)
{
private:
    uint16_t _spriteIndex;
    std::string _name;

public:
    StaffSetNameAction()
    {
    }
    StaffSetNameAction(uint16_t spriteIndex, const std::string& name)
        : _spriteIndex(spriteIndex)
        , _name(name)
    {
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
            return std::make_unique<GameActionResult>(
                GA_ERROR::INVALID_PARAMETERS, STR_STAFF_ERROR_CANT_NAME_STAFF_MEMBER, STR_NONE);
        }

        auto staff = TryGetEntity<Staff>(_spriteIndex);
        if (staff == nullptr)
        {
            log_warning("Invalid game command for sprite %u", _spriteIndex);
            return std::make_unique<GameActionResult>(
                GA_ERROR::INVALID_PARAMETERS, STR_STAFF_ERROR_CANT_NAME_STAFF_MEMBER, STR_NONE);
        }

        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        auto staff = TryGetEntity<Staff>(_spriteIndex);
        if (staff == nullptr)
        {
            log_warning("Invalid game command for sprite %u", _spriteIndex);
            return std::make_unique<GameActionResult>(
                GA_ERROR::INVALID_PARAMETERS, STR_STAFF_ERROR_CANT_NAME_STAFF_MEMBER, STR_NONE);
        }

        auto curName = staff->GetName();
        if (curName == _name)
        {
            return std::make_unique<GameActionResult>(GA_ERROR::OK, STR_NONE);
        }

        if (!staff->SetName(_name))
        {
            return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_CANT_NAME_GUEST, STR_NONE);
        }

        gfx_invalidate_screen();

        auto intent = Intent(INTENT_ACTION_REFRESH_STAFF_LIST);
        context_broadcast_intent(&intent);

        auto res = std::make_unique<GameActionResult>();
        res->Position.x = staff->x;
        res->Position.y = staff->y;
        res->Position.z = staff->z;
        return res;
    }
};
