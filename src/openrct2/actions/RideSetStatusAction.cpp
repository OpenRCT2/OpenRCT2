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

    if (_status >= RideStatus::count)
    {
        LOG_ERROR("Invalid ride status %u for ride %u", EnumValue(_status), _rideIndex.ToUnderlying());
        res.Error = GameActions::Status::InvalidParameters;
        res.ErrorTitle = STR_RIDE_DESCRIPTION_UNKNOWN;
        res.ErrorMessage = kStringIdNone;
        return res;
    }

    res.ErrorTitle = _StatusErrorTitles[EnumValue(_status)];

    Formatter ft(res.ErrorMessageArgs.data());
    ride->formatNameTo(ft);
    if (_status != ride->status)
    {
        if (_status == RideStatus::simulating && (ride->lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN))
        {
            // Simulating will force clear the track, so make sure player can't cheat around a break down
            res.Error = GameActions::Status::Disallowed;
            res.ErrorMessage = STR_HAS_BROKEN_DOWN_AND_REQUIRES_FIXING;
            return res;
        }

        ResultWithMessage modeSwitchResult = { true };
        switch (_status)
        {
            case RideStatus::open:
                modeSwitchResult = ride->open(false);
                break;
            case RideStatus::testing:
                modeSwitchResult = ride->test(false);
                break;
            case RideStatus::simulating:
                modeSwitchResult = ride->simulate(false);
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
    ride->formatNameTo(ft);
    if (!ride->overallView.IsNull())
    {
        auto location = ride->overallView.ToTileCentre();
        res.Position = { location, TileElementHeight(location) };
    }

    auto* windowMgr = Ui::GetWindowManager();

    switch (_status)
    {
        case RideStatus::closed:
            if (ride->status == _status || ride->status == RideStatus::simulating)
            {
                if (!(ride->lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN))
                {
                    ride->lifecycleFlags &= ~RIDE_LIFECYCLE_CRASHED;
                    RideClearForConstruction(*ride);
                    ride->removePeeps();
                }
            }

            ride->status = RideStatus::closed;
            ride->lifecycleFlags &= ~RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
            ride->raceWinner = EntityId::GetNull();
            ride->windowInvalidateFlags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
            windowMgr->InvalidateByNumber(WindowClass::Ride, _rideIndex.ToUnderlying());
            break;
        case RideStatus::simulating:
        {
            ride->lifecycleFlags &= ~RIDE_LIFECYCLE_CRASHED;
            RideClearForConstruction(*ride);
            ride->removePeeps();

            const auto modeSwitchResult = ride->simulate(true);
            if (!modeSwitchResult.Successful)
            {
                res.Error = GameActions::Status::Unknown;
                res.ErrorMessage = modeSwitchResult.Message;
                return res;
            }

            ride->status = _status;
            ride->lifecycleFlags &= ~RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
            ride->raceWinner = EntityId::GetNull();
            ride->currentIssues = 0;
            ride->lastIssueTime = 0;
            ride->getMeasurement();
            ride->windowInvalidateFlags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
            windowMgr->InvalidateByNumber(WindowClass::Ride, _rideIndex.ToUnderlying());
            break;
        }
        case RideStatus::testing:
        case RideStatus::open:
        {
            if (ride->status == _status)
            {
                return res;
            }

            if (ride->status == RideStatus::simulating)
            {
                RideClearForConstruction(*ride);
                ride->removePeeps();
            }

            // Fix #3183: Make sure we close the construction window so the ride finishes any editing code before opening
            //            otherwise vehicles get added to the ride incorrectly (such as to a ghost station)
            WindowBase* constructionWindow = windowMgr->FindByNumber(WindowClass::RideConstruction, _rideIndex.ToUnderlying());
            if (constructionWindow != nullptr)
            {
                windowMgr->Close(*constructionWindow);
            }

            if (_status == RideStatus::testing)
            {
                const auto modeSwitchResult = ride->test(true);
                if (!modeSwitchResult.Successful)
                {
                    res.Error = GameActions::Status::Unknown;
                    res.ErrorMessage = modeSwitchResult.Message;
                    return res;
                }
            }
            else
            {
                const auto modeSwitchResult = ride->open(true);
                if (!modeSwitchResult.Successful)
                {
                    res.Error = GameActions::Status::Unknown;
                    res.ErrorMessage = modeSwitchResult.Message;
                    return res;
                }
            }

            ride->raceWinner = EntityId::GetNull();
            ride->status = _status;
            ride->currentIssues = 0;
            ride->lastIssueTime = 0;
            ride->getMeasurement();
            ride->windowInvalidateFlags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
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
