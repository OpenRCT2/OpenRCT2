/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "BannerSetNameAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../drawing/ScrollingText.h"
#include "../localisation/StringIds.h"
#include "../windows/Intent.h"
#include "../world/Banner.h"
#include "../world/Map.h"
#include "../world/tile_element/BannerElement.h"
#include "GameAction.h"

namespace OpenRCT2::GameActions
{
    BannerSetNameAction::BannerSetNameAction(BannerIndex bannerIndex, const std::string& name)
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
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void BannerSetNameAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_bannerIndex) << DS_TAG(_name);
    }

    Result BannerSetNameAction::Query(GameState_t& gameState) const
    {
        auto banner = GetBanner(_bannerIndex);
        if (banner == nullptr)
        {
            LOG_ERROR("Banner not found for bannerIndex %d", _bannerIndex);
            return Result(Status::invalidParameters, STR_CANT_RENAME_BANNER, STR_ERR_BANNER_ELEMENT_NOT_FOUND);
        }

        TileElement* tileElement = BannerGetTileElement(_bannerIndex);

        if (tileElement == nullptr)
        {
            LOG_ERROR("Banner tile element not found for bannerIndex %d", _bannerIndex);
            return Result(Status::invalidParameters, STR_CANT_RENAME_BANNER, STR_ERR_BANNER_ELEMENT_NOT_FOUND);
        }

        BannerElement* bannerElement = tileElement->AsBanner();
        CoordsXYZD loc = { banner->position.ToCoordsXY(), bannerElement->GetBaseZ(), bannerElement->GetPosition() };

        if (!LocationValid(loc))
        {
            return Result(Status::invalidParameters, STR_CANT_RENAME_BANNER, STR_OFF_EDGE_OF_MAP);
        }
        if (!MapCanBuildAt({ loc.x, loc.y, loc.z - 16 }))
        {
            return Result(Status::notOwned, STR_CANT_RENAME_BANNER, STR_LAND_NOT_OWNED_BY_PARK);
        }

        return Result();
    }

    Result BannerSetNameAction::Execute(GameState_t& gameState) const
    {
        auto banner = GetBanner(_bannerIndex);
        if (banner == nullptr)
        {
            LOG_ERROR("Banner not found for bannerIndex %d", _bannerIndex);
            return Result(Status::invalidParameters, STR_CANT_RENAME_BANNER, STR_ERR_BANNER_ELEMENT_NOT_FOUND);
        }

        banner->text = _name;

        auto intent = Intent(INTENT_ACTION_UPDATE_BANNER);
        intent.PutExtra(INTENT_EXTRA_BANNER_INDEX, _bannerIndex);
        ContextBroadcastIntent(&intent);

        Drawing::ScrollingText::invalidate();
        GfxInvalidateScreen();

        return Result();
    }
} // namespace OpenRCT2::GameActions
