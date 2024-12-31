/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ScenerySetRestrictedAction.h"

#include "../object/SceneryGroupObject.h"
#include "../world/Scenery.h"

using namespace OpenRCT2;

ScenerySetRestrictedAction::ScenerySetRestrictedAction(ObjectType objectType, ObjectEntryIndex entryIndex, bool isRestricted)
    : _objectType(objectType)
    , _objectIndex(entryIndex)
    , _isRestricted(isRestricted)
{
}

void ScenerySetRestrictedAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_objectType) << DS_TAG(_objectIndex) << DS_TAG(_isRestricted);
}

uint16_t ScenerySetRestrictedAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

GameActions::Result ScenerySetRestrictedAction::Query() const
{
    if (!ObjectTypeCanBeRestricted(_objectType))
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_RESTRICT_OBJECT, STR_OBJECT_TYPE_CANNOT_BE_RESTRICTED);
    }
    const auto* loadedObject = ObjectEntryGetObject(_objectType, _objectIndex);
    if (loadedObject == nullptr)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_RESTRICT_OBJECT, STR_OBJECT_NOT_FOUND);
    }

    return GameActions::Result();
}

GameActions::Result ScenerySetRestrictedAction::Execute() const
{
    auto sceneryType = GetSceneryTypeFromObjectType(_objectType);
    SetSceneryItemRestricted({ sceneryType, _objectIndex }, _isRestricted);

    return GameActions::Result();
}
