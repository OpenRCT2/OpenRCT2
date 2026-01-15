/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

namespace OpenRCT2::GameActions
{
    using ClearableItems = uint8_t;

    namespace CLEARABLE_ITEMS
    {
        constexpr ClearableItems kScenerySmall = 1 << 0;
        constexpr ClearableItems kSceneryLarge = 1 << 1;
        constexpr ClearableItems kSceneryFootpath = 1 << 2;
    } // namespace CLEARABLE_ITEMS

    class ClearAction final : public GameActionBase<GameCommand::ClearScenery>
    {
    private:
        MapRange _range;
        ClearableItems _itemsToClear{};

    public:
        ClearAction() = default;
        ClearAction(MapRange range, ClearableItems itemsToClear);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;

    private:
        Result CreateResult() const;
        Result QueryExecute(GameState_t& gameState, bool executing) const;
        money64 ClearSceneryFromTile(const CoordsXY& tilePos, bool executing, GameState_t& gameState) const;

        /**
         * Function to clear the flag that is set to prevent cost duplication
         * when using the clear scenery tool with large scenery.
         */
        static void ResetClearLargeSceneryFlag(GameState_t& gameState);

        static bool MapCanClearAt(const GameState_t& gameState, const CoordsXY& location);
    };
} // namespace OpenRCT2::GameActions
