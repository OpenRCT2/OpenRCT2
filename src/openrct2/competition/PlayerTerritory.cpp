/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PlayerTerritory.h"

#include "../Context.h"
#include "../GameState.h"
#include "../entity/EntityList.h"
#include "../entity/Guest.h"
#include "../entity/Staff.h"
#include "../ride/Ride.h"
#include "../ride/RideManager.hpp"
#include "../world/Park.h"

#include <algorithm>

namespace OpenRCT2::Competition
{
    // TerritoryBounds implementation
    bool TerritoryBounds::Contains(const TileCoordsXY& coord) const
    {
        return coord.x >= TopLeft.x && coord.x <= BottomRight.x && coord.y >= TopLeft.y && coord.y <= BottomRight.y;
    }

    bool TerritoryBounds::Contains(const CoordsXY& coord) const
    {
        auto tileCoord = TileCoordsXY(coord);
        return Contains(tileCoord);
    }

    // PlayerTerritory implementation
    PlayerTerritory::PlayerTerritory(uint8_t ownerId)
        : _ownerId(ownerId)
    {
    }

    void PlayerTerritory::SetBounds(const TerritoryBounds& bounds)
    {
        _bounds = bounds;
        _hasBounds = true;
    }

    bool PlayerTerritory::CanPlayerBuildHere(uint8_t playerId, const TileCoordsXY& coord) const
    {
        if (!_hasBounds)
            return true; // No restrictions if no bounds set

        if (playerId != _ownerId)
            return false; // Not the owner

        return IsInTerritory(coord);
    }

    bool PlayerTerritory::CanPlayerBuildHere(uint8_t playerId, const CoordsXY& coord) const
    {
        return CanPlayerBuildHere(playerId, TileCoordsXY(coord));
    }

    bool PlayerTerritory::IsInTerritory(const TileCoordsXY& coord) const
    {
        if (!_hasBounds)
            return false;

        return _bounds.Contains(coord);
    }

    bool PlayerTerritory::IsInTerritory(const CoordsXY& coord) const
    {
        return IsInTerritory(TileCoordsXY(coord));
    }

    void PlayerTerritory::UpdateStatistics()
    {
        CalculateTerritoryValue();
        CountGuestsInTerritory();
        CountRidesInTerritory();
        CountStaffInTerritory();
        CalculateRatings();
        CalculateFinances();
    }

    void PlayerTerritory::CalculateTerritoryValue()
    {
        if (!_hasBounds)
        {
            _territoryValue = 0.00_GBP;
            return;
        }

        // TODO: Calculate value of all rides and scenery in territory
        // For now, use ride count as a proxy
        _territoryValue = static_cast<money64>(_rideCount) * 1000.00_GBP;
    }

    void PlayerTerritory::CountGuestsInTerritory()
    {
        if (!_hasBounds)
        {
            _guestCount = 0;
            return;
        }

        _guestCount = 0;

        // Count guests within territory bounds
        for (auto peep : EntityList<Guest>())
        {
            if (IsInTerritory(peep->GetLocation()))
            {
                _guestCount++;
            }
        }
    }

    void PlayerTerritory::CountRidesInTerritory()
    {
        if (!_hasBounds)
        {
            _rideCount = 0;
            return;
        }

        _rideCount = 0;

        auto& gameState = OpenRCT2::getGameState();
        for (auto& ride : RideManager(gameState))
        {
            // Check if ride station is in territory
            if (ride.overallView.IsNull())
                continue;

            auto stationCoord = TileCoordsXY(ride.overallView);
            if (IsInTerritory(stationCoord))
            {
                _rideCount++;
            }
        }
    }

    void PlayerTerritory::CountStaffInTerritory()
    {
        if (!_hasBounds)
        {
            _staffCount = 0;
            return;
        }

        _staffCount = 0;

        // Count staff within territory bounds
        for (auto peep : EntityList<Staff>())
        {
            if (IsInTerritory(peep->GetLocation()))
            {
                _staffCount++;
            }
        }
    }

    void PlayerTerritory::CalculateRatings()
    {
        if (!_hasBounds)
        {
            _averageParkRating = 0;
            _bestRideExcitement = 0;
            return;
        }

        // Calculate best ride excitement in territory
        _bestRideExcitement = 0;
        RideRating_t totalExcitement = 0;
        uint32_t ratingCount = 0;

        auto& gameState = OpenRCT2::getGameState();
        for (auto& ride : RideManager(gameState))
        {
            if (ride.overallView.IsNull())
                continue;

            auto stationCoord = TileCoordsXY(ride.overallView);
            if (IsInTerritory(stationCoord))
            {
                if (ride.ratings.excitement > _bestRideExcitement && ride.ratings.excitement != RideRating::kUndefined)
                {
                    _bestRideExcitement = ride.ratings.excitement;
                }

                if (ride.ratings.excitement != RideRating::kUndefined)
                {
                    totalExcitement += ride.ratings.excitement;
                    ratingCount++;
                }
            }
        }

        // Calculate average park rating (based on rides in territory)
        if (ratingCount > 0)
        {
            _averageParkRating = totalExcitement / ratingCount;
        }
        else
        {
            _averageParkRating = 0;
        }
    }

