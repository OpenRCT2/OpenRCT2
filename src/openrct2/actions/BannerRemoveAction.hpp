/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../management/Finance.h"
#include "../world/Banner.h"
#include "../world/MapAnimation.h"
#include "../world/Scenery.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(BannerRemoveAction, GAME_COMMAND_REMOVE_BANNER, GameActionResult)
{
private:
    CoordsXYZD _loc;

public:
    BannerRemoveAction() = default;
    BannerRemoveAction(const CoordsXYZD& loc)
        : _loc(loc)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc);
    }

    GameActionResult::Ptr Query() const override
    {
        auto res = MakeResult();
        res->Expenditure = ExpenditureType::Landscaping;
        res->Position.x = _loc.x + 16;
        res->Position.y = _loc.y + 16;
        res->Position.z = _loc.z;
        res->ErrorTitle = STR_CANT_REMOVE_THIS;

        if (!LocationValid(_loc) || !map_can_build_at({ _loc.x, _loc.y, _loc.z - 16 }))
        {
            return MakeResult(GA_ERROR::NOT_OWNED, STR_CANT_REMOVE_THIS, STR_LAND_NOT_OWNED_BY_PARK);
        }

        BannerElement* bannerElement = GetBannerElementAt();
        if (bannerElement == nullptr)
        {
            log_error(
                "Invalid banner location, x = %d, y = %d, z = %d, direction = %d", _loc.x, _loc.y, _loc.z, _loc.direction);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS);
        }

        if (bannerElement->GetIndex() >= MAX_BANNERS || bannerElement->GetIndex() == BANNER_INDEX_NULL)
        {
            log_error("Invalid banner index. index = ", bannerElement->GetIndex());
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS);
        }

        auto banner = bannerElement->GetBanner();
        if (banner == nullptr)
        {
            log_error("Invalid banner index. index = ", bannerElement->GetIndex());
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS);
        }

        rct_scenery_entry* bannerEntry = get_banner_entry(banner->type);
        if (bannerEntry != nullptr)
        {
            res->Cost = -((bannerEntry->banner.price * 3) / 4);
        }

        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        auto res = MakeResult();
        res->Expenditure = ExpenditureType::Landscaping;
        res->Position.x = _loc.x + 16;
        res->Position.y = _loc.y + 16;
        res->Position.z = _loc.z;
        res->ErrorTitle = STR_CANT_REMOVE_THIS;

        BannerElement* bannerElement = GetBannerElementAt();
        if (bannerElement == nullptr)
        {
            log_error(
                "Invalid banner location, x = %d, y = %d, z = %d, direction = %d", _loc.x, _loc.y, _loc.z, _loc.direction);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS);
        }

        if (bannerElement->GetIndex() >= MAX_BANNERS || bannerElement->GetIndex() == BANNER_INDEX_NULL)
        {
            log_error("Invalid banner index. index = ", bannerElement->GetIndex());
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS);
        }

        auto banner = bannerElement->GetBanner();
        if (banner == nullptr)
        {
            log_error("Invalid banner index. index = ", bannerElement->GetIndex());
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REMOVE_THIS);
        }

        rct_scenery_entry* bannerEntry = get_banner_entry(banner->type);
        if (bannerEntry != nullptr)
        {
            res->Cost = -((bannerEntry->banner.price * 3) / 4);
        }

        tile_element_remove_banner_entry(reinterpret_cast<TileElement*>(bannerElement));
        map_invalidate_tile_zoom1({ _loc, _loc.z, _loc.z + 32 });
        bannerElement->Remove();

        return res;
    }

private:
    BannerElement* GetBannerElementAt() const
    {
        TileElement* tileElement = map_get_first_element_at(_loc);

        // Find the banner element at known z and position
        do
        {
            if (tileElement == nullptr)
                break;
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_BANNER)
                continue;
            if (tileElement->GetBaseZ() != _loc.z)
                continue;
            if (tileElement->IsGhost() && !(GetFlags() & GAME_COMMAND_FLAG_GHOST))
                continue;
            if (tileElement->AsBanner()->GetPosition() != _loc.direction)
                continue;

            return tileElement->AsBanner();
        } while (!(tileElement++)->IsLastForTile());

        return nullptr;
    }
};
