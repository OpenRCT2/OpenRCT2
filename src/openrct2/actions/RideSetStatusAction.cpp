/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideSetStatusAction.h"

#include "../Cheats.h"
#include "../Diagnostic.h"
#include "../core/MemoryStream.h"
#include "../localisation/Formatter.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/Ride.h"
#include "../ui/WindowManager.h"
#include "../world/Park.h"

using namespace OpenRCT2;

static StringId _StatusErrorTitles[] = {
    STR_CANT_CLOSE,
    STR_CANT_OPEN,
    STR_CANT_TEST,
    STR_CANT_SIMULATE,
};

RideSetStatusAction::RideSetStatusAction(RideId rideIndex, RideStatus status)
    : _rideIndex(rideIndex)
    , _status(status)
{
}

void RideSetStatusAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("ride", _rideIndex);
    visitor.Visit("status", _status);
}

uint16_t RideSetStatusAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void RideSetStatusAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_rideIndex) << DS_TAG(_status);
}

GameActions::Result RideSetStatusAction::Query() const
{
    GameActions::Result res = GameActions::Result();

    auto ride = GetRide(_rideIndex);
    if (ride == nullptr)
    {
        LOG_ERROR("Ride not found for rideIndex %u", _rideIndex.ToUnderlying());
        res.Error = GameActions::Status::InvalidParameters;
        res.ErrorTitle = STR_RIDE_DESCRIPTION_UNKNOWN;
        res.ErrorMessage = STR_ERR_RIDE_NOT_FOUND;
        return res;
    }

    if (_status >= RideStatus::Count)
    {
        LOG_ERROR("Invalid ride status %u for ride %u", EnumValue(_status), _rideIndex.ToUnderlying());
        res.Error = GameActions::Status::InvalidParameters;
        res.ErrorTitle = STR_RIDE_DESCRIPTION_UNKNOWN;
        res.ErrorMessage = kStringIdNone;
        return res;
    }

    res.ErrorTitle = _StatusErrorTitles[EnumValue(_status)];

    Formatter ft(res.ErrorMessageArgs.data());
    ride->FormatNameTo(ft);
    if (_status != ride->status)
    {
        if (_status == RideStatus::Simulating && (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN))
        {
            // Simulating will force clear the track, so make sure player can't cheat around a break down
            res.Error = GameActions::Status::Disallowed;
            res.ErrorMessage = STR_HAS_BROKEN_DOWN_AND_REQUIRES_FIXING;
            return res;
        }

        ResultWithMessage modeSwitchResult = { true };
        switch (_status)
        {
            case RideStatus::Open:
                modeSwitchResult = ride->Open(false);
                break;
            case RideStatus::Testing:
                modeSwitchResult = ride->Test(false);
                break;
            case RideStatus::Simulating:
                modeSwitchResult = ride->Simulate(false);
                break;
            default:
                break;
        }

        if (!modeSwitchResult.Successful)
        {
            res.Error = GameActions::Status::Unknown;
            res.ErrorMessage = modeSwitchResult.Message;
            return res;
        }
    }
    return GameActions::Result();
}

GameActions::Result RideSetStatusAction::Execute() const
{
    GameActions::Result res = GameActions::Result();
    res.Expenditure = ExpenditureType::RideRunningCosts;

    auto ride = GetRide(_rideIndex);
    if (ride == nullptr)
    {
        LOG_ERROR("Ride not found for rideIndex %u", _rideIndex.ToUnderlying());
        res.Error = GameActions::Status::InvalidParameters;
        res.ErrorTitle = STR_RIDE_DESCRIPTION_UNKNOWN;
        res.ErrorMessage = STR_ERR_RIDE_NOT_FOUND;
        return res;
    }

    res.ErrorTitle = _StatusErrorTitles[EnumValue(_status)];

    Formatter ft(res.ErrorMessageArgs.data());
    ft.Increment(6);
    ride->FormatNameTo(ft);
    if (!ride->overall_view.IsNull())
    {
        auto location = ride->overall_view.ToTileCentre();
        res.Position = { location, TileElementHeight(location) };
    }

    auto* windowMgr = Ui::GetWindowManager();

    switch (_status)
    {
        case RideStatus::Closed:
            if (ride->status == _status || ride->status == RideStatus::Simulating)
            {
                if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN))
                {
                    ride->lifecycle_flags &= ~RIDE_LIFECYCLE_CRASHED;
                    RideClearForConstruction(*ride);
                    ride->RemovePeeps();
                }
            }

            ride->status = RideStatus::Closed;
            ride->lifecycle_flags &= ~RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
            ride->race_winner = EntityId::GetNull();
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
            windowMgr->InvalidateByNumber(WindowClass::Ride, _rideIndex.ToUnderlying());
            break;
        case RideStatus::Simulating:
        {
            ride->lifecycle_flags &= ~RIDE_LIFECYCLE_CRASHED;
            RideClearForConstruction(*ride);
            ride->RemovePeeps();

            const auto modeSwitchResult = ride->Simulate(true);
            if (!modeSwitchResult.Successful)
            {
                res.Error = GameActions::Status::Unknown;
                res.ErrorMessage = modeSwitchResult.Message;
                return res;
            }

            ride->status = _status;
            ride->lifecycle_flags &= ~RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
            ride->race_winner = EntityId::GetNull();
            ride->current_issues = 0;
            ride->last_issue_time = 0;
            ride->GetMeasurement();
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
            windowMgr->InvalidateByNumber(WindowClass::Ride, _rideIndex.ToUnderlying());
            break;
        }
        case RideStatus::Testing:
        case RideStatus::Open:
        {
            if (ride->status == _status)
            {
                return res;
            }

            if (ride->status == RideStatus::Simulating)
            {
                RideClearForConstruction(*ride);
                ride->RemovePeeps();
            }

            // Fix #3183: Make sure we close the construction window so the ride finishes any editing code before opening
            //            otherwise vehicles get added to the ride incorrectly (such as to a ghost station)
            WindowBase* constructionWindow = windowMgr->FindByNumber(WindowClass::RideConstruction, _rideIndex.ToUnderlying());
            if (constructionWindow != nullptr)
            {
                windowMgr->Close(*constructionWindow);
            }

            if (_status == RideStatus::Testing)
            {
                const auto modeSwitchResult = ride->Test(true);
                if (!modeSwitchResult.Successful)
                {
                    res.Error = GameActions::Status::Unknown;
                    res.ErrorMessage = modeSwitchResult.Message;
                    return res;
                }
            }
            else
            {
                const auto modeSwitchResult = ride->Open(true);
                if (!modeSwitchResult.Successful)
                {
                    res.Error = GameActions::Status::Unknown;
                    res.ErrorMessage = modeSwitchResult.Message;
                    return res;
                }
            }

            ride->race_winner = EntityId::GetNull();
            ride->status = _status;
            ride->current_issues = 0;
            ride->last_issue_time = 0;
            ride->GetMeasurement();
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
            windowMgr->InvalidateByNumber(WindowClass::Ride, _rideIndex.ToUnderlying());
            break;
        }
        default:
            Guard::Assert(false, "Invalid ride status %u", _status);
            break;
    }
    auto windowManager = OpenRCT2::Ui::GetWindowManager();
    windowManager->BroadcastIntent(Intent(INTENT_ACTION_REFRESH_CAMPAIGN_RIDE_LIST));

    return res;
}
