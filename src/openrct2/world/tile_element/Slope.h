/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

constexpr uint8_t kTileSlopeFlat = 0;
constexpr uint8_t kTileSlopeNCornerUp = 0b00000001;
constexpr uint8_t kTileSlopeECornerUp = 0b00000010;
constexpr uint8_t kTileSlopeSCornerUp = 0b00000100;
constexpr uint8_t kTileSlopeWCornerUp = 0b00001000;
constexpr uint8_t kTileSlopeRaisedCornersMask = 0b00001111;
constexpr uint8_t kTileSlopeDiagonalFlag = 0b00010000;
constexpr uint8_t kTileSlopeMask = (kTileSlopeDiagonalFlag | kTileSlopeRaisedCornersMask);

constexpr uint8_t kTileSlopeWCornerDown = kTileSlopeRaisedCornersMask & ~kTileSlopeWCornerUp;
constexpr uint8_t kTileSlopeSCornerDown = kTileSlopeRaisedCornersMask & ~kTileSlopeSCornerUp;
constexpr uint8_t kTileSlopeECornerDown = kTileSlopeRaisedCornersMask & ~kTileSlopeECornerUp;
constexpr uint8_t kTileSlopeNCornerDown = kTileSlopeRaisedCornersMask & ~kTileSlopeNCornerUp;

constexpr uint8_t kTileSlopeNESideUp = kTileSlopeNCornerUp | kTileSlopeECornerUp;
constexpr uint8_t kTileSlopeSESideUp = kTileSlopeECornerUp | kTileSlopeSCornerUp;
constexpr uint8_t kTileSlopeNWSideUp = kTileSlopeNCornerUp | kTileSlopeWCornerUp;
constexpr uint8_t kTileSlopeSWSideUp = kTileSlopeSCornerUp | kTileSlopeWCornerUp;

constexpr uint8_t kTileSlopeWEValley = kTileSlopeECornerUp | kTileSlopeWCornerUp;
constexpr uint8_t kTileSlopeNSValley = kTileSlopeNCornerUp | kTileSlopeSCornerUp;

// Special value, used for support drawing
constexpr uint8_t kTileSlopeAboveTrackOrScenery = 0b00100000;
// Special value, used when raising/lowering land corners/tiles
constexpr uint8_t kTileSlopeRaiseOrLowerBaseHeight = 0b00100000;
