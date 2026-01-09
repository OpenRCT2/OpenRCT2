/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FootpathRemoveAction.h"

#include "../Cheats.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../world/Footpath.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/TileElementsView.h"
#include "../world/tile_element/BannerElement.h"
#include "../world/tile_element/PathElement.h"
#include "BannerRemoveAction.h"

namespace OpenRCT2::GameActions
{
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

    Result FootpathRemoveAction::Query(GameState_t& gameState) const
    {
        auto res = Result();
        res.cost = 0;
        res.expenditure = ExpenditureType::landscaping;
        res.position = { _loc.x + 16, _loc.y + 16, _loc.z };

        if (!LocationValid(_loc))
        {
            return Result(Status::invalidParameters, STR_CANT_REMOVE_FOOTPATH_FROM_HERE, STR_OFF_EDGE_OF_MAP);
        }

        if (!(gLegacyScene == LegacyScene::scenarioEditor || gameState.cheats.sandboxMode) && !MapIsLocationOwned(_loc))
        {
            return Result(Status::notOwned, STR_CANT_REMOVE_FOOTPATH_FROM_HERE, STR_LAND_NOT_OWNED_BY_PARK);
        }

        TileElement* footpathElement = GetFootpathElement();
        if (footpathElement == nullptr)
        {
            return Result(Status::invalidParameters, STR_CANT_REMOVE_FOOTPATH_FROM_HERE, STR_ERR_PATH_ELEMENT_NOT_FOUND);
        }

        res.cost = GetRefundPrice(footpathElement);

        return res;
    }

    Result FootpathRemoveAction::Execute(GameState_t& gameState) const
    {
        auto res = Result();
        res.cost = 0;
        res.expenditure = ExpenditureType::landscaping;
        res.position = { _loc.x + 16, _loc.y + 16, _loc.z };

        if (!GetFlags().has(CommandFlag::ghost))
        {
            FootpathInterruptPeeps(_loc);
            FootpathRemoveLitter(_loc);
        }

        TileElement* footpathElement = GetFootpathElement();
        if (footpathElement != nullptr)
        {
            FootpathQueueChainReset();
            auto bannerRes = RemoveBannersAtElement(gameState, _loc, footpathElement);
            if (bannerRes.error == Status::ok)
            {
                res.cost += bannerRes.cost;
            }
            FootpathRemoveEdgesAt(_loc, footpathElement);
            MapInvalidateTileFull(_loc);
            TileElementRemove(footpathElement);
            FootpathUpdateQueueChains();

            // Remove the spawn point (if there is one in the current tile)
            gameState.peepSpawns.erase(
                std::remove_if(
                    gameState.peepSpawns.begin(), gameState.peepSpawns.end(),
                    [this](const CoordsXYZ& spawn) {
                        {
                            return spawn.ToTileStart() == _loc.ToTileStart();
                        }
                    }),
                gameState.peepSpawns.end());
        }
        else
        {
            return Result(Status::invalidParameters, STR_CANT_REMOVE_FOOTPATH_FROM_HERE, kStringIdNone);
        }

        res.cost += GetRefundPrice(footpathElement);

        return res;
    }

    TileElement* FootpathRemoveAction::GetFootpathElement() const
    {
        bool getGhostPath = GetFlags().has(CommandFlag::ghost);

        for (auto* pathElement : TileElementsView<PathElement>(_loc))
        {
            if (pathElement->IsGhost() != getGhostPath)
                continue;

            if (pathElement->GetBaseZ() != _loc.z)
                continue;

            return reinterpret_cast<TileElement*>(pathElement);
        }

        return nullptr;
    }

    money64 FootpathRemoveAction::GetRefundPrice(TileElement* footpathElement) const
    {
        money64 cost = -10.00_GBP;
        return cost;
    }

    /**
     *
     *  rct2: 0x006BA23E
     */
    Result FootpathRemoveAction::RemoveBannersAtElement(
        GameState_t& gameState, const CoordsXY& loc, TileElement* tileElement) const
    {
        auto result = Result();
        while (!(tileElement++)->IsLastForTile())
        {
            if (tileElement->GetType() == TileElementType::Path)
                return result;

            if (tileElement->GetType() != TileElementType::Banner)
                continue;

            auto bannerRemoveAction = BannerRemoveAction(
                { loc, tileElement->GetBaseZ(), tileElement->AsBanner()->GetPosition() });
            bool isGhost = tileElement->IsGhost();
            auto bannerFlags = GetFlags();
            if (isGhost)
                bannerFlags.set(CommandFlag::ghost);
            bannerRemoveAction.SetFlags(bannerFlags);

            auto res = ExecuteNested(&bannerRemoveAction, gameState);
            // Ghost removal is free
            if (res.error == Status::ok && !isGhost)
            {
                result.cost += res.cost;
            }
            tileElement--;
        }
        return result;
    }
} // namespace OpenRCT2::GameActions
