/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Money.hpp"
#include "Location.hpp"

struct Guest;

namespace OpenRCT2
{
    struct Date;
    struct GameState_t;
} // namespace OpenRCT2

namespace OpenRCT2::Park
{
    struct ParkData;

    void Initialise(GameState_t& gameState);
    void Update(GameState_t& gameState, const Date& date);

    uint32_t CalculateParkSize();
    int32_t CalculateParkRating();
    money64 CalculateParkValue();
    money64 CalculateCompanyValue();

    Guest* GenerateGuest();

    void ResetHistories(GameState_t& gameState);
    void UpdateHistories(GameState_t& gameState);
    void SetForcedRating(int32_t rating);
    int32_t GetForcedRating();

    uint32_t UpdateSize(GameState_t& gameState);

    void UpdateFences(const CoordsXY& coords);
    void UpdateFencesAroundTile(const CoordsXY& coords);

    uint8_t CalculateGuestInitialHappiness(uint8_t percentage);

    bool IsOpen(const ParkData& park);
    void SetOpen(bool open);
    money64 GetEntranceFee();

    bool RidePricesUnlocked();
    bool EntranceFeeUnlocked();
} // namespace OpenRCT2::Park
