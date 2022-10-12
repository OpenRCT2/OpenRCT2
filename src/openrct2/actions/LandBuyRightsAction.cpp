/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LandBuyRightsAction.h"

#include "../Context.h"
#include "../OpenRCT2.h"
#include "../actions/LandSetHeightAction.h"
#include "../audio/audio.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/RideData.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Surface.h"

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

uint16_t LandBuyRightsAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

void LandBuyRightsAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_range) << DS_TAG(_setting);
}

GameActions::Result LandBuyRightsAction::Query() const
{
    return QueryExecute(false);
}

GameActions::Result LandBuyRightsAction::Execute() const
{
    return QueryExecute(true);
}

GameActions::Result LandBuyRightsAction::QueryExecute(bool isExecuting) const
{
    auto res = GameActions::Result();

    MapRange normRange = _range.Normalise();
    // Keep big coordinates within map boundaries
    auto mapSizeMaxXY = GetMapSizeMaxXY();
    auto aX = std::max<decltype(normRange.GetLeft())>(32, normRange.GetLeft());
    auto bX = std::min<decltype(normRange.GetRight())>(mapSizeMaxXY.x, normRange.GetRight());
    auto aY = std::max<decltype(normRange.GetTop())>(32, normRange.GetTop());
    auto bY = std::min<decltype(normRange.GetBottom())>(mapSizeMaxXY.y, normRange.GetBottom());

    MapRange validRange = MapRange{ aX, aY, bX, bY };

    CoordsXYZ centre{ (validRange.GetLeft() + validRange.GetRight()) / 2 + 16,
                      (validRange.GetTop() + validRange.GetBottom()) / 2 + 16, 0 };
    centre.z = TileElementHeight(centre);

    res.Position = centre;
    res.Expenditure = ExpenditureType::LandPurchase;

    // Game command modified to accept selection size
    for (auto y = validRange.GetTop(); y <= validRange.GetBottom(); y += COORDS_XY_STEP)
    {
        for (auto x = validRange.GetLeft(); x <= validRange.GetRight(); x += COORDS_XY_STEP)
        {
            if (!LocationValid({ x, y }))
                continue;
            auto result = map_buy_land_rights_for_tile({ x, y }, isExecuting);
            if (result.Error == GameActions::Status::Ok)
            {
                res.Cost += result.Cost;
            }
        }
    }
    if (isExecuting)
    {
        MapCountRemainingLandRights();
    }
    return res;
}

GameActions::Result LandBuyRightsAction::map_buy_land_rights_for_tile(const CoordsXY& loc, bool isExecuting) const
{
    if (_setting >= LandBuyRightSetting::Count)
    {
        log_warning("Tried calling buy land rights with an incorrect setting. setting = %u", _setting);
        return GameActions::Result(GameActions::Status::InvalidParameters, _ErrorTitles[0], STR_NONE);
    }

    SurfaceElement* surfaceElement = MapGetSurfaceElementAt(loc);
    if (surfaceElement == nullptr)
    {
        log_error("Could not find surface. x = %d, y = %d", loc.x, loc.y);
        return GameActions::Result(GameActions::Status::InvalidParameters, _ErrorTitles[EnumValue(_setting)], STR_NONE);
    }

    auto res = GameActions::Result();
    switch (_setting)
    {
        case LandBuyRightSetting::BuyLand: // 0
            if ((surfaceElement->GetOwnership() & OWNERSHIP_OWNED) != 0)
            { // If the land is already owned
                return res;
            }

            if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) != 0
                || (surfaceElement->GetOwnership() & OWNERSHIP_AVAILABLE) == 0)
            {
                return GameActions::Result(
                    GameActions::Status::NotOwned, _ErrorTitles[EnumValue(_setting)], STR_LAND_NOT_FOR_SALE);
            }
            if (isExecuting)
            {
                surfaceElement->SetOwnership(OWNERSHIP_OWNED);
                ParkUpdateFencesAroundTile(loc);
            }
            res.Cost = gLandPrice;
            return res;

        case LandBuyRightSetting::BuyConstructionRights: // 2
            if ((surfaceElement->GetOwnership() & (OWNERSHIP_OWNED | OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED)) != 0)
            { // If the land or construction rights are already owned
                return res;
            }

            if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) != 0
                || (surfaceElement->GetOwnership() & OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE) == 0)
            {
                return GameActions::Result(
                    GameActions::Status::NotOwned, _ErrorTitles[EnumValue(_setting)], STR_CONSTRUCTION_RIGHTS_NOT_FOR_SALE);
            }

            if (isExecuting)
            {
                surfaceElement->SetOwnership(surfaceElement->GetOwnership() | OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED);
                uint16_t baseZ = surfaceElement->GetBaseZ();
                MapInvalidateTile({ loc, baseZ, baseZ + 16 });
            }
            res.Cost = gConstructionRightsPrice;
            return res;

        default:
            log_warning("Tried calling buy land rights with an incorrect setting. setting = %u", _setting);
            return GameActions::Result(GameActions::Status::InvalidParameters, _ErrorTitles[0], STR_NONE);
    }
}
