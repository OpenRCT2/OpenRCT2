/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideDemolishAction.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../drawing/ScrollingText.h"
#include "../entity/EntityList.h"
#include "../management/NewsItem.h"
#include "../peep/RideUseSystem.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ui/WindowManager.h"
#include "../world/Banner.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/TileElementsView.h"
#include "../world/tile_element/TrackElement.h"
#include "MazeSetTrackAction.h"
#include "TrackRemoveAction.h"

namespace OpenRCT2::GameActions
{
    RideDemolishAction::RideDemolishAction(RideId rideIndex, RideModifyType modifyType)
        : _rideIndex(rideIndex)
        , _modifyType(modifyType)
    {
    }

    void RideDemolishAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("ride", _rideIndex);
        visitor.Visit("modifyType", _modifyType);
    }

    uint32_t RideDemolishAction::GetCooldownTime() const
    {
        return 1000;
    }

    void RideDemolishAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_rideIndex) << DS_TAG(_modifyType);
    }

    Result RideDemolishAction::Query(GameState_t& gameState) const
    {
        auto ride = GetRide(_rideIndex);
        if (ride == nullptr)
        {
            LOG_ERROR("Ride not found for rideIndex %u", _rideIndex.ToUnderlying());
            return Result(Status::invalidParameters, STR_CANT_DEMOLISH_RIDE, STR_ERR_RIDE_NOT_FOUND);
        }

        if ((ride->lifecycleFlags & (RIDE_LIFECYCLE_INDESTRUCTIBLE | RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)
             && _modifyType == RideModifyType::demolish)
            && !gameState.cheats.makeAllDestructible)
        {
            return Result(
                Status::noClearance, STR_CANT_DEMOLISH_RIDE,
                STR_LOCAL_AUTHORITY_FORBIDS_DEMOLITION_OR_MODIFICATIONS_TO_THIS_RIDE);
        }

        Result result = Result();

        if (_modifyType == RideModifyType::renew)
        {
            if (ride->status != RideStatus::closed && ride->status != RideStatus::simulating)
            {
                return Result(Status::disallowed, STR_CANT_REFURBISH_RIDE, STR_MUST_BE_CLOSED_FIRST);
            }

            if (ride->numRiders > 0)
            {
                return Result(Status::disallowed, STR_CANT_REFURBISH_RIDE, STR_RIDE_NOT_YET_EMPTY);
            }

            if (!(ride->lifecycleFlags & RIDE_LIFECYCLE_EVER_BEEN_OPENED)
                || ride->getRideTypeDescriptor().AvailableBreakdowns == 0)
            {
                return Result(Status::disallowed, STR_CANT_REFURBISH_RIDE, STR_CANT_REFURBISH_NOT_NEEDED);
            }

            result.errorTitle = STR_CANT_REFURBISH_RIDE;
            result.cost = GetRefurbishPrice(*ride);
        }

        return result;
    }

    Result RideDemolishAction::Execute(GameState_t& gameState) const
    {
        auto ride = GetRide(_rideIndex);
        if (ride == nullptr)
        {
            LOG_ERROR("Ride not found for rideIndex %u", _rideIndex.ToUnderlying());
            return Result(Status::invalidParameters, STR_CANT_DEMOLISH_RIDE, STR_ERR_RIDE_NOT_FOUND);
        }

        switch (_modifyType)
        {
            case RideModifyType::demolish:
                return DemolishRide(gameState, *ride);
            case RideModifyType::renew:
                return RefurbishRide(gameState, *ride);
            default:
                LOG_ERROR("Unknown ride demolish type %d", _modifyType);
                return Result(Status::invalidParameters, STR_CANT_DO_THIS, STR_ERR_VALUE_OUT_OF_RANGE);
        }
    }

    Result RideDemolishAction::DemolishRide(GameState_t& gameState, Ride& ride) const
    {
        money64 refundPrice = DemolishTracks(gameState);

        RideClearForConstruction(ride);
        ride.removePeeps();
        ride.stopGuestsQueuing();

        ride.validateStations();
        RideClearLeftoverEntrances(ride);

        const auto rideId = ride.id;
        News::DisableNewsItems(News::ItemType::ride, rideId.ToUnderlying());

        UnlinkAllBannersForRide(ride.id);

        RideUse::GetHistory().RemoveValue(ride.id);
        for (auto peep : EntityList<Guest>())
        {
            peep->RemoveRideFromMemory(ride.id);
        }

        MarketingCancelCampaignsForRide(_rideIndex);

        auto res = Result();
        res.expenditure = ExpenditureType::rideConstruction;
        res.cost = refundPrice;

        if (!ride.overallView.IsNull())
        {
            auto xy = ride.overallView.ToTileCentre();
            res.position = { xy, TileElementHeight(xy) };
        }

        ride.remove();
        auto& park = gameState.park;
        park.value = Park::CalculateParkValue(park, gameState);

        // Close windows related to the demolished ride
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->CloseByNumber(WindowClass::rideConstruction, rideId.ToUnderlying());
        windowMgr->CloseByNumber(WindowClass::ride, rideId.ToUnderlying());
        windowMgr->CloseByNumber(WindowClass::demolishRidePrompt, rideId.ToUnderlying());
        windowMgr->CloseByClass(WindowClass::newCampaign);

        // Refresh windows that display the ride name
        windowMgr->BroadcastIntent(Intent(INTENT_ACTION_REFRESH_CAMPAIGN_RIDE_LIST));
        windowMgr->BroadcastIntent(Intent(INTENT_ACTION_REFRESH_RIDE_LIST));
        windowMgr->BroadcastIntent(Intent(INTENT_ACTION_REFRESH_GUEST_LIST));

        Drawing::ScrollingText::invalidate();
        GfxInvalidateScreen();

        return res;
    }

    money64 RideDemolishAction::MazeRemoveTrack(GameState_t& gameState, const CoordsXYZD& coords) const
    {
        auto setMazeTrack = MazeSetTrackAction(coords, false, _rideIndex, MazeBuildMode::fill);
        setMazeTrack.SetFlags(GetFlags());

        auto execRes = ExecuteNested(&setMazeTrack, gameState);
        if (execRes.error == Status::ok)
        {
            return execRes.cost;
        }

        return kMoney64Undefined;
    }

    money64 RideDemolishAction::DemolishTracks(GameState_t& gameState) const
    {
        money64 refundPrice = 0;

        uint8_t oldpaused = gGamePaused;
        gGamePaused = 0;

        for (TileCoordsXY tilePos = {}; tilePos.x < gameState.mapSize.x; ++tilePos.x)
        {
            for (tilePos.y = 0; tilePos.y < gameState.mapSize.y; ++tilePos.y)
            {
                const auto tileCoords = tilePos.ToCoordsXY();
                // Loop over all elements of the tile until there are no more items to remove
                int offset = -1;
                bool lastForTileReached = false;
                while (!lastForTileReached)
                {
                    offset++;
                    auto* tileElement = MapGetFirstElementAt(tileCoords) + offset;
                    if (tileElement == nullptr)
                        break;

                    lastForTileReached = tileElement->IsLastForTile();
                    if (tileElement->GetType() != TileElementType::Track)
                        continue;

                    auto* trackElement = tileElement->AsTrack();
                    if (trackElement->GetRideIndex() != _rideIndex)
                        continue;

                    const auto location = CoordsXYZD(tileCoords, trackElement->GetBaseZ(), trackElement->GetDirection());
                    const auto type = trackElement->GetTrackType();

                    if (type != TrackElemType::maze)
                    {
                        auto trackRemoveAction = TrackRemoveAction(type, trackElement->GetSequenceIndex(), location);
                        trackRemoveAction.SetFlags({ CommandFlag::noSpend });

                        auto removeRes = ExecuteNested(&trackRemoveAction, gameState);
                        if (removeRes.error != Status::ok)
                        {
                            TileElementRemove(tileElement);
                        }
                        else
                        {
                            refundPrice += removeRes.cost;
                        }
                    }
                    else
                    {
                        static constexpr CoordsXY kDirOffsets[] = {
                            { 0, 0 },
                            { 0, 16 },
                            { 16, 16 },
                            { 16, 0 },
                        };
                        for (Direction dir : kAllDirections)
                        {
                            const CoordsXYZ off = { kDirOffsets[dir], 0 };
                            money64 removePrice = MazeRemoveTrack(gameState, { location + off, dir });
                            if (removePrice != kMoney64Undefined)
                            {
                                refundPrice += removePrice;
                            }
                        }
                    }

                    // Now we have removed an element, decrement the offset, or we may skip consecutive track elements
                    offset--;
                }
            }
        }

        gGamePaused = oldpaused;
        return refundPrice;
    }

    Result RideDemolishAction::RefurbishRide(GameState_t& gameState, Ride& ride) const
    {
        auto res = Result();
        res.expenditure = ExpenditureType::rideConstruction;
        res.cost = GetRefurbishPrice(ride);

        ride.renew();

        ride.lifecycleFlags &= ~RIDE_LIFECYCLE_EVER_BEEN_OPENED;
        ride.lastCrashType = RIDE_CRASH_TYPE_NONE;

        ride.windowInvalidateFlags.set(RideInvalidateFlag::maintenance, RideInvalidateFlag::customers);

        if (!ride.overallView.IsNull())
        {
            auto location = ride.overallView.ToTileCentre();
            res.position = { location, TileElementHeight(location) };
        }

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->CloseByNumber(WindowClass::demolishRidePrompt, _rideIndex.ToUnderlying());

        return res;
    }

    money64 RideDemolishAction::GetRefurbishPrice(const Ride& ride) const
    {
        return -GetRefundPrice(ride) / 2;
    }

    money64 RideDemolishAction::GetRefundPrice(const Ride& ride) const
    {
        return RideGetRefundPrice(ride);
    }
} // namespace OpenRCT2::GameActions
