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

class TrackRemoveAction final : public GameActionBase<GameCommand::RemoveTrack>
{
private:
    OpenRCT2::TrackElemType _trackType{};
    int32_t _sequence{};
    CoordsXYZD _origin;

public:
    TrackRemoveAction() = default;
    TrackRemoveAction(OpenRCT2::TrackElemType trackType, int32_t sequence, const CoordsXYZD& origin);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override final;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;
};
