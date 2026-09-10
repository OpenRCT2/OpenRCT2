/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PatrolArea.h"

#include "../core/Algorithm.hpp"
#include "EntityList.h"
#include "Staff.h"

#include <cassert>

namespace OpenRCT2
{
    using namespace OpenRCT2::Core;

    static PatrolArea _consolidatedPatrolArea[EnumValue(StaffType::count)];
    static std::variant<StaffType, EntityId> _patrolAreaToRender;

    static bool CompareTileCoordsXY(const TileCoordsXY& lhs, const TileCoordsXY& rhs)
    {
        if (lhs.y == rhs.y)
            return lhs.x < rhs.x;
        return lhs.y < rhs.y;
    }

    const PatrolArea::Cell* PatrolArea::getCell(const TileCoordsXY& pos) const
    {
        return const_cast<PatrolArea*>(this)->getCell(pos);
    }

    PatrolArea::Cell* PatrolArea::getCell(const TileCoordsXY& pos)
    {
        auto areaPos = TileCoordsXY(pos.x / Cell::kWidth, pos.y / Cell::kHeight);
        if (areaPos.x < 0 || areaPos.x >= kCellColumns || areaPos.y < 0 || areaPos.y >= kCellRows)
            return nullptr;

        auto& area = areas[(areaPos.y * kCellColumns) + areaPos.x];
        return &area;
    }

    bool PatrolArea::isEmpty() const
    {
        return tileCount == 0;
    }

    void PatrolArea::clear()
    {
        for (auto& area : areas)
        {
            area.sortedTiles.clear();
        }
    }

    bool PatrolArea::get(const TileCoordsXY& pos) const
    {
        auto* area = getCell(pos);
        if (area == nullptr)
            return false;

        auto it = Algorithm::binaryFind(area->sortedTiles.begin(), area->sortedTiles.end(), pos, CompareTileCoordsXY);
        auto found = it != area->sortedTiles.end();
        return found;
    }

    bool PatrolArea::get(const CoordsXY& pos) const
    {
        return get(TileCoordsXY(pos));
    }

    void PatrolArea::set(const TileCoordsXY& pos, bool value)
    {
        auto* area = getCell(pos);
        if (area == nullptr)
            return;

        auto it = std::lower_bound(area->sortedTiles.begin(), area->sortedTiles.end(), pos, CompareTileCoordsXY);
        auto found = it != area->sortedTiles.end() && *it == pos;

        if (!found && value)
        {
            area->sortedTiles.insert(it, pos);
            tileCount++;
        }
        else if (found && !value)
        {
            area->sortedTiles.erase(it);
            assert(tileCount != 0);
            tileCount--;
        }
    }

    void PatrolArea::set(const CoordsXY& pos, bool value)
    {
        set(TileCoordsXY(pos), value);
    }

    void PatrolArea::unify(const PatrolArea& other)
    {
        for (size_t i = 0; i < areas.size(); i++)
        {
            for (const auto& pos : other.areas[i].sortedTiles)
            {
                set(pos, true);
            }
        }
    }

    void PatrolArea::unify(const std::vector<TileCoordsXY>& other)
    {
        for (const auto& pos : other)
        {
            set(pos, true);
        }
    }

    std::vector<TileCoordsXY> PatrolArea::toVector() const
    {
        std::vector<TileCoordsXY> result;
        for (const auto& area : areas)
        {
            for (const auto& pos : area.sortedTiles)
            {
                result.push_back(pos);
            }
        }
        return result;
    }

    const PatrolArea& GetMergedPatrolArea(const StaffType type)
    {
        return _consolidatedPatrolArea[EnumValue(type)];
    }

    void UpdateConsolidatedPatrolAreas()
    {
        for (int32_t staffType = 0; staffType < EnumValue(StaffType::count); ++staffType)
        {
            // Reset all of the merged data for the type.
            auto& mergedArea = _consolidatedPatrolArea[staffType];
            mergedArea.clear();

            for (auto staff : EntityList<Staff>())
            {
                if (EnumValue(staff->assignedStaffType) != staffType)
                    continue;

                if (staff->patrolInfo == nullptr)
                    continue;

                mergedArea.unify(*staff->patrolInfo);
            }
        }
    }

    bool IsPatrolAreaSetForStaffType(StaffType type, const CoordsXY& coords)
    {
        return _consolidatedPatrolArea[EnumValue(type)].get(coords);
    }

    std::variant<StaffType, EntityId> GetPatrolAreaToRender()
    {
        return _patrolAreaToRender;
    }

    void ClearPatrolAreaToRender()
    {
        SetPatrolAreaToRender(EntityId::GetNull());
    }

    void SetPatrolAreaToRender(EntityId staffId)
    {
        _patrolAreaToRender = staffId;
    }

    void SetPatrolAreaToRender(StaffType staffType)
    {
        _patrolAreaToRender = staffType;
    }
} // namespace OpenRCT2
