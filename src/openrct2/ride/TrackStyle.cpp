/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackStyle.h"

#include "../core/EnumUtils.hpp"
#include "../ride/TrackData.h"
#include "Track.h"
#include "TrackPaint.h"

using TrackPaintFunctionGetter = TrackPaintFunction (*)(OpenRCT2::TrackElemType trackType);
static TrackPaintFunction DummyGetter(OpenRCT2::TrackElemType trackType)
{
    return TrackPaintFunctionDummy;
}

static constexpr TrackPaintFunctionGetter kPaintFunctionMap[] = {
    GetTrackPaintFunction3dCinema,                 // _3DCinema
    GetTrackPaintFunctionAirPoweredVerticalRC,     // airPoweredVerticalCoaster
    OpenRCT2::AlpineRC::GetTrackPaintFunction,     // alpineCoaster
    GetTrackPaintFunctionBoatHire,                 // boatHire
    GetTrackPaintFunctionBobsleighRC,              // bobsleighCoaster
    GetTrackPaintFunctionCarRide,                  // carRide
    GetTrackPaintFunctionChairlift,                // chairlift
    GetTrackPaintFunctionCircus,                   // circus
    GetTrackPaintFunctionClassicStandUpRC,         // classicStandUpRollerCoaster
    GetTrackPaintFunctionClassicWoodenRC,          // classicWoodenRollerCoaster
    GetTrackPaintFunctionClassicWoodenTwisterRC,   // classicWoodenTwisterRollerCoaster
    GetTrackPaintFunctionCompactInvertedRC,        // compactInvertedCoaster
    GetTrackPaintFunctionCorkscrewRC,              // corkscrewRollerCoaster
    GetTrackPaintFunctionCrookedHouse,             // crookedHouse
    GetTrackPaintFunctionDinghySlide,              // dinghySlide
    GetTrackPaintFunctionDinghySlideCovered,       // dinghySlideCovered
    GetTrackPaintFunctionDodgems,                  // dodgems
    GetTrackPaintFunctionEnterprise,               // enterprise
    GetTrackPaintFunctionFacility,                 // facility
    GetTrackPaintFunctionFerrisWheel,              // ferrisWheel
    GetTrackPaintFunctionFlyingRC,                 // flyingRollerCoaster
    GetTrackPaintFunctionFlyingRCInverted,         // flyingRollerCoasterInverted
    GetTrackPaintFunctionFlyingSaucers,            // flyingSaucers
    GetTrackPaintFunctionGhostTrain,               // ghostTrain
    GetTrackPaintFunctionGoKarts,                  // goKarts
    GetTrackPaintFunctionHauntedHouse,             // hauntedHouse
    GetTrackPaintFunctionHeartlineTwisterRC,       // heartlineTwisterCoaster
    OpenRCT2::HybridRC::GetTrackPaintFunction,     // hybridCoaster
    GetTrackPaintFunctionInvertedHairpinRC,        // invertedHairpinCoaster
    GetTrackPaintFunctionInvertedImpulseRC,        // invertedImpulseCoaster
    GetTrackPaintFunctionInvertedRC,               // invertedRollerCoaster
    GetTrackPaintFunctionJuniorRC,                 // juniorRollerCoaster
    GetTrackPaintFunctionLatticeTriangleTrack,     // latticeTriangle
    GetTrackPaintFunctionLatticeTriangleTrackAlt,  // latticeTriangleAlt
    GetTrackPaintFunctionLaunchedFreefall,         // launchedFreefall
    GetTrackPaintFunctionLayDownRCInverted,        // layDownRollerCoasterInverted
    GetTrackPaintFunctionLift,                     // lift
    GetTrackPaintFunctionLimLaunchedRC,            // limLaunchedRollerCoaster
    GetTrackPaintFunctionLogFlume,                 // logFlume
    GetTrackPaintFunctionLoopingRC,                // loopingRollerCoaster
    GetTrackPaintFunctionMagicCarpet,              // magicCarpet
    GetTrackPaintFunctionMaze,                     // maze
    GetTrackPaintFunctionMerryGoRound,             // merryGoRound
    GetTrackPaintFunctionMineRide,                 // mineRide
    GetTrackPaintFunctionMineTrainRC,              // mineTrainCoaster
    GetTrackPaintFunctionMiniGolf,                 // miniGolf
    GetTrackPaintFunctionMiniHelicopters,          // miniHelicopters
    GetTrackPaintFunctionMiniRC,                   // miniRollerCoaster
    GetTrackPaintFunctionMiniSuspendedRC,          // miniSuspendedCoaster
    GetTrackPaintFunctionMiniatureRailway,         // miniatureRailway
    GetTrackPaintFunctionMonorail,                 // monorail
    GetTrackPaintFunctionMonorailCycles,           // monorailCycles
    GetTrackPaintFunctionMotionsimulator,          // motionSimulator
    GetTrackPaintFunctionMultiDimensionRC,         // multiDimensionRollerCoaster
    GetTrackPaintFunctionMultiDimensionRC,         // multiDimensionRollerCoasterInverted
    GetTrackPaintFunctionObservationTower,         // observationTower
    GetTrackPaintFunctionReverseFreefallRC,        // reverseFreefallCoaster
    GetTrackPaintFunctionReverserRC,               // reverserRollerCoaster
    GetTrackPaintFunctionRiverRapids,              // riverRapids
    GetTrackPaintFunctionRotoDrop,                 // rotoDrop
    GetTrackPaintFunctionShop,                     // shop
    GetTrackPaintFunctionSideFrictionRC,           // sideFrictionRollerCoaster
    OpenRCT2::SingleRailRC::GetTrackPaintFunction, // singleRailRollerCoaster
    GetTrackPaintFunctionSpaceRings,               // spaceRings
    GetTrackPaintFunctionSpiralSlide,              // spiralSlide
    GetTrackPaintFunctionSplashBoats,              // splashBoats
    GetTrackPaintFunctionStandUpRC,                // standUpRollerCoaster
    GetTrackPaintFunctionWildMouse,                // steelWildMouse
    GetTrackPaintFunctionSteeplechase,             // steeplechase
    GetTrackPaintFunctionSubmarineRide,            // submarineRide
    GetTrackPaintFunctionSuspendedMonorail,        // suspendedMonorail
    GetTrackPaintFunctionSuspendedSwingingRC,      // suspendedSwingingCoaster
    GetTrackPaintFunctionSwingingInverterShip,     // swingingInverterShip
    GetTrackPaintFunctionSwingingShip,             // swingingShip
    GetTrackPaintFunctionTopspin,                  // topSpin
    GetTrackPaintFunctionTwist,                    // twist
    GetTrackPaintFunctionTwisterRC,                // twisterRollerCoaster
    GetTrackPaintFunctionVirginiaReel,             // virginiaReel
    GetTrackPaintFunctionWaterRC,                  // waterCoaster
    GetTrackPaintFunctionWoodenRC,                 // woodenRollerCoaster
    GetTrackPaintFunctionWoodenWildMouse,          // woodenWildMouse
    DummyGetter,                                   // null
};
static_assert(std::size(kPaintFunctionMap) == kTrackStyleCount);

