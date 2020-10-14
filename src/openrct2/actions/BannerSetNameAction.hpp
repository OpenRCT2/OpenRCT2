/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#include "../windows/Intent.h"
#include "../world/Banner.h"
#include "../world/Sprite.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(BannerSetNameAction, GAME_COMMAND_SET_BANNER_NAME, GameActions::Result)
{
private:
    BannerIndex _bannerIndex{ BANNER_INDEX_NULL };
    std::string _name;

public:
    BannerSetNameAction() = default;
    BannerSetNameAction(BannerIndex bannerIndex, const std::string& name)
        : _bannerIndex(bannerIndex)
        , _name(name)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_bannerIndex) << DS_TAG(_name);
    }

    GameActions::Result::Ptr Query() const override
    {
        if (_bannerIndex >= MAX_BANNERS)
        {
            log_warning("Invalid game command for setting banner name, banner id = %d", _bannerIndex);
            return MakeResult(GameActions::Status::InvalidParameters, STR_NONE);
        }
        return MakeResult();
    }

    GameActions::Result::Ptr Execute() const override
    {
        auto banner = GetBanner(_bannerIndex);
        banner->text = _name;

        auto intent = Intent(INTENT_ACTION_UPDATE_BANNER);
        intent.putExtra(INTENT_EXTRA_BANNER_INDEX, _bannerIndex);
        context_broadcast_intent(&intent);

        scrolling_text_invalidate();
        gfx_invalidate_screen();

        return MakeResult();
    }
};
