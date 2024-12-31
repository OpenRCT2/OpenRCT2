/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../OpenRCT2.h"
#include "../management/Finance.h"
#include "GameAction.h"

using ClearableItems = uint8_t;

namespace OpenRCT2::CLEARABLE_ITEMS
{
    constexpr ClearableItems SCENERY_SMALL = 1 << 0;
    constexpr ClearableItems SCENERY_LARGE = 1 << 1;
    constexpr ClearableItems SCENERY_FOOTPATH = 1 << 2;
} // namespace OpenRCT2::CLEARABLE_ITEMS

class ClearAction final : public GameActionBase<GameCommand::ClearScenery>
{
private:
    MapRange _range;
    ClearableItems _itemsToClear{};

public:
    ClearAction() = default;
    ClearAction(MapRange range, ClearableItems itemsToClear);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;

private:
    OpenRCT2::GameActions::Result CreateResult() const;
    OpenRCT2::GameActions::Result QueryExecute(bool executing) const;
    money64 ClearSceneryFromTile(const CoordsXY& tilePos, bool executing) const;

    /**
     * Function to clear the flag that is set to prevent cost duplication
     * when using the clear scenery tool with large scenery.
     */
    static void ResetClearLargeSceneryFlag();

    static bool MapCanClearAt(const CoordsXY& location);
};