static TrackPaintFunction GetTrackPaintFunction(TrackStyle trackStyle, OpenRCT2::TrackElemType trackType)
{
    return kPaintFunctionMap[static_cast<uint8_t>(trackStyle)](trackType);
}

static std::array<TrackStylePaintInfo, kTrackStyleCount> kTrackStylePaintInfos{};
static bool kTrackStylePaintInfosCreated = false;

using TrackGroupBlockedSegmentTypes = std::array<OpenRCT2::BlockedSegmentsType, EnumValue(TrackGroup::count)>;

static constexpr const TrackGroupBlockedSegmentTypes kTrackGroupBlockedSegmentsNarrow = []() consteval {
    TrackGroupBlockedSegmentTypes array{};
    array.fill(OpenRCT2::BlockedSegmentsType::narrow);
    return array;
}();

static constexpr const TrackGroupBlockedSegmentTypes kTrackGroupBlockedSegmentsInverted = []() consteval {
    TrackGroupBlockedSegmentTypes array{};
    array.fill(OpenRCT2::BlockedSegmentsType::inverted);
    array[EnumValue(TrackGroup::stationEnd)] = OpenRCT2::BlockedSegmentsType::wide;
    array[EnumValue(TrackGroup::onridePhoto)] = OpenRCT2::BlockedSegmentsType::wide;
    return array;
}();

