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
#include "../actions/LandSetHeightAction.hpp"
#include "../audio/audio.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/RideData.h"
#include "../windows/Intent.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Sprite.h"
#include "../world/Surface.h"
#include "GameAction.h"

enum class LandSetRightSetting : uint8_t
{
    UnownLand,
    UnownConstructionRights,
    SetForSale,
    SetConstructionRightsForSale,
    SetOwnershipWithChecks,
    Count
};

DEFINE_GAME_ACTION(LandSetRightsAction, GAME_COMMAND_SET_LAND_OWNERSHIP, GameActionResult)
{
private:
    MapRange _range;
    uint8_t _setting = static_cast<uint8_t>(LandSetRightSetting::Count);
    uint8_t _ownership = 0;

public:
    LandSetRightsAction() = default;

    LandSetRightsAction(const MapRange& range, LandSetRightSetting setting, uint8_t ownership = 0)
        : _range(range)
        , _setting(static_cast<uint8_t>(setting))
        , _ownership(ownership)
    {
    }

    LandSetRightsAction(const CoordsXY& coord, LandSetRightSetting setting, uint8_t ownership = 0)
        : _range(coord.x, coord.y, coord.x, coord.y)
        , _setting(static_cast<uint8_t>(setting))
        , _ownership(ownership)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::EDITOR_ONLY;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_range) << DS_TAG(_setting) << DS_TAG(_ownership);
    }

    GameActionResult::Ptr Query() const override
    {
        return QueryExecute(false);
    }

    GameActionResult::Ptr Execute() const override
    {
        return QueryExecute(true);
    }

private:
    GameActionResult::Ptr QueryExecute(bool isExecuting) const
    {
        auto res = MakeResult();

        MapRange normRange = _range.Normalise();
        // Keep big coordinates within map boundaries
        auto aX = std::max<decltype(normRange.GetLeft())>(32, normRange.GetLeft());
        auto bX = std::min<decltype(normRange.GetRight())>(gMapSizeMaxXY, normRange.GetRight());
        auto aY = std::max<decltype(normRange.GetTop())>(32, normRange.GetTop());
        auto bY = std::min<decltype(normRange.GetBottom())>(gMapSizeMaxXY, normRange.GetBottom());

        MapRange validRange = MapRange{ aX, aY, bX, bY };

        CoordsXYZ centre{ (validRange.GetLeft() + validRange.GetRight()) / 2 + 16,
                          (validRange.GetTop() + validRange.GetBottom()) / 2 + 16, 0 };
        centre.z = tile_element_height(centre);

        res->Position = centre;
        res->Expenditure = ExpenditureType::LandPurchase;

        if (!(gScreenFlags & SCREEN_FLAGS_EDITOR) && !gCheatsSandboxMode)
        {
            return MakeResult(GA_ERROR::NOT_IN_EDITOR_MODE, STR_NONE, STR_LAND_NOT_FOR_SALE);
        }

        // Game command modified to accept selection size
        for (auto y = validRange.GetTop(); y <= validRange.GetBottom(); y += COORDS_XY_STEP)
        {
            for (auto x = validRange.GetLeft(); x <= validRange.GetRight(); x += COORDS_XY_STEP)
            {
                if (!LocationValid({ x, y }))
                    continue;
                auto result = map_buy_land_rights_for_tile({ x, y }, isExecuting);
                if (result->Error == GA_ERROR::OK)
                {
                    res->Cost += result->Cost;
                }
            }
        }

        if (isExecuting)
        {
            map_count_remaining_land_rights();
            audio_play_sound_at_location(SoundId::PlaceItem, centre);
        }
        return res;
    }

    GameActionResult::Ptr map_buy_land_rights_for_tile(const CoordsXY& loc, bool isExecuting) const
    {
        SurfaceElement* surfaceElement = map_get_surface_element_at(loc);
        if (surfaceElement == nullptr)
        {
            log_error("Could not find surface. x = %d, y = %d", loc.x, loc.y);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE, STR_NONE);
        }

        auto res = MakeResult();
        switch (static_cast<LandSetRightSetting>(_setting))
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

                TileElement* tileElement = map_get_first_element_at(loc);
                do
                {
                    if (tileElement == nullptr)
                        break;

                    if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
                        continue;

                    if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_PARK_ENTRANCE)
                        continue;

                    // Do not allow ownership of park entrance.
                    if (_ownership == OWNERSHIP_OWNED || _ownership == OWNERSHIP_AVAILABLE)
                        return res;

                    // Allow construction rights available / for sale on park entrances on surface.
                    // There is no need to check the height if _ownership is 0 (unowned and no rights available).
                    if (_ownership == OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED
                        || _ownership == OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE)
                    {
                        if (tileElement->base_height - 3 > surfaceElement->base_height
                            || tileElement->base_height < surfaceElement->base_height)
                            return res;
                    }
                } while (!(tileElement++)->IsLastForTile());

                res->Cost = gLandPrice;
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
                return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE, STR_NONE);
        }
    }
};
