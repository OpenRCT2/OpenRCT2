/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideSetNameAction.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Diagnostic.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../drawing/ScrollingText.h"
#include "../localisation/StringIds.h"
#include "../ride/Ride.h"
#include "../ui/WindowManager.h"
#include "../world/Map.h"
#include "../world/Park.h"

namespace OpenRCT2::GameActions
{
    RideSetNameAction::RideSetNameAction(RideId rideIndex, const std::string& name)
        : _rideIndex(rideIndex)
        , _name(name)
    {
    }

    void RideSetNameAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("ride", _rideIndex);
        visitor.Visit("name", _name);
    }

    uint16_t RideSetNameAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void RideSetNameAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_rideIndex) << DS_TAG(_name);
    }

    Result RideSetNameAction::Query(GameState_t& gameState) const
    {
        auto ride = GetRide(_rideIndex);
        if (ride == nullptr)
        {
            LOG_ERROR("Ride not found for rideIndex %u", _rideIndex.ToUnderlying());
            return Result(Status::invalidParameters, STR_CANT_RENAME_RIDE_ATTRACTION, STR_ERR_RIDE_NOT_FOUND);
        }

        if (!_name.empty() && Ride::nameExists(_name, ride->id))
        {
            return Result(Status::invalidParameters, STR_CANT_RENAME_RIDE_ATTRACTION, STR_ERROR_EXISTING_NAME);
        }

        return Result();
    }

    Result RideSetNameAction::Execute(GameState_t& gameState) const
    {
        auto ride = GetRide(_rideIndex);
        if (ride == nullptr)
        {
            LOG_ERROR("Ride not found for rideIndex %u", _rideIndex.ToUnderlying());
            return Result(Status::invalidParameters, STR_CANT_RENAME_RIDE_ATTRACTION, STR_ERR_RIDE_NOT_FOUND);
        }

        if (_name.empty())
        {
            ride->setNameToDefault();
        }
        else
        {
            ride->customName = _name;
        }

        Drawing::ScrollingText::invalidate();
        GfxInvalidateScreen();

        // Refresh windows that display ride name
        auto windowManager = Ui::GetWindowManager();
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_REFRESH_CAMPAIGN_RIDE_LIST));
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_REFRESH_RIDE_LIST));
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_REFRESH_GUEST_LIST));

        auto res = Result();
        auto location = ride->overallView.ToTileCentre();
        res.position = { location, TileElementHeight(location) };

        return res;
    }
} // namespace OpenRCT2::GameActions
