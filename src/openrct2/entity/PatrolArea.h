/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/MapLimits.h"
#include "Staff.h"

#include <variant>
#include <vector>

namespace OpenRCT2
{
    // The number of elements in the GameState_t.StaffPatrolAreas array per staff member. Every bit in the array represents a
    // 4x4 square. Right now, it's a 32-bit array like in RCT2. 32 * 128 = 4096 bits, which is also the number of 4x4 squares on
    // a 256x256 map.
    constexpr size_t kStaffPatrolAreaBlocksPerLine = kMaximumMapSizeTechnical / 4;
    constexpr size_t kStaffPatrolAreaSize = (kStaffPatrolAreaBlocksPerLine * kStaffPatrolAreaBlocksPerLine) / 32;

    class PatrolArea
    {
    private:
        struct Cell
        {
            static constexpr auto kWidth = 64;
            static constexpr auto kHeight = 64;
            static constexpr auto kNumTiles = kWidth * kHeight;

            std::vector<TileCoordsXY> sortedTiles;
        };

        static constexpr auto kCellColumns = (kMaximumMapSizeTechnical + (Cell::kWidth - 1)) / Cell::kWidth;
        static constexpr auto kCellRows = (kMaximumMapSizeTechnical + (Cell::kHeight - 1)) / Cell::kHeight;
        static constexpr auto kNumCells = kCellColumns * kCellRows;

        std::array<Cell, kNumCells> areas;
        size_t tileCount{};

        const Cell* getCell(const TileCoordsXY& pos) const;
        Cell* getCell(const TileCoordsXY& pos);

    public:
        bool isEmpty() const;
        void clear();
        bool get(const TileCoordsXY& pos) const;
        bool get(const CoordsXY& pos) const;
        void set(const TileCoordsXY& pos, bool value);
        void set(const CoordsXY& pos, bool value);
        void Union(const PatrolArea& other);
        void Union(const std::vector<TileCoordsXY>& other);
        std::vector<TileCoordsXY> toVector() const;
    };

    void UpdateConsolidatedPatrolAreas();
    bool IsPatrolAreaSetForStaffType(StaffType type, const CoordsXY& coords);
    std::variant<StaffType, EntityId> GetPatrolAreaToRender();
    void ClearPatrolAreaToRender();
    void SetPatrolAreaToRender(EntityId staffId);
    void SetPatrolAreaToRender(StaffType staffType);
} // namespace OpenRCT2
