/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GuestRideHelper.h"

#include "../../Cheats.h"
#include "../../OpenRCT2.h"
#include "../../config/Config.h"
#include "../../management/Marketing.h"
#include "../../management/NewsItem.h"
#include "../../network/network.h"
#include "../../peep/RideUseSystem.h"
#include "../../ride/RideData.h"
#include "../../scenario/Scenario.h"
#include "../../world/Park.h"
#include "../../world/Scenery.h"
#include "../../world/TileElementsView.h"

using OpenRCT2::TileElementsView;

// Locations of the spiral slide platform that a peep walks from the entrance of the ride to the
// entrance of the slide. Up to 4 waypoints for each 4 sides that an ride entrance can be located
// and 4 different rotations of the ride. 4 * 4 * 4 = 64 locations.
// clang-format off
static constexpr const CoordsXY SpiralSlideWalkingPath[64] = {
    {  56,   8 },
    {   8,   8 },
    {   8,  32 },
    {  32,  32 },
    {   8,   8 },
    {   8,   8 },
    {   8,  32 },
    {  32,  32 },
    {   8,  32 },
    {   8,  32 },
    {   8,  32 },
    {  32,  32 },
    {   8,  56 },
    {   8,  32 },
    {   8,  32 },
    {  32,  32 },
    {  56,  24 },
    {  32,  24 },
    {  32,  24 },
    {  32,   0 },
    {  56, -24 },
    {  56,  24 },
    {  32,  24 },
    {  32,   0 },
    {   8,  24 },
    {  32,  24 },
    {  32,  24 },
    {  32,   0 },
    {  32,  24 },
    {  32,  24 },
    {  32,  24 },
    {  32,   0 },
    {  24,   0 },
    {  24,   0 },
    {  24,   0 },
    {   0,   0 },
    {  24, -24 },
    {  24,   0 },
    {  24,   0 },
    {   0,   0 },
    { -24, -24 },
    {  24, -24 },
    {  24,   0 },
    {   0,   0 },
    {  24,  24 },
    {  24,   0 },
    {  24,   0 },
    {   0,   0 },
    {  24,   8 },
    {   0,   8 },
    {   0,   8 },
    {   0,  32 },
    {   0,   8 },
    {   0,   8 },
    {   0,   8 },
    {   0,  32 },
    { -24,   8 },
    {   0,   8 },
    {   0,   8 },
    {   0,  32 },
    { -24,  56 },
    { -24,   8 },
    {   0,   8 },
    {   0,  32 },
};
// clang-format on

// These arrays contain the base minimum and maximum nausea ratings for peeps, based on their nausea tolerance level.
static constexpr const ride_rating NauseaMinimumThresholds[] = {
    0,
    0,
    200,
    400,
};

static constexpr const ride_rating NauseaMaximumThresholds[] = {
    300,
    600,
    800,
    1000,
};

GuestRideHelper::GuestRideHelper(Guest& guest)
    : _guest(guest)
{
}

/**
 * Updates various peep stats upon entering a ride, as well as updating the
 * ride's satisfaction value.
 *  rct2: 0x0069545B
 */
void GuestRideHelper::OnEnterRide(Ride& ride)
{
    // Calculate how satisfying the ride is for the peep. Can range from -140 to +105.
    int16_t satisfaction = CalculateRideSatisfaction(ride);

    // Update the satisfaction stat of the ride.
    uint8_t rideSatisfaction = 0;
    if (satisfaction >= 40)
        rideSatisfaction = 3;
    else if (satisfaction >= 20)
        rideSatisfaction = 2;
    else if (satisfaction >= 0)
        rideSatisfaction = 1;

    ride.UpdateSatisfaction(rideSatisfaction);

    // Update various peep stats.
    if (_guest.GuestNumRides < 255)
        _guest.GuestNumRides++;

    SetHasRidden(ride);
    UpdateFavouriteRide(ride);
    _guest.HappinessTarget = std::clamp(_guest.HappinessTarget + satisfaction, 0, PEEP_MAX_HAPPINESS);
    UpdateRideNauseaGrowth(ride);
}

/**
 *
 *  rct2: 0x0069576E
 */
void GuestRideHelper::OnExitRide(Ride& ride)
{
    if (_guest.PeepFlags & PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE)
    {
        _guest.PeepFlags &= ~PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE;
        _guest.FavouriteRide = ride.id;
        // TODO fix this flag name or add another one
        _guest.WindowInvalidateFlags |= PEEP_INVALIDATE_STAFF_STATS;
    }
    _guest.Happiness = _guest.HappinessTarget;
    _guest.Nausea = _guest.NauseaTarget;
    _guest.WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_STATS;

    if (_guest.PeepFlags & PEEP_FLAGS_LEAVING_PARK)
        _guest.PeepFlags &= ~(PEEP_FLAGS_PARK_ENTRANCE_CHOSEN);

    if (ShouldGoOnRideAgain(ride))
    {
        _guest.GuestHeadingToRideId = ride.id;
        _guest.GuestIsLostCountdown = 200;
        _guest.ResetPathfindGoal();
        _guest.WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_ACTION;
    }

    if (ShouldPreferredIntensityIncrease())
    {
        if (_guest.Intensity.GetMaximum() < 15)
        {
            _guest.Intensity = _guest.Intensity.WithMaximum(_guest.Intensity.GetMaximum() + 1);
        }
    }

    if (ReallyLikedRide(ride))
    {
        _guest.InsertNewThought(PeepThoughtType::WasGreat, ride.id);

        static constexpr OpenRCT2::Audio::SoundId laughs[3] = {
            OpenRCT2::Audio::SoundId::Laugh1,
            OpenRCT2::Audio::SoundId::Laugh2,
            OpenRCT2::Audio::SoundId::Laugh3,
        };
        int32_t laughType = scenario_rand() & 7;
        if (laughType < 3)
        {
            OpenRCT2::Audio::Play3D(laughs[laughType], _guest.GetLocation());
        }
    }

    ride.total_customers++;
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;
}

/**
 *
 *  rct2: 0x00695DD2
 */
void GuestRideHelper::PickRideToGoOn()
{
    if (_guest.State != PeepState::Walking)
        return;
    if (!_guest.GuestHeadingToRideId.IsNull())
        return;
    if (_guest.PeepFlags & PEEP_FLAGS_LEAVING_PARK)
        return;
    if (_guest.HasFoodOrDrink())
        return;
    if (_guest.x == LOCATION_NULL)
        return;

    auto ride = FindBestRideToGoOn();
    if (ride != nullptr)
    {
        // Head to that ride
        _guest.GuestHeadingToRideId = ride->id;
        _guest.GuestIsLostCountdown = 200;
        _guest.ResetPathfindGoal();
        _guest.WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_ACTION;

        // Make peep look at their map if they have one
        if (_guest.HasItem(ShopItem::Map))
        {
            _guest.ReadMap();
        }
    }
}

Ride* GuestRideHelper::FindBestRideToGoOn()
{
    // Pick the most exciting ride
    auto rideConsideration = FindRidesToGoOn();
    Ride* mostExcitingRide = nullptr;
    for (auto& ride : GetRideManager())
    {
        const auto rideIndex = ride.id.ToUnderlying();
        if (rideConsideration.size() > rideIndex && rideConsideration[rideIndex])
        {
            if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_QUEUE_FULL))
            {
                if (ShouldGoOnRide(ride, StationIndex::FromUnderlying(0), false, true) && ride_has_ratings(ride))
                {
                    if (mostExcitingRide == nullptr || ride.excitement > mostExcitingRide->excitement)
                    {
                        mostExcitingRide = &ride;
                    }
                }
            }
        }
    }
    return mostExcitingRide;
}

OpenRCT2::BitSet<OpenRCT2::Limits::MaxRidesInPark> GuestRideHelper::FindRidesToGoOn()
{
    OpenRCT2::BitSet<OpenRCT2::Limits::MaxRidesInPark> rideConsideration;

    // FIX  Originally checked for a toy, likely a mistake and should be a map,
    //      but then again this seems to only allow the peep to go on
    //      rides they haven't been on before.
    if (_guest.HasItem(ShopItem::Map))
    {
        // Consider rides that peep hasn't been on yet
        for (auto& ride : GetRideManager())
        {
            if (!HasRidden(ride))
            {
                rideConsideration[ride.id.ToUnderlying()] = true;
            }
        }
    }
    else
    {
        // Take nearby rides into consideration
        constexpr auto radius = 10 * 32;
        int32_t cx = floor2(_guest.x, 32);
        int32_t cy = floor2(_guest.y, 32);
        for (int32_t tileX = cx - radius; tileX <= cx + radius; tileX += COORDS_XY_STEP)
        {
            for (int32_t tileY = cy - radius; tileY <= cy + radius; tileY += COORDS_XY_STEP)
            {
                auto location = CoordsXY{ tileX, tileY };
                if (!MapIsLocationValid(location))
                    continue;

                for (auto* trackElement : TileElementsView<TrackElement>(location))
                {
                    auto rideIndex = trackElement->GetRideIndex();
                    if (!rideIndex.IsNull())
                    {
                        rideConsideration[rideIndex.ToUnderlying()] = true;
                    }
                }
            }
        }

        // Always take the tall rides into consideration (realistic as you can usually see them from anywhere in the park)
        for (auto& ride : GetRideManager())
        {
            if (ride.highest_drop_height > 66 || ride.excitement >= RIDE_RATING(8, 00))
            {
                rideConsideration[ride.id.ToUnderlying()] = true;
            }
        }
    }

    return rideConsideration;
}

/**
 * This function is called whenever a peep is deciding whether or not they want
 * to go on a ride or visit a shop. They may be physically present at the
 * ride/shop, or they may just be thinking about it.
 *  rct2: 0x006960AB
 */
bool GuestRideHelper::ShouldGoOnRide(Ride& ride, StationIndex entranceNum, bool atQueue, bool thinking)
{
    // Indicates whether a peep is physically at the ride, or is just thinking about going on the ride.
    bool peepAtRide = !thinking;

    if (ride.status == RideStatus::Open && !(ride.lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN))
    {
        // Peeps that are leaving the park will refuse to go on any rides, with the exception of free transport rides.
        assert(ride.type < std::size(RideTypeDescriptors));
        if (!ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_TRANSPORT_RIDE) || ride.value == RIDE_VALUE_UNDEFINED
            || ride_get_price(ride) != 0)
        {
            if (_guest.PeepFlags & PEEP_FLAGS_LEAVING_PARK)
            {
                ChoseNotToGoOnRide(ride, peepAtRide, false);
                return false;
            }
        }

        if (ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY))
        {
            return ShouldGoToShop(ride, peepAtRide);
        }

        // This used to check !(flags & 2), but the function is only ever called with flags = 0, 1 or 6.
        // This means we can use the existing !(flags & 4) check.
        if (peepAtRide)
        {
            auto& station = ride.GetStation(entranceNum);

            // Rides without queues can only have one peep waiting at a time.
            if (!atQueue)
            {
                if (!station.LastPeepInQueue.IsNull())
                {
                    TriedToEnterFullQueue(ride);
                    return false;
                }
            }
            else
            {
                // Check if there's room in the queue for the peep to enter.
                Guest* lastPeepInQueue = GetEntity<Guest>(station.LastPeepInQueue);
                if (lastPeepInQueue != nullptr && (abs(lastPeepInQueue->z - _guest.z) <= 6))
                {
                    int32_t dx = abs(lastPeepInQueue->x - _guest.x);
                    int32_t dy = abs(lastPeepInQueue->y - _guest.y);
                    int32_t maxD = std::max(dx, dy);

                    // Unlike normal paths, peeps cannot overlap when queueing for a ride.
                    // This check enforces a minimum distance between peeps entering the queue.
                    if (maxD < 8)
                    {
                        TriedToEnterFullQueue(ride);
                        return false;
                    }

                    // This checks if there's a peep standing still at the very end of the queue.
                    if (maxD <= 13 && lastPeepInQueue->TimeInQueue > 10)
                    {
                        TriedToEnterFullQueue(ride);
                        return false;
                    }
                }
            }
        }

        // Assuming the queue conditions are met, peeps will always go on free transport rides.
        // Ride ratings, recent crashes and weather will all be ignored.
        money16 ridePrice = ride_get_price(ride);
        if (!ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_TRANSPORT_RIDE) || ride.value == RIDE_VALUE_UNDEFINED
            || ridePrice != 0)
        {
            if (_guest.PreviousRide == ride.id)
            {
                ChoseNotToGoOnRide(ride, peepAtRide, false);
                return false;
            }

            // Basic price checks
            if (ridePrice != 0 && !HasVoucherForFreeRide(ride) && !(gParkFlags & PARK_FLAGS_NO_MONEY))
            {
                if (ridePrice > _guest.CashInPocket)
                {
                    if (peepAtRide)
                    {
                        if (_guest.CashInPocket <= 0)
                        {
                            _guest.InsertNewThought(PeepThoughtType::SpentMoney);
                        }
                        else
                        {
                            _guest.InsertNewThought(PeepThoughtType::CantAffordRide, ride.id);
                        }
                    }
                    ChoseNotToGoOnRide(ride, peepAtRide, true);
                    return false;
                }
            }

            // If happy enough, peeps will ignore the fact that a ride has recently crashed.
            if (ride.last_crash_type != RIDE_CRASH_TYPE_NONE && _guest.Happiness < 225)
            {
                if (peepAtRide)
                {
                    _guest.InsertNewThought(PeepThoughtType::NotSafe, ride.id);
                    if (_guest.HappinessTarget >= 64)
                    {
                        _guest.HappinessTarget -= 8;
                    }
                    ride.UpdatePopularity(0);
                }
                ChoseNotToGoOnRide(ride, peepAtRide, true);
                return false;
            }

            if (ride_has_ratings(ride))
            {
                // If a peep has already decided that they're going to go on a ride, they'll skip the weather and
                // excitement check and will only do a basic intensity check when they arrive at the ride itself.
                if (ride.id == _guest.GuestHeadingToRideId)
                {
                    if (ride.intensity > RIDE_RATING(10, 00) && !gCheatsIgnoreRideIntensity)
                    {
                        RideIsTooIntense(ride, peepAtRide);
                        return false;
                    }
                }
                else
                {
                    if (ClimateIsRaining() && !ShouldRideWhileRaining(ride))
                    {
                        if (peepAtRide)
                        {
                            _guest.InsertNewThought(PeepThoughtType::NotWhileRaining, ride.id);
                            if (_guest.HappinessTarget >= 64)
                            {
                                _guest.HappinessTarget -= 8;
                            }
                            ride.UpdatePopularity(0);
                        }
                        ChoseNotToGoOnRide(ride, peepAtRide, true);
                        return false;
                    }
                    // If it is raining and the ride provides shelter skip the
                    // ride intensity check and get me on a sheltered ride!
                    if (!ClimateIsRaining() || !ShouldRideWhileRaining(ride))
                    {
                        if (!gCheatsIgnoreRideIntensity)
                        {
                            // Intensity calculations. Even though the max intensity can go up to 15, it's capped
                            // at 10.0 (before happiness calculations). A full happiness bar will increase the max
                            // intensity and decrease the min intensity by about 2.5.
                            ride_rating maxIntensity = std::min(_guest.Intensity.GetMaximum() * 100, 1000) + _guest.Happiness;
                            ride_rating minIntensity = (_guest.Intensity.GetMinimum() * 100) - _guest.Happiness;
                            if (ride.intensity < minIntensity)
                            {
                                if (peepAtRide)
                                {
                                    _guest.InsertNewThought(PeepThoughtType::MoreThrilling, ride.id);
                                    if (_guest.HappinessTarget >= 64)
                                    {
                                        _guest.HappinessTarget -= 8;
                                    }
                                    ride.UpdatePopularity(0);
                                }
                                ChoseNotToGoOnRide(ride, peepAtRide, true);
                                return false;
                            }
                            if (ride.intensity > maxIntensity)
                            {
                                RideIsTooIntense(ride, peepAtRide);
                                return false;
                            }

                            // Nausea calculations.
                            ride_rating maxNausea = NauseaMaximumThresholds[(EnumValue(_guest.NauseaTolerance) & 3)]
                                + _guest.Happiness;

                            if (ride.nausea > maxNausea)
                            {
                                if (peepAtRide)
                                {
                                    _guest.InsertNewThought(PeepThoughtType::Sickening, ride.id);
                                    if (_guest.HappinessTarget >= 64)
                                    {
                                        _guest.HappinessTarget -= 8;
                                    }
                                    ride.UpdatePopularity(0);
                                }
                                ChoseNotToGoOnRide(ride, peepAtRide, true);
                                return false;
                            }

                            // Very nauseous peeps will only go on very gentle rides.
                            if (ride.nausea >= FIXED_2DP(1, 40) && _guest.Nausea > 160)
                            {
                                ChoseNotToGoOnRide(ride, peepAtRide, false);
                                return false;
                            }
                        }
                    }
                }
            }

            // If the ride has not yet been rated and is capable of having g-forces,
            // there's a 90% chance that the peep will ignore it.
            if (!ride_has_ratings(ride) && ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES))
            {
                if ((scenario_rand() & 0xFFFF) > 0x1999U)
                {
                    ChoseNotToGoOnRide(ride, peepAtRide, false);
                    return false;
                }

                if (!gCheatsIgnoreRideIntensity)
                {
                    if (ride.max_positive_vertical_g > FIXED_2DP(5, 00) || ride.max_negative_vertical_g < FIXED_2DP(-4, 00)
                        || ride.max_lateral_g > FIXED_2DP(4, 00))
                    {
                        ChoseNotToGoOnRide(ride, peepAtRide, false);
                        return false;
                    }
                }
            }

            uint32_t value = ride.value;

            // If the value of the ride hasn't yet been calculated, peeps will be willing to pay any amount for the ride.
            if (value != RIDE_VALUE_UNDEFINED && !HasVoucherForFreeRide(ride) && !(gParkFlags & PARK_FLAGS_NO_MONEY))
            {
                // The amount peeps are willing to pay is decreased by 75% if they had to pay to enter the park.
                if (_guest.PeepFlags & PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY)
                    value /= 4;

                // Peeps won't pay more than twice the value of the ride.
                ridePrice = ride_get_price(ride);
                if (ridePrice > static_cast<money16>(value * 2))
                {
                    if (peepAtRide)
                    {
                        _guest.InsertNewThought(PeepThoughtType::BadValue, ride.id);
                        if (_guest.HappinessTarget >= 60)
                        {
                            _guest.HappinessTarget -= 16;
                        }
                        ride.UpdatePopularity(0);
                    }
                    ChoseNotToGoOnRide(ride, peepAtRide, true);
                    return false;
                }

                // A ride is good value if the price is 50% or less of the ride value and the peep didn't pay to enter the park.
                if (ridePrice <= static_cast<money16>(value / 2) && peepAtRide)
                {
                    if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
                    {
                        if (!(_guest.PeepFlags & PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY))
                        {
                            _guest.InsertNewThought(PeepThoughtType::GoodValue, ride.id);
                        }
                    }
                }
            }
        }

        // At this point, the peep has decided to go on the ride.
        if (peepAtRide)
        {
            ride.UpdatePopularity(1);
        }

        if (ride.id == _guest.GuestHeadingToRideId)
        {
            ResetRideHeading();
        }

        ride.lifecycle_flags &= ~RIDE_LIFECYCLE_QUEUE_FULL;
        return true;
    }

    ChoseNotToGoOnRide(ride, peepAtRide, false);
    return false;
}

