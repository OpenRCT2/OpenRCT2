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

DEFINE_GAME_ACTION(BannerPlaceAction, GAME_COMMAND_PLACE_BANNER, GameActionResult)
{
private:
    CoordsXYZD _loc;
    ObjectEntryIndex _bannerType{ BANNER_NULL };
    BannerIndex _bannerIndex{ BANNER_INDEX_NULL };
    uint8_t _primaryColour;

public:
    BannerPlaceAction() = default;
    BannerPlaceAction(const CoordsXYZD& loc, uint8_t bannerType, BannerIndex bannerIndex, uint8_t primaryColour)
        : _loc(loc)
        , _bannerType(bannerType)
        , _bannerIndex(bannerIndex)
        , _primaryColour(primaryColour)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc) << DS_TAG(_bannerType) << DS_TAG(_bannerIndex) << DS_TAG(_primaryColour);
    }

    GameActionResult::Ptr Query() const override
    {
        auto res = MakeResult();
        res->Position.x = _loc.x + 16;
        res->Position.y = _loc.y + 16;
        res->Position.z = _loc.z;
        res->Expenditure = ExpenditureType::Landscaping;
        res->ErrorTitle = STR_CANT_POSITION_THIS_HERE;

        if (!map_check_free_elements_and_reorganise(1))
        {
            log_error("No free map elements.");
            return MakeResult(GA_ERROR::NO_FREE_ELEMENTS, STR_CANT_POSITION_THIS_HERE);
        }

        if (!LocationValid(_loc))
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_POSITION_THIS_HERE);
        }

        auto pathElement = GetValidPathElement();

        if (pathElement == nullptr)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_POSITION_THIS_HERE, STR_CAN_ONLY_BE_BUILT_ACROSS_PATHS);
        }

        if (!map_can_build_at(_loc))
        {
            return MakeResult(GA_ERROR::NOT_OWNED, STR_CANT_POSITION_THIS_HERE, STR_LAND_NOT_OWNED_BY_PARK);
        }

        auto baseHeight = _loc.z + PATH_HEIGHT_STEP;
        BannerElement* existingBannerElement = map_get_banner_element_at({ _loc.x, _loc.y, baseHeight }, _loc.direction);
        if (existingBannerElement != nullptr)
        {
            return MakeResult(GA_ERROR::ITEM_ALREADY_PLACED, STR_CANT_POSITION_THIS_HERE, STR_BANNER_SIGN_IN_THE_WAY);
        }

        if (_bannerIndex == BANNER_INDEX_NULL || _bannerIndex >= MAX_BANNERS)
        {
            log_error("Invalid banner index, bannerIndex = %u", _bannerIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_POSITION_THIS_HERE);
        }

        auto banner = GetBanner(_bannerIndex);
        if (!banner->IsNull())
        {
            log_error("Banner index in use, bannerIndex = %u", _bannerIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_POSITION_THIS_HERE);
        }

        rct_scenery_entry* bannerEntry = get_banner_entry(_bannerType);
        if (bannerEntry == nullptr)
        {
            log_error("Invalid banner object type. bannerType = ", _bannerType);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_POSITION_THIS_HERE);
        }
        res->Cost = bannerEntry->banner.price;
        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        auto res = MakeResult();
        res->Position.x = _loc.x + 16;
        res->Position.y = _loc.y + 16;
        res->Position.z = _loc.z;
        res->Expenditure = ExpenditureType::Landscaping;
        res->ErrorTitle = STR_CANT_POSITION_THIS_HERE;

        if (!map_check_free_elements_and_reorganise(1))
        {
            log_error("No free map elements.");
            return MakeResult(GA_ERROR::NO_FREE_ELEMENTS, STR_CANT_POSITION_THIS_HERE);
        }

        if (_bannerIndex == BANNER_INDEX_NULL || _bannerIndex >= MAX_BANNERS)
        {
            log_error("Invalid banner index, bannerIndex = %u", _bannerIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_POSITION_THIS_HERE);
        }

        rct_scenery_entry* bannerEntry = get_banner_entry(_bannerType);
        if (bannerEntry == nullptr)
        {
            log_error("Invalid banner object type. bannerType = ", _bannerType);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_POSITION_THIS_HERE);
        }

        auto banner = GetBanner(_bannerIndex);
        if (!banner->IsNull())
        {
            log_error("Banner index in use, bannerIndex = %u", _bannerIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_POSITION_THIS_HERE);
        }

        TileElement* newTileElement = tile_element_insert({ _loc, _loc.z + (2 * COORDS_Z_STEP) }, 0b0000);
        assert(newTileElement != nullptr);

        banner->flags = 0;
        banner->text = {};
        banner->text_colour = 2;
        banner->type = _bannerType; // Banner must be deleted after this point in an early return
        banner->colour = _primaryColour;
        banner->position = TileCoordsXY(_loc);
        newTileElement->SetType(TILE_ELEMENT_TYPE_BANNER);
        BannerElement* bannerElement = newTileElement->AsBanner();
        bannerElement->SetClearanceZ(_loc.z + PATH_CLEARANCE);
        bannerElement->SetPosition(_loc.direction);
        bannerElement->ResetAllowedEdges();
        bannerElement->SetIndex(_bannerIndex);
        if (GetFlags() & GAME_COMMAND_FLAG_GHOST)
        {
            bannerElement->SetGhost(true);
        }
        map_invalidate_tile_full(_loc);
        map_animation_create(MAP_ANIMATION_TYPE_BANNER, CoordsXYZ{ _loc, bannerElement->GetBaseZ() });

        res->Cost = bannerEntry->banner.price;
        return res;
    }

private:
    PathElement* GetValidPathElement() const
    {
        TileElement* tileElement = map_get_first_element_at(_loc);
        do
        {
            if (tileElement == nullptr)
                break;

            if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
                continue;

            auto pathElement = tileElement->AsPath();

            if (pathElement->GetBaseZ() != _loc.z && pathElement->GetBaseZ() != _loc.z - PATH_HEIGHT_STEP)
                continue;

            if (!(pathElement->GetEdges() & (1 << _loc.direction)))
                continue;

            if (pathElement->IsGhost() && !(GetFlags() & GAME_COMMAND_FLAG_GHOST))
                continue;

            return pathElement;
        } while (!(tileElement++)->IsLastForTile());
        return nullptr;
    }
};
