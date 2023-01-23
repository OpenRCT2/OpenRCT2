/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../world/Location.hpp"
#include "RideTypes.h"
namespace OpenRCT2
{
    using ride_rating = fixed16_2dp;
    using track_type_t = uint16_t;

// Convenience function for writing ride ratings. The result is a 16 bit signed
// integer. To create the ride rating 3.65 type RIDE_RATING(3,65)
#define RIDE_RATING(whole, fraction) FIXED_2DP(whole, fraction)
#define RIDE_RATING_UNDEFINED static_cast<ride_rating>(static_cast<uint16_t>(0xFFFF))

#pragma pack(push, 1)

    // Used for return values, for functions that modify all three.
    struct RatingTuple
    {
        ride_rating Excitement;
        ride_rating Intensity;
        ride_rating Nausea;
    };
    assert_struct_size(RatingTuple, 6);

#pragma pack(pop)

    enum
    {
        RIDE_RATING_STATION_FLAG_NO_ENTRANCE = 1 << 0
    };

    struct RideRatingUpdateState
    {
        CoordsXYZ Proximity;
        CoordsXYZ ProximityStart;
        RideId CurrentRide;
        uint8_t State;
        track_type_t ProximityTrackType;
        uint8_t ProximityBaseHeight;
        uint16_t ProximityTotal;
        uint16_t ProximityScores[26];
        uint16_t AmountOfBrakes;
        uint16_t AmountOfReversers;
        uint16_t StationFlags;
    };

    extern RideRatingUpdateState gRideRatingUpdateState;

    void RideRatingsUpdateRide(const Ride& ride);
    void RideRatingsUpdateAll();

    using ride_ratings_calculation = void (*)(Ride& ride, RideRatingUpdateState& state);
    ride_ratings_calculation RideRatingsGetCalculateFunc(ride_type_t rideType);

    void RideRatingsCalculateSpiralRollerCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateStandUpRollerCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateSuspendedSwingingCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateInvertedRollerCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateJuniorRollerCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateMiniatureRailway(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateMonorail(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateMiniSuspendedCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateBoatHire(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateWoodenWildMouse(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateSteeplechase(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateCarRide(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateLaunchedFreefall(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateBobsleighCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateObservationTower(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateLoopingRollerCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateDinghySlide(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateMineTrainCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateChairlift(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateCorkscrewRollerCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateMaze(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateSpiralSlide(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateGoKarts(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateLogFlume(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateRiverRapids(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateDodgems(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateSwingingShip(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateInverterShip(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateFoodStall(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateShop(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateMerryGoRound(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateInformationKiosk(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateToilets(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateFerrisWheel(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateMotionSimulator(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculate3dCinema(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateTopSpin(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateSpaceRings(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateReverseFreefallCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateLift(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateVerticalDropRollerCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateCashMachine(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateTwist(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateHauntedHouse(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateFirstAid(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateCircus(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateGhostTrain(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateTwisterRollerCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateWoodenRollerCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateSideFrictionRollerCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateWildMouse(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateMultiDimensionRollerCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateFlyingRollerCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateVirginiaReel(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateSplashBoats(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateMiniHelicopters(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateLayDownRollerCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateSuspendedMonorail(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateReverserRollerCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateHeartlineTwisterCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateMiniGolf(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateGigaCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateRotoDrop(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateFlyingSaucers(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateCrookedHouse(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateMonorailCycles(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateCompactInvertedCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateWaterCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateAirPoweredVerticalCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateInvertedHairpinCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateMagicCarpet(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateSubmarineRide(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateRiverRafts(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateEnterprise(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateInvertedImpulseCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateMiniRollerCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateMineRide(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateLimLaunchedRollerCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateClassicMiniRollerCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateHybridCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateSingleRailRollerCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateAlpineCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateClassicWoodenRollerCoaster(Ride& ride, RideRatingUpdateState& state);
    void RideRatingsCalculateDrinkStall(Ride& ride, RideRatingUpdateState& state);

    // Special Track Element Adjustment functions for RTDs
    void SpecialTrackElementRatingsAjustment_Default(
        const Ride& ride, int32_t& excitement, int32_t& intensity, int32_t& nausea);
    void SpecialTrackElementRatingsAjustment_GhostTrain(
        const Ride& ride, int32_t& excitement, int32_t& intensity, int32_t& nausea);
    void SpecialTrackElementRatingsAjustment_LogFlume(
        const Ride& ride, int32_t& excitement, int32_t& intensity, int32_t& nausea);
} // namespace OpenRCT2