bool GuestRideHelper::ShouldGoToShop(Ride& ride, bool peepAtShop)
{
    // Peeps won't go to the same shop twice in a row.
    if (ride.id == _guest.PreviousRide)
    {
        ChoseNotToGoOnRide(ride, peepAtShop, true);
        return false;
    }

    const auto& rtd = ride.GetRideTypeDescriptor();
    if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_TOILET))
    {
        if (_guest.Toilet < 70)
        {
            ChoseNotToGoOnRide(ride, peepAtShop, true);
            return false;
        }

        // The amount that peeps are willing to pay to use the Toilets scales with their toilet stat.
        // It effectively has a minimum of $0.10 (due to the check above) and a maximum of $0.60.
        if (ride_get_price(ride) * 40 > _guest.Toilet)
        {
            if (peepAtShop)
            {
                _guest.InsertNewThought(PeepThoughtType::NotPaying, ride.id);
                if (_guest.HappinessTarget >= 60)
                {
                    _guest.HappinessTarget -= 16;
                }
                ride.UpdatePopularity(0);
            }
            ChoseNotToGoOnRide(ride, peepAtShop, true);
            return false;
        }
    }

    if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_FIRST_AID))
    {
        if (_guest.Nausea < 128)
        {
            ChoseNotToGoOnRide(ride, peepAtShop, true);
            return false;
        }
    }

    // Basic price checks
    auto ridePrice = ride_get_price(ride);
    if (ridePrice != 0 && ridePrice > _guest.CashInPocket)
    {
        if (peepAtShop)
        {
            if (_guest.CashInPocket <= 0)
            {
                _guest.InsertNewThought(PeepThoughtType::SpentMoney);
            }
            else
            {
                _guest.InsertNewThought(PeepThoughtType::CantAffordRide, ride.id);
            }
        }
        ChoseNotToGoOnRide(ride, peepAtShop, true);
        return false;
    }

    if (peepAtShop)
    {
        ride.UpdatePopularity(1);
        if (ride.id == _guest.GuestHeadingToRideId)
        {
            ResetRideHeading();
        }
    }
    return true;
}

void GuestRideHelper::SetHasRidden(const Ride& ride)
{
    OpenRCT2::RideUse::GetHistory().Add(_guest.sprite_index, ride.id);
    SetHasRiddenRideType(ride.type);
}

bool GuestRideHelper::HasRidden(const Ride& ride) const
{
    return OpenRCT2::RideUse::GetHistory().Contains(_guest.sprite_index, ride.id);
}

void GuestRideHelper::SetHasRiddenRideType(int32_t rideType)
{
    OpenRCT2::RideUse::GetTypeHistory().Add(_guest.sprite_index, rideType);
}

bool GuestRideHelper::HasRiddenRideType(int32_t rideType) const
{
    return OpenRCT2::RideUse::GetTypeHistory().Contains(_guest.sprite_index, rideType);
}

bool GuestRideHelper::ShouldRideWhileRaining(const Ride& ride)
{
    // Peeps will go on rides that are sufficiently undercover while it's raining.
    // The threshold is fairly low and only requires about 10-15% of the ride to be undercover.
    if (ride.sheltered_eighths >= 3)
    {
        return true;
    }

    // Peeps with umbrellas will go on rides where they can use their umbrella on it (like the Maze) 50% of the time
    if (_guest.HasItem(ShopItem::Umbrella) && ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_PEEP_CAN_USE_UMBRELLA)
        && (scenario_rand() & 2) == 0)
    {
        return true;
    }

    return false;
}

void GuestRideHelper::ChoseNotToGoOnRide(const Ride& ride, bool peepAtRide, bool updateLastRide)
{
    if (peepAtRide && updateLastRide)
    {
        _guest.PreviousRide = ride.id;
        _guest.PreviousRideTimeOut = 0;
    }

    if (ride.id == _guest.GuestHeadingToRideId)
    {
        ResetRideHeading();
    }
}

bool GuestRideHelper::HasVoucherForFreeRide(const Ride& ride) const
{
    return _guest.HasItem(ShopItem::Voucher) && _guest.VoucherType == VOUCHER_TYPE_RIDE_FREE && _guest.VoucherRideId == ride.id;
}

/**
 * When the queue is full, peeps will ignore the ride when thinking about what to go on next.
 * Does not effect peeps that walk up to the queue entrance.
 * This flag is reset the next time a peep successfully joins the queue.
 */
void GuestRideHelper::TriedToEnterFullQueue(Ride& ride)
{
    ride.lifecycle_flags |= RIDE_LIFECYCLE_QUEUE_FULL;
    _guest.PreviousRide = ride.id;
    _guest.PreviousRideTimeOut = 0;
    // Change status "Heading to" to "Walking" if queue is full
    if (ride.id == _guest.GuestHeadingToRideId)
    {
        ResetRideHeading();
    }
}

void GuestRideHelper::ResetRideHeading()
{
    _guest.GuestHeadingToRideId = RideId::GetNull();
    _guest.WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_ACTION;
}

void GuestRideHelper::RideIsTooIntense(Ride& ride, bool peepAtRide)
{
    if (peepAtRide)
    {
        _guest.InsertNewThought(PeepThoughtType::Intense, ride.id);
        if (_guest.HappinessTarget >= 64)
        {
            _guest.HappinessTarget -= 8;
        }
        ride.UpdatePopularity(0);
    }
    ChoseNotToGoOnRide(ride, peepAtRide, true);
}

/**
 *
 *  rct2: 0x00691C6E
 */
Vehicle* GuestRideHelper::ChooseCarFromRide(const Ride& ride, std::vector<uint8_t>& car_array)
{
    uint8_t chosen_car = scenario_rand();
    if (ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_G_FORCES) && ((chosen_car & 0xC) != 0xC))
    {
        chosen_car = (scenario_rand() & 1) ? 0 : static_cast<uint8_t>(car_array.size()) - 1;
    }
    else
    {
        chosen_car = (chosen_car * static_cast<uint16_t>(car_array.size())) >> 8;
    }

    _guest.CurrentCar = car_array[chosen_car];

    Vehicle* vehicle = GetEntity<Vehicle>(ride.vehicles[_guest.CurrentTrain]);
    if (vehicle == nullptr)
    {
        return nullptr;
    }
    return vehicle->GetCar(_guest.CurrentCar);
}

/**
 *
 *  rct2: 0x00691CD1
 */
void GuestRideHelper::ChooseSeatFromCar(const Ride& ride, Vehicle& vehicle)
{
    uint8_t chosen_seat = vehicle.next_free_seat;

    if (ride.mode == RideMode::ForwardRotation || ride.mode == RideMode::BackwardRotation)
    {
        chosen_seat = (((~vehicle.Pitch + 1) >> 3) & 0xF) * 2;
        if (vehicle.next_free_seat & 1)
        {
            chosen_seat++;
        }
    }
    _guest.CurrentSeat = chosen_seat;
    vehicle.next_free_seat++;

    vehicle.peep[_guest.CurrentSeat] = _guest.sprite_index;
    vehicle.peep_tshirt_colours[_guest.CurrentSeat] = _guest.TshirtColour;
}

/**
 *
 *  rct2: 0x00691D27
 */
void GuestRideHelper::GoToRideEntrance(const Ride& ride)
{
    const auto& station = ride.GetStation(_guest.CurrentRideStation);
    if (station.Entrance.IsNull())
    {
        RemoveFromQueue();
        return;
    }

    auto location = station.Entrance.ToCoordsXYZD().ToTileCentre();
    int16_t x_shift = DirectionOffsets[location.direction].x;
    int16_t y_shift = DirectionOffsets[location.direction].y;

    uint8_t shift_multiplier = 21;
    rct_ride_entry* rideEntry = get_ride_entry(ride.subtype);
    if (rideEntry != nullptr)
    {
        if (rideEntry->Cars[rideEntry->DefaultCar].flags & CAR_ENTRY_FLAG_MINI_GOLF
            || rideEntry->Cars[rideEntry->DefaultCar].flags & (CAR_ENTRY_FLAG_CHAIRLIFT | CAR_ENTRY_FLAG_GO_KART))
        {
            shift_multiplier = 32;
        }
    }

    x_shift *= shift_multiplier;
    y_shift *= shift_multiplier;

    location.x += x_shift;
    location.y += y_shift;

    _guest.SetDestination(location, 2);
    _guest.SetState(PeepState::EnteringRide);
    _guest.RideSubState = PeepRideSubState::InEntrance;

    _guest.RejoinQueueTimeout = 0;
    _guest.GuestTimeOnRide = 0;

    RemoveFromQueue();
}

