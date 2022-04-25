/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LandSetRightsAction.h"

#include "../Context.h"
#include "../OpenRCT2.h"
#include "../actions/LandSetHeightAction.h"
#include "../audio/audio.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/RideData.h"
#include "../util/Math.hpp"
#include "../windows/Intent.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Surface.h"
#include "../world/TileElementsView.h"

using namespace OpenRCT2;

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

uint16_t LandSetRightsAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::EditorOnly;
}

void LandSetRightsAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_range) << DS_TAG(_setting) << DS_TAG(_ownership);
}

GameActions::Result LandSetRightsAction::Query() const
{
    return QueryExecute(false);
}

GameActions::Result LandSetRightsAction::Execute() const
{
    return QueryExecute(true);
}

GameActions::Result LandSetRightsAction::QueryExecute(bool isExecuting) const
{
    auto res = GameActions::Result();

    auto validRange = ClampRangeWithinMap(_range.Normalise());
    CoordsXYZ centre{ (validRange.GetLeft() + validRange.GetRight()) / 2 + 16,
                      (validRange.GetTop() + validRange.GetBottom()) / 2 + 16, 0 };
    centre.z = tile_element_height(centre);

    res.Position = centre;
    res.Expenditure = ExpenditureType::LandPurchase;

    if (!(gScreenFlags & SCREEN_FLAGS_EDITOR) && !gCheatsSandboxMode)
    {
        return GameActions::Result(GameActions::Status::NotInEditorMode, STR_NONE, STR_LAND_NOT_FOR_SALE);
    }

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
        map_count_remaining_land_rights();
        OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, centre);
    }
    return res;
}

GameActions::Result LandSetRightsAction::map_buy_land_rights_for_tile(const CoordsXY& loc, bool isExecuting) const
{
    SurfaceElement* surfaceElement = map_get_surface_element_at(loc);
    if (surfaceElement == nullptr)
    {
        log_error("Could not find surface. x = %d, y = %d", loc.x, loc.y);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    auto res = GameActions::Result();
    switch (_setting)
    {
        case LandSetRightSetting::UnownLand:
            if (isExecuting)
            {
                surfaceElement->SetOwnership(
                    surfaceElement->GetOwnership() & ~(OWNERSHIP_OWNED | OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED));
                update_park_fences_around_tile(loc);
            }
            return res;
        case LandSetRightSetting::UnownConstructionRights:
            if (isExecuting)
            {
                surfaceElement->SetOwnership(surfaceElement->GetOwnership() & ~OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED);
                uint16_t baseZ = surfaceElement->GetBaseZ();
                map_invalidate_tile({ loc, baseZ, baseZ + 16 });
            }
            return res;
        case LandSetRightSetting::SetForSale:
            if (isExecuting)
            {
                surfaceElement->SetOwnership(surfaceElement->GetOwnership() | OWNERSHIP_AVAILABLE);
                uint16_t baseZ = surfaceElement->GetBaseZ();
                map_invalidate_tile({ loc, baseZ, baseZ + 16 });
            }
            return res;
        case LandSetRightSetting::SetConstructionRightsForSale:
            if (isExecuting)
            {
                surfaceElement->SetOwnership(surfaceElement->GetOwnership() | OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE);
                uint16_t baseZ = surfaceElement->GetBaseZ();
                map_invalidate_tile({ loc, baseZ, baseZ + 16 });
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
                if (_ownership == OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED || _ownership == OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE)
                {
                    if (entranceElement->base_height - 3 > surfaceElement->base_height
                        || entranceElement->base_height < surfaceElement->base_height)
                    {
                        return res;
                    }
                }
            }

            res.Cost = gLandPrice;
            if (isExecuting)
            {
                if (_ownership != OWNERSHIP_UNOWNED)
                {
                    gPeepSpawns.erase(
                        std::remove_if(
                            gPeepSpawns.begin(), gPeepSpawns.end(),
                            [x = loc.x, y = loc.y](const auto& spawn) {
                                return floor2(spawn.x, 32) == x && floor2(spawn.y, 32) == y;
                            }),
                        gPeepSpawns.end());
                }
                surfaceElement->SetOwnership(_ownership);
                update_park_fences_around_tile(loc);
                gMapLandRightsUpdateSuccess = true;
            }
            return res;
        }
        default:
            log_warning("Tried calling set land rights with an incorrect setting. setting = %u", _setting);
            return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }
}
