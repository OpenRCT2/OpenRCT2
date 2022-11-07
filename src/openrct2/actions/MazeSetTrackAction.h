/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"
// clang-format off
/** rct2: 0x00993CE9 */
static constexpr const uint8_t byte_993CE9[] = {
    0xFF, 0xE0, 0xFF,
    14, 0, 1, 2,
    6, 2, 4, 5,
    9, 10, 6, 8,
    12, 13, 14, 10,
};

/** rct2: 0x00993CFC */
static constexpr const uint8_t byte_993CFC[] = {
    5, 12, 0xFF, 0xFF,
    9, 0, 0xFF, 0xFF,
    13, 4, 0xFF, 0xFF,
    1, 8, 0xFF, 0xFF,
};

/** rct2: 0x00993D0C */
static constexpr const uint8_t byte_993D0C[] = {
    3, 0, 0xFF, 0xFF,
    0, 1, 0xFF, 0xFF,
    1, 2, 0xFF, 0xFF,
    2, 3, 0xFF, 0xFF,
};
// clang-format on

class MazeSetTrackAction final : public GameActionBase<GameCommand::SetMazeTrack>
{
private:
    CoordsXYZD _loc;
    bool _initialPlacement{};
    RideId _rideIndex{ RideId::GetNull() };
    uint8_t _mode{};

public:
    MazeSetTrackAction() = default;
    MazeSetTrackAction(const CoordsXYZD& location, bool initialPlacement, RideId rideIndex, uint8_t mode);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;
    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;

private:
    uint8_t MazeGetSegmentBit(const CoordsXY&) const;
};