bool GuestRideHelper::FindVehicleToEnter(const Ride& ride, std::vector<uint8_t>& car_array)
{
    uint8_t chosen_train = RideStation::NO_TRAIN;

    if (ride.mode == RideMode::Dodgems || ride.mode == RideMode::Race)
    {
        if (ride.lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
            return false;

        for (int32_t i = 0; i < ride.NumTrains; ++i)
        {
            Vehicle* vehicle = GetEntity<Vehicle>(ride.vehicles[i]);
            if (vehicle == nullptr)
                continue;

            if (vehicle->next_free_seat >= vehicle->num_seats)
                continue;

            if (vehicle->status != Vehicle::Status::WaitingForPassengers)
                continue;
            chosen_train = i;
            break;
        }
    }
    else
    {
        chosen_train = ride.GetStation(_guest.CurrentRideStation).TrainAtStation;
    }
    if (chosen_train >= OpenRCT2::Limits::MaxTrainsPerRide)
    {
        return false;
    }

    _guest.CurrentTrain = chosen_train;

    int32_t i = 0;

    auto vehicle_id = ride.vehicles[chosen_train];
    for (Vehicle* vehicle = GetEntity<Vehicle>(vehicle_id); vehicle != nullptr;
         vehicle = GetEntity<Vehicle>(vehicle->next_vehicle_on_train), ++i)
    {
        uint8_t num_seats = vehicle->num_seats;
        if (vehicle->IsUsedInPairs())
        {
            if (vehicle->next_free_seat & 1)
            {
                car_array.clear();
                car_array.push_back(i);
                return true;
            }
            num_seats &= VEHICLE_SEAT_NUM_MASK;
        }
        if (num_seats == vehicle->next_free_seat)
            continue;

        if (ride.mode == RideMode::ForwardRotation || ride.mode == RideMode::BackwardRotation)
        {
            uint8_t position = (((~vehicle->Pitch + 1) >> 3) & 0xF) * 2;
            if (!vehicle->peep[position].IsNull())
                continue;
        }
        car_array.push_back(i);
    }

    return !car_array.empty();
}

void GuestRideHelper::UpdateRideAtEntranceTryLeave()
{
    // Destination Tolerance is zero when peep has completely
    // entered entrance
    if (_guest.DestinationTolerance == 0)
    {
        RemoveFromQueue();
        _guest.SetState(PeepState::Falling);
    }
}

bool GuestRideHelper::CheckRidePriceAtEntrance(const Ride& ride, money32 ridePrice)
{
    if ((_guest.HasItem(ShopItem::Voucher)) && _guest.VoucherType == VOUCHER_TYPE_RIDE_FREE
        && _guest.VoucherRideId == _guest.CurrentRide)
        return true;

    if (_guest.CashInPocket <= 0 && !(gParkFlags & PARK_FLAGS_NO_MONEY))
    {
        _guest.InsertNewThought(PeepThoughtType::SpentMoney);
        UpdateRideAtEntranceTryLeave();
        return false;
    }

    if (ridePrice > _guest.CashInPocket)
    {
        // Prevent looping of same thought / animation since Destination Tolerance
        // is only 0 exactly at entrance and will immediately change as guest
        // tries to leave hereafter
        if (_guest.DestinationTolerance == 0)
        {
            _guest.InsertNewThought(PeepThoughtType::CantAffordRide, _guest.CurrentRide);
        }
        UpdateRideAtEntranceTryLeave();
        return false;
    }

    uint16_t value = ride.value;
    if (value != RIDE_VALUE_UNDEFINED)
    {
        if (value * 2 < ridePrice)
        {
            _guest.InsertNewThought(PeepThoughtType::BadValue, _guest.CurrentRide);
            UpdateRideAtEntranceTryLeave();
            return false;
        }
    }
    return true;
}

/**
 * The satisfaction values calculated here are used to determine how happy the peep is with the ride,
 * and also affects the satisfaction stat of the ride itself. The factors that affect satisfaction include:
 * - The price of the ride compared to the ride's value
 * - How closely the intensity and nausea of the ride matches the peep's preferences
 * - How long the peep was waiting in the queue
 * - If the peep has been on the ride before, or on another ride of the same type
 */
int16_t GuestRideHelper::CalculateRideSatisfaction(const Ride& ride) const
{
    int16_t satisfaction = CalculateRideValueSatisfaction(ride);
    satisfaction += CalculateRideIntensityNauseaSatisfaction(ride);

    // Calculate satisfaction based on how long the peep has been in the queue for.
    // (For comparison: peeps start thinking "I've been queueing for a long time" at 3500 and
    // start leaving the queue at 4300.)
    if (_guest.TimeInQueue >= 4500)
        satisfaction -= 35;
    else if (_guest.TimeInQueue >= 2250)
        satisfaction -= 10;
    else if (_guest.TimeInQueue <= 750)
        satisfaction += 10;

    // Peeps get a small boost in satisfaction if they've been on a ride of the same type before,
    // and this boost is doubled if they've already been on this particular ride.
    if (HasRiddenRideType(ride.type))
        satisfaction += 10;

    if (HasRidden(*get_ride(_guest.CurrentRide)))
        satisfaction += 10;

    return satisfaction;
}

/**
 * Check to see if the specified ride should become the peep's favourite.
 * For this, a "ride rating" is calculated based on the excitement of the ride and the peep's current happiness.
 * As this value cannot exceed 255, the happier the peep is, the more irrelevant the ride's excitement becomes.
 * Due to the minimum happiness requirement, an excitement rating of more than 3.8 has no further effect.
 *
 * If the ride rating is higher than any ride the peep has already been on and the happiness criteria is met,
 * the ride becomes the peep's favourite. (This doesn't happen right away, but will be updated once the peep
 * exits the ride.)
 */
void GuestRideHelper::UpdateFavouriteRide(const Ride& ride)
{
    _guest.PeepFlags &= ~PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE;
    uint8_t peepRideRating = std::clamp((ride.excitement / 4) + _guest.Happiness, 0, PEEP_MAX_HAPPINESS);
    if (peepRideRating >= _guest.FavouriteRideRating)
    {
        if (_guest.Happiness >= 160 && _guest.HappinessTarget >= 160)
        {
            _guest.FavouriteRideRating = peepRideRating;
            _guest.PeepFlags |= PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE;
        }
    }
}

/* rct2: 0x00695555 */
int16_t GuestRideHelper::CalculateRideValueSatisfaction(const Ride& ride) const
{
    if (gParkFlags & PARK_FLAGS_NO_MONEY)
    {
        return -30;
    }

    if (ride.value == RIDE_VALUE_UNDEFINED)
    {
        return -30;
    }

    auto ridePrice = ride_get_price(ride);
    if (ride.value >= ridePrice)
    {
        return -5;
    }

    if ((ride.value + ((ride.value * _guest.Happiness) / 256)) >= ridePrice)
    {
        return -30;
    }

    return 0;
}

/**
 * Calculate satisfaction based on the intensity and nausea of the ride.
 * The best possible score from this section is achieved by having the intensity and nausea
 * of the ride fall exactly within the peep's preferences, but lower scores can still be achieved
 * if the peep's happiness is enough to offset it.
 */
int16_t GuestRideHelper::CalculateRideIntensityNauseaSatisfaction(const Ride& ride) const
{
    if (!ride_has_ratings(ride))
    {
        return 70;
    }

    uint8_t intensitySatisfaction = 3;
    uint8_t nauseaSatisfaction = 3;
    ride_rating maxIntensity = _guest.Intensity.GetMaximum() * 100;
    ride_rating minIntensity = _guest.Intensity.GetMinimum() * 100;
    if (minIntensity <= ride.intensity && maxIntensity >= ride.intensity)
    {
        intensitySatisfaction--;
    }
    minIntensity -= _guest.Happiness * 2;
    maxIntensity += _guest.Happiness;
    if (minIntensity <= ride.intensity && maxIntensity >= ride.intensity)
    {
        intensitySatisfaction--;
    }
    minIntensity -= _guest.Happiness * 2;
    maxIntensity += _guest.Happiness;
    if (minIntensity <= ride.intensity && maxIntensity >= ride.intensity)
    {
        intensitySatisfaction--;
    }

    // Although it's not shown in the interface, a peep with Average or High nausea tolerance
    // has a minimum preferred nausea value. (For peeps with None or Low, this is set to zero.)
    ride_rating minNausea = NauseaMinimumThresholds[(EnumValue(_guest.NauseaTolerance) & 3)];
    ride_rating maxNausea = NauseaMaximumThresholds[(EnumValue(_guest.NauseaTolerance) & 3)];
    if (minNausea <= ride.nausea && maxNausea >= ride.nausea)
    {
        nauseaSatisfaction--;
    }
    minNausea -= _guest.Happiness * 2;
    maxNausea += _guest.Happiness;
    if (minNausea <= ride.nausea && maxNausea >= ride.nausea)
    {
        nauseaSatisfaction--;
    }
    minNausea -= _guest.Happiness * 2;
    maxNausea += _guest.Happiness;
    if (minNausea <= ride.nausea && maxNausea >= ride.nausea)
    {
        nauseaSatisfaction--;
    }

    uint8_t highestSatisfaction = std::max(intensitySatisfaction, nauseaSatisfaction);
    uint8_t lowestSatisfaction = std::min(intensitySatisfaction, nauseaSatisfaction);

    switch (highestSatisfaction)
    {
        default:
        case 0:
            return 70;
        case 1:
            switch (lowestSatisfaction)
            {
                default:
                case 0:
                    return 50;
                case 1:
                    return 35;
            }
        case 2:
            switch (lowestSatisfaction)
            {
                default:
                case 0:
                    return 35;
                case 1:
                    return 20;
                case 2:
                    return 10;
            }
        case 3:
            switch (lowestSatisfaction)
            {
                default:
                case 0:
                    return -35;
                case 1:
                    return -50;
                case 2:
                    return -60;
                case 3:
                    return -60;
            }
    }
}

/**
 * Update the nausea growth of the peep based on a ride. This is calculated based on:
 * - The nausea rating of the ride
 * - Their new happiness growth rate (the higher, the less nauseous)
 * - How hungry the peep is (+0% nausea at 50% hunger up to +100% nausea at 100% hunger)
 * - The peep's nausea tolerance (Final modifier: none: 100%, low: 50%, average: 25%, high: 12.5%)
 */
void GuestRideHelper::UpdateRideNauseaGrowth(const Ride& ride)
{
    uint32_t nauseaMultiplier = std::clamp(256 - _guest.HappinessTarget, 64, 200);
    uint32_t nauseaGrowthRateChange = (ride.nausea * nauseaMultiplier) / 512;
    nauseaGrowthRateChange *= std::max(static_cast<uint8_t>(128), _guest.Hunger) / 64;
    nauseaGrowthRateChange >>= (EnumValue(_guest.NauseaTolerance) & 3);
    _guest.NauseaTarget = static_cast<uint8_t>(std::min(_guest.NauseaTarget + nauseaGrowthRateChange, 255u));
}

bool GuestRideHelper::ShouldGoOnRideAgain(const Ride& ride) const
{
    if (!ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_PEEP_WILL_RIDE_AGAIN))
        return false;
    if (!ride_has_ratings(ride))
        return false;
    if (ride.intensity > RIDE_RATING(10, 00) && !gCheatsIgnoreRideIntensity)
        return false;
    if (_guest.Happiness < 180)
        return false;
    if (_guest.Energy < 100)
        return false;
    if (_guest.Nausea > 160)
        return false;
    if (_guest.Hunger < 30)
        return false;
    if (_guest.Thirst < 20)
        return false;
    if (_guest.Toilet > 170)
        return false;

    uint8_t r = (scenario_rand() & 0xFF);
    if (r <= 128)
    {
        if (_guest.GuestNumRides > 7)
            return false;
        if (r > 64)
            return false;
    }

    return true;
}

bool GuestRideHelper::ShouldPreferredIntensityIncrease() const
{
    if (gParkFlags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES)
        return false;
    if (_guest.Happiness < 200)
        return false;

    return (scenario_rand() & 0xFF) >= static_cast<uint8_t>(_guest.Intensity);
}

bool GuestRideHelper::ReallyLikedRide(const Ride& ride) const
{
    if (_guest.Happiness < 215)
        return false;
    if (_guest.Nausea > 120)
        return false;
    if (!ride_has_ratings(ride))
        return false;
    if (ride.intensity > RIDE_RATING(10, 00) && !gCheatsIgnoreRideIntensity)
        return false;
    return true;
}

template<typename T> void GuestHeadForNearestRide(Guest& guest, bool considerOnlyCloseRides, T predicate)
{
    if (guest.State != PeepState::Sitting && guest.State != PeepState::Watching && guest.State != PeepState::Walking)
    {
        return;
    }
    if (guest.PeepFlags & PEEP_FLAGS_LEAVING_PARK)
        return;
    if (guest.x == LOCATION_NULL)
        return;
    if (!guest.GuestHeadingToRideId.IsNull())
    {
        auto ride = get_ride(guest.GuestHeadingToRideId);
        if (ride != nullptr && predicate(*ride))
        {
            return;
        }
    }

    OpenRCT2::BitSet<OpenRCT2::Limits::MaxRidesInPark> rideConsideration;
    if (!considerOnlyCloseRides && (guest.HasItem(ShopItem::Map)))
    {
        // Consider all rides in the park
        for (const auto& ride : GetRideManager())
        {
            if (predicate(ride))
            {
                rideConsideration[ride.id.ToUnderlying()] = true;
            }
        }
    }
    else
    {
        // Take nearby rides into consideration
        constexpr auto searchRadius = 10 * 32;
        int32_t cx = floor2(guest.x, 32);
        int32_t cy = floor2(guest.y, 32);
        for (auto x = cx - searchRadius; x <= cx + searchRadius; x += COORDS_XY_STEP)
        {
            for (auto y = cy - searchRadius; y <= cy + searchRadius; y += COORDS_XY_STEP)
            {
                auto location = CoordsXY{ x, y };
                if (!MapIsLocationValid(location))
                    continue;

                for (auto* trackElement : TileElementsView<TrackElement>(location))
                {
                    auto rideIndex = trackElement->GetRideIndex();
                    auto ride = get_ride(rideIndex);
                    if (ride == nullptr)
                        continue;

                    if (!predicate(*ride))
                        continue;

                    rideConsideration[ride->id.ToUnderlying()] = true;
                }
            }
        }
    }

    // Filter the considered rides
    RideId potentialRides[OpenRCT2::Limits::MaxRidesInPark];
    size_t numPotentialRides = 0;
    for (auto& ride : GetRideManager())
    {
        if (rideConsideration[ride.id.ToUnderlying()])
        {
            if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_QUEUE_FULL))
            {
                if (GuestRideHelper(guest).ShouldGoOnRide(ride, StationIndex::FromUnderlying(0), false, true))
                {
                    potentialRides[numPotentialRides++] = ride.id;
                }
            }
        }
    }

    // Pick the closest ride
    Ride* closestRide{};
    auto closestRideDistance = std::numeric_limits<int32_t>::max();
    for (size_t i = 0; i < numPotentialRides; i++)
    {
        auto ride = get_ride(potentialRides[i]);
        if (ride != nullptr)
        {
            auto rideLocation = ride->GetStation().Start;
            int32_t distance = abs(rideLocation.x - guest.x) + abs(rideLocation.y - guest.y);
            if (distance < closestRideDistance)
            {
                closestRide = ride;
                closestRideDistance = distance;
            }
        }
    }
    if (closestRide != nullptr)
    {
        // Head to that ride
        guest.GuestHeadingToRideId = closestRide->id;
        guest.GuestIsLostCountdown = 200;
        guest.ResetPathfindGoal();
        guest.WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_ACTION;
        guest.TimeLost = 0;
    }
}

void GuestRideHelper::HeadForNearestRideWithFlags(bool considerOnlyCloseRides, int64_t rideTypeFlags)
{
    if ((rideTypeFlags & RIDE_TYPE_FLAG_IS_TOILET) && _guest.HasFoodOrDrink())
    {
        return;
    }
    GuestHeadForNearestRide(_guest, considerOnlyCloseRides, [rideTypeFlags](const Ride& ride) {
        return ride.GetRideTypeDescriptor().HasFlag(rideTypeFlags);
    });
}

/**
 *
 *  rct2: 0x00691A3B
 */
void GuestRideHelper::UpdateRideAtEntrance()
{
    auto ride = get_ride(_guest.CurrentRide);
    if (ride == nullptr)
        return;

    // The peep will keep advancing in the entranceway
    // whilst in this state. When it has reached the very
    // front of the queue destination tolerance is set to
    // zero to indicate it is final decision time (try_leave will pass).
    // When a peep has to return to the queue without getting on a ride
    // this is the state it will return to.
    if (_guest.DestinationTolerance != 0)
    {
        int16_t xy_distance;
        if (auto loc = _guest.UpdateAction(xy_distance); loc.has_value())
        {
            int16_t actionZ = _guest.z;
            if (xy_distance < 16)
            {
                const auto& station = ride->GetStation(_guest.CurrentRideStation);
                auto entrance = station.Entrance.ToCoordsXYZ();
                actionZ = entrance.z + 2;
            }
            _guest.MoveTo({ loc.value(), actionZ });
        }
        else
        {
            _guest.DestinationTolerance = 0;
            _guest.sprite_direction ^= (1 << 4);
            _guest.Invalidate();
        }
    }

    std::vector<uint8_t> carArray;

    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES))
    {
        if (ride->num_riders >= ride->operation_option)
            return;
    }
    else
    {
        if (!FindVehicleToEnter(*ride, carArray))
            return;
    }

    if (ride->status != RideStatus::Open || ride->vehicle_change_timeout != 0)
    {
        UpdateRideAtEntranceTryLeave();
        return;
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
        return;

    money16 ridePrice = ride_get_price(*ride);
    if (ridePrice != 0)
    {
        if (!CheckRidePriceAtEntrance(*ride, ridePrice))
            return;
    }

    if (!ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES))
    {
        Vehicle* vehicle = ChooseCarFromRide(*ride, carArray);
        if (vehicle != nullptr)
            ChooseSeatFromCar(*ride, *vehicle);
    }
    GoToRideEntrance(*ride);
}

