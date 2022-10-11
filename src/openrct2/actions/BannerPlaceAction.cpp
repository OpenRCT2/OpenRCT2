/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "BannerPlaceAction.h"

#include "../management/Finance.h"
#include "../world/Banner.h"
#include "../world/MapAnimation.h"
#include "../world/Scenery.h"
#include "../world/TileElementsView.h"
#include "GameAction.h"

using namespace OpenRCT2;

BannerPlaceAction::BannerPlaceAction(const CoordsXYZD& loc, ObjectEntryIndex bannerType, colour_t primaryColour)
    : _loc(loc)
    , _bannerType(bannerType)
    , _primaryColour(primaryColour)
{
}

void BannerPlaceAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_loc);
    visitor.Visit("object", _bannerType);
    visitor.Visit("primaryColour", _primaryColour);
}

uint16_t BannerPlaceAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

void BannerPlaceAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_loc) << DS_TAG(_bannerType) << DS_TAG(_primaryColour);
}

GameActions::Result BannerPlaceAction::Query() const
{
    auto res = GameActions::Result();
    res.Position.x = _loc.x + 16;
    res.Position.y = _loc.y + 16;
    res.Position.z = _loc.z;
    res.Expenditure = ExpenditureType::Landscaping;
    res.ErrorTitle = STR_CANT_POSITION_THIS_HERE;

    if (!LocationValid(_loc))
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_NONE);
    }

    if (!MapCheckCapacityAndReorganise(_loc))
    {
        log_error("No free map elements.");
        return GameActions::Result(
            GameActions::Status::NoFreeElements, STR_CANT_POSITION_THIS_HERE, STR_TILE_ELEMENT_LIMIT_REACHED);
    }

    auto pathElement = GetValidPathElement();

    if (pathElement == nullptr)
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_CAN_ONLY_BE_BUILT_ACROSS_PATHS);
    }

    if (!MapCanBuildAt(_loc))
    {
        return GameActions::Result(GameActions::Status::NotOwned, STR_CANT_POSITION_THIS_HERE, STR_LAND_NOT_OWNED_BY_PARK);
    }

    auto baseHeight = _loc.z + PATH_HEIGHT_STEP;
    BannerElement* existingBannerElement = MapGetBannerElementAt({ _loc.x, _loc.y, baseHeight }, _loc.direction);
    if (existingBannerElement != nullptr)
    {
        return GameActions::Result(
            GameActions::Status::ItemAlreadyPlaced, STR_CANT_POSITION_THIS_HERE, STR_BANNER_SIGN_IN_THE_WAY);
    }

    if (HasReachedBannerLimit())
    {
        log_error("No free banners available");
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_TOO_MANY_BANNERS_IN_GAME);
    }

    auto* bannerEntry = GetBannerEntry(_bannerType);
    if (bannerEntry == nullptr)
    {
        log_error("Invalid banner object type. bannerType = ", _bannerType);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_NONE);
    }
    res.Cost = bannerEntry->price;

    return res;
}

GameActions::Result BannerPlaceAction::Execute() const
{
    auto res = GameActions::Result();
    res.Position.x = _loc.x + 16;
    res.Position.y = _loc.y + 16;
    res.Position.z = _loc.z;
    res.Expenditure = ExpenditureType::Landscaping;
    res.ErrorTitle = STR_CANT_POSITION_THIS_HERE;

    if (!MapCheckCapacityAndReorganise(_loc))
    {
        log_error("No free map elements.");
        return GameActions::Result(
            GameActions::Status::NoFreeElements, STR_CANT_POSITION_THIS_HERE, STR_TILE_ELEMENT_LIMIT_REACHED);
    }

    auto* bannerEntry = GetBannerEntry(_bannerType);
    if (bannerEntry == nullptr)
    {
        log_error("Invalid banner object type. bannerType = ", _bannerType);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_NONE);
    }

    auto banner = CreateBanner();
    if (banner == nullptr)
    {
        log_error("No free banners available");
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_POSITION_THIS_HERE, STR_TOO_MANY_BANNERS_IN_GAME);
    }
    banner->flags = 0;
    banner->text = {};
    banner->text_colour = 2;
    banner->type = _bannerType; // Banner must be deleted after this point in an early return
    banner->colour = _primaryColour;
    banner->position = TileCoordsXY(_loc);

    res.SetData(BannerPlaceActionResult{ banner->id });
    auto* bannerElement = TileElementInsert<BannerElement>({ _loc, _loc.z + (2 * COORDS_Z_STEP) }, 0b0000);
    Guard::Assert(bannerElement != nullptr);

    bannerElement->SetClearanceZ(_loc.z + PATH_CLEARANCE);
    bannerElement->SetPosition(_loc.direction);
    bannerElement->ResetAllowedEdges();
    bannerElement->SetIndex(banner->id);
    bannerElement->SetGhost(GetFlags() & GAME_COMMAND_FLAG_GHOST);

    map_invalidate_tile_full(_loc);
    MapAnimationCreate(MAP_ANIMATION_TYPE_BANNER, CoordsXYZ{ _loc, bannerElement->GetBaseZ() });

    res.Cost = bannerEntry->price;
    return res;
}

PathElement* BannerPlaceAction::GetValidPathElement() const
{
    for (auto* pathElement : TileElementsView<PathElement>(_loc))
    {
        if (pathElement->GetBaseZ() != _loc.z && pathElement->GetBaseZ() != _loc.z - PATH_HEIGHT_STEP)
            continue;

        if (!(pathElement->GetEdges() & (1 << _loc.direction)))
            continue;

        if (pathElement->IsGhost() && !(GetFlags() & GAME_COMMAND_FLAG_GHOST))
            continue;

        return pathElement;
    }

    return nullptr;
}
