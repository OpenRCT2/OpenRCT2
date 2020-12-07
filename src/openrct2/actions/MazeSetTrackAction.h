/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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

DEFINE_GAME_ACTION(MazeSetTrackAction, GAME_COMMAND_SET_MAZE_TRACK, GameActions::Result)
{
private:
    CoordsXYZD _loc;
    bool _initialPlacement{};
    NetworkRideId_t _rideIndex{ RIDE_ID_NULL };
    uint8_t _mode{};

    uint8_t MazeGetSegmentBit(uint16_t x, uint16_t y) const;

public:
    MazeSetTrackAction() = default;
    MazeSetTrackAction(const CoordsXYZD& location, bool initialPlacement, NetworkRideId_t rideIndex, uint8_t mode)
        : _loc(location)
        , _initialPlacement(initialPlacement)
        , _rideIndex(rideIndex)
        , _mode(mode)
    {
    }

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

    void Serialise(DataSerialiser & stream) override;

    GameActions::Result::Ptr Query() const override;

    GameActions::Result::Ptr Execute() const override;
};