static constexpr const TrackGroupBlockedSegmentTypes kTrackGroupBlockedSegmentsWide = []() consteval {
    TrackGroupBlockedSegmentTypes array{};
    array.fill(OpenRCT2::BlockedSegmentsType::wide);
    return array;
}();

static constexpr const TrackGroupBlockedSegmentTypes kTrackGroupBlockedSegmentsCarRideGhostTrain = []() consteval {
    TrackGroupBlockedSegmentTypes array{};
    array.fill(OpenRCT2::BlockedSegmentsType::narrow);
    array[EnumValue(TrackGroup::curveVerySmall)] = OpenRCT2::BlockedSegmentsType::wide;
    return array;
}();

static constexpr const TrackGroupBlockedSegmentTypes kTrackGroupBlockedSegmentsFlyingRollerCoaster = []() consteval {
    TrackGroupBlockedSegmentTypes array{};
    array.fill(OpenRCT2::BlockedSegmentsType::narrow);

    // invalid inverted pieces but kept for backwards compatibility
    array[EnumValue(TrackGroup::inlineTwistInverted)] = OpenRCT2::BlockedSegmentsType::inverted;
    array[EnumValue(TrackGroup::flyingLargeHalfLoopInvertedDown)] = OpenRCT2::BlockedSegmentsType::inverted;
    array[EnumValue(TrackGroup::flyingLargeHalfLoopInvertedUp)] = OpenRCT2::BlockedSegmentsType::inverted;
    array[EnumValue(TrackGroup::flyingHalfLoopInvertedUp)] = OpenRCT2::BlockedSegmentsType::inverted;
    return array;
}();

static constexpr const TrackGroupBlockedSegmentTypes kTrackGroupBlockedSegmentsFlyingRollerCoasterInverted = []() consteval {
    TrackGroupBlockedSegmentTypes array{};
    array.fill(OpenRCT2::BlockedSegmentsType::inverted);
    array[EnumValue(TrackGroup::stationEnd)] = OpenRCT2::BlockedSegmentsType::wide;
    array[EnumValue(TrackGroup::onridePhoto)] = OpenRCT2::BlockedSegmentsType::wide;

    // invalid uninverted pieces but kept for backwards compatibility
    array[EnumValue(TrackGroup::verticalLoop)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::twist)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::corkscrew)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::helixUpBankedHalf)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::helixDownBankedHalf)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::flatToSteepSlope)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::barrelRoll)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::halfLoop)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::halfLoopLarge)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::quarterLoop)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::quarterLoopUninvertedDown)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::quarterLoopUninvertedUp)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::inlineTwistUninverted)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::flyingLargeHalfLoopUninvertedDown)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::flyingLargeHalfLoopUninvertedUp)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::flyingHalfLoopUninvertedUp)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::flyingHalfLoopUninvertedDown)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::corkscrewLarge)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::halfLoopMedium)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::zeroGRoll)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::zeroGRollLarge)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::slopeCurveLarge)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::slopeCurveLargeBanked)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::slopeSteepLong)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::diveLoop)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::poweredLift)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::brakeForDrop)] = OpenRCT2::BlockedSegmentsType::narrow;
    return array;
}();

static constexpr const TrackGroupBlockedSegmentTypes kTrackGroupBlockedSegmentsMultiDimensionRollerCoaster = []() consteval {
    TrackGroupBlockedSegmentTypes array{};
    array.fill(OpenRCT2::BlockedSegmentsType::narrow);
    array[EnumValue(TrackGroup::straight)] = OpenRCT2::BlockedSegmentsType::wide;
    array[EnumValue(TrackGroup::slope)] = OpenRCT2::BlockedSegmentsType::wide;
    array[EnumValue(TrackGroup::slopeSteepUp)] = OpenRCT2::BlockedSegmentsType::wide;
    array[EnumValue(TrackGroup::slopeSteepDown)] = OpenRCT2::BlockedSegmentsType::wide;
    array[EnumValue(TrackGroup::flatRollBanking)] = OpenRCT2::BlockedSegmentsType::wide;
    array[EnumValue(TrackGroup::sBend)] = OpenRCT2::BlockedSegmentsType::wide;
    array[EnumValue(TrackGroup::brakes)] = OpenRCT2::BlockedSegmentsType::wide;
    array[EnumValue(TrackGroup::blockBrakes)] = OpenRCT2::BlockedSegmentsType::wide;

    // invalid inverted pieces but kept for backwards compatibility
    array[EnumValue(TrackGroup::inlineTwistInverted)] = OpenRCT2::BlockedSegmentsType::inverted;
    return array;
}();