/** rct2: 0x00981FD4, 0x00981FD6 */
static constexpr const CoordsXY _MazeEntranceStart[] = {
    { 8, 8 },
    { 8, 24 },
    { 24, 24 },
    { 24, 8 },
};

void PeepUpdateRideLeaveEntranceMaze(Guest& guest, Ride& ride, CoordsXYZD& entrance_loc)
{
    guest.MazeLastEdge = entrance_loc.direction + 1;

    entrance_loc.x += CoordsDirectionDelta[entrance_loc.direction].x;
    entrance_loc.y += CoordsDirectionDelta[entrance_loc.direction].y;

    uint8_t direction = entrance_loc.direction * 4 + 11;
    if (scenario_rand() & 0x40)
    {
        direction += 4;
        guest.MazeLastEdge += 2;
    }

    direction &= 0xF;
    // Direction is 11, 15, 3, or 7
    guest.Var37 = direction;
    guest.MazeLastEdge &= 3;

    entrance_loc.x += _MazeEntranceStart[direction / 4].x;
    entrance_loc.y += _MazeEntranceStart[direction / 4].y;

    guest.SetDestination(entrance_loc, 3);

    ride.cur_num_customers++;
    GuestRideHelper(guest).OnEnterRide(ride);
    guest.RideSubState = PeepRideSubState::MazePathfinding;
}

void PeepUpdateRideLeaveEntranceSpiralSlide(Guest& guest, Ride& ride, CoordsXYZD& entrance_loc)
{
    entrance_loc = { ride.GetStation(guest.CurrentRideStation).GetStart(), entrance_loc.direction };

    TileElement* tile_element = ride_get_station_start_track_element(ride, guest.CurrentRideStation);

    uint8_t direction_track = (tile_element == nullptr ? 0 : tile_element->GetDirection());

    guest.Var37 = (entrance_loc.direction << 2) | (direction_track << 4);

    entrance_loc += SpiralSlideWalkingPath[guest.Var37];

    guest.SetDestination(entrance_loc);
    guest.CurrentCar = 0;

    ride.cur_num_customers++;
    GuestRideHelper(guest).OnEnterRide(ride);
    guest.RideSubState = PeepRideSubState::ApproachSpiralSlide;
}

void PeepUpdateRideLeaveEntranceDefault(Guest& guest, Ride& ride, CoordsXYZD& entrance_loc)
{
    // If the ride type was changed guests will become stuck.
    // Inform the player about this if its a new issue or hasn't been addressed within 120 seconds.
    if ((ride.current_issues & RIDE_ISSUE_GUESTS_STUCK) == 0 || gCurrentTicks - ride.last_issue_time > 3000)
    {
        ride.current_issues |= RIDE_ISSUE_GUESTS_STUCK;
        ride.last_issue_time = gCurrentTicks;

        auto ft = Formatter();
        ride.FormatNameTo(ft);
        if (gConfigNotifications.RideWarnings)
        {
            News::AddItemToQueue(News::ItemType::Ride, STR_GUESTS_GETTING_STUCK_ON_RIDE, guest.CurrentRide.ToUnderlying(), ft);
        }
    }
}

uint8_t GuestRideHelper::GetWaypointedSeatLocation(const Ride& ride, CarEntry* vehicle_type, uint8_t track_direction) const
{
    // The seatlocation can be split into segments around the ride base
    // to decide the segment first split off the segmentable seat location
    // from the fixed section
    uint8_t seatLocationSegment = _guest.CurrentSeat & 0x7;
    uint8_t seatLocationFixed = _guest.CurrentSeat & 0xF8;

    // Enterprise has more segments (8) compared to the normal (4)
    if (ride.type != RIDE_TYPE_ENTERPRISE)
        track_direction *= 2;

    // Type 1 loading doesn't do segments and all peeps go to the same
    // location on the ride
    if (vehicle_type->peep_loading_waypoint_segments == 0)
    {
        track_direction /= 2;
        seatLocationSegment = 0;
        seatLocationFixed = 0;
    }
    seatLocationSegment += track_direction;
    seatLocationSegment &= 0x7;
    return seatLocationSegment + seatLocationFixed;
}

void GuestRideHelper::UpdateRideLeaveEntranceWaypoints(const Ride& ride)
{
    const auto& station = ride.GetStation(_guest.CurrentRideStation);
    Guard::Assert(!station.Entrance.IsNull());
    uint8_t direction_entrance = station.Entrance.direction;

    TileElement* tile_element = ride_get_station_start_track_element(ride, _guest.CurrentRideStation);

    uint8_t direction_track = (tile_element == nullptr ? 0 : tile_element->GetDirection());

    auto vehicle = GetEntity<Vehicle>(ride.vehicles[_guest.CurrentTrain]);
    if (vehicle == nullptr)
    {
        // TODO: Goto ride exit on failure.
        return;
    }
    auto ride_entry = vehicle->GetRideEntry();
    auto vehicle_type = &ride_entry->Cars[vehicle->vehicle_type];

    _guest.Var37 = (direction_entrance | GetWaypointedSeatLocation(ride, vehicle_type, direction_track) * 4) * 4;

    const auto& rtd = ride.GetRideTypeDescriptor();
    CoordsXY waypoint = rtd.GetGuestWaypointLocation(*vehicle, ride, _guest.CurrentRideStation);

    const auto waypointIndex = _guest.Var37 / 4;
    Guard::Assert(vehicle_type->peep_loading_waypoints.size() >= static_cast<size_t>(waypointIndex));
    waypoint.x += vehicle_type->peep_loading_waypoints[waypointIndex][0].x;
    waypoint.y += vehicle_type->peep_loading_waypoints[waypointIndex][0].y;

    _guest.SetDestination(waypoint);
    _guest.RideSubState = PeepRideSubState::ApproachVehicleWaypoints;
}

/**
 *
 *  rct2: 0x006921D3
 */
void GuestRideHelper::UpdateRideAdvanceThroughEntrance()
{
    auto ride = get_ride(_guest.CurrentRide);
    if (ride == nullptr)
        return;

    int16_t actionZ, xy_distance;

    auto ride_entry = ride->GetRideEntry();

    if (auto loc = _guest.UpdateAction(xy_distance); loc.has_value())
    {
        uint16_t distanceThreshold = 16;
        if (ride_entry != nullptr)
        {
            uint8_t vehicle = ride_entry->DefaultCar;
            if (ride_entry->Cars[vehicle].flags & CAR_ENTRY_FLAG_MINI_GOLF
                || ride_entry->Cars[vehicle].flags & (CAR_ENTRY_FLAG_CHAIRLIFT | CAR_ENTRY_FLAG_GO_KART))
            {
                distanceThreshold = 28;
            }
        }

        if (_guest.RideSubState == PeepRideSubState::InEntrance && xy_distance < distanceThreshold)
        {
            _guest.RideSubState = PeepRideSubState::FreeVehicleCheck;
        }

        actionZ = ride->GetStation(_guest.CurrentRideStation).GetBaseZ();

        distanceThreshold += 4;
        if (xy_distance < distanceThreshold)
        {
            actionZ += ride->GetRideTypeDescriptor().Heights.PlatformHeight;
        }

        _guest.MoveTo({ loc.value(), actionZ });
        return;
    }

    if (_guest.RideSubState == PeepRideSubState::InEntrance)
    {
        _guest.RideSubState = PeepRideSubState::FreeVehicleCheck;
        return;
    }

    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES))
    {
        const auto& station = ride->GetStation(_guest.CurrentRideStation);
        auto entranceLocation = station.Entrance.ToCoordsXYZD();
        Guard::Assert(!entranceLocation.IsNull());

        const auto& rtd = GetRideTypeDescriptor(ride->type);
        rtd.UpdateLeaveEntrance(_guest, *ride, entranceLocation);
        return;
    }

    Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[_guest.CurrentTrain]);
    if (vehicle == nullptr)
    {
        return;
    }

    vehicle = vehicle->GetCar(_guest.CurrentCar);
    if (vehicle == nullptr)
    {
        return;
    }

    ride_entry = vehicle->GetRideEntry();
    if (ride_entry == nullptr)
    {
        return;
    }

    CarEntry* vehicle_type = &ride_entry->Cars[vehicle->vehicle_type];

    if (vehicle_type->flags & CAR_ENTRY_FLAG_LOADING_WAYPOINTS)
    {
        UpdateRideLeaveEntranceWaypoints(*ride);
        return;
    }

    if (vehicle_type->flags & CAR_ENTRY_FLAG_DODGEM_CAR_PLACEMENT)
    {
        _guest.SetDestination(vehicle->GetLocation(), 15);
        _guest.RideSubState = PeepRideSubState::ApproachVehicle;
        return;
    }

    int8_t load_position = 0;
    // Safe, in case current seat > number of loading positions
    uint16_t numSeatPositions = static_cast<uint16_t>(vehicle_type->peep_loading_positions.size());
    if (numSeatPositions != 0)
    {
        size_t loadPositionIndex = numSeatPositions - 1;
        if (_guest.CurrentSeat < numSeatPositions)
        {
            loadPositionIndex = _guest.CurrentSeat;
        }
        load_position = vehicle_type->peep_loading_positions[loadPositionIndex];
    }

    auto destination = _guest.GetDestination();
    switch (vehicle->sprite_direction / 8)
    {
        case 0:
            destination.x = vehicle->x - load_position;
            break;
        case 1:
            destination.y = vehicle->y + load_position;
            break;
        case 2:
            destination.x = vehicle->x + load_position;
            break;
        case 3:
            destination.y = vehicle->y - load_position;
            break;
    }
    _guest.SetDestination(destination);

    _guest.RideSubState = PeepRideSubState::ApproachVehicle;
}

/**
 *
 *  rct2: 0x0069321D
 */
void GuestRideHelper::GoToRideExit(const Ride& ride, int16_t x, int16_t y, int16_t z, uint8_t exit_direction)
{
    z += ride.GetRideTypeDescriptor().Heights.PlatformHeight;

    _guest.MoveTo({ x, y, z });

    Guard::Assert(_guest.CurrentRideStation.ToUnderlying() < OpenRCT2::Limits::MaxStationsPerRide);
    auto exit = ride.GetStation(_guest.CurrentRideStation).Exit;
    Guard::Assert(!exit.IsNull());
    x = exit.x;
    y = exit.y;
    x *= 32;
    y *= 32;
    x += 16;
    y += 16;

    int16_t x_shift = DirectionOffsets[exit_direction].x;
    int16_t y_shift = DirectionOffsets[exit_direction].y;

    int16_t shift_multiplier = 20;

    rct_ride_entry* rideEntry = get_ride_entry(ride.subtype);
    if (rideEntry != nullptr)
    {
        CarEntry* carEntry = &rideEntry->Cars[rideEntry->DefaultCar];
        if (carEntry->flags & CAR_ENTRY_FLAG_MINI_GOLF || carEntry->flags & (CAR_ENTRY_FLAG_CHAIRLIFT | CAR_ENTRY_FLAG_GO_KART))
        {
            shift_multiplier = 32;
        }
    }

    x_shift *= shift_multiplier;
    y_shift *= shift_multiplier;

    x -= x_shift;
    y -= y_shift;

    _guest.SetDestination({ x, y }, 2);

    _guest.sprite_direction = exit_direction * 8;
    _guest.RideSubState = PeepRideSubState::ApproachExit;
}

/**
 *
 *  rct2: 0x006920B4
 */
void GuestRideHelper::UpdateRideFreeVehicleEnterRide(Ride& ride)
{
    money16 ridePrice = ride_get_price(ride);
    if (ridePrice != 0)
    {
        if ((_guest.HasItem(ShopItem::Voucher)) && (_guest.VoucherType == VOUCHER_TYPE_RIDE_FREE)
            && (_guest.VoucherRideId == _guest.CurrentRide))
        {
            _guest.RemoveItem(ShopItem::Voucher);
            _guest.WindowInvalidateFlags |= PEEP_INVALIDATE_PEEP_INVENTORY;
        }
        else
        {
            ride.total_profit += ridePrice;
            ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
            _guest.SpendMoney(_guest.PaidOnRides, ridePrice, ExpenditureType::ParkRideTickets);
        }
    }

    _guest.RideSubState = PeepRideSubState::LeaveEntrance;
    uint8_t queueTime = _guest.DaysInQueue;
    if (queueTime < 253)
        queueTime += 3;

    queueTime /= 2;
    auto& station = ride.GetStation(_guest.CurrentRideStation);
    if (queueTime != station.QueueTime)
    {
        station.QueueTime = queueTime;
        window_invalidate_by_number(WindowClass::Ride, _guest.CurrentRide.ToUnderlying());
    }

    if (_guest.PeepFlags & PEEP_FLAGS_TRACKING)
    {
        auto ft = Formatter();
        _guest.FormatNameTo(ft);
        ride.FormatNameTo(ft);

        StringId msg_string;
        if (ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IN_RIDE))
            msg_string = STR_PEEP_TRACKING_PEEP_IS_IN_X;
        else
            msg_string = STR_PEEP_TRACKING_PEEP_IS_ON_X;

        if (gConfigNotifications.GuestOnRide)
        {
            News::AddItemToQueue(News::ItemType::PeepOnRide, msg_string, _guest.sprite_index, ft);
        }
    }

    const auto& rtd = ride.GetRideTypeDescriptor();
    if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_SPIRAL_SLIDE))
    {
        _guest.SwitchToSpecialSprite(1);
    }

    UpdateRideAdvanceThroughEntrance();
}

/**
 *
 *  rct2: 0x00691FD4
 */
void GuestRideHelper::UpdateRideNoFreeVehicleRejoinQueue(Ride& ride)
{
    TileCoordsXYZD entranceLocation = ride.GetStation(_guest.CurrentRideStation).Entrance;

    int32_t x = entranceLocation.x * 32;
    int32_t y = entranceLocation.y * 32;
    x += 16 - DirectionOffsets[entranceLocation.direction].x * 20;
    y += 16 - DirectionOffsets[entranceLocation.direction].y * 20;

    _guest.SetDestination({ x, y }, 2);
    _guest.SetState(PeepState::QueuingFront);
    _guest.RideSubState = PeepRideSubState::AtEntrance;

    ride.QueueInsertGuestAtFront(_guest.CurrentRideStation, &_guest);
}

/**
 *
 *  rct2: 0x00691E42
 * Note: Before this was the entry
 * point for sub state 1 and 3. The
 * check has been removed that would
 * branch it out to 1 and 3. Now uses
 * separate functions.
 */