    void PlayerTerritory::CalculateFinances()
    {
        if (!_hasBounds)
        {
            _totalIncome = 0.00_GBP;
            _totalExpenses = 0.00_GBP;
            return;
        }

        // TODO: Track per-territory income and expenses
        // For now, estimate based on ride count
        _totalIncome = static_cast<money64>(_rideCount) * 100.00_GBP;
        _totalExpenses = static_cast<money64>(_staffCount) * 50.00_GBP;
    }

    // TerritoryManager implementation
    void TerritoryManager::CreateTerritory(uint8_t playerId, const TerritoryBounds& bounds)
    {
        // Remove existing territory for this player if any
        RemoveTerritory(playerId);

        auto territory = std::make_unique<PlayerTerritory>(playerId);
        territory->SetBounds(bounds);
        _territories.push_back(std::move(territory));
    }

    void TerritoryManager::RemoveTerritory(uint8_t playerId)
    {
        _territories.erase(
            std::remove_if(
                _territories.begin(), _territories.end(),
                [playerId](const auto& territory) { return territory->GetOwnerId() == playerId; }),
            _territories.end());
    }

    PlayerTerritory* TerritoryManager::GetTerritory(uint8_t playerId)
    {
        for (auto& territory : _territories)
        {
            if (territory->GetOwnerId() == playerId)
                return territory.get();
        }
        return nullptr;
    }

    const PlayerTerritory* TerritoryManager::GetTerritory(uint8_t playerId) const
    {
        for (const auto& territory : _territories)
        {
            if (territory->GetOwnerId() == playerId)
                return territory.get();
        }
        return nullptr;
    }

    uint8_t TerritoryManager::GetTerritoryOwner(const TileCoordsXY& coord) const
    {
        for (const auto& territory : _territories)
        {
            if (territory->IsInTerritory(coord))
                return territory->GetOwnerId();
        }
        return 0; // No owner (neutral territory)
    }

    uint8_t TerritoryManager::GetTerritoryOwner(const CoordsXY& coord) const
    {
        return GetTerritoryOwner(TileCoordsXY(coord));
    }

    bool TerritoryManager::CanPlayerBuildAt(uint8_t playerId, const TileCoordsXY& coord) const
    {
        // Check if any territory contains this coordinate
        for (const auto& territory : _territories)
        {
            if (territory->IsInTerritory(coord))
            {
                // Found the territory, check if player owns it
                return territory->GetOwnerId() == playerId;
            }
        }

        // No territory owns this location - allow building (neutral zone)
        return true;
    }

    bool TerritoryManager::CanPlayerBuildAt(uint8_t playerId, const CoordsXY& coord) const
    {
        return CanPlayerBuildAt(playerId, TileCoordsXY(coord));
    }

    void TerritoryManager::DivideMapEqually(uint8_t numPlayers, uint32_t mapWidth, uint32_t mapHeight)
    {
        if (numPlayers == 0)
            return;

        auto divisions = CalculateTerritoryDivision(numPlayers, mapWidth, mapHeight);

        // Clear existing territories
        Clear();

        // Create territories (don't assign to players yet)
        for (uint8_t i = 0; i < numPlayers && i < divisions.size(); i++)
        {
            CreateTerritory(i + 1, divisions[i]); // Player IDs start at 1
        }
    }

    void TerritoryManager::AssignTerritoryToPlayer(uint8_t playerId, uint32_t territoryIndex)
    {
        if (territoryIndex >= _territories.size())
            return;

        _territories[territoryIndex]->SetOwnerId(playerId);
    }

    void TerritoryManager::UpdateAllTerritories()
    {
        for (auto& territory : _territories)
        {
            territory->UpdateStatistics();
        }
    }

    void TerritoryManager::Clear()
    {
        _territories.clear();
    }

    std::vector<TerritoryBounds> TerritoryManager::CalculateTerritoryDivision(
        uint8_t numPlayers, uint32_t mapWidth, uint32_t mapHeight)
    {
        std::vector<TerritoryBounds> result;

        if (numPlayers == 0)
            return result;

        // Determine grid layout (rows x cols)
        uint32_t cols = 1;
        uint32_t rows = 1;

        if (numPlayers == 2)
        {
            cols = 2;
            rows = 1;
        }
        else if (numPlayers <= 4)
        {
            cols = 2;
            rows = 2;
        }
        else if (numPlayers <= 6)
        {
            cols = 3;
            rows = 2;
        }
        else if (numPlayers <= 9)
        {
            cols = 3;
            rows = 3;
        }
        else
        {
            cols = 4;
            rows = (numPlayers + 3) / 4; // Ceiling division
        }

        uint32_t territoryWidth = mapWidth / cols;
        uint32_t territoryHeight = mapHeight / rows;

        // Create territories
        for (uint32_t row = 0; row < rows && result.size() < numPlayers; row++)
        {
            for (uint32_t col = 0; col < cols && result.size() < numPlayers; col++)
            {
                TerritoryBounds bounds;
                bounds.TopLeft.x = col * territoryWidth;
                bounds.TopLeft.y = row * territoryHeight;
                bounds.BottomRight.x = (col + 1) * territoryWidth - 1;
                bounds.BottomRight.y = (row + 1) * territoryHeight - 1;

                // Extend last column/row to map edge
                if (col == cols - 1)
                    bounds.BottomRight.x = mapWidth - 1;
                if (row == rows - 1)
                    bounds.BottomRight.y = mapHeight - 1;

                result.push_back(bounds);
            }
        }

        return result;
    }

} // namespace OpenRCT2::Competition
