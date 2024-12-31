/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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

static PatrolArea _consolidatedPatrolArea[EnumValue(StaffType::Count)];
static std::variant<StaffType, EntityId> _patrolAreaToRender;

static bool CompareTileCoordsXY(const TileCoordsXY& lhs, const TileCoordsXY& rhs)
{
    if (lhs.y == rhs.y)
        return lhs.x < rhs.x;
    return lhs.y < rhs.y;
}

const PatrolArea::Cell* PatrolArea::GetCell(const TileCoordsXY& pos) const
{
    return const_cast<PatrolArea*>(this)->GetCell(pos);
}

PatrolArea::Cell* PatrolArea::GetCell(const TileCoordsXY& pos)
{
    auto areaPos = TileCoordsXY(pos.x / Cell::Width, pos.y / Cell::Height);
    if (areaPos.x < 0 || areaPos.x >= CellColumns || areaPos.y < 0 || areaPos.y >= CellRows)
        return nullptr;

    auto& area = Areas[(areaPos.y * CellColumns) + areaPos.x];
    return &area;
}

bool PatrolArea::IsEmpty() const
{
    return TileCount == 0;
}

void PatrolArea::Clear()
{
    for (auto& area : Areas)
    {
        area.SortedTiles.clear();
    }
}

bool PatrolArea::Get(const TileCoordsXY& pos) const
{
    auto* area = GetCell(pos);
    if (area == nullptr)
        return false;

    auto it = BinaryFind(area->SortedTiles.begin(), area->SortedTiles.end(), pos, CompareTileCoordsXY);
    auto found = it != area->SortedTiles.end();
    return found;
}

bool PatrolArea::Get(const CoordsXY& pos) const
{
    return Get(TileCoordsXY(pos));
}

void PatrolArea::Set(const TileCoordsXY& pos, bool value)
{
    auto* area = GetCell(pos);
    if (area == nullptr)
        return;

    auto it = std::lower_bound(area->SortedTiles.begin(), area->SortedTiles.end(), pos, CompareTileCoordsXY);
    auto found = it != area->SortedTiles.end() && *it == pos;

    if (!found && value)
    {
        area->SortedTiles.insert(it, pos);
        TileCount++;
    }
    else if (found && !value)
    {
        area->SortedTiles.erase(it);
        assert(TileCount != 0);
        TileCount--;
    }
}

void PatrolArea::Set(const CoordsXY& pos, bool value)
{
    Set(TileCoordsXY(pos), value);
}

void PatrolArea::Union(const PatrolArea& other)
{
    for (size_t i = 0; i < Areas.size(); i++)
    {
        for (const auto& pos : other.Areas[i].SortedTiles)
        {
            Set(pos, true);
        }
    }
}

void PatrolArea::Union(const std::vector<TileCoordsXY>& other)
{
    for (const auto& pos : other)
    {
        Set(pos, true);
    }
}

std::vector<TileCoordsXY> PatrolArea::ToVector() const
{
    std::vector<TileCoordsXY> result;
    for (const auto& area : Areas)
    {
        for (const auto& pos : area.SortedTiles)
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
    for (int32_t staffType = 0; staffType < EnumValue(StaffType::Count); ++staffType)
    {
        // Reset all of the merged data for the type.
        auto& mergedArea = _consolidatedPatrolArea[staffType];
        mergedArea.Clear();

        for (auto staff : EntityList<Staff>())
        {
            if (EnumValue(staff->AssignedStaffType) != staffType)
                continue;

            if (staff->PatrolInfo == nullptr)
                continue;

            mergedArea.Union(*staff->PatrolInfo);
        }
    }
}

bool IsPatrolAreaSetForStaffType(StaffType type, const CoordsXY& coords)
{
    return _consolidatedPatrolArea[EnumValue(type)].Get(coords);
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