static constexpr const TrackGroupBlockedSegmentTypes kTrackGroupBlockedSegmentsMultiDimensionRollerCoasterInverted =
    []() consteval {
        TrackGroupBlockedSegmentTypes array{};
        array.fill(OpenRCT2::BlockedSegmentsType::inverted);
        array[EnumValue(TrackGroup::onridePhoto)] = OpenRCT2::BlockedSegmentsType::wide;

        // invalid uninverted pieces but kept for backwards compatibility
        array[EnumValue(TrackGroup::stationEnd)] = OpenRCT2::BlockedSegmentsType::wide;
        array[EnumValue(TrackGroup::helixUpBankedHalf)] = OpenRCT2::BlockedSegmentsType::narrow;
        array[EnumValue(TrackGroup::helixDownBankedHalf)] = OpenRCT2::BlockedSegmentsType::narrow;
        array[EnumValue(TrackGroup::inlineTwistUninverted)] = OpenRCT2::BlockedSegmentsType::narrow;
        return array;
    }();

static constexpr const TrackGroupBlockedSegmentTypes kTrackGroupBlockedSegmentsWoodenRollerCoaster = []() consteval {
    TrackGroupBlockedSegmentTypes array{};
    array.fill(OpenRCT2::BlockedSegmentsType::wide);
    array[EnumValue(TrackGroup::verticalLoop)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::halfLoopMedium)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::halfLoopLarge)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::helixUpBankedHalf)] = OpenRCT2::BlockedSegmentsType::narrow;
    array[EnumValue(TrackGroup::helixDownBankedHalf)] = OpenRCT2::BlockedSegmentsType::narrow;
    return array;
}();

static constexpr const TrackGroupBlockedSegmentTypes kTrackGroupBlockedSegmentsWoodenWildMouse = []() consteval {
    TrackGroupBlockedSegmentTypes array{};
    array.fill(OpenRCT2::BlockedSegmentsType::wide);
    array[EnumValue(TrackGroup::straight)] = OpenRCT2::BlockedSegmentsType::narrow;
    return array;
}();

