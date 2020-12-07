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
#include "GameAction.h"

using namespace OpenRCT2;

using ClearableItems = uint8_t;

namespace CLEARABLE_ITEMS
{
    constexpr ClearableItems SCENERY_SMALL = 1 << 0;
    constexpr ClearableItems SCENERY_LARGE = 1 << 1;
    constexpr ClearableItems SCENERY_FOOTPATH = 1 << 2;
} // namespace CLEARABLE_ITEMS

DEFINE_GAME_ACTION(ClearAction, GAME_COMMAND_CLEAR_SCENERY, GameActions::Result)
{
private:
    MapRange _range;
    ClearableItems _itemsToClear{};

    GameActions::Result::Ptr CreateResult() const;
    GameActions::Result::Ptr QueryExecute(bool executing) const;
    money32 ClearSceneryFromTile(const CoordsXY& tilePos, bool executing) const;
    static void ResetClearLargeSceneryFlag();
    static bool MapCanClearAt(const CoordsXY& location);

public:
    ClearAction() = default;
    ClearAction(MapRange range, ClearableItems itemsToClear)
        : _range(range)
        , _itemsToClear(itemsToClear)
    {
    }

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};