/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../ride/TrackDesign.h"
#include "GameAction.h"

namespace OpenRCT2::GameActions
{
    class TrackDesignAction final : public GameActionBase<GameCommand::PlaceTrackDesign>
    {
    private:
        CoordsXYZD _loc;
        TrackDesign _td;

    public:
        TrackDesignAction() = default;
        TrackDesignAction(const CoordsXYZD& location, const TrackDesign& td);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;

        Result Query() const override;
        Result Execute() const override;
    };
} // namespace OpenRCT2::GameActions