static constexpr auto kTrackStyleBlockedSegmentType = std::to_array<TrackGroupBlockedSegmentTypes>({
    {},                                                            // _3DCinema
    kTrackGroupBlockedSegmentsWide,                                // airPoweredVerticalCoaster
    kTrackGroupBlockedSegmentsNarrow,                              // alpineCoaster
    kTrackGroupBlockedSegmentsNarrow,                              // boatHire
    kTrackGroupBlockedSegmentsNarrow,                              // bobsleighCoaster
    kTrackGroupBlockedSegmentsCarRideGhostTrain,                   // carRide
    kTrackGroupBlockedSegmentsWide,                                // chairlift
    {},                                                            // circus
    kTrackGroupBlockedSegmentsNarrow,                              // classicStandUpRollerCoaster
    kTrackGroupBlockedSegmentsWoodenRollerCoaster,                 // classicWoodenRollerCoaster
    kTrackGroupBlockedSegmentsWoodenRollerCoaster,                 // classicWoodenTwisterRollerCoaster
    kTrackGroupBlockedSegmentsInverted,                            // compactInvertedCoaster
    kTrackGroupBlockedSegmentsNarrow,                              // corkscrewRollerCoaster
    {},                                                            // crookedHouse
    kTrackGroupBlockedSegmentsNarrow,                              // dinghySlide
    kTrackGroupBlockedSegmentsNarrow,                              // dinghySlideCovered
    {},                                                            // dodgems
    {},                                                            // enterprise
    {},                                                            // facility
    {},                                                            // ferrisWheel
    kTrackGroupBlockedSegmentsFlyingRollerCoaster,                 // flyingRollerCoaster
    kTrackGroupBlockedSegmentsFlyingRollerCoasterInverted,         // flyingRollerCoasterInverted
    {},                                                            // flyingSaucers
    kTrackGroupBlockedSegmentsCarRideGhostTrain,                   // ghostTrain
    kTrackGroupBlockedSegmentsWide,                                // goKarts
    {},                                                            // hauntedHouse
    kTrackGroupBlockedSegmentsWide,                                // heartlineTwisterCoaster
    kTrackGroupBlockedSegmentsWide,                                // hybridCoaster
    kTrackGroupBlockedSegmentsInverted,                            // invertedHairpinCoaster
    kTrackGroupBlockedSegmentsInverted,                            // invertedImpulseCoaster
    kTrackGroupBlockedSegmentsInverted,                            // invertedRollerCoaster
    kTrackGroupBlockedSegmentsNarrow,                              // juniorRollerCoaster
    kTrackGroupBlockedSegmentsNarrow,                              // latticeTriangle
    kTrackGroupBlockedSegmentsNarrow,                              // latticeTriangleAlt
    {},                                                            // launchedFreefall
    kTrackGroupBlockedSegmentsInverted,                            // layDownRollerCoasterInverted
    {},                                                            // lift
    kTrackGroupBlockedSegmentsNarrow,                              // limLaunchedRollerCoaster
    kTrackGroupBlockedSegmentsNarrow,                              // logFlume
    kTrackGroupBlockedSegmentsNarrow,                              // loopingRollerCoaster
    {},                                                            // magicCarpet
    {},                                                            // maze
    {},                                                            // merryGoRound
    kTrackGroupBlockedSegmentsNarrow,                              // mineRide
    kTrackGroupBlockedSegmentsWoodenRollerCoaster,                 // mineTrainCoaster
    kTrackGroupBlockedSegmentsNarrow,                              // miniGolf
    kTrackGroupBlockedSegmentsNarrow,                              // miniHelicopters
    kTrackGroupBlockedSegmentsNarrow,                              // miniRollerCoaster
    kTrackGroupBlockedSegmentsInverted,                            // miniSuspendedCoaster
    kTrackGroupBlockedSegmentsWide,                                // miniatureRailway
    kTrackGroupBlockedSegmentsNarrow,                              // monorail
    kTrackGroupBlockedSegmentsNarrow,                              // monorailCycles
    {},                                                            // motionSimulator
    kTrackGroupBlockedSegmentsMultiDimensionRollerCoaster,         // multiDimensionRollerCoaster
    kTrackGroupBlockedSegmentsMultiDimensionRollerCoasterInverted, // multiDimensionRollerCoasterInverted
    {},                                                            // observationTower
    kTrackGroupBlockedSegmentsWide,                                // reverseFreefallCoaster
    kTrackGroupBlockedSegmentsWide,                                // reverserRollerCoaster
    kTrackGroupBlockedSegmentsWide,                                // riverRapids
    {},                                                            // rotoDrop
    {},                                                            // shop
    kTrackGroupBlockedSegmentsWide,                                // sideFrictionRollerCoaster
    kTrackGroupBlockedSegmentsNarrow,                              // singleRailRollerCoaster
    {},                                                            // spaceRings
    {},                                                            // spiralSlide
    kTrackGroupBlockedSegmentsWide,                                // splashBoats
    kTrackGroupBlockedSegmentsNarrow,                              // standUpRollerCoaster
    kTrackGroupBlockedSegmentsNarrow,                              // steelWildMouse
    kTrackGroupBlockedSegmentsNarrow,                              // steeplechase
    kTrackGroupBlockedSegmentsNarrow,                              // submarineRide
    kTrackGroupBlockedSegmentsInverted,                            // suspendedMonorail
    kTrackGroupBlockedSegmentsInverted,                            // suspendedSwingingCoaster
    {},                                                            // swingingInverterShip
    {},                                                            // swingingShip
    {},                                                            // topSpin
    {},                                                            // twist
    kTrackGroupBlockedSegmentsNarrow,                              // twisterRollerCoaster
    kTrackGroupBlockedSegmentsWide,                                // virginiaReel
    kTrackGroupBlockedSegmentsNarrow,                              // waterCoaster
    kTrackGroupBlockedSegmentsWoodenRollerCoaster,                 // woodenRollerCoaster
    kTrackGroupBlockedSegmentsWoodenWildMouse,                     // woodenWildMouse
    {},                                                            // null
});
static_assert(std::size(kTrackStyleBlockedSegmentType) == kTrackStyleCount);

