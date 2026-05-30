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
#include "../core/Money.hpp"
#include "Location.hpp"

namespace OpenRCT2
{
    struct GameState_t;
    struct Guest;
} // namespace OpenRCT2

namespace OpenRCT2::Park
{
    // Park ownership constants (nibbles), used for e.g. legacy park conversion
    constexpr auto kDefaultParkOwnerId = ParkId::FromUnderlying(0);
    constexpr auto kNullOwnerId = ParkId::GetNull();

    struct ParkData;

    void Initialise(ParkData& park, GameState_t& gameState);
    void Update(ParkData& park, GameState_t& gameState);

    uint32_t CalculateParkSize(ParkData& park);
    int32_t CalculateParkRating(const ParkData& park, const GameState_t& gameState);
    money64 CalculateParkValue(const ParkData& park, const GameState_t& gameState);
    money64 CalculateCompanyValue(const ParkData& park);

    Guest* GenerateGuest();

    void ResetHistories(ParkData& park);
    void UpdateHistories(ParkData& park);
    void SetForcedRating(ParkData& park, int32_t rating);
    int32_t GetForcedRating();

    uint32_t UpdateSize(ParkData& park);

    void UpdateFences(const CoordsXY& coords);
    void UpdateFencesAroundTile(const CoordsXY& coords);

    uint8_t CalculateGuestInitialHappiness(uint8_t percentage);

    bool IsOpen(const ParkData& park);
    void SetOpen(const ParkData& park, bool open);
    money64 GetEntranceFee(const ParkData& park);

    bool RidePricesUnlocked(const ParkData& park);
    bool EntranceFeeUnlocked(const ParkData& park);

    void resetTileOwnerData(GameState_t& gameState);
} // namespace OpenRCT2::Park
