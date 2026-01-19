/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LandSetRightsAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../actions/LandSetHeightAction.h"
#include "../audio/Audio.h"
#include "../core/Numerics.hpp"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/RideData.h"
#include "../windows/Intent.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/TileElementsView.h"
#include "../world/tile_element/EntranceElement.h"
#include "../world/tile_element/SurfaceElement.h"

namespace OpenRCT2::GameActions
{
    using namespace OpenRCT2::Numerics;

    LandSetRightsAction::LandSetRightsAction(const MapRange& range, LandSetRightSetting setting, uint8_t ownership)
        : _range(range)
        , _setting(setting)
        , _ownership(ownership)
    {
    }

    LandSetRightsAction::LandSetRightsAction(const CoordsXY& coord, LandSetRightSetting setting, uint8_t ownership)
        : _range(coord.x, coord.y, coord.x, coord.y)
        , _setting(setting)
        , _ownership(ownership)
    {
    }

    void LandSetRightsAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit(_range);
        visitor.Visit("setting", _setting);
        visitor.Visit("ownership", _ownership);
    }

    uint16_t LandSetRightsAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::EditorOnly;
    }

    void LandSetRightsAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_range) << DS_TAG(_setting) << DS_TAG(_ownership);
    }

    Result LandSetRightsAction::Query(GameState_t& gameState) const
    {
        return QueryExecute(gameState, false);
    }

    Result LandSetRightsAction::Execute(GameState_t& gameState) const
    {
        return QueryExecute(gameState, true);
    }

    Result LandSetRightsAction::QueryExecute(GameState_t& gameState, bool isExecuting) const
    {
        auto res = Result();

        auto validRange = ClampRangeWithinMap(_range.Normalise());
        CoordsXYZ centre{ (validRange.GetX1() + validRange.GetX2()) / 2 + 16,
                          (validRange.GetY1() + validRange.GetY2()) / 2 + 16, 0 };
        centre.z = TileElementHeight(centre);

        res.position = centre;
        res.expenditure = ExpenditureType::landPurchase;

        if (!isInEditorMode() && !gameState.cheats.sandboxMode)
        {
            return Result(Status::notInEditorMode, kStringIdNone, STR_LAND_NOT_FOR_SALE);
        }

        // Game command modified to accept selection size
        for (auto y = validRange.GetY1(); y <= validRange.GetY2(); y += kCoordsXYStep)
        {
            for (auto x = validRange.GetX1(); x <= validRange.GetX2(); x += kCoordsXYStep)
            {
                if (!LocationValid({ x, y }))
                    continue;
                auto result = MapBuyLandRightsForTile(gameState, { x, y }, isExecuting);
                if (result.error == Status::ok)
                {
                    res.cost += result.cost;
                }
            }
        }

        if (isExecuting)
        {
            MapCountRemainingLandRights();
            Audio::Play3D(Audio::SoundId::placeItem, centre);
        }
        return res;
    }

    Result LandSetRightsAction::MapBuyLandRightsForTile(GameState_t& gameState, const CoordsXY& loc, bool isExecuting) const
    {
        SurfaceElement* surfaceElement = MapGetSurfaceElementAt(loc);
        if (surfaceElement == nullptr)
        {
            LOG_ERROR("No surface at x = %d, y = %d", loc.x, loc.y);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_SURFACE_ELEMENT_NOT_FOUND);
        }

        auto res = Result();
        switch (_setting)
        {
            case LandSetRightSetting::UnownLand:
                if (isExecuting)
                {
                    surfaceElement->SetOwnership(
                        surfaceElement->GetOwnership() & ~(OWNERSHIP_OWNED | OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED));
                    Park::UpdateFencesAroundTile(loc);
                }
                return res;
            case LandSetRightSetting::UnownConstructionRights:
                if (isExecuting)
                {
                    surfaceElement->SetOwnership(surfaceElement->GetOwnership() & ~OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED);
                    uint16_t baseZ = surfaceElement->GetBaseZ();
                    MapInvalidateTile({ loc, baseZ, baseZ + 16 });
                }
                return res;
            case LandSetRightSetting::SetForSale:
                if (isExecuting)
                {
                    surfaceElement->SetOwnership(surfaceElement->GetOwnership() | OWNERSHIP_AVAILABLE);
                    uint16_t baseZ = surfaceElement->GetBaseZ();
                    MapInvalidateTile({ loc, baseZ, baseZ + 16 });
                }
                return res;
            case LandSetRightSetting::SetConstructionRightsForSale:
                if (isExecuting)
                {
                    surfaceElement->SetOwnership(surfaceElement->GetOwnership() | OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE);
                    uint16_t baseZ = surfaceElement->GetBaseZ();
                    MapInvalidateTile({ loc, baseZ, baseZ + 16 });
                }
                return res;
            case LandSetRightSetting::SetOwnershipWithChecks:
            {
                if (_ownership == surfaceElement->GetOwnership())
                {
                    return res;
                }

                for (auto* entranceElement : TileElementsView<EntranceElement>(loc))
                {
                    if (entranceElement->GetEntranceType() != ENTRANCE_TYPE_PARK_ENTRANCE)
                        continue;

                    // Do not allow ownership of park entrance.
                    if (_ownership == OWNERSHIP_OWNED || _ownership == OWNERSHIP_AVAILABLE)
                        return res;

                    // Allow construction rights available / for sale on park entrances on surface.
                    // There is no need to check the height if _ownership is 0 (unowned and no rights available).
                    if (_ownership == OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED
                        || _ownership == OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE)
                    {
                        if (entranceElement->BaseHeight - 3 > surfaceElement->BaseHeight
                            || entranceElement->BaseHeight < surfaceElement->BaseHeight)
                        {
                            return res;
                        }
                    }
                }

                const uint8_t currentOwnership = surfaceElement->GetOwnership();

                // Are land rights or construction rights currently owned?
                if (!(currentOwnership & (OWNERSHIP_OWNED | OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED)))
                {
                    // Buying land
                    if (!(currentOwnership & OWNERSHIP_OWNED) && (_ownership & OWNERSHIP_OWNED))
                        res.cost = gameState.scenarioOptions.landPrice;

                    // Buying construction rights
                    if (!(currentOwnership & OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED)
                        && (_ownership & OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED))
                        res.cost = gameState.scenarioOptions.constructionRightsPrice;
                }
                else
                {
                    // Selling land
                    if ((currentOwnership & OWNERSHIP_OWNED) && !(_ownership & OWNERSHIP_OWNED))
                        res.cost = -gameState.scenarioOptions.landPrice;

                    // Selling construction rights
                    if ((currentOwnership & OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED)
                        && !(_ownership & OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED))
                        res.cost = -gameState.scenarioOptions.constructionRightsPrice;
                }

                if (isExecuting)
                {
                    if (_ownership != OWNERSHIP_UNOWNED)
                    {
                        gameState.peepSpawns.erase(
                            std::remove_if(
                                gameState.peepSpawns.begin(), gameState.peepSpawns.end(),
                                [x = loc.x, y = loc.y](const auto& spawn) {
                                    return floor2(spawn.x, 32) == x && floor2(spawn.y, 32) == y;
                                }),
                            gameState.peepSpawns.end());
                    }
                    surfaceElement->SetOwnership(_ownership);
                    Park::UpdateFencesAroundTile(loc);
                    gMapLandRightsUpdateSuccess = true;
                }
                return res;
            }
            default:
                LOG_ERROR("Invalid setting %u to set land rights", _setting);
                return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
        }
    }
} // namespace OpenRCT2::GameActions
