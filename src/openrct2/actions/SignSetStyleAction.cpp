/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SignSetStyleAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../localisation/StringIds.h"
#include "../windows/Intent.h"
#include "../world/Banner.h"
#include "../world/Scenery.h"
#include "../world/tile_element/LargeSceneryElement.h"
#include "../world/tile_element/WallElement.h"

using namespace OpenRCT2;

SignSetStyleAction::SignSetStyleAction(BannerIndex bannerIndex, uint8_t mainColour, uint8_t textColour, bool isLarge)
    : _bannerIndex(bannerIndex)
    , _mainColour(mainColour)
    , _textColour(textColour)
    , _isLarge(isLarge)
{
}

void SignSetStyleAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("id", _bannerIndex);
    visitor.Visit("mainColour", _mainColour);
    visitor.Visit("textColour", _textColour);
    visitor.Visit("isLarge", _isLarge);
}

uint16_t SignSetStyleAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void SignSetStyleAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_bannerIndex) << DS_TAG(_mainColour) << DS_TAG(_textColour) << DS_TAG(_isLarge);
}

GameActions::Result SignSetStyleAction::Query() const
{
    auto banner = GetBanner(_bannerIndex);
    if (banner == nullptr)
    {
        LOG_ERROR("Banner not found for bannerIndex %u", _bannerIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, kStringIdNone);
    }

    if (_isLarge)
    {
        TileElement* tileElement = BannerGetTileElement(_bannerIndex);
        if (tileElement == nullptr)
        {
            LOG_ERROR("Banner tile element not found for bannerIndex %u", _bannerIndex);
            return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, kStringIdNone);
        }
        if (tileElement->GetType() != TileElementType::LargeScenery)
        {
            LOG_ERROR(
                "Tile element has type %u, expected %d (LargeScenery)", tileElement->GetType(), TileElementType::LargeScenery);
            return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, kStringIdNone);
        }
    }
    else
    {
        WallElement* wallElement = BannerGetScrollingWallTileElement(_bannerIndex);

        if (wallElement == nullptr)
        {
            LOG_ERROR("Wall element not found for bannerIndex", _bannerIndex);
            return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, kStringIdNone);
        }
    }

    return GameActions::Result();
}

GameActions::Result SignSetStyleAction::Execute() const
{
    auto banner = GetBanner(_bannerIndex);
    if (banner == nullptr)
    {
        LOG_ERROR("Invalid banner id %u", _bannerIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, kStringIdNone);
    }

    CoordsXY coords = banner->position.ToCoordsXY();

    if (_isLarge)
    {
        TileElement* tileElement = BannerGetTileElement(_bannerIndex);
        if (!MapLargeScenerySignSetColour(
                { coords, tileElement->GetBaseZ(), tileElement->GetDirection() },
                tileElement->AsLargeScenery()->GetSequenceIndex(), _mainColour, _textColour))
        {
            return GameActions::Result(GameActions::Status::Unknown, STR_CANT_REPAINT_THIS, kStringIdNone);
        }
    }
    else
    {
        WallElement* wallElement = BannerGetScrollingWallTileElement(_bannerIndex);

        wallElement->SetPrimaryColour(_mainColour);
        wallElement->SetSecondaryColour(_textColour);
        MapInvalidateTile({ coords, wallElement->GetBaseZ(), wallElement->GetClearanceZ() });
    }

    auto intent = Intent(INTENT_ACTION_UPDATE_BANNER);
    intent.PutExtra(INTENT_EXTRA_BANNER_INDEX, _bannerIndex);
    ContextBroadcastIntent(&intent);

    return GameActions::Result();
}
