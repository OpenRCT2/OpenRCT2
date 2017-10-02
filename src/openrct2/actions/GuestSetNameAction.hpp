#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include "../core/MemoryStream.h"
#include "../localisation/string_ids.h"
#include "GameAction.h"

#include "../cheats.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../world/park.h"

struct GuestSetNameAction : public GameActionBase<GAME_COMMAND_SET_GUEST_NAME, GameActionResult>
{
private:
    uint16 _spriteIndex;
    std::string _name;

public:
    GuestSetNameAction() {}
    GuestSetNameAction(uint16 spriteIndex, const std::string& name)
        : _spriteIndex(spriteIndex),
        _name(name)
    {
    }

    uint16 GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);

        stream << _spriteIndex << _name;
    }

    GameActionResult::Ptr Query() const override
    {
        if (_spriteIndex >= MAX_SPRITES)
        {
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        if (_name.empty())
        {
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_ERR_INVALID_NAME_FOR_GUEST);
        }

        rct_peep *peep = GET_PEEP(_spriteIndex);
        if (peep->type != PEEP_TYPE_GUEST)
        {
            log_warning("Invalid game command for sprite %u", _spriteIndex);
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        rct_string_id newUserStringId = user_string_allocate(USER_STRING_HIGH_ID_NUMBER | USER_STRING_DUPLICATION_PERMITTED, _name.c_str());
        if (newUserStringId == 0)
        {
            // TODO: Probably exhausted, introduce new error.
            return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);
        }
        user_string_free(newUserStringId);

        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        rct_string_id newUserStringId = user_string_allocate(USER_STRING_HIGH_ID_NUMBER | USER_STRING_DUPLICATION_PERMITTED, _name.c_str());

        rct_peep *peep = GET_PEEP(_spriteIndex);
        if (peep->type != PEEP_TYPE_GUEST)
        {
            log_warning("Invalid game command for sprite %u", _spriteIndex);
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        set_format_arg(0, uint32, peep->id);
        utf8* curName = gCommonStringFormatBuffer;
        rct_string_id curId = peep->name_string_idx;
        format_string(curName, 256, curId, gCommonFormatArgs);

        if (strcmp(curName, _name.c_str()) == 0)
        {
            return std::make_unique<GameActionResult>(GA_ERROR::OK, STR_NONE);
        }

        user_string_free(peep->name_string_idx);
        peep->name_string_idx = newUserStringId;

        peep_update_name_sort(peep);

        peep_handle_easteregg_name(peep);

        gfx_invalidate_screen();

        // Force guest list window refresh
        rct_window *w = window_find_by_class(WC_GUEST_LIST);
        if (w != NULL)
        {
            w->no_list_items = 0;
        }

        auto res = std::make_unique<GameActionResult>();
        res->Position.x = peep->x;
        res->Position.y = peep->y;
        res->Position.z = peep->z;
        return res;
    }
};
