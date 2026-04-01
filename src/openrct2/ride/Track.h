/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../actions/CommandFlag.h"
#include "../world/Location.hpp"
#include "RideTypes.h"

#include <optional>

constexpr uint8_t kRCT2DefaultBlockBrakeSpeed = 2;
constexpr int32_t kBlockBrakeBaseSpeed = 0x20364;
constexpr int32_t kBlockBrakeSpeedOffset = kBlockBrakeBaseSpeed - (kRCT2DefaultBlockBrakeSpeed << 16);
constexpr auto kTrackDirectionDiagonalMask = 0b0100;

constexpr uint8_t kMaximumTrackSpeed = 30;

constexpr int8_t kMaxStationPlatformLength = 32;
constexpr uint16_t kMaximumTrackHeight = 254 * kCoordsZStep;
constexpr uint8_t kDefaultSeatRotation = 4;

namespace OpenRCT2
{
    struct TileElement;

    namespace TrackMetadata
    {
        enum class TrackRoll : uint8_t;
    } // namespace TrackMetadata
} // namespace OpenRCT2

struct ResultWithMessage;

int32_t TrackIsConnectedByShape(OpenRCT2::TileElement* a, OpenRCT2::TileElement* b);

OpenRCT2::TrackMetadata::TrackRoll TrackGetActualBank(
    OpenRCT2::TileElement* tileElement, OpenRCT2::TrackMetadata::TrackRoll bank);
OpenRCT2::TrackMetadata::TrackRoll TrackGetActualBank2(
    ride_type_t rideType, bool isInverted, OpenRCT2::TrackMetadata::TrackRoll bank);
OpenRCT2::TrackMetadata::TrackRoll TrackGetActualBank3(bool useInvertedSprites, OpenRCT2::TileElement* tileElement);

ResultWithMessage TrackAddStationElement(
    CoordsXYZD loc, RideId rideIndex, OpenRCT2::GameActions::CommandFlags flags, bool fromTrackDesign);
ResultWithMessage TrackRemoveStationElement(const CoordsXYZD& loc, RideId rideIndex, OpenRCT2::GameActions::CommandFlags flags);

std::optional<CoordsXYZD> GetTrackSegmentOrigin(const CoordsXYE& posEl);

constexpr bool TrackPieceDirectionIsDiagonal(const uint8_t direction)
{
    return direction & kTrackDirectionDiagonalMask;
}

bool TrackGetIsSheltered(const CoordsXYZ& input);
