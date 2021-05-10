/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SignSetNameAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../common.h"
#include "../drawing/Drawing.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../ride/Ride.h"
#include "../world/Banner.h"

#include <string>

SignSetNameAction::SignSetNameAction(BannerIndex bannerIndex, const std::string& name)
    : _bannerIndex(bannerIndex)
    , _name(name)
{
}

uint16_t SignSetNameAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void SignSetNameAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_bannerIndex) << DS_TAG(_name);
}

GameActions::Result::Ptr SignSetNameAction::Query() const
{
    auto banner = GetBanner(_bannerIndex);
    if (banner == nullptr)
    {
        log_warning("Invalid game command for setting sign name, banner id = %d", _bannerIndex);
        return MakeResult(GameActions::Status::InvalidParameters, STR_NONE);
    }
    return MakeResult();
}

GameActions::Result::Ptr SignSetNameAction::Execute() const
{
    auto banner = GetBanner(_bannerIndex);
    if (banner == nullptr)
    {
        log_warning("Invalid game command for setting sign name, banner id = %d", _bannerIndex);
        return MakeResult(GameActions::Status::InvalidParameters, STR_NONE);
    }

    if (!_name.empty())
    {
        banner->flags &= ~BANNER_FLAG_LINKED_TO_RIDE;
        banner->ride_index = RIDE_ID_NULL;
        banner->text = _name;
    }
    else
    {
        // If empty name take closest ride name.
        ride_id_t rideIndex = banner_get_closest_ride_index({ banner->position.ToCoordsXY(), 16 });
        if (rideIndex == RIDE_ID_NULL)
        {
            banner->flags &= ~BANNER_FLAG_LINKED_TO_RIDE;
            banner->ride_index = RIDE_ID_NULL;
            banner->text = {};
        }
        else
        {
            banner->flags |= BANNER_FLAG_LINKED_TO_RIDE;
            banner->ride_index = rideIndex;
            banner->text = {};
        }
    }

    scrolling_text_invalidate();
    gfx_invalidate_screen();
    return MakeResult();
}
