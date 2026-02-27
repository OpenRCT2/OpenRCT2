/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Money.hpp"
#include "../ride/RideRatings.h"
#include "../world/Location.hpp"

#include <vector>
#include <memory>

namespace OpenRCT2::Competition
{
    /**
     * Represents a rectangular territory owned by a player in competitive mode
     */
    struct TerritoryBounds
    {
        TileCoordsXY TopLeft;
        TileCoordsXY BottomRight;

        bool Contains(const TileCoordsXY& coord) const;
        bool Contains(const CoordsXY& coord) const;
        uint32_t GetWidth() const { return BottomRight.x - TopLeft.x + 1; }
        uint32_t GetHeight() const { return BottomRight.y - TopLeft.y + 1; }
        uint32_t GetTileCount() const { return GetWidth() * GetHeight(); }
    };

    /**
     * Manages a player's territory in competitive mode
     */
    class PlayerTerritory
    {
    public:
        PlayerTerritory() = default;
        explicit PlayerTerritory(uint8_t ownerId);
        ~PlayerTerritory() = default;

        // Ownership
        uint8_t GetOwnerId() const { return _ownerId; }
        void SetOwnerId(uint8_t ownerId) { _ownerId = ownerId; }

        // Territory bounds management
        void SetBounds(const TerritoryBounds& bounds);
        const TerritoryBounds& GetBounds() const { return _bounds; }
        bool HasBounds() const { return _hasBounds; }

        // Ownership checks
        bool CanPlayerBuildHere(uint8_t playerId, const TileCoordsXY& coord) const;
        bool CanPlayerBuildHere(uint8_t playerId, const CoordsXY& coord) const;
        bool IsInTerritory(const TileCoordsXY& coord) const;
        bool IsInTerritory(const CoordsXY& coord) const;

        // Statistics (calculated from territory)
        void UpdateStatistics();
        money64 GetTerritoryValue() const { return _territoryValue; }
        uint32_t GetGuestCount() const { return _guestCount; }
        uint32_t GetRideCount() const { return _rideCount; }
        uint32_t GetStaffCount() const { return _staffCount; }
        RideRating_t GetAverageParkRating() const { return _averageParkRating; }
        RideRating_t GetBestRideExcitement() const { return _bestRideExcitement; }
        money64 GetTotalIncome() const { return _totalIncome; }
        money64 GetTotalExpenses() const { return _totalExpenses; }

    private:
        uint8_t _ownerId = 0;
        TerritoryBounds _bounds;
        bool _hasBounds = false;

        // Cached statistics
        money64 _territoryValue = 0.00_GBP;
        uint32_t _guestCount = 0;
        uint32_t _rideCount = 0;
        uint32_t _staffCount = 0;
        RideRating_t _averageParkRating = 0;
        RideRating_t _bestRideExcitement = 0;
        money64 _totalIncome = 0.00_GBP;
        money64 _totalExpenses = 0.00_GBP;

        void CalculateTerritoryValue();
        void CountGuestsInTerritory();
        void CountRidesInTerritory();
        void CountStaffInTerritory();
        void CalculateRatings();
        void CalculateFinances();
    };

    /**
     * Manages all player territories in a competitive game
     */
    class TerritoryManager
    {
    public:
        TerritoryManager() = default;
        ~TerritoryManager() = default;

        // Territory management
        void CreateTerritory(uint8_t playerId, const TerritoryBounds& bounds);
        void RemoveTerritory(uint8_t playerId);
        PlayerTerritory* GetTerritory(uint8_t playerId);
        const PlayerTerritory* GetTerritory(uint8_t playerId) const;

        // Ownership queries
        uint8_t GetTerritoryOwner(const TileCoordsXY& coord) const;
        uint8_t GetTerritoryOwner(const CoordsXY& coord) const;
        bool CanPlayerBuildAt(uint8_t playerId, const TileCoordsXY& coord) const;
        bool CanPlayerBuildAt(uint8_t playerId, const CoordsXY& coord) const;

        // Map division
        void DivideMapEqually(uint8_t numPlayers, uint32_t mapWidth, uint32_t mapHeight);
        void AssignTerritoryToPlayer(uint8_t playerId, uint32_t territoryIndex);

        // Statistics update
        void UpdateAllTerritories();

        // Reset
        void Clear();
        uint32_t GetTerritoryCount() const { return static_cast<uint32_t>(_territories.size()); }

    private:
        std::vector<std::unique_ptr<PlayerTerritory>> _territories;

        std::vector<TerritoryBounds> CalculateTerritoryDivision(
            uint8_t numPlayers, uint32_t mapWidth, uint32_t mapHeight);
    };

} // namespace OpenRCT2::Competition