void GuestRideHelper::UpdateRideFreeVehicleCheck()
{
    auto ride = get_ride(_guest.CurrentRide);
    if (ride == nullptr)
        return;

    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES))
    {
        if (ride->status != RideStatus::Open || ride->vehicle_change_timeout != 0 || (++_guest.RejoinQueueTimeout) == 0)
        {
            UpdateRideNoFreeVehicleRejoinQueue(*ride);
            return;
        }

        UpdateRideFreeVehicleEnterRide(*ride);
        return;
    }

    Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[_guest.CurrentTrain]);
    if (vehicle == nullptr)
    {
        // TODO: Leave ride on failure goes for all returns on nullptr in this function
        return;
    }
    vehicle = vehicle->GetCar(_guest.CurrentCar);
    if (vehicle == nullptr)
        return;

    rct_ride_entry* ride_entry = vehicle->GetRideEntry();
    if (ride_entry == nullptr)
    {
        return;
    }

    if (ride_entry->Cars[0].flags & CAR_ENTRY_FLAG_MINI_GOLF)
    {
        vehicle->mini_golf_flags &= ~MiniGolfFlag::Flag5;

        for (size_t i = 0; i < ride->NumTrains; ++i)
        {
            Vehicle* train = GetEntity<Vehicle>(ride->vehicles[i]);
            if (train == nullptr)
                continue;

            Vehicle* second_vehicle = GetEntity<Vehicle>(train->next_vehicle_on_train);
            if (second_vehicle == nullptr)
                continue;

            if (second_vehicle->num_peeps == 0)
                continue;

            if (second_vehicle->mini_golf_flags & MiniGolfFlag::Flag5)
                continue;

            return;
        }
    }

    if (!vehicle->IsUsedInPairs())
    {
        UpdateRideFreeVehicleEnterRide(*ride);
        return;
    }

    if (ride->mode == RideMode::ForwardRotation || ride->mode == RideMode::BackwardRotation)
    {
        if (_guest.CurrentSeat & 1 || !(vehicle->next_free_seat & 1))
        {
            UpdateRideFreeVehicleEnterRide(*ride);
            return;
        }
    }
    else
    {
        uint8_t seat = _guest.CurrentSeat | 1;
        if (seat < vehicle->next_free_seat)
        {
            UpdateRideFreeVehicleEnterRide(*ride);
            return;
        }
    }

    Vehicle* currentTrain = GetEntity<Vehicle>(ride->vehicles[_guest.CurrentTrain]);
    if (currentTrain == nullptr)
    {
        return;
    }
    if (ride->status == RideStatus::Open && ++_guest.RejoinQueueTimeout != 0
        && !currentTrain->HasUpdateFlag(VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART))
    {
        return;
    }

    if (ride->mode != RideMode::ForwardRotation && ride->mode != RideMode::BackwardRotation)
    {
        if (vehicle->next_free_seat - 1 != _guest.CurrentSeat)
            return;
    }

    vehicle->next_free_seat--;
    vehicle->peep[_guest.CurrentSeat] = EntityId::GetNull();

    UpdateRideNoFreeVehicleRejoinQueue(*ride);
}

void GuestRideHelper::UpdateRideApproachVehicle()
{
    if (auto loc = _guest.UpdateAction(); loc.has_value())
    {
        _guest.MoveTo({ loc.value(), _guest.z });
        return;
    }
    _guest.RideSubState = PeepRideSubState::EnterVehicle;
}

void GuestRideHelper::UpdateRideEnterVehicle()
{
    auto* ride = get_ride(_guest.CurrentRide);
    if (ride != nullptr)
    {
        auto* vehicle = GetEntity<Vehicle>(ride->vehicles[_guest.CurrentTrain]);
        if (vehicle != nullptr)
        {
            vehicle = vehicle->GetCar(_guest.CurrentCar);
            if (vehicle == nullptr)
            {
                return;
            }

            if (ride->mode != RideMode::ForwardRotation && ride->mode != RideMode::BackwardRotation)
            {
                if (_guest.CurrentSeat != vehicle->num_peeps)
                    return;
            }

            if (vehicle->IsUsedInPairs())
            {
                auto* seatedGuest = GetEntity<Guest>(vehicle->peep[_guest.CurrentSeat ^ 1]);
                if (seatedGuest != nullptr)
                {
                    if (seatedGuest->RideSubState != PeepRideSubState::EnterVehicle)
                        return;

                    vehicle->num_peeps++;
                    ride->cur_num_customers++;

                    vehicle->ApplyMass(seatedGuest->Mass);
                    seatedGuest->MoveTo({ LOCATION_NULL, 0, 0 });
                    seatedGuest->SetState(PeepState::OnRide);
                    seatedGuest->GuestTimeOnRide = 0;
                    seatedGuest->RideSubState = PeepRideSubState::OnRide;
                    GuestRideHelper(*seatedGuest).OnEnterRide(*ride);
                }
            }

            vehicle->num_peeps++;
            ride->cur_num_customers++;

            vehicle->ApplyMass(_guest.Mass);
            vehicle->Invalidate();

            _guest.MoveTo({ LOCATION_NULL, 0, 0 });

            _guest.SetState(PeepState::OnRide);

            _guest.GuestTimeOnRide = 0;
            _guest.RideSubState = PeepRideSubState::OnRide;
            OnEnterRide(*ride);
        }
    }
}

/**
 *
 *  rct2: 0x00693028
 */
void GuestRideHelper::UpdateRideLeaveVehicle()
{
    auto ride = get_ride(_guest.CurrentRide);
    if (ride == nullptr)
        return;

    Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[_guest.CurrentTrain]);
    if (vehicle == nullptr)
        return;

    StationIndex ride_station = vehicle->current_station;
    vehicle = vehicle->GetCar(_guest.CurrentCar);
    if (vehicle == nullptr)
    {
        return;
    }

    // Check if ride is NOT Ferris Wheel.
    if (ride->mode != RideMode::ForwardRotation && ride->mode != RideMode::BackwardRotation)
    {
        if (vehicle->num_peeps - 1 != _guest.CurrentSeat)
            return;
    }

    _guest.ActionSpriteImageOffset++;
    if (_guest.ActionSpriteImageOffset & 3)
        return;

    _guest.ActionSpriteImageOffset = 0;

    vehicle->num_peeps--;
    vehicle->ApplyMass(-_guest.Mass);
    vehicle->Invalidate();

    if (ride_station.ToUnderlying() >= OpenRCT2::Limits::MaxStationsPerRide)
    {
        // HACK #5658: Some parks have hacked rides which end up in this state
        auto bestStationIndex = ride_get_first_valid_station_exit(*ride);
        if (bestStationIndex.IsNull())
        {
            bestStationIndex = StationIndex::FromUnderlying(0);
        }
        ride_station = bestStationIndex;
    }
    _guest.CurrentRideStation = ride_station;
    rct_ride_entry* rideEntry = vehicle->GetRideEntry();
    if (rideEntry == nullptr)
    {
        return;
    }

    CarEntry* carEntry = &rideEntry->Cars[vehicle->vehicle_type];

    assert(_guest.CurrentRideStation.ToUnderlying() < OpenRCT2::Limits::MaxStationsPerRide);
    auto& station = ride->GetStation(_guest.CurrentRideStation);

    if (!(carEntry->flags & CAR_ENTRY_FLAG_LOADING_WAYPOINTS))
    {
        TileCoordsXYZD exitLocation = station.Exit;
        CoordsXYZD platformLocation;
        platformLocation.z = station.GetBaseZ();

        platformLocation.direction = DirectionReverse(exitLocation.direction);

        if (!ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_VEHICLE_IS_INTEGRAL))
        {
            for (; vehicle != nullptr && !vehicle->IsHead(); vehicle = GetEntity<Vehicle>(vehicle->prev_vehicle_on_ride))
            {
                auto trackType = vehicle->GetTrackType();
                if (trackType == TrackElemType::Flat || trackType > TrackElemType::MiddleStation)
                    continue;

                bool foundStation = false;
                for (auto* trackElement : TileElementsView<TrackElement>(vehicle->TrackLocation))
                {
                    if (trackElement->GetBaseZ() != vehicle->TrackLocation.z)
                        continue;

                    if (trackElement->GetStationIndex() != _guest.CurrentRideStation)
                        continue;

                    foundStation = true;
                    break;
                }

                if (foundStation)
                    break;
            }

            if (vehicle == nullptr)
            {
                return;
            }
            uint8_t shiftMultiplier = 12;
            uint8_t specialDirection = platformLocation.direction;

            rideEntry = get_ride_entry(ride->subtype);

            if (rideEntry != nullptr)
            {
                carEntry = &rideEntry->Cars[rideEntry->DefaultCar];

                if (carEntry->flags & CAR_ENTRY_FLAG_GO_KART)
                {
                    shiftMultiplier = 9;
                }

                if (carEntry->flags & (CAR_ENTRY_FLAG_CHAIRLIFT | CAR_ENTRY_FLAG_GO_KART))
                {
                    specialDirection = ((vehicle->sprite_direction + 3) / 8) + 1;
                    specialDirection &= 3;

                    if (vehicle->TrackSubposition == VehicleTrackSubposition::GoKartsRightLane)
                        specialDirection = DirectionReverse(specialDirection);
                }
            }

            int16_t xShift = DirectionOffsets[specialDirection].x;
            int16_t yShift = DirectionOffsets[specialDirection].y;

            platformLocation.x = vehicle->x + xShift * shiftMultiplier;
            platformLocation.y = vehicle->y + yShift * shiftMultiplier;

            GoToRideExit(*ride, platformLocation.x, platformLocation.y, platformLocation.z, platformLocation.direction);
            return;
        }

        platformLocation.x = vehicle->x + DirectionOffsets[platformLocation.direction].x * 12;
        platformLocation.y = vehicle->y + DirectionOffsets[platformLocation.direction].y * 12;

        // This can evaluate to false with buggy custom rides.
        if (_guest.CurrentSeat < carEntry->peep_loading_positions.size())
        {
            int8_t loadPosition = carEntry->peep_loading_positions[_guest.CurrentSeat];

            switch (vehicle->sprite_direction / 8)
            {
                case 0:
                    platformLocation.x -= loadPosition;
                    break;
                case 1:
                    platformLocation.y += loadPosition;
                    break;
                case 2:
                    platformLocation.x += loadPosition;
                    break;
                case 3:
                    platformLocation.y -= loadPosition;
                    break;
            }
        }
        else
        {
            log_verbose(
                "CurrentSeat %d is too large! (Vehicle entry has room for %d.)", _guest.CurrentSeat,
                carEntry->peep_loading_positions.size());
        }

        platformLocation.z = station.GetBaseZ();

        GoToRideExit(*ride, platformLocation.x, platformLocation.y, platformLocation.z, platformLocation.direction);
        return;
    }

    auto exitLocation = station.Exit.ToCoordsXYZD();
    Guard::Assert(!exitLocation.IsNull());

    TileElement* trackElement = ride_get_station_start_track_element(*ride, _guest.CurrentRideStation);

    Direction station_direction = (trackElement == nullptr ? 0 : trackElement->GetDirection());

    vehicle = GetEntity<Vehicle>(ride->vehicles[_guest.CurrentTrain]);
    if (vehicle == nullptr)
    {
        return;
    }

    CoordsXYZ waypointLoc;
    const auto& rtd = ride->GetRideTypeDescriptor();
    waypointLoc = { rtd.GetGuestWaypointLocation(*vehicle, *ride, _guest.CurrentRideStation),
                    exitLocation.z + ride->GetRideTypeDescriptor().Heights.PlatformHeight };

    rideEntry = vehicle->GetRideEntry();
    carEntry = &rideEntry->Cars[vehicle->vehicle_type];
    if (carEntry == nullptr)
        return;

    _guest.Var37 = ((exitLocation.direction | GetWaypointedSeatLocation(*ride, carEntry, station_direction) * 4) * 4) | 1;
    Guard::Assert(carEntry->peep_loading_waypoints.size() >= static_cast<size_t>(_guest.Var37 / 4));
    CoordsXYZ exitWaypointLoc = waypointLoc;

    exitWaypointLoc.x += carEntry->peep_loading_waypoints[_guest.Var37 / 4][2].x;
    exitWaypointLoc.y += carEntry->peep_loading_waypoints[_guest.Var37 / 4][2].y;

    if (ride->type == RIDE_TYPE_MOTION_SIMULATOR)
        exitWaypointLoc.z += 15;

    _guest.MoveTo(exitWaypointLoc);

    waypointLoc.x += carEntry->peep_loading_waypoints[_guest.Var37 / 4][1].x;
    waypointLoc.y += carEntry->peep_loading_waypoints[_guest.Var37 / 4][1].y;

    _guest.SetDestination(waypointLoc, 2);
    _guest.RideSubState = PeepRideSubState::ApproachExitWaypoints;
}

/**
 *
 *  rct2: 0x0069376A
 */
void GuestRideHelper::UpdateRidePrepareForExit()
{
    auto ride = get_ride(_guest.CurrentRide);
    if (ride == nullptr || _guest.CurrentRideStation.ToUnderlying() >= std::size(ride->GetStations()))
        return;

    auto exit = ride->GetStation(_guest.CurrentRideStation).Exit;
    auto newDestination = exit.ToCoordsXY().ToTileCentre();
    auto xShift = DirectionOffsets[exit.direction].x;
    auto yShift = DirectionOffsets[exit.direction].y;

    int16_t shiftMultiplier = 20;

    rct_ride_entry* rideEntry = ride->GetRideEntry();
    if (rideEntry != nullptr)
    {
        CarEntry* carEntry = &rideEntry->Cars[rideEntry->DefaultCar];
        if (carEntry->flags & (CAR_ENTRY_FLAG_CHAIRLIFT | CAR_ENTRY_FLAG_GO_KART))
        {
            shiftMultiplier = 32;
        }
    }

    xShift *= shiftMultiplier;
    yShift *= shiftMultiplier;

    newDestination.x -= xShift;
    newDestination.y -= yShift;

    _guest.SetDestination(newDestination, 2);
    _guest.RideSubState = PeepRideSubState::InExit;
}

/**
 *
 *  rct2: 0x0069374F
 */
void GuestRideHelper::UpdateRideApproachExit()
{
    if (auto loc = _guest.UpdateAction(); loc.has_value())
    {
        _guest.MoveTo({ loc.value(), _guest.z });
        return;
    }

    UpdateRidePrepareForExit();
}

/**
 *
 *  rct2: 0x0069382E
 */
void GuestRideHelper::UpdateRideInExit()
{
    auto ride = get_ride(_guest.CurrentRide);
    if (ride == nullptr)
        return;

    int16_t xy_distance;

    if (auto loc = _guest.UpdateAction(xy_distance); loc.has_value())
    {
        if (xy_distance >= 16)
        {
            int16_t actionZ = ride->GetStation(_guest.CurrentRideStation).GetBaseZ();

            actionZ += ride->GetRideTypeDescriptor().Heights.PlatformHeight;
            _guest.MoveTo({ loc.value(), actionZ });
            return;
        }

        _guest.SwitchToSpecialSprite(0);
        _guest.MoveTo({ loc.value(), _guest.z });
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO)
    {
        ShopItem secondaryItem = ride->GetRideTypeDescriptor().PhotoItem;
        if (_guest.DecideAndBuyItem(*ride, secondaryItem, ride->price[1]))
        {
            ride->no_secondary_items_sold++;
        }
    }
    _guest.RideSubState = PeepRideSubState::LeaveExit;
}
#pragma warning(default : 6011)

