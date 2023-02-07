/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "BannerRemoveAction.h"

#include "../management/Finance.h"
#include "../object/BannerSceneryEntry.h"
#include "../world/Banner.h"
#include "../world/MapAnimation.h"
#include "../world/Scenery.h"
#include "../world/TileElementsView.h"
#include "../object/ObjectEntryManager.h"
#include "GameAction.h"

using namespace OpenRCT2;

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

GameActions::Result BannerRemoveAction::Query() const
{
    auto res = GameActions::Result();
    res.Expenditure = ExpenditureType::Landscaping;
    res.Position.x = _loc.x + 16;
    res.Position.y = _loc.y + 16;
    res.Position.z = _loc.z;
    res.ErrorTitle = STR_CANT_REMOVE_THIS;

    if (!LocationValid(_loc) || !MapCanBuildAt({ _loc.x, _loc.y, _loc.z - 16 }))
    {
        return GameActions::Result(GameActions::Status::NotOwned, STR_CANT_REMOVE_THIS, STR_LAND_NOT_OWNED_BY_PARK);
    }

    BannerElement* bannerElement = GetBannerElementAt();
    if (bannerElement == nullptr)
    {
        LOG_ERROR("Invalid banner location, x = %d, y = %d, z = %d, direction = %d", _loc.x, _loc.y, _loc.z, _loc.direction);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_NONE);
    }

    auto bannerIndex = bannerElement->GetIndex();
    if (bannerIndex == BannerIndex::GetNull())
    {
        LOG_ERROR("Invalid banner index. index = ", bannerIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_NONE);
    }

    auto banner = bannerElement->GetBanner();
    if (banner == nullptr)
    {
        LOG_ERROR("Invalid banner index. index = ", bannerIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_NONE);
    }

    auto* bannerEntry = OpenRCT2::ObjectManager::GetObjectEntry<BannerSceneryEntry>(banner->type);
    if (bannerEntry != nullptr)
    {
        res.Cost = -((bannerEntry->price * 3) / 4);
    }

    return res;
}

GameActions::Result BannerRemoveAction::Execute() const
{
    auto res = GameActions::Result();
    res.Expenditure = ExpenditureType::Landscaping;
    res.Position.x = _loc.x + 16;
    res.Position.y = _loc.y + 16;
    res.Position.z = _loc.z;
    res.ErrorTitle = STR_CANT_REMOVE_THIS;

    BannerElement* bannerElement = GetBannerElementAt();
    if (bannerElement == nullptr)
    {
        LOG_ERROR("Invalid banner location, x = %d, y = %d, z = %d, direction = %d", _loc.x, _loc.y, _loc.z, _loc.direction);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_NONE);
    }

    auto bannerIndex = bannerElement->GetIndex();
    if (bannerIndex == BannerIndex::GetNull())
    {
        LOG_ERROR("Invalid banner index. index = ", bannerIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_NONE);
    }

    auto banner = bannerElement->GetBanner();
    if (banner == nullptr)
    {
        LOG_ERROR("Invalid banner index. index = ", bannerIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_NONE);
    }

    auto* bannerEntry = OpenRCT2::ObjectManager::GetObjectEntry<BannerSceneryEntry>(banner->type);
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
