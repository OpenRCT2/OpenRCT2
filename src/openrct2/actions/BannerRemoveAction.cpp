/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "BannerRemoveAction.h"

#include "../Diagnostic.h"
#include "../management/Finance.h"
#include "../object/BannerSceneryEntry.h"
#include "../object/ObjectEntryManager.h"
#include "../world/Banner.h"
#include "../world/Map.h"
#include "../world/Scenery.h"
#include "../world/TileElementsView.h"
#include "../world/tile_element/BannerElement.h"
#include "GameAction.h"

namespace OpenRCT2::GameActions
{
    BannerRemoveAction::BannerRemoveAction(const CoordsXYZD& loc)
        : _loc(loc)
    {
    }

    void BannerRemoveAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit(_loc);
    }

    uint16_t BannerRemoveAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags();
    }

    void BannerRemoveAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc);
    }

    Result BannerRemoveAction::Query(GameState_t& gameState) const
    {
        auto res = Result();
        res.Expenditure = ExpenditureType::landscaping;
        res.Position.x = _loc.x + 16;
        res.Position.y = _loc.y + 16;
        res.Position.z = _loc.z;
        res.ErrorTitle = STR_CANT_REMOVE_THIS;

        if (!LocationValid(_loc))
        {
            return Result(Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_OFF_EDGE_OF_MAP);
        }
        if (!MapCanBuildAt({ _loc.x, _loc.y, _loc.z - 16 }))
        {
            return Result(Status::NotOwned, STR_CANT_REMOVE_THIS, STR_LAND_NOT_OWNED_BY_PARK);
        }

        BannerElement* bannerElement = GetBannerElementAt();
        if (bannerElement == nullptr)
        {
            return Result(Status::InvalidParameters, STR_CANT_REMOVE_THIS, kStringIdNone);
        }

        auto bannerIndex = bannerElement->GetIndex();
        if (bannerIndex == BannerIndex::GetNull())
        {
            return Result(Status::InvalidParameters, STR_CANT_REMOVE_THIS, kStringIdNone);
        }

        auto banner = bannerElement->GetBanner();
        if (banner == nullptr)
        {
            return Result(Status::InvalidParameters, STR_CANT_REMOVE_THIS, kStringIdNone);
        }

        auto* bannerEntry = ObjectManager::GetObjectEntry<BannerSceneryEntry>(banner->type);
        if (bannerEntry != nullptr)
        {
            res.Cost = -((bannerEntry->price * 3) / 4);
        }

        return res;
    }

    Result BannerRemoveAction::Execute(GameState_t& gameState) const
    {
        auto res = Result();
        res.Expenditure = ExpenditureType::landscaping;
        res.Position.x = _loc.x + 16;
        res.Position.y = _loc.y + 16;
        res.Position.z = _loc.z;
        res.ErrorTitle = STR_CANT_REMOVE_THIS;

        BannerElement* bannerElement = GetBannerElementAt();
        if (bannerElement == nullptr)
        {
            LOG_ERROR(
                "Invalid banner location, x = %d, y = %d, z = %d, direction = %d", _loc.x, _loc.y, _loc.z, _loc.direction);
            return Result(Status::InvalidParameters, STR_CANT_REMOVE_THIS, kStringIdNone);
        }

        auto bannerIndex = bannerElement->GetIndex();
        if (bannerIndex == BannerIndex::GetNull())
        {
            LOG_ERROR("Invalid banner index %u", bannerIndex);
            return Result(Status::InvalidParameters, STR_CANT_REMOVE_THIS, kStringIdNone);
        }

        auto banner = bannerElement->GetBanner();
        if (banner == nullptr)
        {
            LOG_ERROR("Invalid banner index %u", bannerIndex);
            return Result(Status::InvalidParameters, STR_CANT_REMOVE_THIS, kStringIdNone);
        }

        auto* bannerEntry = ObjectManager::GetObjectEntry<BannerSceneryEntry>(banner->type);
        if (bannerEntry != nullptr)
        {
            res.Cost = -((bannerEntry->price * 3) / 4);
        }

        reinterpret_cast<TileElement*>(bannerElement)->RemoveBannerEntry();
        MapInvalidateTileZoom1({ _loc, _loc.z, _loc.z + 32 });
        bannerElement->Remove();

        return res;
    }

    BannerElement* BannerRemoveAction::GetBannerElementAt() const
    {
        // Find the banner element at known z and position
        for (auto* bannerElement : TileElementsView<BannerElement>(_loc))
        {
            if (bannerElement->GetBaseZ() != _loc.z)
                continue;
            if (bannerElement->IsGhost() && !(GetFlags() & GAME_COMMAND_FLAG_GHOST))
                continue;
            if (bannerElement->GetPosition() != _loc.direction)
                continue;

            return bannerElement;
        }

        return nullptr;
    }
} // namespace OpenRCT2::GameActions