CoordsXY GetGuestWaypointLocationDefault(const Vehicle& vehicle, const Ride& ride, const StationIndex& CurrentRideStation)
{
    return ride.GetStation(CurrentRideStation).Start.ToTileCentre();
}

CoordsXY GetGuestWaypointLocationEnterprise(const Vehicle& vehicle, const Ride& ride, const StationIndex& CurrentRideStation)
{
    return { vehicle.x, vehicle.y };
}

/**
 *
 *  rct2: 0x006926AD
 */
void GuestRideHelper::UpdateRideApproachVehicleWaypoints()
{
    auto ride = get_ride(_guest.CurrentRide);
    if (ride == nullptr)
        return;

    int16_t xy_distance;
    uint8_t waypoint = _guest.Var37 & 3;

    const auto& rtd = ride->GetRideTypeDescriptor();
    if (auto loc = _guest.UpdateAction(xy_distance); loc.has_value())
    {
        rtd.UpdateRideApproachVehicleWaypoints(_guest, loc.value(), xy_distance);
        return;
    }

    if (waypoint == 2)
    {
        _guest.RideSubState = PeepRideSubState::EnterVehicle;
        return;
    }

    waypoint++;
    // This is incrementing the actual peep waypoint
    _guest.Var37++;

    Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[_guest.CurrentTrain]);
    if (vehicle == nullptr)
    {
        return;
    }

    CoordsXY targetLoc = rtd.GetGuestWaypointLocation(*vehicle, *ride, _guest.CurrentRideStation);

    rct_ride_entry* ride_entry = vehicle->GetRideEntry();
    if (ride_entry == nullptr)
    {
        return;
    }

    CarEntry* vehicle_type = &ride_entry->Cars[vehicle->vehicle_type];
    Guard::Assert(waypoint < 3);
    targetLoc.x += vehicle_type->peep_loading_waypoints[_guest.Var37 / 4][waypoint].x;
    targetLoc.y += vehicle_type->peep_loading_waypoints[_guest.Var37 / 4][waypoint].y;

    _guest.SetDestination(targetLoc);
}

void UpdateRideApproachVehicleWaypointsMotionSimulator(Guest& guest, const CoordsXY& loc, int16_t& xy_distance)
{
    int16_t actionZ;
    auto ride = get_ride(guest.CurrentRide);
    // Motion simulators have steps this moves the peeps up the steps
    actionZ = ride->GetStation(guest.CurrentRideStation).GetBaseZ() + 2;

    uint8_t waypoint = guest.Var37 & 3;
    if (waypoint == 2)
    {
        xy_distance -= 12;
        if (xy_distance < 0)
            xy_distance = 0;

        if (xy_distance <= 15)
        {
            actionZ += 15 - xy_distance;
        }
    }
    guest.MoveTo({ loc, actionZ });
}

void UpdateRideApproachVehicleWaypointsDefault(Guest& guest, const CoordsXY& loc, int16_t& xy_distance)
{
    guest.MoveTo({ loc, guest.z });
}

/**
 *
 *  rct2: 0x0069357D
 */
void GuestRideHelper::UpdateRideApproachExitWaypoints()
{
    auto ride = get_ride(_guest.CurrentRide);
    if (ride == nullptr)
        return;

    int16_t xy_distance;

    if (auto loc = _guest.UpdateAction(xy_distance); loc.has_value())
    {
        int16_t actionZ;

        if (ride->type == RIDE_TYPE_MOTION_SIMULATOR)
        {
            actionZ = ride->GetStation(_guest.CurrentRideStation).GetBaseZ() + 2;

            if ((_guest.Var37 & 3) == 1)
            {
                if (xy_distance > 15)
                    xy_distance = 15;

                actionZ += xy_distance;
            }
        }
        else
        {
            actionZ = _guest.z;
        }
        _guest.MoveTo({ loc.value(), actionZ });
        return;
    }

    if ((_guest.Var37 & 3) != 0)
    {
        if ((_guest.Var37 & 3) == 3)
        {
            UpdateRidePrepareForExit();
            return;
        }

        _guest.Var37--;
        Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[_guest.CurrentTrain]);
        if (vehicle == nullptr)
        {
            return;
        }

        const auto& rtd = ride->GetRideTypeDescriptor();
        CoordsXY targetLoc = rtd.GetGuestWaypointLocation(*vehicle, *ride, _guest.CurrentRideStation);

        rct_ride_entry* rideEntry = vehicle->GetRideEntry();
        CarEntry* carEntry = &rideEntry->Cars[vehicle->vehicle_type];

        Guard::Assert((_guest.Var37 & 3) < 3);
        targetLoc.x += carEntry->peep_loading_waypoints[_guest.Var37 / 4][_guest.Var37 & 3].x;
        targetLoc.y += carEntry->peep_loading_waypoints[_guest.Var37 / 4][_guest.Var37 & 3].y;

        _guest.SetDestination(targetLoc);
        return;
    }

    _guest.Var37 |= 3;

    auto targetLoc = ride->GetStation(_guest.CurrentRideStation).Exit.ToCoordsXYZD().ToTileCentre();
    uint8_t exit_direction = DirectionReverse(targetLoc.direction);

    int16_t x_shift = DirectionOffsets[exit_direction].x;
    int16_t y_shift = DirectionOffsets[exit_direction].y;

    int16_t shift_multiplier = 20;

    auto rideEntry = get_ride_entry(ride->subtype);
    if (rideEntry != nullptr)
    {
        auto carEntry = &rideEntry->Cars[rideEntry->DefaultCar];
        if (carEntry->flags & (CAR_ENTRY_FLAG_CHAIRLIFT | CAR_ENTRY_FLAG_GO_KART))
        {
            shift_multiplier = 32;
        }
    }

    x_shift *= shift_multiplier;
    y_shift *= shift_multiplier;

    targetLoc.x -= x_shift;
    targetLoc.y -= y_shift;

    _guest.SetDestination(targetLoc);
}

/**
 *
 *  rct2: 0x006927B3
 */
void GuestRideHelper::UpdateRideApproachSpiralSlide()
{
    auto ride = get_ride(_guest.CurrentRide);
    if (ride == nullptr)
        return;

    if (auto loc = _guest.UpdateAction(); loc.has_value())
    {
        _guest.MoveTo({ loc.value(), _guest.z });
        return;
    }

    uint8_t waypoint = _guest.Var37 & 3;

    if (waypoint == 3)
    {
        _guest.SubState = 15;
        _guest.SetDestination({ 0, 0 });
        _guest.Var37 = (_guest.Var37 / 4) & 0xC;
        _guest.MoveTo({ LOCATION_NULL, _guest.y, _guest.z });
        return;
    }

    [[maybe_unused]] const auto& rtd = ride->GetRideTypeDescriptor();
    if (waypoint == 2)
    {
        bool lastRide = false;
        if (ride->status != RideStatus::Open)
            lastRide = true;
        else if (_guest.CurrentCar++ != 0)
        {
            if (ride->mode == RideMode::SingleRidePerAdmission)
                lastRide = true;
            if (static_cast<uint8_t>(_guest.CurrentCar - 1) > (scenario_rand() & 0xF))
                lastRide = true;
        }

        if (lastRide)
        {
            auto exit = ride->GetStation(_guest.CurrentRideStation).Exit;
            waypoint = 1;
            _guest.Var37 = (exit.direction * 4) | (_guest.Var37 & 0x30) | waypoint;
            CoordsXY targetLoc = ride->GetStation(_guest.CurrentRideStation).Start;

            assert(rtd.HasFlag(RIDE_TYPE_FLAG_IS_SPIRAL_SLIDE));
            targetLoc += SpiralSlideWalkingPath[_guest.Var37];

            _guest.SetDestination(targetLoc);
            _guest.RideSubState = PeepRideSubState::LeaveSpiralSlide;
            return;
        }
    }
    waypoint++;
    // Actually increment the real peep waypoint
    _guest.Var37++;

    CoordsXY targetLoc = ride->GetStation(_guest.CurrentRideStation).Start;

    assert(rtd.HasFlag(RIDE_TYPE_FLAG_IS_SPIRAL_SLIDE));
    targetLoc += SpiralSlideWalkingPath[_guest.Var37];

    _guest.SetDestination(targetLoc);
}

/** rct2: 0x00981F0C, 0x00981F0E */
static constexpr const CoordsXY _SpiralSlideEnd[] = {
    { 25, 56 },
    { 56, 7 },
    { 7, -24 },
    { -24, 25 },
};

/** rct2: 0x00981F1C, 0x00981F1E */
static constexpr const CoordsXY _SpiralSlideEndWaypoint[] = {
    { 8, 56 },
    { 56, 24 },
    { 24, -24 },
    { -24, 8 },
};

/**
 *
 *  rct2: 0x00692D83
 */
void GuestRideHelper::UpdateRideOnSpiralSlide()
{
    auto ride = get_ride(_guest.CurrentRide);

    if (ride == nullptr)
        return;

    const auto& rtd = ride->GetRideTypeDescriptor();
    if (!rtd.HasFlag(RIDE_TYPE_FLAG_IS_SPIRAL_SLIDE))
        return;

    auto destination = _guest.GetDestination();
    if ((_guest.Var37 & 3) == 0)
    {
        switch (destination.x)
        {
            case 0:
                destination.y++;
                if (destination.y >= 30)
                    destination.x++;

                _guest.SetDestination(destination);
                return;
            case 1:
                if (ride->slide_in_use != 0)
                    return;

                ride->slide_in_use++;
                ride->slide_peep = _guest.sprite_index;
                ride->slide_peep_t_shirt_colour = _guest.TshirtColour;
                ride->spiral_slide_progress = 0;
                destination.x++;

                _guest.SetDestination(destination);
                return;
            case 2:
                return;
            case 3:
            {
                auto newLocation = ride->GetStation(_guest.CurrentRideStation).Start;
                uint8_t dir = (_guest.Var37 / 4) & 3;

                // Set the location that the peep walks to go on slide again
                destination = newLocation + _SpiralSlideEndWaypoint[dir];
                _guest.SetDestination(destination);

                // Move the peep sprite to just at the end of the slide
                newLocation.x += _SpiralSlideEnd[dir].x;
                newLocation.y += _SpiralSlideEnd[dir].y;

                _guest.MoveTo({ newLocation, _guest.z });

                _guest.sprite_direction = (_guest.Var37 & 0xC) * 2;

                _guest.Var37++;
                return;
            }
            default:
                return;
        }
    }

    if (auto loc = _guest.UpdateAction(); loc.has_value())
    {
        _guest.MoveTo({ loc.value(), _guest.z });
        return;
    }

    uint8_t waypoint = 2;
    _guest.Var37 = (_guest.Var37 * 4 & 0x30) + waypoint;

    CoordsXY targetLoc = ride->GetStation(_guest.CurrentRideStation).Start;

    targetLoc += SpiralSlideWalkingPath[_guest.Var37];

    _guest.SetDestination(targetLoc);
    _guest.RideSubState = PeepRideSubState::ApproachSpiralSlide;
}

/**
 *
 *  rct2: 0x00692C6B
 */
void GuestRideHelper::UpdateRideLeaveSpiralSlide()
{
    // Iterates through the spiral slide waypoints until it reaches
    // waypoint 0. Then it readies to leave the ride by the entrance.
    if (auto loc = _guest.UpdateAction(); loc.has_value())
    {
        _guest.MoveTo({ loc.value(), _guest.z });
        return;
    }

    auto ride = get_ride(_guest.CurrentRide);
    if (ride == nullptr)
        return;

    uint8_t waypoint = _guest.Var37 & 3;

    if (waypoint != 0)
    {
        if (waypoint == 3)
        {
            UpdateRidePrepareForExit();
            return;
        }

        waypoint--;
        // Actually decrement the peep waypoint
        _guest.Var37--;
        CoordsXY targetLoc = ride->GetStation(_guest.CurrentRideStation).Start;

        [[maybe_unused]] const auto& rtd = ride->GetRideTypeDescriptor();
        assert(rtd.HasFlag(RIDE_TYPE_FLAG_IS_SPIRAL_SLIDE));
        targetLoc += SpiralSlideWalkingPath[_guest.Var37];

        _guest.SetDestination(targetLoc);
        return;
    }

    // Actually force the final waypoint
    _guest.Var37 |= 3;

    auto targetLoc = ride->GetStation(_guest.CurrentRideStation).Exit.ToCoordsXYZD().ToTileCentre();

    int16_t xShift = DirectionOffsets[DirectionReverse(targetLoc.direction)].x;
    int16_t yShift = DirectionOffsets[DirectionReverse(targetLoc.direction)].y;

    int16_t shiftMultiplier = 20;

    xShift *= shiftMultiplier;
    yShift *= shiftMultiplier;

    targetLoc.x -= xShift;
    targetLoc.y -= yShift;

    _guest.SetDestination(targetLoc);
}

/** rct2: 0x00981FE4 */
static constexpr const uint8_t _MazeGetNewDirectionFromEdge[][4] = {
    { 15, 7, 15, 7 },
    { 11, 3, 11, 3 },
    { 7, 15, 7, 15 },
    { 3, 11, 3, 11 },
};

/** rct2: 0x00981FF4 */
static constexpr const uint8_t _MazeCurrentDirectionToOpenHedge[][4] = {
    { 1, 2, 14, 0 },
    { 4, 5, 6, 2 },
    { 6, 8, 9, 10 },
    { 14, 10, 12, 13 },
};

/**
 *
 *  rct2: 0x00692A83
 */
