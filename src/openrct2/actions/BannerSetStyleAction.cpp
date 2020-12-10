/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "BannerSetStyleAction.h"

#include "../Context.h"
#include "../management/Finance.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Banner.h"
#include "GameAction.h"

void BannerSetStyleAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("id", _bannerIndex);
    visitor.Visit("type", _type);
    visitor.Visit("parameter", _parameter);
}

void BannerSetStyleAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_type) << DS_TAG(_bannerIndex) << DS_TAG(_parameter);
}

GameActions::Result::Ptr BannerSetStyleAction::Query() const
{
    auto res = MakeResult();

    if (_bannerIndex >= MAX_BANNERS || _bannerIndex == BANNER_INDEX_NULL)
    {
        return MakeResult(GameActions::Status::InvalidParameters, STR_INVALID_SELECTION_OF_OBJECTS);
    }

    auto banner = GetBanner(_bannerIndex);

    res->Expenditure = ExpenditureType::Landscaping;
    auto location = banner->position.ToCoordsXY().ToTileCentre();
    res->Position = { location, tile_element_height(location) };

    TileElement* tileElement = banner_get_tile_element(_bannerIndex);

    if (tileElement == nullptr)
    {
        log_error("Could not find banner index = %u", _bannerIndex);
        return MakeResult(GameActions::Status::InvalidParameters, STR_NONE);
    }

    switch (_type)
    {
        case BannerSetStyleType::PrimaryColour:
            if (_parameter > 31)
            {
                log_error("Invalid primary colour: colour = %u", _parameter);
                return MakeResult(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS);
            }
            break;

        case BannerSetStyleType::TextColour:
            if (_parameter > 13)
            {
                log_error("Invalid text colour: colour = %u", _parameter);
                return MakeResult(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS);
            }
            break;
        case BannerSetStyleType::NoEntry:
            if (tileElement->AsBanner() == nullptr)
            {
                log_error("Tile element was not a banner.");
                return MakeResult(GameActions::Status::Unknown, STR_NONE);
            }
            break;
        default:
            log_error("Invalid type: %u", _type);
            return MakeResult(GameActions::Status::InvalidParameters, STR_NONE);
    }
    return res;
}

GameActions::Result::Ptr BannerSetStyleAction::Execute() const
{
    auto res = MakeResult();

    auto banner = GetBanner(_bannerIndex);

    res->Expenditure = ExpenditureType::Landscaping;
    auto location = banner->position.ToCoordsXY().ToTileCentre();
    res->Position = { location, tile_element_height(location) };

    TileElement* tileElement = banner_get_tile_element(_bannerIndex);

    if (tileElement == nullptr)
    {
        log_error("Could not find banner index = %u", _bannerIndex);
        return MakeResult(GameActions::Status::InvalidParameters, STR_NONE);
    }

    switch (_type)
    {
        case BannerSetStyleType::PrimaryColour:
            banner->colour = _parameter;
            break;
        case BannerSetStyleType::TextColour:
            banner->text_colour = _parameter;
            break;
        case BannerSetStyleType::NoEntry:
        {
            BannerElement* bannerElement = tileElement->AsBanner();
            if (bannerElement == nullptr)
            {
                log_error("Tile element was not a banner.");
                return MakeResult(GameActions::Status::Unknown, STR_NONE);
            }

            banner->flags &= ~BANNER_FLAG_NO_ENTRY;
            banner->flags |= (_parameter != 0) ? BANNER_FLAG_NO_ENTRY : 0;
            uint8_t allowedEdges = 0xF;
            if (banner->flags & BANNER_FLAG_NO_ENTRY)
            {
                allowedEdges &= ~(1 << bannerElement->GetPosition());
            }
            bannerElement->SetAllowedEdges(allowedEdges);
            break;
        }
        default:
            log_error("Invalid type: %u", _type);
            return MakeResult(GameActions::Status::InvalidParameters, STR_NONE);
    }

    auto intent = Intent(INTENT_ACTION_UPDATE_BANNER);
    intent.putExtra(INTENT_EXTRA_BANNER_INDEX, _bannerIndex);
    context_broadcast_intent(&intent);

    return res;
}
