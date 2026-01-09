/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SurfaceSetStyleAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../management/Finance.h"
#include "../object/ObjectManager.h"
#include "../object/TerrainEdgeObject.h"
#include "../object/TerrainSurfaceObject.h"
#include "../world/Footpath.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/tile_element/SurfaceElement.h"

namespace OpenRCT2::GameActions
{
    SurfaceSetStyleAction::SurfaceSetStyleAction(MapRange range, ObjectEntryIndex surfaceStyle, ObjectEntryIndex edgeStyle)
        : _range(range)
        , _surfaceStyle(surfaceStyle)
        , _edgeStyle(edgeStyle)
    {
    }

    void SurfaceSetStyleAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit(_range);
        visitor.Visit("surfaceStyle", _surfaceStyle);
        visitor.Visit("edgeStyle", _edgeStyle);
    }

    void SurfaceSetStyleAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_range) << DS_TAG(_surfaceStyle) << DS_TAG(_edgeStyle);
    }

    Result SurfaceSetStyleAction::Query(GameState_t& gameState) const
    {
        auto res = Result();
        res.errorTitle = STR_CANT_CHANGE_LAND_TYPE;
        res.expenditure = ExpenditureType::landscaping;

        auto validRange = ClampRangeWithinMap(_range.Normalise());
        auto& objManager = GetContext()->GetObjectManager();
        if (_surfaceStyle != kObjectEntryIndexNull)
        {
            const auto surfaceObj = objManager.GetLoadedObject<TerrainSurfaceObject>(_surfaceStyle);

            if (surfaceObj == nullptr)
            {
                LOG_ERROR("Invalid surface style %u", _surfaceStyle);
                return Result(Status::invalidParameters, STR_CANT_CHANGE_LAND_TYPE, STR_UNKNOWN_OBJECT_TYPE);
            }
        }

        if (_edgeStyle != kObjectEntryIndexNull)
        {
            const auto edgeObj = objManager.GetLoadedObject<TerrainEdgeObject>(_edgeStyle);

            if (edgeObj == nullptr)
            {
                LOG_ERROR("Invalid edge style %u", _edgeStyle);
                return Result(Status::invalidParameters, STR_CANT_CHANGE_LAND_TYPE, STR_UNKNOWN_OBJECT_TYPE);
            }
        }

        auto xMid = (validRange.GetX1() + validRange.GetX2()) / 2 + 16;
        auto yMid = (validRange.GetY1() + validRange.GetY2()) / 2 + 16;
        auto heightMid = TileElementHeight({ xMid, yMid });

        res.position.x = xMid;
        res.position.y = yMid;
        res.position.z = heightMid;

        // Do nothing if not in editor, sandbox mode or landscaping is forbidden
        if (gLegacyScene != LegacyScene::scenarioEditor && !gameState.cheats.sandboxMode
            && (gameState.park.flags & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES))
        {
            return Result(Status::disallowed, STR_CANT_CHANGE_LAND_TYPE, STR_FORBIDDEN_BY_THE_LOCAL_AUTHORITY);
        }

        money64 surfaceCost = 0;
        money64 edgeCost = 0;
        for (CoordsXY coords = { validRange.GetX1(), validRange.GetY1() }; coords.x <= validRange.GetX2();
             coords.x += kCoordsXYStep)
        {
            for (coords.y = validRange.GetY1(); coords.y <= validRange.GetY2(); coords.y += kCoordsXYStep)
            {
                if (!LocationValid(coords))
                    continue;

                if (gLegacyScene != LegacyScene::scenarioEditor && !gameState.cheats.sandboxMode)
                {
                    if (!MapIsLocationInPark(coords))
                        continue;
                }

                auto surfaceElement = MapGetSurfaceElementAt(coords);
                if (surfaceElement == nullptr)
                {
                    continue;
                }

                if (_surfaceStyle != kObjectEntryIndexNull)
                {
                    uint8_t curSurfaceStyle = surfaceElement->GetSurfaceObjectIndex();

                    if (_surfaceStyle != curSurfaceStyle)
                    {
                        const auto* surfaceObject = objManager.GetLoadedObject<TerrainSurfaceObject>(_surfaceStyle);
                        if (surfaceObject != nullptr)
                        {
                            surfaceCost += surfaceObject->Price;
                        }
                    }
                }

                if (_edgeStyle != kObjectEntryIndexNull)
                {
                    uint8_t curEdgeStyle = surfaceElement->GetEdgeObjectIndex();

                    if (_edgeStyle != curEdgeStyle)
                    {
                        edgeCost += 100;
                    }
                }
            }
        }
        res.cost = surfaceCost + edgeCost;

        return res;
    }

    Result SurfaceSetStyleAction::Execute(GameState_t& gameState) const
    {
        auto res = Result();
        res.errorTitle = STR_CANT_CHANGE_LAND_TYPE;
        res.expenditure = ExpenditureType::landscaping;

        auto validRange = ClampRangeWithinMap(_range.Normalise());
        auto xMid = (validRange.GetX1() + validRange.GetX2()) / 2 + 16;
        auto yMid = (validRange.GetY1() + validRange.GetY2()) / 2 + 16;
        auto heightMid = TileElementHeight({ xMid, yMid });

        res.position.x = xMid;
        res.position.y = yMid;
        res.position.z = heightMid;

        money64 surfaceCost = 0;
        money64 edgeCost = 0;
        for (CoordsXY coords = { validRange.GetX1(), validRange.GetY1() }; coords.x <= validRange.GetX2();
             coords.x += kCoordsXYStep)
        {
            for (coords.y = validRange.GetY1(); coords.y <= validRange.GetY2(); coords.y += kCoordsXYStep)
            {
                if (!LocationValid(coords))
                    continue;

                if (gLegacyScene != LegacyScene::scenarioEditor && !gameState.cheats.sandboxMode)
                {
                    if (!MapIsLocationInPark(coords))
                        continue;
                }

                auto surfaceElement = MapGetSurfaceElementAt(coords);
                if (surfaceElement == nullptr)
                {
                    continue;
                }

                if (_surfaceStyle != kObjectEntryIndexNull)
                {
                    uint8_t curSurfaceStyle = surfaceElement->GetSurfaceObjectIndex();

                    if (_surfaceStyle != curSurfaceStyle)
                    {
                        auto& objManager = GetContext()->GetObjectManager();
                        const auto* surfaceObject = objManager.GetLoadedObject<TerrainSurfaceObject>(_surfaceStyle);
                        if (surfaceObject != nullptr)
                        {
                            surfaceCost += surfaceObject->Price;

                            surfaceElement->SetSurfaceObjectIndex(_surfaceStyle);

                            MapInvalidateTileFull(coords);
                            FootpathRemoveLitter({ coords, TileElementHeight(coords) });
                        }
                    }
                }

                if (_edgeStyle != kObjectEntryIndexNull)
                {
                    uint8_t curEdgeStyle = surfaceElement->GetEdgeObjectIndex();

                    if (_edgeStyle != curEdgeStyle)
                    {
                        edgeCost += 100;

                        surfaceElement->SetEdgeObjectIndex(_edgeStyle);
                        MapInvalidateTileFull(coords);
                    }
                }

                if (surfaceElement->CanGrassGrow() && (surfaceElement->GetGrassLength() & 7) != GRASS_LENGTH_CLEAR_0)
                {
                    surfaceElement->SetGrassLength(GRASS_LENGTH_CLEAR_0);
                    MapInvalidateTileFull(coords);
                }
            }
        }
        res.cost = surfaceCost + edgeCost;

        return res;
    }
} // namespace OpenRCT2::GameActions