void GuestRideHelper::UpdateRideMazePathfinding()
{
    if (auto loc = _guest.UpdateAction(); loc.has_value())
    {
        _guest.MoveTo({ loc.value(), _guest.z });
        return;
    }

    auto ride = get_ride(_guest.CurrentRide);
    if (ride == nullptr)
        return;

    if (_guest.Var37 == 16)
    {
        UpdateRidePrepareForExit();
        return;
    }

    if (_guest.IsActionInterruptable())
    {
        if (_guest.Energy > 80 && !(_guest.PeepFlags & PEEP_FLAGS_SLOW_WALK) && !ClimateIsRaining()
            && (scenario_rand() & 0xFFFF) <= 2427)
        {
            _guest.Action = PeepActionType::Jump;
            _guest.ActionFrame = 0;
            _guest.ActionSpriteImageOffset = 0;
            _guest.UpdateCurrentActionSpriteType();
        }
    }

    auto targetLoc = _guest.GetDestination().ToTileStart();

    auto stationBaseZ = ride->GetStation().GetBaseZ();

    // Find the station track element
    auto trackElement = MapGetTrackElementAt({ targetLoc, stationBaseZ });
    if (trackElement == nullptr)
    {
        return;
    }

    uint16_t mazeEntry = trackElement->GetMazeEntry();
    // _guest.Var37 is 3, 7, 11 or 15
    uint8_t hedges[4]{ 0xFF, 0xFF, 0xFF, 0xFF };
    uint8_t openCount = 0;
    uint8_t mazeReverseLastEdge = DirectionReverse(_guest.MazeLastEdge);
    for (uint8_t i = 0; i < NumOrthogonalDirections; ++i)
    {
        if (!(mazeEntry & (1 << _MazeCurrentDirectionToOpenHedge[_guest.Var37 / 4][i])) && i != mazeReverseLastEdge)
        {
            hedges[openCount++] = i;
        }
    }

    if (openCount == 0)
    {
        if ((mazeEntry & (1 << _MazeCurrentDirectionToOpenHedge[_guest.Var37 / 4][mazeReverseLastEdge])))
        {
            return;
        }
        hedges[openCount++] = mazeReverseLastEdge;
    }

    uint8_t chosenEdge = hedges[scenario_rand() % openCount];
    assert(chosenEdge != 0xFF);

    targetLoc = _guest.GetDestination() + CoordsDirectionDelta[chosenEdge] / 2;

    enum class maze_type
    {
        invalid,
        hedge,
        entrance_or_exit
    };
    maze_type mazeType = maze_type::invalid;

    auto tileElement = MapGetFirstElementAt(targetLoc);
    if (tileElement == nullptr)
        return;
    do
    {
        if (stationBaseZ != tileElement->GetBaseZ())
            continue;

        if (tileElement->GetType() == TileElementType::Track)
        {
            mazeType = maze_type::hedge;
            break;
        }

        if (tileElement->GetType() == TileElementType::Entrance
            && tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_RIDE_EXIT)
        {
            mazeType = maze_type::entrance_or_exit;
            break;
        }
    } while (!(tileElement++)->IsLastForTile());

    switch (mazeType)
    {
        case maze_type::invalid:
            _guest.MazeLastEdge++;
            _guest.MazeLastEdge &= 3;
            return;
        case maze_type::hedge:
            _guest.SetDestination(targetLoc);
            _guest.Var37 = _MazeGetNewDirectionFromEdge[_guest.Var37 / 4][chosenEdge];
            _guest.MazeLastEdge = chosenEdge;
            break;
        case maze_type::entrance_or_exit:
            targetLoc = _guest.GetDestination();
            if (chosenEdge & 1)
            {
                targetLoc.x = targetLoc.ToTileCentre().x;
            }
            else
            {
                targetLoc.y = targetLoc.ToTileCentre().y;
            }
            _guest.SetDestination(targetLoc);
            _guest.Var37 = 16;
            _guest.MazeLastEdge = chosenEdge;
            break;
    }

    if (auto loc = _guest.UpdateAction(); loc.has_value())
    {
        _guest.MoveTo({ loc.value(), _guest.z });
        return;
    }
}

/**
 *
 *  rct2: 0x006938D2
 */
void GuestRideHelper::UpdateRideLeaveExit()
{
    auto ride = get_ride(_guest.CurrentRide);

    if (auto loc = _guest.UpdateAction(); loc.has_value())
    {
        if (ride != nullptr)
        {
            _guest.MoveTo({ loc.value(), ride->GetStation(_guest.CurrentRideStation).GetBaseZ() });
        }
        return;
    }

    OnExitRide(*ride);

    if (ride != nullptr && (_guest.PeepFlags & PEEP_FLAGS_TRACKING))
    {
        auto ft = Formatter();
        _guest.FormatNameTo(ft);
        ride->FormatNameTo(ft);

        if (gConfigNotifications.GuestLeftRide)
        {
            News::AddItemToQueue(News::ItemType::PeepOnRide, STR_PEEP_TRACKING_LEFT_RIDE_X, _guest.sprite_index, ft);
        }
    }

    _guest.InteractionRideIndex = RideId::GetNull();
    _guest.SetState(PeepState::Falling);

    CoordsXY targetLoc = { _guest.x, _guest.y };

    // Find the station track element
    for (auto* pathElement : TileElementsView<PathElement>(targetLoc))
    {
        int16_t height = MapHeightFromSlope(targetLoc, pathElement->GetSlopeDirection(), pathElement->IsSloped());
        height += pathElement->GetBaseZ();

        int16_t z_diff = _guest.z - height;
        if (z_diff > 0 || z_diff < -16)
            continue;

        _guest.MoveTo({ _guest.x, _guest.y, height });
        return;
    }
}

/**
 *
 *  rct2: 0x0069299C
 */
void GuestRideHelper::UpdateRideShopApproach()
{
    if (auto loc = _guest.UpdateAction(); loc.has_value())
    {
        _guest.MoveTo({ loc.value(), _guest.z });
        return;
    }

    _guest.RideSubState = PeepRideSubState::InteractShop;
}

/**
 *
 *  rct2: 0x006929BB
 */
void GuestRideHelper::UpdateRideShopInteract()
{
    auto ride = get_ride(_guest.CurrentRide);
    if (ride == nullptr)
        return;

    const int16_t tileCentreX = _guest.NextLoc.x + 16;
    const int16_t tileCentreY = _guest.NextLoc.y + 16;

    const auto& rtd = ride->GetRideTypeDescriptor();
    if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_FIRST_AID))
    {
        if (_guest.Nausea <= 35)
        {
            _guest.RideSubState = PeepRideSubState::LeaveShop;

            _guest.SetDestination({ tileCentreX, tileCentreY }, 3);
            _guest.HappinessTarget = std::min(_guest.HappinessTarget + 30, PEEP_MAX_HAPPINESS);
            _guest.Happiness = _guest.HappinessTarget;
        }
        else
        {
            _guest.Nausea--;
            _guest.NauseaTarget = _guest.Nausea;
        }
        return;
    }

    if (_guest.Toilet != 0)
    {
        _guest.Toilet--;
        return;
    }

    // Do not play toilet flush sound on title screen as it's considered loud and annoying
    if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
    {
        OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::ToiletFlush, _guest.GetLocation());
    }

    _guest.RideSubState = PeepRideSubState::LeaveShop;

    _guest.SetDestination({ tileCentreX, tileCentreY }, 3);

    _guest.HappinessTarget = std::min(_guest.HappinessTarget + 30, PEEP_MAX_HAPPINESS);
    _guest.Happiness = _guest.HappinessTarget;
    _guest.StopPurchaseThought(ride->type);
}

/**
 *
 *  rct2: 0x00692935
 */
void GuestRideHelper::UpdateRideShopLeave()
{
    if (auto loc = _guest.UpdateAction(); loc.has_value())
    {
        const auto curLoc = _guest.GetLocation();
        _guest.MoveTo({ loc.value(), curLoc.z });

        const auto newLoc = _guest.GetLocation().ToTileStart();
        if (newLoc.x != _guest.NextLoc.x)
            return;
        if (newLoc.y != _guest.NextLoc.y)
            return;
    }

    // #11758 Previously SetState(PeepState::Walking) caused Peeps to double-back to exit point of shop
    _guest.SetState(PeepState::Falling);

    auto ride = get_ride(_guest.CurrentRide);
    if (ride != nullptr)
    {
        ride->total_customers++;
        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;
        ride->UpdateSatisfaction(_guest.Happiness / 64);
    }
}

/**
 *
 *  rct2: 0x691A30
 * Used by entering_ride and queueing_front */
void GuestRideHelper::UpdateRide()
{
    _guest.NextFlags &= ~PEEP_NEXT_FLAG_IS_SLOPED;

    switch (_guest.RideSubState)
    {
        case PeepRideSubState::AtEntrance:
            UpdateRideAtEntrance();
            break;
        case PeepRideSubState::InEntrance:
            UpdateRideAdvanceThroughEntrance();
            break;
        case PeepRideSubState::FreeVehicleCheck:
            UpdateRideFreeVehicleCheck();
            break;
        case PeepRideSubState::LeaveEntrance:
            UpdateRideAdvanceThroughEntrance();
            break;
        case PeepRideSubState::ApproachVehicle:
            UpdateRideApproachVehicle();
            break;
        case PeepRideSubState::EnterVehicle:
            UpdateRideEnterVehicle();
            break;
        case PeepRideSubState::OnRide:
            // No action, on ride.
            break;
        case PeepRideSubState::LeaveVehicle:
            UpdateRideLeaveVehicle();
            break;
        case PeepRideSubState::ApproachExit:
            UpdateRideApproachExit();
            break;
        case PeepRideSubState::InExit:
            UpdateRideInExit();
            break;
        case PeepRideSubState::ApproachVehicleWaypoints:
            UpdateRideApproachVehicleWaypoints();
            break;
        case PeepRideSubState::ApproachExitWaypoints:
            UpdateRideApproachExitWaypoints();
            break;
        case PeepRideSubState::ApproachSpiralSlide:
            UpdateRideApproachSpiralSlide();
            break;
        case PeepRideSubState::OnSpiralSlide:
            UpdateRideOnSpiralSlide();
            break;
        case PeepRideSubState::LeaveSpiralSlide:
            UpdateRideLeaveSpiralSlide();
            break;
        case PeepRideSubState::MazePathfinding:
            UpdateRideMazePathfinding();
            break;
        case PeepRideSubState::LeaveExit:
            UpdateRideLeaveExit();
            break;
        case PeepRideSubState::ApproachShop:
            UpdateRideShopApproach();
            break;
        case PeepRideSubState::InteractShop:
            UpdateRideShopInteract();
            break;
        case PeepRideSubState::LeaveShop:
            UpdateRideShopLeave();
            break;
        default:
            // Invalid peep sub-state
            assert(false);
            break;
    }
}

/**
 *
 *  rct2: 0x69185D
 */
void GuestRideHelper::UpdateQueuing()
{
    if (!_guest.CheckForPath())
    {
        RemoveFromQueue();
        return;
    }
    auto ride = get_ride(_guest.CurrentRide);
    if (ride == nullptr || ride->status != RideStatus::Open)
    {
        RemoveFromQueue();
        _guest.SetState(PeepState::One);
        return;
    }

    // If not in the queue then at front of queue
    if (_guest.RideSubState != PeepRideSubState::InQueue)
    {
        bool is_front = true;
        // Fix #4819: Occasionally the _guest.GuestNextInQueue is incorrectly set
        // to prevent this from causing the peeps to enter a loop
        // first check if the next in queue is actually nearby
        // if they are not then it's safe to assume that this is
        // the front of the queue.
        Peep* nextGuest = GetEntity<Guest>(_guest.GuestNextInQueue);
        if (nextGuest != nullptr)
        {
            if (abs(nextGuest->x - _guest.x) < 32 && abs(nextGuest->y - _guest.y) < 32)
            {
                is_front = false;
            }
        }

        if (is_front)
        {
            // Happens every time peep goes onto ride.
            _guest.DestinationTolerance = 0;
            _guest.SetState(PeepState::QueuingFront);
            _guest.RideSubState = PeepRideSubState::AtEntrance;
        }

        return;
    }

    uint8_t pathingResult;
    _guest.PerformNextAction(pathingResult);
    if (!_guest.IsActionInterruptable())
        return;
    if (_guest.SpriteType == PeepSpriteType::Normal)
    {
        if (_guest.TimeInQueue >= 2000 && (0xFFFF & scenario_rand()) <= 119)
        {
            // Eat Food/Look at watch
            _guest.Action = PeepActionType::EatFood;
            _guest.ActionFrame = 0;
            _guest.ActionSpriteImageOffset = 0;
            _guest.UpdateCurrentActionSpriteType();
        }
        if (_guest.TimeInQueue >= 3500 && (0xFFFF & scenario_rand()) <= 93)
        {
            // Create the I have been waiting in line ages thought
            _guest.InsertNewThought(PeepThoughtType::QueuingAges, _guest.CurrentRide);
        }
    }
    else
    {
        if (!(_guest.TimeInQueue & 0x3F) && _guest.IsActionIdle()
            && _guest.NextActionSpriteType == PeepActionSpriteType::WatchRide)
        {
            switch (_guest.SpriteType)
            {
                case PeepSpriteType::IceCream:
                case PeepSpriteType::Chips:
                case PeepSpriteType::Burger:
                case PeepSpriteType::Drink:
                case PeepSpriteType::Candyfloss:
                case PeepSpriteType::Pizza:
                case PeepSpriteType::Popcorn:
                case PeepSpriteType::HotDog:
                case PeepSpriteType::Tentacle:
                case PeepSpriteType::ToffeeApple:
                case PeepSpriteType::Doughnut:
                case PeepSpriteType::Coffee:
                case PeepSpriteType::Chicken:
                case PeepSpriteType::Lemonade:
                case PeepSpriteType::Pretzel:
                case PeepSpriteType::SuJongkwa:
                case PeepSpriteType::Juice:
                case PeepSpriteType::FunnelCake:
                case PeepSpriteType::Noodles:
                case PeepSpriteType::Sausage:
                case PeepSpriteType::Soup:
                case PeepSpriteType::Sandwich:
                    // Eat food
                    _guest.Action = PeepActionType::EatFood;
                    _guest.ActionFrame = 0;
                    _guest.ActionSpriteImageOffset = 0;
                    _guest.UpdateCurrentActionSpriteType();
                    break;
                default:
                    break;
            }
        }
    }
    if (_guest.TimeInQueue < 4300)
        return;

    if (_guest.Happiness <= 65 && (0xFFFF & scenario_rand()) < 2184)
    {
        // Give up queueing for the ride
        _guest.sprite_direction ^= (1 << 4);
        _guest.Invalidate();
        RemoveFromQueue();
        _guest.SetState(PeepState::One);
    }
}

/**
 * rct2: 0x0069101A
 *
 * @return (CF)
 */
