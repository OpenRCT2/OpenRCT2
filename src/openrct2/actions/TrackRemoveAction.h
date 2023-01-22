/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"
namespace OpenRCT2
{
    class TrackRemoveAction final : public GameActionBase<GameCommand::RemoveTrack>
    {
    private:
        track_type_t _trackType{};
        int32_t _sequence{};
        CoordsXYZD _origin;

    public:
        TrackRemoveAction() = default;
        TrackRemoveAction(track_type_t trackType, int32_t sequence, const CoordsXYZD& origin);

        void AcceptParameters(GameActionParameterVisitor& visitor) override;

        uint16_t GetActionFlags() const override final;

        void Serialise(DataSerialiser& stream) override;
        GameActions::Result Query() const override;
        GameActions::Result Execute() const override;
    };
} // namespace OpenRCT2
