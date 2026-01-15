/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SignSetNameAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../drawing/Drawing.h"
#include "../drawing/ScrollingText.h"
#include "../localisation/StringIds.h"
#include "../ride/Ride.h"
#include "../world/Banner.h"
#include "../world/Map.h"

#include <string>

namespace OpenRCT2::GameActions
{
    SignSetNameAction::SignSetNameAction(BannerIndex bannerIndex, const std::string& name)
        : _bannerIndex(bannerIndex)
        , _name(name)
    {
    }

    void SignSetNameAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("id", _bannerIndex);
        visitor.Visit("name", _name);
    }

    uint16_t SignSetNameAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void SignSetNameAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_bannerIndex) << DS_TAG(_name);
    }

    Result SignSetNameAction::Query(GameState_t& gameState) const
    {
        auto banner = GetBanner(_bannerIndex);
        if (banner == nullptr)
        {
            LOG_ERROR("Banner not found for bannerIndex %d", _bannerIndex);
            return Result(Status::invalidParameters, STR_CANT_RENAME_SIGN, kStringIdNone);
        }

        TileElement* tileElement = BannerGetTileElement(_bannerIndex);

        if (tileElement == nullptr)
        {
            LOG_ERROR("Banner tile element not found for bannerIndex %d", _bannerIndex);
            return Result(Status::invalidParameters, STR_CANT_RENAME_BANNER, STR_ERR_BANNER_ELEMENT_NOT_FOUND);
        }

        CoordsXYZ loc = { banner->position.ToCoordsXY(), tileElement->GetBaseZ() };

        if (!LocationValid(loc))
        {
            return Result(Status::invalidParameters, STR_CANT_RENAME_BANNER, STR_OFF_EDGE_OF_MAP);
        }
        if (!MapCanBuildAt({ loc.x, loc.y, loc.z - 16 }))
        {
            return Result(Status::notOwned, STR_CANT_RENAME_BANNER, STR_LAND_NOT_OWNED_BY_PARK);
        }

        return Result();
    }

    Result SignSetNameAction::Execute(GameState_t& gameState) const
    {
        auto banner = GetBanner(_bannerIndex);
        if (banner == nullptr)
        {
            LOG_ERROR("Banner not found for bannerIndex %d", _bannerIndex);
            return Result(Status::invalidParameters, STR_CANT_RENAME_SIGN, kStringIdNone);
        }

        if (!_name.empty())
        {
            banner->flags.unset(BannerFlag::linkedToRide);
            banner->rideIndex = RideId::GetNull();
            banner->text = _name;
        }
        else
        {
            // If empty name take closest ride name.
            RideId rideIndex = BannerGetClosestRideIndex({ banner->position.ToCoordsXY(), 16 });
            if (rideIndex.IsNull())
            {
                banner->flags.unset(BannerFlag::linkedToRide);
                banner->rideIndex = RideId::GetNull();
                banner->text = {};
            }
            else
            {
                banner->flags.set(BannerFlag::linkedToRide);
                banner->rideIndex = rideIndex;
                banner->text = {};
            }
        }

        Drawing::ScrollingText::invalidate();
        GfxInvalidateScreen();
        return Result();
    }
} // namespace OpenRCT2::GameActions
