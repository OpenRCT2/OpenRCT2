/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Map.h"
#include "Peep.h"

#include <variant>
namespace OpenRCT2
{
    // The number of elements in the gStaffPatrolAreas array per staff member. Every bit in the array represents a 4x4 square.
    // Right now, it's a 32-bit array like in RCT2. 32 * 128 = 4096 bits, which is also the number of 4x4 squares on a 256x256
    // map.
    constexpr size_t STAFF_PATROL_AREA_BLOCKS_PER_LINE = MAXIMUM_MAP_SIZE_TECHNICAL / 4;
    constexpr size_t STAFF_PATROL_AREA_SIZE = (STAFF_PATROL_AREA_BLOCKS_PER_LINE * STAFF_PATROL_AREA_BLOCKS_PER_LINE) / 32;

    class PatrolArea
    {
    private:
        struct Cell
        {
            static constexpr auto Width = 64;
            static constexpr auto Height = 64;
            static constexpr auto NumTiles = Width * Height;

            std::vector<TileCoordsXY> SortedTiles;
        };

        static constexpr auto CellColumns = (MAXIMUM_MAP_SIZE_TECHNICAL + (Cell::Width - 1)) / Cell::Width;
        static constexpr auto CellRows = (MAXIMUM_MAP_SIZE_TECHNICAL + (Cell::Height - 1)) / Cell::Height;
        static constexpr auto NumCells = CellColumns * CellRows;

        std::array<Cell, NumCells> Areas;
        size_t TileCount{};

        const Cell* GetCell(const TileCoordsXY& pos) const;
        Cell* GetCell(const TileCoordsXY& pos);

    public:
        bool IsEmpty() const;
        void Clear();
        bool Get(const TileCoordsXY& pos) const;
        bool Get(const CoordsXY& pos) const;
        void Set(const TileCoordsXY& pos, bool value);
        void Set(const CoordsXY& pos, bool value);
        void Union(const PatrolArea& other);
        void Union(const std::vector<TileCoordsXY>& other);
        std::vector<TileCoordsXY> ToVector() const;
    };

    void UpdateConsolidatedPatrolAreas();
    bool IsPatrolAreaSetForStaffType(StaffType type, const CoordsXY& coords);
    std::variant<StaffType, EntityId> GetPatrolAreaToRender();
    void ClearPatrolAreaToRender();
    void SetPatrolAreaToRender(EntityId staffId);
    void SetPatrolAreaToRender(StaffType staffType);
} // namespace OpenRCT2