static bool peep_should_watch_ride(TileElement* tileElement)
{
    // Ghosts are purely this-client-side and should not cause any interaction,
    // as that may lead to a desync.
    if (network_get_mode() != NETWORK_MODE_NONE)
    {
        if (tileElement->IsGhost())
            return false;
    }

    auto ride = get_ride(tileElement->AsTrack()->GetRideIndex());
    if (ride == nullptr || !ride->IsRide())
    {
        return false;
    }

    // This is most likely to have peeps watch new rides
    if (ride->excitement == RIDE_RATING_UNDEFINED)
    {
        return true;
    }

    if (ride->excitement >= RIDE_RATING(4, 70))
    {
        return true;
    }

    if (ride->intensity >= RIDE_RATING(4, 50))
    {
        return true;
    }

    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_INTERESTING_TO_LOOK_AT))
    {
        if ((scenario_rand() & 0xFFFF) > 0x3333)
        {
            return false;
        }
    }
    else if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_SLIGHTLY_INTERESTING_TO_LOOK_AT))
    {
        if ((scenario_rand() & 0xFFFF) > 0x1000)
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool GuestRideHelper::Loc690FD0(RideId* rideToView, uint8_t* rideSeatToView, TileElement* tileElement) const
{
    auto ride = get_ride(tileElement->AsTrack()->GetRideIndex());
    if (ride == nullptr)
        return false;

    *rideToView = ride->id;
    if (ride->excitement == RIDE_RATING_UNDEFINED)
    {
        *rideSeatToView = 1;
        if (ride->status != RideStatus::Open)
        {
            if (tileElement->GetClearanceZ() > _guest.NextLoc.z + (8 * COORDS_Z_STEP))
            {
                *rideSeatToView |= (1 << 1);
            }

            return true;
        }
    }
    else
    {
        *rideSeatToView = 0;
        if (ride->status == RideStatus::Open && !(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN))
        {
            if (tileElement->GetClearanceZ() > _guest.NextLoc.z + (8 * COORDS_Z_STEP))
            {
                *rideSeatToView = 0x02;
            }

            return true;
        }
    }

    return false;
}

/**
 *
 *  rct2: 0x00690B99
 *
 * @param edge (eax)
 * @param peep (esi)
 * @param[out] rideToView (cl)
 * @param[out] rideSeatToView (ch)
 * @return !CF
 */
bool GuestRideHelper::FindRideToLookAt(uint8_t edge, RideId* rideToView, uint8_t* rideSeatToView) const
{
    TileElement* tileElement;

    auto surfaceElement = MapGetSurfaceElementAt(_guest.NextLoc);

    tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    if (tileElement == nullptr)
    {
        return false;
    }

    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetType() != TileElementType::Wall)
            continue;
        if (tileElement->GetDirection() != edge)
            continue;
        auto wallEntry = tileElement->AsWall()->GetEntry();
        if (wallEntry == nullptr || (wallEntry->flags2 & WALL_SCENERY_2_IS_OPAQUE))
            continue;
        if (_guest.NextLoc.z + (4 * COORDS_Z_STEP) <= tileElement->GetBaseZ())
            continue;
        if (_guest.NextLoc.z + (1 * COORDS_Z_STEP) >= tileElement->GetClearanceZ())
            continue;

        return false;
    } while (!(tileElement++)->IsLastForTile());

    uint16_t x = _guest.NextLoc.x + CoordsDirectionDelta[edge].x;
    uint16_t y = _guest.NextLoc.y + CoordsDirectionDelta[edge].y;
    if (!MapIsLocationValid({ x, y }))
    {
        return false;
    }

    surfaceElement = MapGetSurfaceElementAt(CoordsXY{ x, y });

    tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    if (tileElement == nullptr)
    {
        return false;
    }

    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetType() != TileElementType::Wall)
            continue;
        if (DirectionReverse(tileElement->GetDirection()) != edge)
            continue;
        auto wallEntry = tileElement->AsWall()->GetEntry();
        if (wallEntry == nullptr || (wallEntry->flags2 & WALL_SCENERY_2_IS_OPAQUE))
            continue;
        // TODO: Check whether this shouldn't be <=, as the other loops use. If so, also extract as loop A.
        if (_guest.NextLoc.z + (4 * COORDS_Z_STEP) >= tileElement->GetBaseZ())
            continue;
        if (_guest.NextLoc.z + (1 * COORDS_Z_STEP) >= tileElement->GetClearanceZ())
            continue;

        return false;
    } while (!(tileElement++)->IsLastForTile());

    // TODO: Extract loop B
    tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }

        if (tileElement->GetClearanceZ() + (1 * COORDS_Z_STEP) < _guest.NextLoc.z)
            continue;
        if (_guest.NextLoc.z + (6 * COORDS_Z_STEP) < tileElement->GetBaseZ())
            continue;

        if (tileElement->GetType() == TileElementType::Track)
        {
            if (peep_should_watch_ride(tileElement))
            {
                return Loc690FD0(rideToView, rideSeatToView, tileElement);
            }
        }

        if (tileElement->GetType() == TileElementType::LargeScenery)
        {
            const auto* sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
            if (sceneryEntry == nullptr || !(sceneryEntry->flags & LARGE_SCENERY_FLAG_PHOTOGENIC))
            {
                continue;
            }

            *rideSeatToView = 0;
            if (tileElement->GetClearanceZ() >= _guest.NextLoc.z + (8 * COORDS_Z_STEP))
            {
                *rideSeatToView = 0x02;
            }

            *rideToView = RideId::GetNull();

            return true;
        }
    } while (!(tileElement++)->IsLastForTile());

    // TODO: Extract loop C
    tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetClearanceZ() + (1 * COORDS_Z_STEP) < _guest.NextLoc.z)
            continue;
        if (_guest.NextLoc.z + (6 * COORDS_Z_STEP) < tileElement->GetBaseZ())
            continue;
        if (tileElement->GetType() == TileElementType::Surface)
            continue;
        if (tileElement->GetType() == TileElementType::Path)
            continue;

        if (tileElement->GetType() == TileElementType::Wall)
        {
            auto wallEntry = tileElement->AsWall()->GetEntry();
            if (wallEntry == nullptr || (wallEntry->flags2 & WALL_SCENERY_2_IS_OPAQUE))
            {
                continue;
            }
        }

        return false;
    } while (!(tileElement++)->IsLastForTile());

    x += CoordsDirectionDelta[edge].x;
    y += CoordsDirectionDelta[edge].y;
    if (!MapIsLocationValid({ x, y }))
    {
        return false;
    }

    surfaceElement = MapGetSurfaceElementAt(CoordsXY{ x, y });

    // TODO: extract loop A
    tileElement = reinterpret_cast<TileElement*>(surfaceElement);

    if (tileElement == nullptr)
    {
        return false;
    }

    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetType() != TileElementType::Wall)
            continue;
        if (DirectionReverse(tileElement->GetDirection()) != edge)
            continue;
        auto wallEntry = tileElement->AsWall()->GetEntry();
        if (wallEntry == nullptr || (wallEntry->flags2 & WALL_SCENERY_2_IS_OPAQUE))
            continue;
        if (_guest.NextLoc.z + (6 * COORDS_Z_STEP) <= tileElement->GetBaseZ())
            continue;
        if (_guest.NextLoc.z >= tileElement->GetClearanceZ())
            continue;

        return false;
    } while (!(tileElement++)->IsLastForTile());

    // TODO: Extract loop B
    tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetClearanceZ() + (1 * COORDS_Z_STEP) < _guest.NextLoc.z)
            continue;
        if (_guest.NextLoc.z + (8 * COORDS_Z_STEP) < tileElement->GetBaseZ())
            continue;

        if (tileElement->GetType() == TileElementType::Track)
        {
            if (peep_should_watch_ride(tileElement))
            {
                return Loc690FD0(rideToView, rideSeatToView, tileElement);
            }
        }

        if (tileElement->GetType() == TileElementType::LargeScenery)
        {
            auto* sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
            if (!(sceneryEntry == nullptr || sceneryEntry->flags & LARGE_SCENERY_FLAG_PHOTOGENIC))
            {
                continue;
            }

            *rideSeatToView = 0;
            if (tileElement->GetClearanceZ() >= _guest.NextLoc.z + (8 * COORDS_Z_STEP))
            {
                *rideSeatToView = 0x02;
            }

            *rideToView = RideId::GetNull();

            return true;
        }
    } while (!(tileElement++)->IsLastForTile());

    // TODO: Extract loop C
    tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetClearanceZ() + (1 * COORDS_Z_STEP) < _guest.NextLoc.z)
            continue;
        if (_guest.NextLoc.z + (8 * COORDS_Z_STEP) < tileElement->GetBaseZ())
            continue;
        if (tileElement->GetType() == TileElementType::Surface)
            continue;
        if (tileElement->GetType() == TileElementType::Path)
            continue;

        if (tileElement->GetType() == TileElementType::Wall)
        {
            auto wallEntry = tileElement->AsWall()->GetEntry();
            if (wallEntry == nullptr || (wallEntry->flags2 & WALL_SCENERY_2_IS_OPAQUE))
            {
                continue;
            }
        }

        return false;
    } while (!(tileElement++)->IsLastForTile());

    x += CoordsDirectionDelta[edge].x;
    y += CoordsDirectionDelta[edge].y;
    if (!MapIsLocationValid({ x, y }))
    {
        return false;
    }

    surfaceElement = MapGetSurfaceElementAt(CoordsXY{ x, y });

    // TODO: extract loop A
    tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    if (tileElement == nullptr)
    {
        return false;
    }

    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetType() != TileElementType::Wall)
            continue;
        if (DirectionReverse(tileElement->GetDirection()) != edge)
            continue;
        auto wallEntry = tileElement->AsWall()->GetEntry();
        if (wallEntry == nullptr || (wallEntry->flags2 & WALL_SCENERY_2_IS_OPAQUE))
            continue;
        if (_guest.NextLoc.z + (8 * COORDS_Z_STEP) <= tileElement->GetBaseZ())
            continue;
        if (_guest.NextLoc.z >= tileElement->GetClearanceZ())
            continue;

        return false;
    } while (!(tileElement++)->IsLastForTile());

    // TODO: Extract loop B
    tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }
        if (tileElement->GetClearanceZ() + (1 * COORDS_Z_STEP) < _guest.NextLoc.z)
            continue;
        if (_guest.NextLoc.z + (10 * COORDS_Z_STEP) < tileElement->GetBaseZ())
            continue;

        if (tileElement->GetType() == TileElementType::Track)
        {
            if (peep_should_watch_ride(tileElement))
            {
                return Loc690FD0(rideToView, rideSeatToView, tileElement);
            }
        }

        if (tileElement->GetType() == TileElementType::LargeScenery)
        {
            const auto* sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
            if (sceneryEntry == nullptr || !(sceneryEntry->flags & LARGE_SCENERY_FLAG_PHOTOGENIC))
            {
                continue;
            }

            *rideSeatToView = 0;
            if (tileElement->GetClearanceZ() >= _guest.NextLoc.z + (8 * COORDS_Z_STEP))
            {
                *rideSeatToView = 0x02;
            }

            *rideToView = RideId::GetNull();

            return true;
        }
    } while (!(tileElement++)->IsLastForTile());

    return false;
}

/**
 *
 *  rct2: 0x00693CBB
 */
bool GuestRideHelper::UpdateQueuePosition(PeepActionType previous_action)
{
    _guest.TimeInQueue++;

    auto* guestNext = GetEntity<Guest>(_guest.GuestNextInQueue);
    if (guestNext == nullptr)
    {
        return false;
    }

    int16_t x_diff = abs(guestNext->x - _guest.x);
    int16_t y_diff = abs(guestNext->y - _guest.y);
    int16_t z_diff = abs(guestNext->z - _guest.z);

    if (z_diff > 10)
        return false;

    if (x_diff < y_diff)
    {
        int16_t temp_x = x_diff;
        x_diff = y_diff;
        y_diff = temp_x;
    }

    x_diff += y_diff / 2;
    if (x_diff > 7)
    {
        if (x_diff > 13)
        {
            if ((_guest.x & 0xFFE0) != (guestNext->x & 0xFFE0) || (_guest.y & 0xFFE0) != (guestNext->y & 0xFFE0))
                return false;
        }

        if (_guest.sprite_direction != guestNext->sprite_direction)
            return false;

        switch (guestNext->sprite_direction / 8)
        {
            case 0:
                if (_guest.x >= guestNext->x)
                    return false;
                break;
            case 1:
                if (_guest.y <= guestNext->y)
                    return false;
                break;
            case 2:
                if (_guest.x <= guestNext->x)
                    return false;
                break;
            case 3:
                if (_guest.y >= guestNext->y)
                    return false;
                break;
        }
    }

    if (!_guest.IsActionInterruptable())
        _guest.UpdateAction();

    if (!_guest.IsActionWalking())
        return true;

    _guest.Action = PeepActionType::Idle;
    _guest.NextActionSpriteType = PeepActionSpriteType::WatchRide;
    if (previous_action != PeepActionType::Idle)
        _guest.Invalidate();
    return true;
}

/**
 *
 *  rct2: 0x006966A9
 */
void GuestRideHelper::RemoveFromQueue()
{
    auto ride = get_ride(_guest.CurrentRide);
    if (ride == nullptr)
        return;

    auto& station = ride->GetStation(_guest.CurrentRideStation);
    // Make sure we don't underflow, building while paused might reset it to 0 where peeps have
    // not yet left the queue.
    if (station.QueueLength > 0)
    {
        station.QueueLength--;
    }

    if (_guest.sprite_index == station.LastPeepInQueue)
    {
        station.LastPeepInQueue = _guest.GuestNextInQueue;
        return;
    }

    auto* otherGuest = GetEntity<Guest>(station.LastPeepInQueue);
    if (otherGuest == nullptr)
    {
        log_error("Invalid Guest Queue list!");
        return;
    }
    for (; otherGuest != nullptr; otherGuest = GetEntity<Guest>(otherGuest->GuestNextInQueue))
    {
        if (_guest.sprite_index == otherGuest->GuestNextInQueue)
        {
            otherGuest->GuestNextInQueue = _guest.GuestNextInQueue;
            return;
        }
    }
}

void GuestRideHelper::RemoveRideFromMemory(RideId rideId)
{
    if (_guest.State == PeepState::Watching)
    {
        if (_guest.CurrentRide == rideId)
        {
            _guest.CurrentRide = RideId::GetNull();
            if (_guest.TimeToStand >= 50)
            {
                // make peep stop watching the ride
                _guest.TimeToStand = 50;
            }
        }
    }

    // remove any free voucher for this ride from peep
    if (_guest.HasItem(ShopItem::Voucher))
    {
        if (_guest.VoucherType == VOUCHER_TYPE_RIDE_FREE && _guest.VoucherRideId == rideId)
        {
            _guest.RemoveItem(ShopItem::Voucher);
        }
    }

    // remove any photos of this ride from peep
    if (_guest.HasItem(ShopItem::Photo))
    {
        if (_guest.Photo1RideRef == rideId)
        {
            _guest.RemoveItem(ShopItem::Photo);
        }
    }
    if (_guest.HasItem(ShopItem::Photo2))
    {
        if (_guest.Photo2RideRef == rideId)
        {
            _guest.RemoveItem(ShopItem::Photo2);
        }
    }
    if (_guest.HasItem(ShopItem::Photo3))
    {
        if (_guest.Photo3RideRef == rideId)
        {
            _guest.RemoveItem(ShopItem::Photo3);
        }
    }
    if (_guest.HasItem(ShopItem::Photo4))
    {
        if (_guest.Photo4RideRef == rideId)
        {
            _guest.RemoveItem(ShopItem::Photo4);
        }
    }

    if (_guest.GuestHeadingToRideId == rideId)
    {
        _guest.GuestHeadingToRideId = RideId::GetNull();
    }
    if (_guest.FavouriteRide == rideId)
    {
        _guest.FavouriteRide = RideId::GetNull();
    }

    // Erase all thoughts that contain the ride.
    for (auto it = std::begin(_guest.Thoughts); it != std::end(_guest.Thoughts);)
    {
        const auto& entry = *it;
        if (entry.type == PeepThoughtType::None)
            break;

        // Ride ids and shop item ids might have the same value, look only for ride thoughts.
        if (IsThoughtShopItemRelated(entry.type) || entry.rideId != rideId)
        {
            it++;
            continue;
        }

        if (auto itNext = std::next(it); itNext != std::end(_guest.Thoughts))
        {
            // Overwrite this entry by shifting all entries that follow.
            std::rotate(it, itNext, std::end(_guest.Thoughts));
        }

        // Last slot is now free.
        auto& lastEntry = _guest.Thoughts.back();
        lastEntry.type = PeepThoughtType::None;
        lastEntry.item = PeepThoughtItemNone;
    }
}
