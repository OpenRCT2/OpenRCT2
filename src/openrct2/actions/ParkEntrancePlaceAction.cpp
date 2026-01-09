/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ParkEntrancePlaceAction.h"

#include "../Cheats.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../core/Guard.hpp"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../world/ConstructionClearance.h"
#include "../world/Entrance.h"
#include "../world/Footpath.h"
#include "../world/Map.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/tile_element/EntranceElement.h"
#include "../world/tile_element/SurfaceElement.h"

namespace OpenRCT2::GameActions
{
    ParkEntrancePlaceAction::ParkEntrancePlaceAction(
        const CoordsXYZD& location, ObjectEntryIndex pathType, ObjectEntryIndex entranceType, bool pathTypeIsLegacy)
        : _loc(location)
        , _pathType(pathType)
        , _entranceType(entranceType)
    {
    }

    void ParkEntrancePlaceAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit(_loc);
        visitor.Visit("footpathSurfaceObject", _pathType);
        visitor.Visit("entranceObject", _entranceType);
        visitor.Visit("footpathTypeIsLegacy", _pathTypeIsLegacy);
    }

    uint16_t ParkEntrancePlaceAction::GetActionFlags() const
    {
        return GameActionBase::GetActionFlags() | Flags::EditorOnly;
    }

    void ParkEntrancePlaceAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc);
        stream << DS_TAG(_pathType);
        stream << DS_TAG(_entranceType);
        stream << DS_TAG(_pathTypeIsLegacy);
    }

    Result ParkEntrancePlaceAction::Query(GameState_t& gameState) const
    {
        if (!isInEditorMode() && !gameState.cheats.sandboxMode)
        {
            return Result(Status::notInEditorMode, STR_CANT_BUILD_THIS_HERE, kStringIdNone);
        }

        auto res = Result();
        res.expenditure = ExpenditureType::landPurchase;
        res.position = _loc;

        auto mapSizeUnits = GetMapSizeUnits() - CoordsXY{ kCoordsXYStep, kCoordsXYStep };
        if (!LocationValid(_loc) || _loc.x <= kCoordsXYStep || _loc.y <= kCoordsXYStep || _loc.x >= mapSizeUnits.x
            || _loc.y >= mapSizeUnits.y)
        {
            return Result(Status::invalidParameters, STR_CANT_BUILD_THIS_HERE, STR_TOO_CLOSE_TO_EDGE_OF_MAP);
        }

        if (!CheckMapCapacity(3))
        {
            return Result(Status::noFreeElements, STR_CANT_BUILD_THIS_HERE, STR_ERR_LANDSCAPE_DATA_AREA_FULL);
        }

        if (gameState.park.entrances.size() >= Limits::kMaxParkEntrances)
        {
            return Result(Status::invalidParameters, STR_CANT_BUILD_THIS_HERE, STR_ERR_TOO_MANY_PARK_ENTRANCES);
        }

        auto zLow = _loc.z;
        auto zHigh = zLow + ParkEntranceHeight;
        CoordsXYZ entranceLoc = _loc;
        for (uint8_t index = 0; index < 3; index++)
        {
            if (index == 1)
            {
                entranceLoc += CoordsDirectionDelta[(_loc.direction - 1) & 0x3];
            }
            else if (index == 2)
            {
                entranceLoc.x += CoordsDirectionDelta[(_loc.direction + 1) & 0x3].x * 2;
                entranceLoc.y += CoordsDirectionDelta[(_loc.direction + 1) & 0x3].y * 2;
            }

            if (auto res2 = MapCanConstructAt({ entranceLoc, zLow, zHigh }, { 0b1111, 0 }); res2.error != Status::ok)
            {
                res2.errorTitle = STR_CANT_BUILD_THIS_HERE;
                return res2;
            }

            // Check that entrance element does not already exist at this location
            EntranceElement* entranceElement = MapGetParkEntranceElementAt(entranceLoc, false);
            if (entranceElement != nullptr)
            {
                return Result(Status::itemAlreadyPlaced, STR_CANT_BUILD_THIS_HERE, kStringIdNone);
            }
        }

        return res;
    }

    Result ParkEntrancePlaceAction::Execute(GameState_t& gameState) const
    {
        auto res = Result();
        res.expenditure = ExpenditureType::landPurchase;
        res.position = CoordsXYZ{ _loc.x, _loc.y, _loc.z };

        auto flags = GetFlags();

        gameState.park.entrances.push_back(_loc);

        auto zLow = _loc.z;
        auto zHigh = zLow + ParkEntranceHeight;
        CoordsXY entranceLoc = { _loc.x, _loc.y };
        for (uint8_t index = 0; index < 3; index++)
        {
            if (index == 1)
            {
                entranceLoc.x += CoordsDirectionDelta[(_loc.direction - 1) & 0x3].x;
                entranceLoc.y += CoordsDirectionDelta[(_loc.direction - 1) & 0x3].y;
            }
            else if (index == 2)
            {
                entranceLoc.x += CoordsDirectionDelta[(_loc.direction + 1) & 0x3].x * 2;
                entranceLoc.y += CoordsDirectionDelta[(_loc.direction + 1) & 0x3].y * 2;
            }

            if (!flags.has(CommandFlag::ghost))
            {
                SurfaceElement* surfaceElement = MapGetSurfaceElementAt(entranceLoc);
                if (surfaceElement != nullptr)
                {
                    surfaceElement->SetOwnership(OWNERSHIP_UNOWNED);
                }
            }

            auto* entranceElement = TileElementInsert<EntranceElement>(CoordsXYZ{ entranceLoc, zLow }, 0b1111);
            Guard::Assert(entranceElement != nullptr);

            entranceElement->SetClearanceZ(zHigh);
            entranceElement->SetGhost(flags.has(CommandFlag::ghost));
            entranceElement->SetDirection(_loc.direction);
            entranceElement->SetSequenceIndex(index);
            entranceElement->SetEntranceType(ENTRANCE_TYPE_PARK_ENTRANCE);
            entranceElement->setEntryIndex(_entranceType);
            if (!_pathTypeIsLegacy)
            {
                entranceElement->SetSurfaceEntryIndex(_pathType);
            }
            else
            {
                entranceElement->SetLegacyPathEntryIndex(_pathType);
            }

            if (!entranceElement->IsGhost())
            {
                FootpathConnectEdges(entranceLoc, entranceElement->as<TileElement>(), { CommandFlag::apply });
            }

            Park::UpdateFences(entranceLoc);
            Park::UpdateFences({ entranceLoc.x - kCoordsXYStep, entranceLoc.y });
            Park::UpdateFences({ entranceLoc.x + kCoordsXYStep, entranceLoc.y });
            Park::UpdateFences({ entranceLoc.x, entranceLoc.y - kCoordsXYStep });
            Park::UpdateFences({ entranceLoc.x, entranceLoc.y + kCoordsXYStep });

            MapInvalidateTile({ entranceLoc, entranceElement->GetBaseZ(), entranceElement->GetClearanceZ() });

            if (index == 0)
            {
                MapAnimations::MarkTileForInvalidation(TileCoordsXY(entranceLoc));
            }
        }

        return res;
    }

    bool ParkEntrancePlaceAction::CheckMapCapacity(int16_t numTiles) const
    {
        CoordsXYZ entranceLoc = _loc;
        for (uint8_t index = 0; index < 3; index++)
        {
            if (index == 1)
            {
                entranceLoc += CoordsDirectionDelta[(_loc.direction - 1) & 0x3];
            }
            else if (index == 2)
            {
                entranceLoc.x += CoordsDirectionDelta[(_loc.direction + 1) & 0x3].x * 2;
                entranceLoc.y += CoordsDirectionDelta[(_loc.direction + 1) & 0x3].y * 2;
            }
            if (!MapCheckCapacityAndReorganise(entranceLoc, numTiles))
            {
                return false;
            }
        }
        return true;
    }
} // namespace OpenRCT2::GameActions
