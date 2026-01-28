/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include "../world/Map.h"
#include "../world/Scenery.h"
#include "../world/tile_element/LargeSceneryElement.h"
#include "../world/tile_element/WallElement.h"

namespace OpenRCT2::GameActions
{
    SignSetStyleAction::SignSetStyleAction(
        BannerIndex bannerIndex, Drawing::Colour mainColour, Drawing::Colour textColour, bool isLarge)
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
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void SignSetStyleAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_bannerIndex) << DS_TAG(_mainColour) << DS_TAG(_textColour) << DS_TAG(_isLarge);
    }

    Result SignSetStyleAction::Query(GameState_t& gameState) const
    {
        auto banner = GetBanner(_bannerIndex);
        if (banner == nullptr)
        {
            LOG_ERROR("Banner not found for bannerIndex %u", _bannerIndex);
            return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, kStringIdNone);
        }

        CoordsXYZ loc;

        if (_isLarge)
        {
            TileElement* tileElement = BannerGetTileElement(_bannerIndex);
            if (tileElement == nullptr)
            {
                LOG_ERROR("Banner tile element not found for bannerIndex %u", _bannerIndex);
                return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, kStringIdNone);
            }
            if (tileElement->GetType() != TileElementType::LargeScenery)
            {
                LOG_ERROR(
                    "Tile element has type %u, expected %d (LargeScenery)", tileElement->GetType(),
                    TileElementType::LargeScenery);
                return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, kStringIdNone);
            }
            loc = { banner->position.ToCoordsXY(), tileElement->GetBaseZ() };
        }
        else
        {
            WallElement* wallElement = BannerGetScrollingWallTileElement(_bannerIndex);

            if (wallElement == nullptr)
            {
                LOG_ERROR("Wall element not found for bannerIndex", _bannerIndex);
                return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, kStringIdNone);
            }
            loc = { banner->position.ToCoordsXY(), wallElement->GetBaseZ() };
        }

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

    Result SignSetStyleAction::Execute(GameState_t& gameState) const
    {
        auto banner = GetBanner(_bannerIndex);
        if (banner == nullptr)
        {
            LOG_ERROR("Invalid banner id %u", _bannerIndex);
            return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, kStringIdNone);
        }

        CoordsXY coords = banner->position.ToCoordsXY();

        if (_isLarge)
        {
            TileElement* tileElement = BannerGetTileElement(_bannerIndex);
            if (!MapLargeScenerySignSetColour(
                    { coords, tileElement->GetBaseZ(), tileElement->GetDirection() },
                    tileElement->AsLargeScenery()->GetSequenceIndex(), _mainColour, _textColour))
            {
                return Result(Status::unknown, STR_CANT_REPAINT_THIS, kStringIdNone);
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

        return Result();
    }
} // namespace OpenRCT2::GameActions
