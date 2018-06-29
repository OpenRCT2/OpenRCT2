/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Context.h"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../ui/UiContext.h"
#include "../world/Sprite.h"
#include "../world/Banner.h"
#include "../windows/Intent.h"
#include "GameAction.h"

struct BannerSetNameAction : public GameActionBase<GAME_COMMAND_SET_BANNER_NAME, GameActionResult>
{
private:
    BannerIndex _bannerIndex;
    std::string _name;

public:
    BannerSetNameAction() {}
    BannerSetNameAction(BannerIndex bannerIndex, const std::string& name)
        : _bannerIndex(bannerIndex),
        _name(name)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);
        stream << _bannerIndex << _name;
    }

    GameActionResult::Ptr Query() const override
    {
        if (_bannerIndex >= MAX_BANNERS)
        {
            log_warning("Invalid game command for setting banner name, banner id = %d", _bannerIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        // Ensure user string space.
        rct_string_id string_id = user_string_allocate(USER_STRING_DUPLICATION_PERMITTED, _name.c_str());
        if (string_id != 0)
        {
            user_string_free(string_id);
        }
        else
        {
            return MakeResult(GA_ERROR::NO_FREE_ELEMENTS, STR_ERR_CANT_SET_BANNER_TEXT);
        }

        return MakeResult();
    }

    GameActionResult::Ptr Execute() const override
    {
        rct_banner* banner = &gBanners[_bannerIndex];

        utf8 *buffer = gCommonStringFormatBuffer;
        utf8 *dst = buffer;
        dst = utf8_write_codepoint(dst, FORMAT_COLOUR_CODE_START + banner->text_colour);
        String::Set(dst, sizeof(gCommonStringFormatBuffer) - (dst - buffer), _name.c_str(), _name.size());

        rct_string_id string_id = user_string_allocate(USER_STRING_DUPLICATION_PERMITTED, buffer);
        if (string_id == 0)
        {
            return MakeResult(GA_ERROR::NO_FREE_ELEMENTS, STR_ERR_CANT_SET_BANNER_TEXT);
        }

        rct_string_id prev_string_id = banner->string_idx;
        banner->string_idx = string_id;
        user_string_free(prev_string_id);

        auto intent = Intent(INTENT_ACTION_UPDATE_BANNER);
        intent.putExtra(INTENT_EXTRA_BANNER_INDEX, _bannerIndex);
        context_broadcast_intent(&intent);

        return MakeResult();
    }
};
