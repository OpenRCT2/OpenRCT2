/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "BannerSetNameAction.h"

#include "../Context.h"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../ui/UiContext.h"
#include "../windows/Intent.h"
#include "../world/Banner.h"
#include "GameAction.h"

BannerSetNameAction::BannerSetNameAction(BannerIndex bannerIndex, std::string_view name)
    : _bannerIndex(bannerIndex)
    , _name(name)
{
}

void BannerSetNameAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("id", _bannerIndex);
    visitor.Visit("name", _name);
}

uint16_t BannerSetNameAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void BannerSetNameAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_bannerIndex) << DS_TAG(_name);
}

GameActions::Result BannerSetNameAction::Query() const
{
    auto banner = GetBanner(_bannerIndex);
    if (banner == nullptr)
    {
        log_warning("Invalid banner id, banner id = %d", _bannerIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_RENAME_BANNER, STR_NONE);
    }
    return GameActions::Result();
}

GameActions::Result BannerSetNameAction::Execute() const
{
    auto banner = GetBanner(_bannerIndex);
    if (banner == nullptr)
    {
        log_warning("Invalid banner id, banner id = %d", _bannerIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_RENAME_BANNER, STR_NONE);
    }

    banner->text = _name;

    auto intent = Intent(INTENT_ACTION_UPDATE_BANNER);
    intent.putExtra(INTENT_EXTRA_BANNER_INDEX, _bannerIndex);
    context_broadcast_intent(&intent);

    scrolling_text_invalidate();
    gfx_invalidate_screen();

    return GameActions::Result();
}
