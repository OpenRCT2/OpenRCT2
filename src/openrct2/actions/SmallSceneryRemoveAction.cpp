/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SmallSceneryRemoveAction.h"

#include "../Cheats.h"
#include "../OpenRCT2.h"
#include "../common.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../object/ObjectEntryManager.h"
#include "../object/SmallSceneryEntry.h"
#include "../ride/Ride.h"
#include "../world/Park.h"
#include "../world/TileElementsView.h"
#include "GameAction.h"
#include "SmallSceneryPlaceAction.h"

using namespace OpenRCT2;

SmallSceneryRemoveAction::SmallSceneryRemoveAction(const CoordsXYZ& location, uint8_t quadrant, ObjectEntryIndex sceneryType)
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

GameActions::Result SmallSceneryRemoveAction::Query() const
{
    GameActions::Result res = GameActions::Result();

    if (!LocationValid(_loc))
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_LAND_NOT_OWNED_BY_PARK);
    }

    auto* entry = OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(_sceneryType);
    if (entry == nullptr)
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
    }

    res.Cost = entry->removal_price;
    res.Expenditure = ExpenditureType::Landscaping;
    res.Position = _loc;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !(GetFlags() & GAME_COMMAND_FLAG_GHOST) && !gCheatsSandboxMode)
    {
        // Check if allowed to remove item
        if (gParkFlags & PARK_FLAGS_FORBID_TREE_REMOVAL)
        {
            if (entry->HasFlag(SMALL_SCENERY_FLAG_IS_TREE))
            {
                res.Error = GameActions::Status::NoClearance;
                res.ErrorTitle = STR_CANT_REMOVE_THIS;
                res.ErrorMessage = STR_FORBIDDEN_BY_THE_LOCAL_AUTHORITY;
                return res;
            }
        }

        // Check if the land is owned
        if (!MapIsLocationOwned(_loc))
        {
            res.Error = GameActions::Status::NoClearance;
            res.ErrorTitle = STR_CANT_REMOVE_THIS;
            res.ErrorMessage = STR_LAND_NOT_OWNED_BY_PARK;
            return res;
        }
    }

    TileElement* tileElement = FindSceneryElement();
    if (tileElement == nullptr)
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
    }

    return res;
}

GameActions::Result SmallSceneryRemoveAction::Execute() const
{
    GameActions::Result res = GameActions::Result();

    auto* entry = OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(_sceneryType);
    if (entry == nullptr)
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
    }

    res.Cost = entry->removal_price;
    res.Expenditure = ExpenditureType::Landscaping;
    res.Position = _loc;

    TileElement* tileElement = FindSceneryElement();
    if (tileElement == nullptr)
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
    }

    res.Position.z = TileElementHeight(res.Position);

    MapInvalidateTileFull(_loc);
    TileElementRemove(tileElement);

    return res;
}

TileElement* SmallSceneryRemoveAction::FindSceneryElement() const
{
    const bool isGhost = GetFlags() & GAME_COMMAND_FLAG_GHOST;
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