static BlockedSegmentsType OverrideBlockedSegmentsType(
    const TrackStyle trackStyle, const OpenRCT2::TrackElemType trackElemType, const uint8_t sequence,
    const BlockedSegmentsType blockedSegmentsType)
{
    if (trackStyle == TrackStyle::mineTrainCoaster)
    {
        // this is narrow since RCT1
        if (trackElemType == TrackElemType::DiagLeftBank || trackElemType == TrackElemType::DiagRightBank)
        {
            return BlockedSegmentsType::narrow;
        }
    }
    if (trackStyle == TrackStyle::flyingRollerCoasterInverted)
    {
        // invalid uninverted pieces but kept for backwards compatibility
        if (trackElemType == TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25
            || trackElemType == TrackElemType::RightBankToRightQuarterTurn3TilesUp25
            || trackElemType == TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank
            || trackElemType == TrackElemType::RightQuarterTurn3TilesDown25ToRightBank
            || trackElemType == TrackElemType::DiagFlatToUp60 || trackElemType == TrackElemType::DiagUp60ToFlat
            || trackElemType == TrackElemType::DiagFlatToDown60 || trackElemType == TrackElemType::DiagDown60ToFlat)
        {
            return BlockedSegmentsType::narrow;
        }
    }
    if (trackStyle == TrackStyle::multiDimensionRollerCoaster)
    {
        // multi dimension coaster uses a mix of narrow and wide blocks
        if ((trackElemType == TrackElemType::LeftQuarterTurn5Tiles
             || trackElemType == TrackElemType::BankedLeftQuarterTurn5Tiles)
            && sequence == 0)
        {
            return BlockedSegmentsType::wide;
        }
        if ((trackElemType == TrackElemType::RightQuarterTurn5Tiles
             || trackElemType == TrackElemType::BankedRightQuarterTurn5Tiles)
            && sequence == 6)
        {
            return BlockedSegmentsType::wide;
        }
        if (trackElemType == TrackElemType::BankedLeftQuarterTurn5Tiles
            || trackElemType == TrackElemType::BankedRightQuarterTurn5Tiles)
        {
            return BlockedSegmentsType::narrow;
        }
        if ((trackElemType == TrackElemType::LeftQuarterTurn3Tiles
             || trackElemType == TrackElemType::LeftBankedQuarterTurn3Tiles)
            && sequence == 0)
        {
            return BlockedSegmentsType::wide;
        }
        if ((trackElemType == TrackElemType::RightQuarterTurn3Tiles
             || trackElemType == TrackElemType::RightBankedQuarterTurn3Tiles)
            && sequence == 3)
        {
            return BlockedSegmentsType::wide;
        }
        if (trackElemType == TrackElemType::LeftBankedQuarterTurn3Tiles
            || trackElemType == TrackElemType::RightBankedQuarterTurn3Tiles)
        {
            return BlockedSegmentsType::narrow;
        }
        if ((trackElemType == TrackElemType::LeftEighthToDiag || trackElemType == TrackElemType::RightEighthToDiag
             || trackElemType == TrackElemType::LeftEighthBankToDiag || trackElemType == TrackElemType::RightEighthBankToDiag)
            && (sequence == 0 || sequence == 1 || sequence == 4))
        {
            return BlockedSegmentsType::wide;
        }
        if ((trackElemType == TrackElemType::LeftEighthToOrthogonal || trackElemType == TrackElemType::RightEighthToOrthogonal
             || trackElemType == TrackElemType::LeftEighthBankToOrthogonal
             || trackElemType == TrackElemType::RightEighthBankToOrthogonal)
            && (sequence == 0 || sequence == 3 || sequence == 4))
        {
            return BlockedSegmentsType::wide;
        }
    }
    if (trackStyle == TrackStyle::multiDimensionRollerCoaster || trackStyle == TrackStyle::multiDimensionRollerCoasterInverted)
    {
        // multi dimension coaster uses a mix of narrow and wide blocks
        // uninverted helices are invalid for inverted multi dimension but kept for backwards compatibility
        if (trackElemType == TrackElemType::LeftHalfBankedHelixUpSmall && (sequence == 0 || sequence == 4 || sequence == 7))
        {
            return BlockedSegmentsType::wide;
        }
        if (trackElemType == TrackElemType::RightHalfBankedHelixUpSmall && (sequence == 0 || sequence == 3 || sequence == 7))
        {
            return BlockedSegmentsType::wide;
        }
        if (trackElemType == TrackElemType::LeftHalfBankedHelixDownSmall
            && (sequence == 0 || sequence == 3 || sequence == 4 || sequence == 7))
        {
            return BlockedSegmentsType::wide;
        }
        if (trackElemType == TrackElemType::RightHalfBankedHelixDownSmall && (sequence == 3 || sequence == 7))
        {
            return BlockedSegmentsType::wide;
        }
        if (trackElemType == TrackElemType::LeftHalfBankedHelixUpLarge && (sequence == 0 || sequence == 7 || sequence == 13))
        {
            return BlockedSegmentsType::wide;
        }
        if (trackElemType == TrackElemType::RightHalfBankedHelixUpLarge && (sequence == 0 || sequence == 6 || sequence == 13))
        {
            return BlockedSegmentsType::wide;
        }
        if (trackElemType == TrackElemType::LeftHalfBankedHelixDownLarge
            && (sequence == 0 || sequence == 6 || sequence == 7 || sequence == 13))
        {
            return BlockedSegmentsType::wide;
        }
        if (trackElemType == TrackElemType::RightHalfBankedHelixDownLarge && (sequence == 6 || sequence == 13))
        {
            return BlockedSegmentsType::wide;
        }
    }
    return blockedSegmentsType;
}

