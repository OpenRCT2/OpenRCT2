/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "BannerSetStyleAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../drawing/Drawing.h"
#include "../drawing/ScrollingText.h"
#include "../localisation/StringIdType.h"
#include "../management/Finance.h"
#include "../windows/Intent.h"
#include "../world/Banner.h"
#include "../world/Map.h"
#include "../world/tile_element/BannerElement.h"
#include "GameAction.h"

namespace OpenRCT2::GameActions
{
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
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void BannerSetStyleAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_type) << DS_TAG(_bannerIndex) << DS_TAG(_parameter);
    }

    Result BannerSetStyleAction::Query(GameState_t& gameState) const
    {
        StringId errorTitle = STR_CANT_REPAINT_THIS;
        if (_type == BannerSetStyleType::NoEntry)
        {
            errorTitle = STR_CANT_RENAME_BANNER;
        }

        auto res = Result();

        auto banner = GetBanner(_bannerIndex);
        if (banner == nullptr)
        {
            LOG_ERROR("Banner not found for bannerIndex %d", _bannerIndex);
            return Result(Status::invalidParameters, errorTitle, STR_ERR_BANNER_ELEMENT_NOT_FOUND);
        }

        res.expenditure = ExpenditureType::landscaping;
        auto location = banner->position.ToCoordsXY().ToTileCentre();
        res.position = { location, TileElementHeight(location) };

        TileElement* tileElement = BannerGetTileElement(_bannerIndex);

        if (tileElement == nullptr)
        {
            LOG_ERROR("Banner tile element not found for bannerIndex %d", _bannerIndex);
            return Result(Status::invalidParameters, errorTitle, STR_ERR_BANNER_ELEMENT_NOT_FOUND);
        }

        BannerElement* bannerElement = tileElement->AsBanner();
        CoordsXYZ loc = { banner->position.ToCoordsXY(), bannerElement->GetBaseZ() };

        if (!LocationValid(loc))
        {
            return Result(Status::invalidParameters, errorTitle, STR_OFF_EDGE_OF_MAP);
        }
        if (!MapCanBuildAt({ loc.x, loc.y, loc.z - 16 }))
        {
            return Result(Status::notOwned, errorTitle, STR_LAND_NOT_OWNED_BY_PARK);
        }

        switch (_type)
        {
            case BannerSetStyleType::PrimaryColour:
                if (_parameter > Drawing::kColourNumTotal)
                {
                    LOG_ERROR("Invalid primary colour %u", _parameter);
                    return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_INVALID_COLOUR);
                }
                break;

            case BannerSetStyleType::TextColour:
                if (_parameter > 13)
                {
                    LOG_ERROR("Invalid text colour %u", _parameter);
                    return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_INVALID_COLOUR);
                }
                break;
            case BannerSetStyleType::NoEntry:
                if (tileElement->AsBanner() == nullptr)
                {
                    LOG_ERROR("Tile element was not a banner.");
                    return Result(Status::unknown, STR_CANT_RENAME_BANNER, kStringIdNone);
                }
                break;
            default:
                LOG_ERROR("Invalid banner style type %u", _type);
                return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_VALUE_OUT_OF_RANGE);
        }
        return res;
    }

    Result BannerSetStyleAction::Execute(GameState_t& gameState) const
    {
        auto res = Result();

        auto banner = GetBanner(_bannerIndex);
        if (banner == nullptr)
        {
            LOG_ERROR("Banner not found for bannerIndex %d", _bannerIndex);
            return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_BANNER_ELEMENT_NOT_FOUND);
        }

        res.expenditure = ExpenditureType::landscaping;
        auto location = banner->position.ToCoordsXY().ToTileCentre();
        res.position = { location, TileElementHeight(location) };

        TileElement* tileElement = BannerGetTileElement(_bannerIndex);

        if (tileElement == nullptr)
        {
            LOG_ERROR("Banner tile element not found for bannerIndex &u", _bannerIndex);
            return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, kStringIdNone);
        }

        switch (_type)
        {
            case BannerSetStyleType::PrimaryColour:
                banner->colour = static_cast<Drawing::Colour>(_parameter);
                break;
            case BannerSetStyleType::TextColour:
                banner->textColour = static_cast<Drawing::TextColour>(_parameter);
                break;
            case BannerSetStyleType::NoEntry:
            {
                BannerElement* bannerElement = tileElement->AsBanner();
                if (bannerElement == nullptr)
                {
                    LOG_ERROR("Tile element was not a banner.");
                    return Result(Status::unknown, STR_CANT_REPAINT_THIS, STR_ERR_BANNER_ELEMENT_NOT_FOUND);
                }

                banner->flags.set(BannerFlag::noEntry, (_parameter != 0));
                uint8_t allowedEdges = 0xF;
                if (banner->flags.has(BannerFlag::noEntry))
                {
                    allowedEdges &= ~(1 << bannerElement->GetPosition());
                }
                bannerElement->SetAllowedEdges(allowedEdges);
                break;
            }
            default:
                LOG_ERROR("Invalid banner style type %u", _type);
                return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        auto intent = Intent(INTENT_ACTION_UPDATE_BANNER);
        intent.PutExtra(INTENT_EXTRA_BANNER_INDEX, _bannerIndex);
        ContextBroadcastIntent(&intent);

        Drawing::ScrollingText::invalidate();
        GfxInvalidateScreen();

        return res;
    }
} // namespace OpenRCT2::GameActions
