/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "BannerSetColourAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../management/Finance.h"
#include "../windows/Intent.h"
#include "../world/Banner.h"
#include "../world/tile_element/BannerElement.h"
#include "GameAction.h"

using namespace OpenRCT2;

BannerSetColourAction::BannerSetColourAction(const CoordsXYZD& loc, uint8_t primaryColour)
    : _loc(loc)
    , _primaryColour(primaryColour)
{
}

void BannerSetColourAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_loc);
    visitor.Visit("primaryColour", _primaryColour);
}

uint16_t BannerSetColourAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void BannerSetColourAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_loc) << DS_TAG(_primaryColour);
}

GameActions::Result BannerSetColourAction::Query() const
{
    return QueryExecute(false);
}

GameActions::Result BannerSetColourAction::Execute() const
{
    return QueryExecute(true);
}

GameActions::Result BannerSetColourAction::QueryExecute(bool isExecuting) const
{
    auto res = GameActions::Result();
    res.Expenditure = ExpenditureType::Landscaping;
    res.Position.x = _loc.x + 16;
    res.Position.y = _loc.y + 16;
    res.Position.z = _loc.z;
    res.ErrorTitle = STR_CANT_REPAINT_THIS;

    if (!LocationValid(_loc))
    {
        LOG_ERROR("Invalid x / y coordinates: x = %d, y = %d", _loc.x, _loc.y);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_OFF_EDGE_OF_MAP);
    }

    if (_primaryColour > 31)
    {
        LOG_ERROR("Invalid primary colour %u", _primaryColour);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_INVALID_COLOUR);
    }

    if (!MapCanBuildAt({ _loc.x, _loc.y, _loc.z - 16 }))
    {
        return GameActions::Result(GameActions::Status::NotOwned, STR_CANT_REPAINT_THIS, STR_LAND_NOT_OWNED_BY_PARK);
    }

    auto bannerElement = MapGetBannerElementAt(_loc, _loc.direction);

    if (bannerElement == nullptr)
    {
        LOG_ERROR("No banner at x = %d, y = %d, z = %d, direction = %u", _loc.x, _loc.y, _loc.z, _loc.direction);
        return GameActions::Result(GameActions::Status::Unknown, STR_CANT_REPAINT_THIS, STR_ERR_BANNER_ELEMENT_NOT_FOUND);
    }

    auto index = bannerElement->GetIndex();
    auto banner = GetBanner(index);
    if (banner == nullptr)
    {
        LOG_ERROR("Invalid banner index %u", index);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_NONE);
    }

    if (isExecuting)
    {
        auto intent = Intent(INTENT_ACTION_UPDATE_BANNER);
        intent.PutExtra(INTENT_EXTRA_BANNER_INDEX, index);
        ContextBroadcastIntent(&intent);

        banner->colour = _primaryColour;
        MapInvalidateTileZoom1({ _loc, _loc.z, _loc.z + 32 });
    }

    return res;
}
