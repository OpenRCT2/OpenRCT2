/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SurfaceSetStyleAction.h"

#include "../../Context.h"
#include "../../Diagnostic.h"
#include "../../GameState.h"
#include "../../OpenRCT2.h"
#include "../../management/Finance.h"
#include "../../object/ObjectManager.h"
#include "../../object/TerrainEdgeObject.h"
#include "../../object/TerrainSurfaceObject.h"
#include "../../world/Footpath.h"
#include "../../world/Map.h"
#include "../../world/tile_element/SurfaceElement.h"

namespace OpenRCT2::GameActions
{
    SurfaceSetStyleAction::SurfaceSetStyleAction(
        MapRange range, ObjectEntryIndex surfaceStyle, ObjectEntryIndex edgeStyle, Drawing::Colour surfaceColour1,
        Drawing::Colour edgeColour1)
        : _range(range)
        , _surfaceStyle(surfaceStyle)
        , _edgeStyle(edgeStyle)
        , _surfaceColour1(surfaceColour1)
        , _edgeColour1(edgeColour1)
    {
    }

    void SurfaceSetStyleAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit(_range);
        visitor.Visit("surfaceStyle", _surfaceStyle);
        visitor.Visit("edgeStyle", _edgeStyle);
        visitor.Visit("surfaceColour1", _surfaceColour1);
        visitor.Visit("edgeColour1", _edgeColour1);
    }

    void SurfaceSetStyleAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_range) << DS_TAG(_surfaceStyle) << DS_TAG(_edgeStyle) << DS_TAG(_surfaceColour1)
               << DS_TAG(_edgeColour1);
    }

    bool SurfaceSetStyleAction::surfaceColour1NeedsRecolour(
        const SurfaceElement& surfaceElement, const TerrainSurfaceObject& surfaceObject) const
    {
        if (!surfaceObject.Flags.has(TerrainSurfaceFlag::hasPrimaryColour))
            return false;

        return _surfaceColour1 != surfaceElement.getPrimarySurfaceColour();
    }

    bool SurfaceSetStyleAction::edgeColour1NeedsRecolour(
        const SurfaceElement& surfaceElement, const TerrainEdgeObject& edgeObject) const
    {
        if (!edgeObject.flags.has(TerrainEdgeFlag::hasPrimaryColour))
            return false;

        return _edgeColour1 != surfaceElement.getPrimaryEdgeColour();
    }

    Result SurfaceSetStyleAction::Query(GameState_t& gameState, Park::ParkData& park) const
    {
        auto res = Result();
        res.errorTitle = STR_CANT_CHANGE_LAND_TYPE;
        res.expenditure = ExpenditureType::landscaping;

        auto validRange = ClampRangeWithinMap(_range.normalise());
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

        auto xMid = (validRange.getX1() + validRange.getX2()) / 2 + 16;
        auto yMid = (validRange.getY1() + validRange.getY2()) / 2 + 16;
        auto heightMid = TileElementHeight({ xMid, yMid });

        res.position.x = xMid;
        res.position.y = yMid;
        res.position.z = heightMid;

        // Do nothing if not in editor, sandbox mode or landscaping is forbidden
        if (gLegacyScene != LegacyScene::scenarioEditor && !gameState.cheats.sandboxMode
            && park.flags.has(ParkFlag::forbidLandscapeChanges))
        {
            return Result(Status::disallowed, STR_CANT_CHANGE_LAND_TYPE, STR_FORBIDDEN_BY_THE_LOCAL_AUTHORITY);
        }

        money64 surfaceCost = 0;
        money64 edgeCost = 0;
        for (CoordsXY coords = { validRange.getX1(), validRange.getY1() }; coords.x <= validRange.getX2();
             coords.x += kCoordsXYStep)
        {
            for (coords.y = validRange.getY1(); coords.y <= validRange.getY2(); coords.y += kCoordsXYStep)
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
                    const auto* surfaceObject = objManager.GetLoadedObject<TerrainSurfaceObject>(_surfaceStyle);
                    if (surfaceObject != nullptr)
                    {
                        auto curSurfaceStyle = surfaceElement->getSurfaceObjectIndex();
                        if (_surfaceStyle != curSurfaceStyle || surfaceColour1NeedsRecolour(*surfaceElement, *surfaceObject))
                        {
                            surfaceCost += surfaceObject->Price;
                        }
                    }
                }

                if (_edgeStyle != kObjectEntryIndexNull)
                {
                    const auto* edgeObject = objManager.GetLoadedObject<TerrainEdgeObject>(_edgeStyle);
                    if (edgeObject != nullptr)
                    {
                        auto curEdgeStyle = surfaceElement->getEdgeObjectIndex();
                        if (_edgeStyle != curEdgeStyle || edgeColour1NeedsRecolour(*surfaceElement, *edgeObject))
                        {
                            edgeCost += 100;
                        }
                    }
                }
            }
        }
        res.cost = surfaceCost + edgeCost;

        return res;
    }

    Result SurfaceSetStyleAction::Execute(GameState_t& gameState, Park::ParkData& park) const
    {
        auto res = Result();
        res.errorTitle = STR_CANT_CHANGE_LAND_TYPE;
        res.expenditure = ExpenditureType::landscaping;

        auto validRange = ClampRangeWithinMap(_range.normalise());
        auto xMid = (validRange.getX1() + validRange.getX2()) / 2 + 16;
        auto yMid = (validRange.getY1() + validRange.getY2()) / 2 + 16;
        auto heightMid = TileElementHeight({ xMid, yMid });

        res.position.x = xMid;
        res.position.y = yMid;
        res.position.z = heightMid;

        money64 surfaceCost = 0;
        money64 edgeCost = 0;
        for (CoordsXY coords = { validRange.getX1(), validRange.getY1() }; coords.x <= validRange.getX2();
             coords.x += kCoordsXYStep)
        {
            for (coords.y = validRange.getY1(); coords.y <= validRange.getY2(); coords.y += kCoordsXYStep)
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
                    auto& objManager = GetContext()->GetObjectManager();
                    const auto* surfaceObject = objManager.GetLoadedObject<TerrainSurfaceObject>(_surfaceStyle);
                    if (surfaceObject != nullptr)
                    {
                        auto curSurfaceStyle = surfaceElement->getSurfaceObjectIndex();
                        if (_surfaceStyle != curSurfaceStyle || surfaceColour1NeedsRecolour(*surfaceElement, *surfaceObject))
                        {
                            surfaceCost += surfaceObject->Price;

                            surfaceElement->setSurfaceObjectIndex(_surfaceStyle);
                            if (surfaceObject->Flags.has(TerrainSurfaceFlag::hasPrimaryColour))
                                surfaceElement->setPrimarySurfaceColour(surfaceObject->getPrimaryColour(_surfaceColour1));

                            MapInvalidateTileFull(coords);
                            FootpathRemoveLitter({ coords, TileElementHeight(coords) });
                        }
                    }
                }

                if (_edgeStyle != kObjectEntryIndexNull)
                {
                    auto& objManager = GetContext()->GetObjectManager();
                    const auto* edgeObject = objManager.GetLoadedObject<TerrainEdgeObject>(_edgeStyle);
                    if (edgeObject != nullptr)
                    {
                        auto curEdgeStyle = surfaceElement->getEdgeObjectIndex();
                        if (_edgeStyle != curEdgeStyle || edgeColour1NeedsRecolour(*surfaceElement, *edgeObject))
                        {
                            edgeCost += 100;

                            surfaceElement->setEdgeObjectIndex(_edgeStyle);
                            surfaceElement->setPrimaryEdgeColour(_edgeColour1);
                            MapInvalidateTileFull(coords);
                        }
                    }
                }

                if (surfaceElement->canGrassGrow() && (surfaceElement->getGrassLength() & 7) != GRASS_LENGTH_CLEAR_0)
                {
                    surfaceElement->setGrassLength(GRASS_LENGTH_CLEAR_0);
                    MapInvalidateTileFull(coords);
                }
            }
        }
        res.cost = surfaceCost + edgeCost;

        return res;
    }
} // namespace OpenRCT2::GameActions
