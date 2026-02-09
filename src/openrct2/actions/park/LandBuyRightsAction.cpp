/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LandBuyRightsAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../actions/LandSetHeightAction.h"
#include "../audio/Audio.h"
#include "../core/EnumUtils.hpp"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/RideData.h"
#include "../windows/Intent.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/tile_element/SurfaceElement.h"

namespace OpenRCT2::GameActions
{
    LandBuyRightsAction::LandBuyRightsAction(const MapRange& range, LandBuyRightSetting setting)
        : _range(range)
        , _setting(setting)
    {
    }

    LandBuyRightsAction::LandBuyRightsAction(const CoordsXY& coord, LandBuyRightSetting setting)
        : _range(coord.x, coord.y, coord.x, coord.y)
        , _setting(setting)
    {
    }

    void LandBuyRightsAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit(_range);
        visitor.Visit("setting", _setting);
    }

    uint16_t LandBuyRightsAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags();
    }

    void LandBuyRightsAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_range) << DS_TAG(_setting);
    }

    Result LandBuyRightsAction::Query(GameState_t& gameState) const
    {
        return QueryExecute(gameState, false);
    }

    Result LandBuyRightsAction::Execute(GameState_t& gameState) const
    {
        return QueryExecute(gameState, true);
    }

    Result LandBuyRightsAction::QueryExecute(GameState_t& gameState, bool isExecuting) const
    {
        auto res = Result();

        MapRange normRange = _range.Normalise();
        // Keep big coordinates within map boundaries
        auto mapSizeMaxXY = GetMapSizeMaxXY();
        auto aX = std::max<decltype(normRange.GetX1())>(32, normRange.GetX1());
        auto bX = std::min<decltype(normRange.GetX2())>(mapSizeMaxXY.x, normRange.GetX2());
        auto aY = std::max<decltype(normRange.GetY1())>(32, normRange.GetY1());
        auto bY = std::min<decltype(normRange.GetY2())>(mapSizeMaxXY.y, normRange.GetY2());

        MapRange validRange = MapRange{ aX, aY, bX, bY };

        CoordsXYZ centre{ (validRange.GetX1() + validRange.GetX2()) / 2 + 16,
                          (validRange.GetY1() + validRange.GetY2()) / 2 + 16, 0 };
        centre.z = TileElementHeight(centre);

        res.position = centre;
        res.expenditure = ExpenditureType::landPurchase;

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
        }
        return res;
    }

    Result LandBuyRightsAction::MapBuyLandRightsForTile(GameState_t& gameState, const CoordsXY& loc, bool isExecuting) const
    {
        if (_setting >= LandBuyRightSetting::Count)
        {
            LOG_ERROR("Invalid land buying setting %u", _setting);
            return Result(Status::invalidParameters, kErrorTitles[0], STR_ERR_VALUE_OUT_OF_RANGE);
        }

        SurfaceElement* surfaceElement = MapGetSurfaceElementAt(loc);
        if (surfaceElement == nullptr)
        {
            LOG_ERROR("No surface at x = %d, y = %d", loc.x, loc.y);
            return Result(Status::invalidParameters, kErrorTitles[EnumValue(_setting)], STR_ERR_SURFACE_ELEMENT_NOT_FOUND);
        }

        auto res = Result();
        switch (_setting)
        {
            case LandBuyRightSetting::BuyLand: // 0
                if ((surfaceElement->GetOwnership() & OWNERSHIP_OWNED) != 0)
                { // If the land is already owned
                    return res;
                }

                if (gLegacyScene == LegacyScene::scenarioEditor || (surfaceElement->GetOwnership() & OWNERSHIP_AVAILABLE) == 0)
                {
                    return Result(Status::notOwned, kErrorTitles[EnumValue(_setting)], STR_LAND_NOT_FOR_SALE);
                }
                if (isExecuting)
                {
                    surfaceElement->SetOwnership(OWNERSHIP_OWNED);
                    Park::UpdateFencesAroundTile(loc);
                }
                res.cost = gameState.scenarioOptions.landPrice;
                return res;

            case LandBuyRightSetting::BuyConstructionRights: // 2
                if ((surfaceElement->GetOwnership() & (OWNERSHIP_OWNED | OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED)) != 0)
                { // If the land or construction rights are already owned
                    return res;
                }

                if (gLegacyScene == LegacyScene::scenarioEditor
                    || (surfaceElement->GetOwnership() & OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE) == 0)
                {
                    return Result(Status::notOwned, kErrorTitles[EnumValue(_setting)], STR_CONSTRUCTION_RIGHTS_NOT_FOR_SALE);
                }

                if (isExecuting)
                {
                    surfaceElement->SetOwnership(surfaceElement->GetOwnership() | OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED);
                    uint16_t baseZ = surfaceElement->GetBaseZ();
                    MapInvalidateTile({ loc, baseZ, baseZ + 16 });
                }
                res.cost = gameState.scenarioOptions.constructionRightsPrice;
                return res;

            default:
                LOG_ERROR("Invalid land buying setting %u", _setting);
                return Result(Status::invalidParameters, kErrorTitles[0], STR_ERR_VALUE_OUT_OF_RANGE);
        }
    }
} // namespace OpenRCT2::GameActions