static uint16_t OverrideBlockedSegments(
    const TrackStyle trackStyle, const OpenRCT2::TrackElemType trackElemType, const uint8_t sequence, const uint16_t segments)
{
    if (trackStyle == TrackStyle::boatHire)
    {
        // lacks one segment compared to the default but that support glitches with the track, so maybe this should be removed
        constexpr uint16_t leftQuarterTurn1Tile = EnumsToFlags(
            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topLeft);
        if (trackElemType == TrackElemType::LeftQuarterTurn1Tile)
        {
            return leftQuarterTurn1Tile;
        }
        if (trackElemType == TrackElemType::RightQuarterTurn1Tile)
        {
            return paintSegmentsFlipXAxis(leftQuarterTurn1Tile);
        }
        // boat hire accidentally blocks all segments on these elements, fix is to remove this
        if (trackElemType == TrackElemType::LeftQuarterTurn3Tiles || trackElemType == TrackElemType::RightQuarterTurn3Tiles
            || trackElemType == TrackElemType::LeftQuarterTurn5Tiles || trackElemType == TrackElemType::RightQuarterTurn5Tiles
            || trackElemType == TrackElemType::LeftEighthToDiag || trackElemType == TrackElemType::RightEighthToDiag
            || trackElemType == TrackElemType::LeftEighthToOrthogonal || trackElemType == TrackElemType::RightEighthToOrthogonal
            || trackElemType == TrackElemType::DiagFlat || trackElemType == TrackElemType::SBendLeft
            || trackElemType == TrackElemType::SBendRight)
        {
            return kSegmentsAll;
        }
    }
    if (trackStyle == TrackStyle::goKarts)
    {
        // go karts have some unique small turn segments
        constexpr uint16_t leftQuarterTurn3TilesSeq1 = EnumsToFlags(
            PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight);
        if ((trackElemType == TrackElemType::LeftQuarterTurn3Tiles || trackElemType == TrackElemType::LeftQuarterTurn3TilesUp25
             || trackElemType == TrackElemType::LeftQuarterTurn3TilesDown25)
            && sequence == 1)
        {
            return leftQuarterTurn3TilesSeq1;
        }
        if ((trackElemType == TrackElemType::RightQuarterTurn3Tiles
             || trackElemType == TrackElemType::RightQuarterTurn3TilesUp25
             || trackElemType == TrackElemType::RightQuarterTurn3TilesDown25)
            && sequence == 1)
        {
            return paintSegmentsFlipXAxis(leftQuarterTurn3TilesSeq1);
        }
    }
    if (trackStyle == TrackStyle::miniSuspendedCoaster || trackStyle == TrackStyle::suspendedSwingingCoaster)
    {
        // these block more segments presumably so that the supports are away from the swinging trains
        constexpr uint16_t leftEighthToDiagSeq0 = EnumsToFlags(
            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t leftEighthToDiagSeq4 = EnumsToFlags(
            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
            PaintSegment::bottomLeft, PaintSegment::bottomRight);
        if (trackElemType == TrackElemType::LeftEighthToDiag)
        {
            if (sequence == 0)
                return leftEighthToDiagSeq0;
            else if (sequence == 4)
                return leftEighthToDiagSeq4;
        }
        else if (trackElemType == TrackElemType::RightEighthToDiag)
        {
            if (sequence == 0)
                return paintSegmentsFlipXAxis(leftEighthToDiagSeq0);
            else if (sequence == 4)
                return paintSegmentsFlipXAxis(leftEighthToDiagSeq4);
        }
        else if (trackElemType == TrackElemType::LeftEighthToOrthogonal)
        {
            if (sequence == 0)
                return paintSegmentsRotate(paintSegmentsFlipXAxis(leftEighthToDiagSeq4), 2);
            else if (sequence == 4)
                return paintSegmentsRotate(paintSegmentsFlipXAxis(leftEighthToDiagSeq0), 2);
        }
        else if (trackElemType == TrackElemType::RightEighthToOrthogonal)
        {
            if (sequence == 0)
                return paintSegmentsRotate(leftEighthToDiagSeq4, 3);
            else if (sequence == 4)
                return paintSegmentsRotate(leftEighthToDiagSeq0, 3);
        }
    }
    if (trackStyle == TrackStyle::multiDimensionRollerCoaster)
    {
        // multi dimension sbends have some unique segments
        if ((trackElemType == TrackElemType::SBendLeft || trackElemType == TrackElemType::SBendRight)
            && (sequence == 1 || sequence == 2))
        {
            return kSegmentsAll;
        }
    }
    return segments;
}

void CreateTrackStylePaintInfos()
{
    if (kTrackStylePaintInfosCreated == true)
    {
        return;
    }

    for (size_t trackStyleIndex = 0; trackStyleIndex < kTrackStyleCount; trackStyleIndex++)
    {
        auto& trackStylePaintInfo = kTrackStylePaintInfos[trackStyleIndex];
        const TrackStyle trackStyle = static_cast<TrackStyle>(trackStyleIndex);

        for (size_t trackElemTypeIndex = 0; trackElemTypeIndex < EnumValue(OpenRCT2::TrackElemType::Count);
             trackElemTypeIndex++)
        {
            const auto trackElemType = static_cast<OpenRCT2::TrackElemType>(trackElemTypeIndex);
            TrackPaintFunction paintFunction = GetTrackPaintFunction(static_cast<TrackStyle>(trackStyleIndex), trackElemType);

            if (paintFunction == TrackPaintFunctionDummy)
            {
                trackStylePaintInfo.trackElemTypePaintInfos.emplace_back(paintFunction);
                continue;
            }

            std::array<TrackSequencePaintInfo, OpenRCT2::TrackMetaData::kMaxSequencesPerPiece> sequenceInfo{};

            const auto& ted = OpenRCT2::TrackMetaData::GetTrackElementDescriptor(trackElemType);
            for (uint8_t trackSequenceIndex = 0; trackSequenceIndex < ted.numSequences; trackSequenceIndex++)
            {
                const auto& sequenceDesc = ted.sequences[trackSequenceIndex];

                BlockedSegmentsType segmentsType = kTrackStyleBlockedSegmentType[trackStyleIndex]
                                                                                [EnumValue(ted.definition.group)];
                segmentsType = OverrideBlockedSegmentsType(trackStyle, trackElemType, trackSequenceIndex, segmentsType);

                uint16_t segments = sequenceDesc.blockedSegments[EnumValue(segmentsType)];
                segments = OverrideBlockedSegments(trackStyle, trackElemType, trackSequenceIndex, segments);

                const bool inverted = segmentsType == OpenRCT2::BlockedSegmentsType::inverted;

                sequenceInfo[trackSequenceIndex] = TrackSequencePaintInfo{ sequenceDesc.invertSegmentBlocking != inverted,
                                                                           segments };
            }

            trackStylePaintInfo.trackElemTypePaintInfos.emplace_back(paintFunction, sequenceInfo);
        }
    }

    kTrackStylePaintInfosCreated = true;
}

const TrackStylePaintInfo& GetTrackStylePaintInfo(const TrackStyle trackStyle)
{
    return kTrackStylePaintInfos[EnumValue(trackStyle)];
}
