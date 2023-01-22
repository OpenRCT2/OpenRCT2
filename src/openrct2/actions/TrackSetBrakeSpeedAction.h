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
    class TrackSetBrakeSpeedAction final : public GameActionBase<GameCommand::SetBrakesSpeed>
    {
    private:
        CoordsXYZ _loc;
        track_type_t _trackType{};
        uint8_t _brakeSpeed{};

    public:
        TrackSetBrakeSpeedAction() = default;
        TrackSetBrakeSpeedAction(const CoordsXYZ& loc, track_type_t trackType, uint8_t brakeSpeed);

        void AcceptParameters(GameActionParameterVisitor& visitor) override;

        uint16_t GetActionFlags() const override final;

        void Serialise(DataSerialiser& stream) override;
        GameActions::Result Query() const override;
        GameActions::Result Execute() const override;

    private:
        GameActions::Result QueryExecute(bool isExecuting) const;
    };
} // namespace OpenRCT2
