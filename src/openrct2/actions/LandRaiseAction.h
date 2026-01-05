/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

enum class MapSelectType : uint8_t;

namespace OpenRCT2::GameActions
{
    class LandRaiseAction final : public GameActionBase<GameCommand::RaiseLand>
    {
    private:
        CoordsXY _coords;
        MapRange _range;
        MapSelectType _selectionType{};

    public:
        LandRaiseAction() = default;
        LandRaiseAction(const CoordsXY& coords, MapRange range, MapSelectType selectionType);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;

    private:
        Result QueryExecute(GameState_t& gameState, bool isExecuting) const;
    };
} // namespace OpenRCT2::GameActions
