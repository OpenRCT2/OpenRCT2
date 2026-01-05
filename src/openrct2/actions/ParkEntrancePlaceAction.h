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

namespace OpenRCT2::GameActions
{
    class ParkEntrancePlaceAction final : public GameActionBase<GameCommand::PlaceParkEntrance>
    {
    private:
        CoordsXYZD _loc;
        ObjectEntryIndex _pathType;
        ObjectEntryIndex _entranceType;
        bool _pathTypeIsLegacy;

    public:
        ParkEntrancePlaceAction() = default;
        ParkEntrancePlaceAction(
            const CoordsXYZD& location, ObjectEntryIndex pathType, ObjectEntryIndex entranceType, bool pathTypeIsLegacy);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;

    private:
        bool CheckMapCapacity(int16_t numTiles) const;
    };
} // namespace OpenRCT2::GameActions
