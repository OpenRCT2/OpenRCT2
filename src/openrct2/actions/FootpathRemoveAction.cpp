/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FootpathRemoveAction.h"

#include "../Cheats.h"
#include "../OpenRCT2.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../world/Footpath.h"
#include "../world/Location.hpp"
#include "../world/Park.h"
#include "../world/Wall.h"
#include "BannerRemoveAction.h"

FootpathRemoveAction::FootpathRemoveAction(const CoordsXYZ& location)
    : _loc(location)
{
}

void FootpathRemoveAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_loc);
}

uint16_t FootpathRemoveAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

void FootpathRemoveAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_loc);
}

GameActions::Result FootpathRemoveAction::Query() const
{
    auto res = GameActions::Result();
    res.Cost = 0;
    res.Expenditure = ExpenditureType::Landscaping;
    res.Position = { _loc.x + 16, _loc.y + 16, _loc.z };

    if (!LocationValid(_loc))
    {
        return GameActions::Result(
            GameActions::Status::NotOwned, STR_CANT_REMOVE_FOOTPATH_FROM_HERE, STR_LAND_NOT_OWNED_BY_PARK);
    }

    if (!((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode) && !map_is_location_owned(_loc))
    {
        return GameActions::Result(
            GameActions::Status::NotOwned, STR_CANT_REMOVE_FOOTPATH_FROM_HERE, STR_LAND_NOT_OWNED_BY_PARK);
    }

    TileElement* footpathElement = GetFootpathElement();
    if (footpathElement == nullptr)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REMOVE_FOOTPATH_FROM_HERE, STR_NONE);
    }

    res.Cost = GetRefundPrice(footpathElement);

    return res;
}

GameActions::Result FootpathRemoveAction::Execute() const
{
    auto res = GameActions::Result();
    res.Cost = 0;
    res.Expenditure = ExpenditureType::Landscaping;
    res.Position = { _loc.x + 16, _loc.y + 16, _loc.z };

    if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
    {
        footpath_interrupt_peeps(_loc);
        footpath_remove_litter(_loc);
    }

    TileElement* footpathElement = GetFootpathElement();
    if (footpathElement != nullptr)
    {
        footpath_queue_chain_reset();
        auto bannerRes = RemoveBannersAtElement(_loc, footpathElement);
        if (bannerRes.Error == GameActions::Status::Ok)
        {
            res.Cost += bannerRes.Cost;
        }
        footpath_remove_edges_at(_loc, footpathElement);
        map_invalidate_tile_full(_loc);
        tile_element_remove(footpathElement);
        footpath_update_queue_chains();

        // Remove the spawn point (if there is one in the current tile)
        gPeepSpawns.erase(
            std::remove_if(
                gPeepSpawns.begin(), gPeepSpawns.end(),
                [this](const CoordsXYZ& spawn) {
                    {
                        return spawn.ToTileStart() == _loc.ToTileStart();
                    }
                }),
            gPeepSpawns.end());
    }
    else
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REMOVE_FOOTPATH_FROM_HERE, STR_NONE);
    }

    res.Cost += GetRefundPrice(footpathElement);

    return res;
}

TileElement* FootpathRemoveAction::GetFootpathElement() const
{
    bool getGhostPath = GetFlags() & GAME_COMMAND_FLAG_GHOST;

    TileElement* tileElement = map_get_footpath_element(_loc);
    TileElement* footpathElement = nullptr;
    if (tileElement != nullptr)
    {
        if (getGhostPath && !tileElement->IsGhost())
        {
            while (!(tileElement++)->IsLastForTile())
            {
                if (tileElement->GetType() != TileElementType::Path && !tileElement->IsGhost())
                {
                    continue;
                }
                footpathElement = tileElement;
                break;
            }
        }
        else
        {
            footpathElement = tileElement;
        }
    }

    return footpathElement;
}

money32 FootpathRemoveAction::GetRefundPrice(TileElement* footpathElement) const
{
    money32 cost = -10.00_GBP;
    return cost;
}

/**
 *
 *  rct2: 0x006BA23E
 */
GameActions::Result FootpathRemoveAction::RemoveBannersAtElement(const CoordsXY& loc, TileElement* tileElement) const
{
    auto result = GameActions::Result();
    while (!(tileElement++)->IsLastForTile())
    {
        if (tileElement->GetType() == TileElementType::Path)
            return result;

        if (tileElement->GetType() != TileElementType::Banner)
            continue;

        auto bannerRemoveAction = BannerRemoveAction({ loc, tileElement->GetBaseZ(), tileElement->AsBanner()->GetPosition() });
        bool isGhost = tileElement->IsGhost();
        auto bannerFlags = GetFlags() | (isGhost ? static_cast<uint32_t>(GAME_COMMAND_FLAG_GHOST) : 0);
        bannerRemoveAction.SetFlags(bannerFlags);
        auto res = GameActions::ExecuteNested(&bannerRemoveAction);
        // Ghost removal is free
        if (res.Error == GameActions::Status::Ok && !isGhost)
        {
            result.Cost += res.Cost;
        }
        tileElement--;
    }
    return result;
}
