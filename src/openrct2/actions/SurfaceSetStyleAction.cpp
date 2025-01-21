/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include "../world/Park.h"
#include "../world/tile_element/SurfaceElement.h"

using namespace OpenRCT2;

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

GameActions::Result SurfaceSetStyleAction::Query() const
{
    auto res = GameActions::Result();
    res.ErrorTitle = STR_CANT_CHANGE_LAND_TYPE;
    res.Expenditure = ExpenditureType::Landscaping;

    auto validRange = ClampRangeWithinMap(_range.Normalise());
    auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
    if (_surfaceStyle != kObjectEntryIndexNull)
    {
        const auto surfaceObj = static_cast<TerrainSurfaceObject*>(
            objManager.GetLoadedObject(ObjectType::TerrainSurface, _surfaceStyle));

        if (surfaceObj == nullptr)
        {
            LOG_ERROR("Invalid surface style %u", _surfaceStyle);
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_CANT_CHANGE_LAND_TYPE, STR_UNKNOWN_OBJECT_TYPE);
        }
    }

    if (_edgeStyle != kObjectEntryIndexNull)
    {
        const auto edgeObj = objManager.GetLoadedObject<TerrainEdgeObject>(_edgeStyle);

        if (edgeObj == nullptr)
        {
            LOG_ERROR("Invalid edge style %u", _edgeStyle);
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_CANT_CHANGE_LAND_TYPE, STR_UNKNOWN_OBJECT_TYPE);
        }
    }

    auto xMid = (validRange.GetLeft() + validRange.GetRight()) / 2 + 16;
    auto yMid = (validRange.GetTop() + validRange.GetBottom()) / 2 + 16;
    auto heightMid = TileElementHeight({ xMid, yMid });

    res.Position.x = xMid;
    res.Position.y = yMid;
    res.Position.z = heightMid;

    auto& gameState = GetGameState();

    // Do nothing if not in editor, sandbox mode or landscaping is forbidden
    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gameState.Cheats.sandboxMode
        && (gameState.Park.Flags & PARK_FLAGS_FORBID_LANDSCAPE_CHANGES))
    {
        return GameActions::Result(
            GameActions::Status::Disallowed, STR_CANT_CHANGE_LAND_TYPE, STR_FORBIDDEN_BY_THE_LOCAL_AUTHORITY);
    }

    money64 surfaceCost = 0;
    money64 edgeCost = 0;
    for (CoordsXY coords = { validRange.GetLeft(), validRange.GetTop() }; coords.x <= validRange.GetRight();
         coords.x += kCoordsXYStep)
    {
        for (coords.y = validRange.GetTop(); coords.y <= validRange.GetBottom(); coords.y += kCoordsXYStep)
        {
            if (!LocationValid(coords))
                continue;

            if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gameState.Cheats.sandboxMode)
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
                    const auto surfaceObject = static_cast<TerrainSurfaceObject*>(
                        objManager.GetLoadedObject(ObjectType::TerrainSurface, _surfaceStyle));
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
    res.Cost = surfaceCost + edgeCost;

    return res;
}

GameActions::Result SurfaceSetStyleAction::Execute() const
{
    auto res = GameActions::Result();
    res.ErrorTitle = STR_CANT_CHANGE_LAND_TYPE;
    res.Expenditure = ExpenditureType::Landscaping;

    auto validRange = ClampRangeWithinMap(_range.Normalise());
    auto xMid = (validRange.GetLeft() + validRange.GetRight()) / 2 + 16;
    auto yMid = (validRange.GetTop() + validRange.GetBottom()) / 2 + 16;
    auto heightMid = TileElementHeight({ xMid, yMid });

    res.Position.x = xMid;
    res.Position.y = yMid;
    res.Position.z = heightMid;

    money64 surfaceCost = 0;
    money64 edgeCost = 0;
    for (CoordsXY coords = { validRange.GetLeft(), validRange.GetTop() }; coords.x <= validRange.GetRight();
         coords.x += kCoordsXYStep)
    {
        for (coords.y = validRange.GetTop(); coords.y <= validRange.GetBottom(); coords.y += kCoordsXYStep)
        {
            if (!LocationValid(coords))
                continue;

            if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !GetGameState().Cheats.sandboxMode)
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
                    auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
                    const auto surfaceObject = static_cast<TerrainSurfaceObject*>(
                        objManager.GetLoadedObject(ObjectType::TerrainSurface, _surfaceStyle));
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
    res.Cost = surfaceCost + edgeCost;

    return res;
}
