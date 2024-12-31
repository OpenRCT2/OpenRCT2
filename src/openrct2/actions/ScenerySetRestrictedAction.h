/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

class ScenerySetRestrictedAction final : public GameActionBase<GameCommand::SetRestrictedScenery>
{
private:
    ObjectType _objectType;
    ObjectEntryIndex _objectIndex;
    bool _isRestricted;

public:
    ScenerySetRestrictedAction() = default;
    ScenerySetRestrictedAction(ObjectType objectType, ObjectEntryIndex entryIndex, bool isRestricted);

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;
};
