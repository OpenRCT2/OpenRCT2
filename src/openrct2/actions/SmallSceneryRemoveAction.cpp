/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SmallSceneryRemoveAction.h"

#include "../Cheats.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../object/ObjectEntryManager.h"
#include "../object/SmallSceneryEntry.h"
#include "../ride/Ride.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/TileElementsView.h"
#include "../world/tile_element/SmallSceneryElement.h"
#include "GameAction.h"
#include "SmallSceneryPlaceAction.h"

namespace OpenRCT2::GameActions
{
    SmallSceneryRemoveAction::SmallSceneryRemoveAction(
        const CoordsXYZ& location, uint8_t quadrant, ObjectEntryIndex sceneryType)
        : _loc(location)
        , _quadrant(quadrant)
        , _sceneryType(sceneryType)
    {
    }

    void SmallSceneryRemoveAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit(_loc);
        visitor.Visit("object", _sceneryType);
        visitor.Visit("quadrant", _quadrant);
    }

    uint16_t SmallSceneryRemoveAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags();
    }

    void SmallSceneryRemoveAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc) << DS_TAG(_quadrant) << DS_TAG(_sceneryType);
    }

    Result SmallSceneryRemoveAction::Query(GameState_t& gameState) const
    {
        Result res = Result();

        if (!LocationValid(_loc))
        {
            return Result(Status::invalidParameters, STR_CANT_REMOVE_THIS, STR_OFF_EDGE_OF_MAP);
        }

        auto* entry = ObjectManager::GetObjectEntry<SmallSceneryEntry>(_sceneryType);
        if (entry == nullptr)
        {
            return Result(Status::invalidParameters, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
        }

        res.cost = entry->removal_price;
        res.expenditure = ExpenditureType::landscaping;
        res.position = _loc;

        if (gLegacyScene != LegacyScene::scenarioEditor && !GetFlags().has(CommandFlag::ghost) && !gameState.cheats.sandboxMode)
        {
            // Check if allowed to remove item
            if (gameState.park.flags & PARK_FLAGS_FORBID_TREE_REMOVAL)
            {
                if (entry->HasFlag(SMALL_SCENERY_FLAG_IS_TREE))
                {
                    res.error = Status::noClearance;
                    res.errorTitle = STR_CANT_REMOVE_THIS;
                    res.errorMessage = STR_FORBIDDEN_BY_THE_LOCAL_AUTHORITY;
                    return res;
                }
            }

            // Check if the land is owned
            if (!MapIsLocationOwned(_loc))
            {
                res.error = Status::noClearance;
                res.errorTitle = STR_CANT_REMOVE_THIS;
                res.errorMessage = STR_LAND_NOT_OWNED_BY_PARK;
                return res;
            }
        }

        TileElement* tileElement = FindSceneryElement();
        if (tileElement == nullptr)
        {
            return Result(Status::invalidParameters, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
        }

        return res;
    }

    Result SmallSceneryRemoveAction::Execute(GameState_t& gameState) const
    {
        Result res = Result();

        auto* entry = ObjectManager::GetObjectEntry<SmallSceneryEntry>(_sceneryType);
        if (entry == nullptr)
        {
            LOG_ERROR("Invalid small scenery type %u", _sceneryType);
            return Result(Status::invalidParameters, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
        }

        res.cost = entry->removal_price;
        res.expenditure = ExpenditureType::landscaping;
        res.position = _loc;

        TileElement* tileElement = FindSceneryElement();
        if (tileElement == nullptr)
        {
            return Result(Status::invalidParameters, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
        }

        MapInvalidateTileFull(_loc);
        TileElementRemove(tileElement);

        return res;
    }

    TileElement* SmallSceneryRemoveAction::FindSceneryElement() const
    {
        const bool isGhost = GetFlags().has(CommandFlag::ghost);
        for (auto* sceneryElement : TileElementsView<SmallSceneryElement>(_loc))
        {
            // If we are removing ghost elements
            if (isGhost && sceneryElement->IsGhost() == false)
                continue;

            if (sceneryElement->GetSceneryQuadrant() != _quadrant)
                continue;

            if (sceneryElement->GetBaseZ() != _loc.z)
                continue;

            if (sceneryElement->GetEntryIndex() != _sceneryType)
                continue;

            return sceneryElement->as<TileElement>();
        }
        return nullptr;
    }
} // namespace OpenRCT2::GameActions
