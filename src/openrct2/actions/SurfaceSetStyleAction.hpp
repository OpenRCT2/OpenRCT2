/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Context.h"
#include "../OpenRCT2.h"
#include "../management/Finance.h"
#include "../object/ObjectManager.h"
#include "../object/TerrainEdgeObject.h"
#include "../object/TerrainSurfaceObject.h"
#include "../world/Park.h"
#include "../world/Surface.h"
#include "../world/TileElement.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(SurfaceSetStyleAction, GAME_COMMAND_CHANGE_SURFACE_STYLE, GameActionResult)
{
private:
    MapRange _range;
    ObjectEntryIndex _surfaceStyle;
    ObjectEntryIndex _edgeStyle;

public:
    SurfaceSetStyleAction()
    {
    }

    SurfaceSetStyleAction(MapRange range, ObjectEntryIndex surfaceStyle, ObjectEntryIndex edgeStyle)
        : _range(range)
        , _surfaceStyle(surfaceStyle)
        , _edgeStyle(edgeStyle)
    {
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_range) << DS_TAG(_surfaceStyle) << DS_TAG(_edgeStyle);
    }

    GameActionResult::Ptr Query() const override
    {
        auto res = MakeResult();
        res->ErrorTitle = STR_CANT_CHANGE_LAND_TYPE;
        res->Expenditure = ExpenditureType::Landscaping;

        auto normRange = _range.Normalise();
        auto x0 = std::max(normRange.GetLeft(), 32);
        auto y0 = std::max(normRange.GetTop(), 32);
        auto x1 = std::min(normRange.GetRight(), static_cast<int32_t>(gMapSizeMaxXY));
        auto y1 = std::min(normRange.GetBottom(), static_cast<int32_t>(gMapSizeMaxXY));

        MapRange validRange{ x0, y0, x1, y1 };

        auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
        if (_surfaceStyle != OBJECT_ENTRY_INDEX_NULL)
        {
            if (_surfaceStyle > 0x1F)
            {
                log_error("Invalid surface style.");
                return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_CHANGE_LAND_TYPE);
            }

            const auto surfaceObj = static_cast<TerrainSurfaceObject*>(
                objManager.GetLoadedObject(OBJECT_TYPE_TERRAIN_SURFACE, _surfaceStyle));

            if (surfaceObj == nullptr)
            {
                log_error("Invalid surface style.");
                return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_CHANGE_LAND_TYPE);
            }
        }

        if (_edgeStyle != OBJECT_ENTRY_INDEX_NULL)
        {
            if (_edgeStyle > 0xF)
            {
                log_error("Invalid edge style.");
                return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_CHANGE_LAND_TYPE);
            }

            const auto edgeObj = static_cast<TerrainEdgeObject*>(
                objManager.GetLoadedObject(OBJECT_TYPE_TERRAIN_EDGE, _edgeStyle));

            if (edgeObj == nullptr)
            {
                log_error("Invalid edge style.");
                return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_CHANGE_LAND_TYPE);
            }
        }

        auto xMid = (validRange.GetLeft() + validRange.GetRight()) / 2 + 16;
        auto yMid = (validRange.GetTop() + validRange.GetBottom()) / 2 + 16;
        auto heightMid = tile_element_height({ xMid, yMid });

        res->Position.x = xMid;
        res->Position.y = yMid;
        res->Position.z = heightMid;

        // Do nothing if not in editor, sandbox mode or landscaping is forbidden
        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode
            && (gParkFlags & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES))
        {
            return MakeResult(GA_ERROR::DISALLOWED, STR_CANT_CHANGE_LAND_TYPE, STR_FORBIDDEN_BY_THE_LOCAL_AUTHORITY);
        }

        money32 surfaceCost = 0;
        money32 edgeCost = 0;
        for (CoordsXY coords = { validRange.GetLeft(), validRange.GetTop() }; coords.x <= validRange.GetRight();
             coords.x += COORDS_XY_STEP)
        {
            for (coords.y = validRange.GetTop(); coords.y <= validRange.GetBottom(); coords.y += COORDS_XY_STEP)
            {
                if (!LocationValid(coords))
                    continue;

                if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
                {
                    if (!map_is_location_in_park(coords))
                        continue;
                }

                auto surfaceElement = map_get_surface_element_at(coords);
                if (surfaceElement == nullptr)
                {
                    continue;
                }

                if (_surfaceStyle != OBJECT_ENTRY_INDEX_NULL)
                {
                    uint8_t curSurfaceStyle = surfaceElement->GetSurfaceStyle();

                    if (_surfaceStyle != curSurfaceStyle)
                    {
                        const auto surfaceObject = static_cast<TerrainSurfaceObject*>(
                            objManager.GetLoadedObject(OBJECT_TYPE_TERRAIN_SURFACE, _surfaceStyle));
                        if (surfaceObject != nullptr)
                        {
                            surfaceCost += surfaceObject->Price;
                        }
                    }
                }

                if (_edgeStyle != OBJECT_ENTRY_INDEX_NULL)
                {
                    uint8_t curEdgeStyle = surfaceElement->GetEdgeStyle();

                    if (_edgeStyle != curEdgeStyle)
                    {
                        edgeCost += 100;
                    }
                }
            }
        }
        res->Cost = surfaceCost + edgeCost;

        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        auto res = MakeResult();
        res->ErrorTitle = STR_CANT_CHANGE_LAND_TYPE;
        res->Expenditure = ExpenditureType::Landscaping;

        auto normRange = _range.Normalise();
        auto x0 = std::max(normRange.GetLeft(), 32);
        auto y0 = std::max(normRange.GetTop(), 32);
        auto x1 = std::min(normRange.GetRight(), static_cast<int32_t>(gMapSizeMaxXY));
        auto y1 = std::min(normRange.GetBottom(), static_cast<int32_t>(gMapSizeMaxXY));

        MapRange validRange{ x0, y0, x1, y1 };

        auto xMid = (validRange.GetLeft() + validRange.GetRight()) / 2 + 16;
        auto yMid = (validRange.GetTop() + validRange.GetBottom()) / 2 + 16;
        auto heightMid = tile_element_height({ xMid, yMid });

        res->Position.x = xMid;
        res->Position.y = yMid;
        res->Position.z = heightMid;

        money32 surfaceCost = 0;
        money32 edgeCost = 0;
        for (CoordsXY coords = { validRange.GetLeft(), validRange.GetTop() }; coords.x <= validRange.GetRight();
             coords.x += COORDS_XY_STEP)
        {
            for (coords.y = validRange.GetTop(); coords.y <= validRange.GetBottom(); coords.y += COORDS_XY_STEP)
            {
                if (!LocationValid(coords))
                    continue;

                if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
                {
                    if (!map_is_location_in_park(coords))
                        continue;
                }

                auto surfaceElement = map_get_surface_element_at(coords);
                if (surfaceElement == nullptr)
                {
                    continue;
                }

                if (_surfaceStyle != OBJECT_ENTRY_INDEX_NULL)
                {
                    uint8_t curSurfaceStyle = surfaceElement->GetSurfaceStyle();

                    if (_surfaceStyle != curSurfaceStyle)
                    {
                        auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
                        const auto surfaceObject = static_cast<TerrainSurfaceObject*>(
                            objManager.GetLoadedObject(OBJECT_TYPE_TERRAIN_SURFACE, _surfaceStyle));
                        if (surfaceObject != nullptr)
                        {
                            surfaceCost += surfaceObject->Price;

                            surfaceElement->SetSurfaceStyle(_surfaceStyle);

                            map_invalidate_tile_full(coords);
                            footpath_remove_litter({ coords, tile_element_height(coords) });
                        }
                    }
                }

                if (_edgeStyle != OBJECT_ENTRY_INDEX_NULL)
                {
                    uint8_t curEdgeStyle = surfaceElement->GetEdgeStyle();

                    if (_edgeStyle != curEdgeStyle)
                    {
                        edgeCost += 100;

                        surfaceElement->SetEdgeStyle(_edgeStyle);
                        map_invalidate_tile_full(coords);
                    }
                }

                if (surfaceElement->CanGrassGrow() && (surfaceElement->GetGrassLength() & 7) != GRASS_LENGTH_CLEAR_0)
                {
                    surfaceElement->SetGrassLength(GRASS_LENGTH_CLEAR_0);
                    map_invalidate_tile_full(coords);
                }
            }
        }
        res->Cost = surfaceCost + edgeCost;

        return res;
    }
};
