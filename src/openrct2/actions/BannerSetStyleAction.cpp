/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "BannerSetStyleAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../management/Finance.h"
#include "../windows/Intent.h"
#include "../world/Banner.h"
#include "../world/tile_element/BannerElement.h"
#include "GameAction.h"

using namespace OpenRCT2;

BannerSetStyleAction::BannerSetStyleAction(BannerSetStyleType type, BannerIndex bannerIndex, uint8_t parameter)
    : _type(type)
    , _bannerIndex(bannerIndex)
    , _parameter(parameter)
{
}

void BannerSetStyleAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("id", _bannerIndex);
    visitor.Visit("type", _type);
    visitor.Visit("parameter", _parameter);
}

uint16_t BannerSetStyleAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void BannerSetStyleAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_type) << DS_TAG(_bannerIndex) << DS_TAG(_parameter);
}

GameActions::Result BannerSetStyleAction::Query() const
{
    auto res = GameActions::Result();

    auto banner = GetBanner(_bannerIndex);
    if (banner == nullptr)
    {
        LOG_ERROR("Banner not found for bannerIndex %d", _bannerIndex);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_BANNER_ELEMENT_NOT_FOUND);
    }

    res.Expenditure = ExpenditureType::Landscaping;
    auto location = banner->position.ToCoordsXY().ToTileCentre();
    res.Position = { location, TileElementHeight(location) };

    TileElement* tileElement = BannerGetTileElement(_bannerIndex);

    if (tileElement == nullptr)
    {
        LOG_ERROR("Banner tile element not found for bannerIndex %d", _bannerIndex);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_BANNER_ELEMENT_NOT_FOUND);
    }

    switch (_type)
    {
        case BannerSetStyleType::PrimaryColour:
            if (_parameter > COLOUR_COUNT)
            {
                LOG_ERROR("Invalid primary colour %u", _parameter);
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_INVALID_COLOUR);
            }
            break;

        case BannerSetStyleType::TextColour:
            if (_parameter > 13)
            {
                LOG_ERROR("Invalid text colour %u", _parameter);
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_INVALID_COLOUR);
            }
            break;
        case BannerSetStyleType::NoEntry:
            if (tileElement->AsBanner() == nullptr)
            {
                LOG_ERROR("Tile element was not a banner.");
                return GameActions::Result(GameActions::Status::Unknown, STR_CANT_REPAINT_THIS, STR_NONE);
            }
            break;
        default:
            LOG_ERROR("Invalid banner style type %u", _type);
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_VALUE_OUT_OF_RANGE);
    }
    return res;
}

GameActions::Result BannerSetStyleAction::Execute() const
{
    auto res = GameActions::Result();

    auto banner = GetBanner(_bannerIndex);
    if (banner == nullptr)
    {
        LOG_ERROR("Banner not found for bannerIndex %d", _bannerIndex);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_BANNER_ELEMENT_NOT_FOUND);
    }

    res.Expenditure = ExpenditureType::Landscaping;
    auto location = banner->position.ToCoordsXY().ToTileCentre();
    res.Position = { location, TileElementHeight(location) };

    TileElement* tileElement = BannerGetTileElement(_bannerIndex);

    if (tileElement == nullptr)
    {
        LOG_ERROR("Banner tile element not found for bannerIndex &u", _bannerIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_NONE);
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
                LOG_ERROR("Tile element was not a banner.");
                return GameActions::Result(
                    GameActions::Status::Unknown, STR_CANT_REPAINT_THIS, STR_ERR_BANNER_ELEMENT_NOT_FOUND);
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
            LOG_ERROR("Invalid banner style type %u", _type);
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_VALUE_OUT_OF_RANGE);
    }

    auto intent = Intent(INTENT_ACTION_UPDATE_BANNER);
    intent.PutExtra(INTENT_EXTRA_BANNER_INDEX, _bannerIndex);
    ContextBroadcastIntent(&intent);

    ScrollingTextInvalidate();
    GfxInvalidateScreen();

    return res;
}
