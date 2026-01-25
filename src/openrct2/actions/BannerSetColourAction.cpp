/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include "../world/Map.h"
#include "../world/tile_element/BannerElement.h"
#include "GameAction.h"

namespace OpenRCT2::GameActions
{
    BannerSetColourAction::BannerSetColourAction(const CoordsXYZD& loc, Drawing::Colour primaryColour)
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
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void BannerSetColourAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc) << DS_TAG(_primaryColour);
    }

    Result BannerSetColourAction::Query(GameState_t& gameState) const
    {
        return QueryExecute(false);
    }

    Result BannerSetColourAction::Execute(GameState_t& gameState) const
    {
        return QueryExecute(true);
    }

    Result BannerSetColourAction::QueryExecute(bool isExecuting) const
    {
        auto res = Result();
        res.expenditure = ExpenditureType::landscaping;
        res.position.x = _loc.x + 16;
        res.position.y = _loc.y + 16;
        res.position.z = _loc.z;
        res.errorTitle = STR_CANT_REPAINT_THIS;

        if (!LocationValid(_loc))
        {
            LOG_ERROR("Invalid x / y coordinates: x = %d, y = %d", _loc.x, _loc.y);
            return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, STR_OFF_EDGE_OF_MAP);
        }

        if (!Drawing::colourIsValid(_primaryColour))
        {
            LOG_ERROR("Invalid primary colour %u", _primaryColour);
            return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_INVALID_COLOUR);
        }

        if (!MapCanBuildAt({ _loc.x, _loc.y, _loc.z - 16 }))
        {
            return Result(Status::notOwned, STR_CANT_REPAINT_THIS, STR_LAND_NOT_OWNED_BY_PARK);
        }

        auto bannerElement = MapGetBannerElementAt(_loc, _loc.direction);

        if (bannerElement == nullptr)
        {
            LOG_ERROR("No banner at x = %d, y = %d, z = %d, direction = %u", _loc.x, _loc.y, _loc.z, _loc.direction);
            return Result(Status::unknown, STR_CANT_REPAINT_THIS, STR_ERR_BANNER_ELEMENT_NOT_FOUND);
        }

        auto index = bannerElement->GetIndex();
        auto banner = GetBanner(index);
        if (banner == nullptr)
        {
            LOG_ERROR("Invalid banner index %u", index);
            return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, kStringIdNone);
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
} // namespace OpenRCT2::GameActions
