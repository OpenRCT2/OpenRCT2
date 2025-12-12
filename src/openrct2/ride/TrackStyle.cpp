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

using TrackGroupTunnelStyles = std::array<std::optional<TunnelStyle>, EnumValue(TrackGroup::count)>;

constexpr TrackGroupTunnelStyles kTrackGroupTunnelStylesStandard = []() consteval {
    TrackGroupTunnelStyles array{};
    array.fill(TunnelStyle::standard);
    array[EnumValue(TrackGroup::stationEnd)] = TunnelStyle::square;
    array[EnumValue(TrackGroup::onridePhoto)] = TunnelStyle::square;
    return array;
}();

constexpr TrackGroupTunnelStyles kTrackGroupTunnelStylesSquare = []() consteval {
    TrackGroupTunnelStyles array{};
    array.fill(TunnelStyle::square);
    return array;
}();

constexpr TrackGroupTunnelStyles kTrackGroupTunnelStylesInverted = []() consteval {
    TrackGroupTunnelStyles array{};
    array.fill(TunnelStyle::inverted);
    array[EnumValue(TrackGroup::stationEnd)] = TunnelStyle::invertedSquare;
    array[EnumValue(TrackGroup::onridePhoto)] = TunnelStyle::square;
    return array;
}();

constexpr TrackGroupTunnelStyles kTrackGroupTunnelStylesInvertedSquare = []() consteval {
    TrackGroupTunnelStyles array{};
    array.fill(TunnelStyle::invertedSquare);
    return array;
}();

constexpr TrackGroupTunnelStyles kTrackGroupTunnelStylesWoodenRollerCoaster = []() consteval {
    TrackGroupTunnelStyles array{};
    array.fill(TunnelStyle::square);
    array[EnumValue(TrackGroup::onridePhoto)] = TunnelStyle::invertedSquare;
    return array;
}();

constexpr TrackGroupTunnelStyles kTrackGroupInvertedMultiDimensionRollerCoaster = []() consteval {
    TrackGroupTunnelStyles array{};
    array.fill(TunnelStyle::square);
    array[EnumValue(TrackGroup::slopeVertical)] = TunnelStyle::inverted;
    return array;
}();

constexpr TrackGroupTunnelStyles kTrackGroupMiniGolf = []() consteval {
    TrackGroupTunnelStyles array{};
    array.fill(TunnelStyle::standardWithPath);
    array[EnumValue(TrackGroup::stationEnd)] = TunnelStyle::square;
    return array;
}();

constexpr TrackGroupTunnelStyles kTrackGroupLayDownRollerCoasterInverted = []() consteval {
    TrackGroupTunnelStyles array{};
    array.fill(TunnelStyle::standard);
    array[EnumValue(TrackGroup::stationEnd)] = TunnelStyle::invertedSquare;
    array[EnumValue(TrackGroup::onridePhoto)] = TunnelStyle::square;
    return array;
}();

constexpr TrackGroupTunnelStyles kTrackGroupBoatHire = []() consteval {
    TrackGroupTunnelStyles array{};
    array[EnumValue(TrackGroup::stationEnd)] = TunnelStyle::square;
    return array;
}();

struct TrackStyleTunnelInfo
{
    TrackGroupTunnelStyles styles;
    TunnelGroup group;
};

static constexpr auto kTrackStyleTunnelInfos = std::to_array<TrackStyleTunnelInfo>({
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // _3DCinema
    { kTrackGroupTunnelStylesSquare, TunnelGroup::uninverted },                      // airPoweredVerticalCoaster
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // alpineCoaster
    { kTrackGroupBoatHire, TunnelGroup::uninverted },                                // boatHire
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // bobsleighCoaster
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // carRide
    { kTrackGroupTunnelStylesSquare, TunnelGroup::uninverted },                      // chairlift
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // circus
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // classicStandUpRollerCoaster
    { kTrackGroupTunnelStylesWoodenRollerCoaster, TunnelGroup::uninverted },         // classicWoodenRollerCoaster
    { kTrackGroupTunnelStylesWoodenRollerCoaster, TunnelGroup::uninverted },         // classicWoodenTwisterRollerCoaster
    { kTrackGroupTunnelStylesInverted, TunnelGroup::inverted },                      // compactInvertedCoaster
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // corkscrewRollerCoaster
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // crookedHouse
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // dinghySlide
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // dinghySlideCovered
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // dodgems
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // enterprise
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // facility
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // ferrisWheel
    { kTrackGroupTunnelStylesSquare, TunnelGroup::uninverted },                      // flyingRollerCoaster
    { kTrackGroupTunnelStylesSquare, TunnelGroup::invertedFlying },                  // flyingRollerCoasterInverted
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // flyingSaucers
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // ghostTrain
    { kTrackGroupTunnelStylesSquare, TunnelGroup::uninverted },                      // goKarts
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // hauntedHouse
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // heartlineTwisterCoaster
    { kTrackGroupTunnelStylesSquare, TunnelGroup::uninverted },                      // hybridCoaster
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // invertedHairpinCoaster
    { kTrackGroupTunnelStylesInverted, TunnelGroup::inverted },                      // invertedImpulseCoaster
    { kTrackGroupTunnelStylesInverted, TunnelGroup::inverted },                      // invertedRollerCoaster
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // juniorRollerCoaster
    { kTrackGroupTunnelStylesSquare, TunnelGroup::uninverted },                      // latticeTriangle
    { kTrackGroupTunnelStylesSquare, TunnelGroup::uninverted },                      // latticeTriangleAlt
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // launchedFreefall
    { kTrackGroupLayDownRollerCoasterInverted, TunnelGroup::invertedFlying },        // layDownRollerCoasterInverted
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // lift
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // limLaunchedRollerCoaster
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // logFlume
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // loopingRollerCoaster
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // magicCarpet
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // maze
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // merryGoRound
    { kTrackGroupTunnelStylesSquare, TunnelGroup::uninverted },                      // mineRide
    { kTrackGroupTunnelStylesSquare, TunnelGroup::uninverted },                      // mineTrainCoaster
    { kTrackGroupMiniGolf, TunnelGroup::uninverted },                                // miniGolf
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // miniHelicopters
    { kTrackGroupTunnelStylesSquare, TunnelGroup::uninverted },                      // miniRollerCoaster
    { kTrackGroupTunnelStylesStandard, TunnelGroup::inverted },                      // miniSuspendedCoaster
    { kTrackGroupTunnelStylesSquare, TunnelGroup::uninverted },                      // miniatureRailway
    { kTrackGroupTunnelStylesSquare, TunnelGroup::uninverted },                      // monorail
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // monorailCycles
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // motionSimulator
    { kTrackGroupTunnelStylesSquare, TunnelGroup::uninverted },                      // multiDimensionRollerCoaster
    { kTrackGroupInvertedMultiDimensionRollerCoaster, TunnelGroup::invertedFlying }, // multiDimensionRollerCoasterInverted
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // observationTower
    { kTrackGroupTunnelStylesSquare, TunnelGroup::uninverted },                      // reverseFreefallCoaster
    { kTrackGroupTunnelStylesSquare, TunnelGroup::uninverted },                      // reverserRollerCoaster
    { kTrackGroupTunnelStylesSquare, TunnelGroup::uninverted },                      // riverRapids
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // rotoDrop
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // shop
    { kTrackGroupTunnelStylesSquare, TunnelGroup::uninverted },                      // sideFrictionRollerCoaster
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // singleRailRollerCoaster
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // spaceRings
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // spiralSlide
    { kTrackGroupTunnelStylesSquare, TunnelGroup::uninverted },                      // splashBoats
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // standUpRollerCoaster
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // steelWildMouse
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // steeplechase
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // submarineRide
    { kTrackGroupTunnelStylesInvertedSquare, TunnelGroup::inverted },                // suspendedMonorail
    { kTrackGroupTunnelStylesInverted, TunnelGroup::inverted },                      // suspendedSwingingCoaster
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // swingingInverterShip
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // swingingShip
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // topSpin
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // twist
    { kTrackGroupTunnelStylesSquare, TunnelGroup::uninverted },                      // twisterRollerCoaster
    { kTrackGroupTunnelStylesSquare, TunnelGroup::uninverted },                      // virginiaReel
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // waterCoaster
    { kTrackGroupTunnelStylesWoodenRollerCoaster, TunnelGroup::uninverted },         // woodenRollerCoaster
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // woodenWildMouse
    { kTrackGroupTunnelStylesStandard, TunnelGroup::uninverted },                    // null
});
static_assert(std::size(kTrackStyleTunnelInfos) == kTrackStyleCount);

static TunnelGroup OverrideTunnelGroup(
    const TrackStyle trackStyle, const TrackElemType trackElemType, const TunnelGroup tunnelGroup)
{
    if (trackStyle == TrackStyle::flyingRollerCoaster)
    {
        // invalid inverted pieces but kept for backwards compatibility
        if (trackElemType == TrackElemType::LeftFlyerTwistDown || trackElemType == TrackElemType::RightFlyerTwistDown
            || trackElemType == TrackElemType::MultiDimInvertedUp90ToFlatQuarterLoop
            || trackElemType == TrackElemType::MultiDimInvertedFlatToDown90QuarterLoop
            || trackElemType == TrackElemType::LeftFlyerLargeHalfLoopInvertedDown
            || trackElemType == TrackElemType::RightFlyerLargeHalfLoopInvertedDown
            || trackElemType == TrackElemType::LeftFlyerLargeHalfLoopInvertedUp
            || trackElemType == TrackElemType::RightFlyerLargeHalfLoopInvertedUp
            || trackElemType == TrackElemType::FlyerHalfLoopInvertedUp)
        {
            return TunnelGroup::invertedFlying;
        }
    }
    if (trackStyle == TrackStyle::flyingRollerCoasterInverted)
    {
        // invalid inverted pieces but kept for backwards compatibility
        switch (trackElemType)
        {
            case TrackElemType::LeftTwistDownToUp:
            case TrackElemType::RightTwistDownToUp:
            case TrackElemType::LeftTwistUpToDown:
            case TrackElemType::RightTwistUpToDown:
            case TrackElemType::LeftHalfBankedHelixUpSmall:
            case TrackElemType::RightHalfBankedHelixUpSmall:
            case TrackElemType::LeftHalfBankedHelixDownSmall:
            case TrackElemType::RightHalfBankedHelixDownSmall:
            case TrackElemType::LeftHalfBankedHelixUpLarge:
            case TrackElemType::RightHalfBankedHelixUpLarge:
            case TrackElemType::LeftHalfBankedHelixDownLarge:
            case TrackElemType::RightHalfBankedHelixDownLarge:
            case TrackElemType::LeftFlyerTwistUp:
            case TrackElemType::RightFlyerTwistUp:
            case TrackElemType::LeftBarrelRollUpToDown:
            case TrackElemType::RightBarrelRollUpToDown:
            case TrackElemType::LeftBarrelRollDownToUp:
            case TrackElemType::RightBarrelRollDownToUp:
            case TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25:
            case TrackElemType::RightBankToRightQuarterTurn3TilesUp25:
            case TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank:
            case TrackElemType::RightQuarterTurn3TilesDown25ToRightBank:
            case TrackElemType::LeftLargeHalfLoopUp:
            case TrackElemType::RightLargeHalfLoopUp:
            case TrackElemType::LeftLargeHalfLoopDown:
            case TrackElemType::RightLargeHalfLoopDown:
            case TrackElemType::Up90ToInvertedFlatQuarterLoop:
            case TrackElemType::InvertedFlatToDown90QuarterLoop:
            case TrackElemType::LeftLargeCorkscrewUp:
            case TrackElemType::RightLargeCorkscrewUp:
            case TrackElemType::LeftLargeCorkscrewDown:
            case TrackElemType::RightLargeCorkscrewDown:
            case TrackElemType::LeftMediumHalfLoopUp:
            case TrackElemType::RightMediumHalfLoopUp:
            case TrackElemType::LeftMediumHalfLoopDown:
            case TrackElemType::RightMediumHalfLoopDown:
            case TrackElemType::LeftZeroGRollUp:
            case TrackElemType::RightZeroGRollUp:
            case TrackElemType::LeftZeroGRollDown:
            case TrackElemType::RightZeroGRollDown:
            case TrackElemType::LeftLargeZeroGRollUp:
            case TrackElemType::RightLargeZeroGRollUp:
            case TrackElemType::LeftLargeZeroGRollDown:
            case TrackElemType::RightLargeZeroGRollDown:
            case TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp:
            case TrackElemType::RightFlyerLargeHalfLoopUninvertedUp:
            case TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown:
            case TrackElemType::RightFlyerLargeHalfLoopUninvertedDown:
            case TrackElemType::FlyerHalfLoopUninvertedDown:
            case TrackElemType::LeftEighthToDiagUp25:
            case TrackElemType::RightEighthToDiagUp25:
            case TrackElemType::LeftEighthToDiagDown25:
            case TrackElemType::RightEighthToDiagDown25:
            case TrackElemType::LeftEighthToOrthogonalUp25:
            case TrackElemType::RightEighthToOrthogonalUp25:
            case TrackElemType::LeftEighthToOrthogonalDown25:
            case TrackElemType::RightEighthToOrthogonalDown25:
            case TrackElemType::LeftEighthBankToDiagUp25:
            case TrackElemType::RightEighthBankToDiagUp25:
            case TrackElemType::LeftEighthBankToDiagDown25:
            case TrackElemType::RightEighthBankToDiagDown25:
            case TrackElemType::LeftEighthBankToOrthogonalUp25:
            case TrackElemType::RightEighthBankToOrthogonalUp25:
            case TrackElemType::LeftEighthBankToOrthogonalDown25:
            case TrackElemType::RightEighthBankToOrthogonalDown25:
            case TrackElemType::DiagFlatToUp60LongBase:
            case TrackElemType::DiagUp60ToFlatLongBase:
            case TrackElemType::DiagFlatToDown60LongBase:
            case TrackElemType::DiagDown60ToFlatLongBase:
            case TrackElemType::LeftEighthDiveLoopUpToOrthogonal:
            case TrackElemType::RightEighthDiveLoopUpToOrthogonal:
            case TrackElemType::LeftEighthDiveLoopDownToDiag:
            case TrackElemType::RightEighthDiveLoopDownToDiag:
                return TunnelGroup::uninverted;
            default:
                return tunnelGroup;
        }
    }
    if (trackStyle == TrackStyle::multiDimensionRollerCoaster)
    {
        // invalid inverted pieces but kept for backwards compatibility
        if (trackElemType == TrackElemType::LeftFlyerTwistDown || trackElemType == TrackElemType::RightFlyerTwistDown)
        {
            return TunnelGroup::invertedFlying;
        }
    }
    if (trackStyle == TrackStyle::multiDimensionRollerCoasterInverted)
    {
        // invalid inverted pieces but kept for backwards compatibility
        if (trackElemType == TrackElemType::LeftHalfBankedHelixUpSmall
            || trackElemType == TrackElemType::RightHalfBankedHelixUpSmall
            || trackElemType == TrackElemType::LeftHalfBankedHelixDownSmall
            || trackElemType == TrackElemType::RightHalfBankedHelixDownSmall
            || trackElemType == TrackElemType::LeftHalfBankedHelixUpLarge
            || trackElemType == TrackElemType::RightHalfBankedHelixUpLarge
            || trackElemType == TrackElemType::LeftHalfBankedHelixDownLarge
            || trackElemType == TrackElemType::RightHalfBankedHelixDownLarge || trackElemType == TrackElemType::LeftFlyerTwistUp
            || trackElemType == TrackElemType::RightFlyerTwistUp)
        {
            return TunnelGroup::uninverted;
        }
    }
    return tunnelGroup;
}

static OpenRCT2::TrackMetaData::SequenceTunnelGroup OverrideSequenceTunnelGroup(
    const TrackStyle trackStyle, OpenRCT2::TrackMetaData::SequenceTunnelGroup sequenceTunnelGroup,
    const OpenRCT2::TrackElemType trackElemType, const uint8_t trackSequence)
{
    if (trackStyle == TrackStyle::airPoweredVerticalCoaster)
    {
        // incomplete inner tunnels
        if (trackElemType == OpenRCT2::TrackElemType::ReverseFreefallSlope && trackSequence == 0)
        {
            sequenceTunnelGroup[1] = OpenRCT2::TrackMetaData::SequenceTunnel{ 0, 2, TunnelSlope::flat };
        }
        if (trackElemType == OpenRCT2::TrackElemType::AirThrustVerticalDownToLevel && trackSequence == 6)
        {
            sequenceTunnelGroup[1] = OpenRCT2::TrackMetaData::SequenceTunnel{ 0, 0, TunnelSlope::flat };
        }
    }
    if (trackStyle == TrackStyle::classicWoodenRollerCoaster || trackStyle == TrackStyle::classicWoodenTwisterRollerCoaster
        || trackStyle == TrackStyle::woodenRollerCoaster)
    {
        if (trackElemType == OpenRCT2::TrackElemType::Watersplash)
        {
            // incomplete inner tunnels
            if (trackSequence == 0)
            {
                sequenceTunnelGroup[1] = OpenRCT2::TrackMetaData::SequenceTunnel{ 16, 2, TunnelSlope::flat };
            }
            if (trackSequence == 4)
            {
                sequenceTunnelGroup[1] = OpenRCT2::TrackMetaData::SequenceTunnel{ 16, 0, TunnelSlope::flat };
            }
        }
    }
    if (trackStyle == TrackStyle::flyingRollerCoaster || trackStyle == TrackStyle::flyingRollerCoasterInverted
        || trackStyle == TrackStyle::hybridCoaster || trackStyle == TrackStyle::twisterRollerCoaster)
    {
        // quarter loop tunnels are all inconsistent
        if (trackElemType == OpenRCT2::TrackElemType::Up90ToInvertedFlatQuarterLoop && trackSequence == 2)
        {
            for (auto& tunnel : sequenceTunnelGroup)
            {
                if (tunnel.has_value())
                {
                    tunnel.value().height += 16;
                }
            }
        }
        if (trackElemType == OpenRCT2::TrackElemType::InvertedFlatToDown90QuarterLoop && trackSequence == 0)
        {
            for (auto& tunnel : sequenceTunnelGroup)
            {
                if (tunnel.has_value())
                {
                    tunnel.value().height += 16;
                }
            }
        }
    }
    if (trackStyle == TrackStyle::flyingRollerCoasterInverted)
    {
        // invalid inverted piece but kept for backwards compatibility
        if (trackElemType == TrackElemType::MultiDimUp90ToInvertedFlatQuarterLoop && trackSequence == 2)
        {
            sequenceTunnelGroup[0] = OpenRCT2::TrackMetaData::SequenceTunnel{ 16, 0, TunnelSlope::flat };
        }
    }
    if (trackStyle == TrackStyle::logFlume)
    {
        // different from standard, incorrect use of tunnel type, should probably be changed
        if (trackElemType == OpenRCT2::TrackElemType::Down25ToDown60 && trackSequence == 0)
        {
            sequenceTunnelGroup[0] = OpenRCT2::TrackMetaData::SequenceTunnel{ 0, 2, TunnelSlope::flatToDown25 };
        }
        // default tunnel looks bad
        if (trackElemType == OpenRCT2::TrackElemType::Down60ToDown25 && trackSequence == 0)
        {
            sequenceTunnelGroup[1] = OpenRCT2::TrackMetaData::SequenceTunnel{ 24, 0, TunnelSlope::flat };
        }
    }
    if (trackStyle == TrackStyle::multiDimensionRollerCoaster)
    {
        // invalid inverted pieces but kept for backwards compatibility
        if (trackElemType == TrackElemType::MultiDimInvertedFlatToDown90QuarterLoop && trackSequence == 0)
        {
            sequenceTunnelGroup[0] = OpenRCT2::TrackMetaData::SequenceTunnel{ 32, 0, TunnelSlope::flat };
        }
        else if (trackElemType == TrackElemType::MultiDimInvertedUp90ToFlatQuarterLoop && trackSequence == 2)
        {
            sequenceTunnelGroup[0] = OpenRCT2::TrackMetaData::SequenceTunnel{ 16, 0, TunnelSlope::flat };
        }
    }
    if (trackStyle == TrackStyle::multiDimensionRollerCoasterInverted)
    {
        // invalid inverted pieces but kept for backwards compatibility
        if (trackElemType == TrackElemType::MultiDimUp90ToInvertedFlatQuarterLoop && trackSequence == 2)
        {
            sequenceTunnelGroup[0] = OpenRCT2::TrackMetaData::SequenceTunnel{ 0, 0, TunnelSlope::flat };
        }
        else if (trackElemType == TrackElemType::MultiDimFlatToDown90QuarterLoop && trackSequence == 0)
        {
            sequenceTunnelGroup[0] = OpenRCT2::TrackMetaData::SequenceTunnel{ 16, 0, TunnelSlope::flat };
        }
    }
    if (trackStyle == TrackStyle::reverseFreefallCoaster)
    {
        if (trackElemType == OpenRCT2::TrackElemType::ReverseFreefallSlope)
        {
            // incomplete inner tunnels on one side only
            if (trackSequence == 1 || trackSequence == 2)
            {
                sequenceTunnelGroup[0] = OpenRCT2::TrackMetaData::SequenceTunnel{ 0, 0, TunnelSlope::flat };
            }
            else if (trackSequence == 3)
            {
                sequenceTunnelGroup[0] = OpenRCT2::TrackMetaData::SequenceTunnel{ 16, 0, TunnelSlope::flat };
            }
            else if (trackSequence == 4)
            {
                sequenceTunnelGroup[0] = OpenRCT2::TrackMetaData::SequenceTunnel{ 64, 0, TunnelSlope::flat };
            }
        }
    }
    if (trackStyle == TrackStyle::submarineRide)
    {
        // submarine ride has lower tunnels
        if (trackElemType != OpenRCT2::TrackElemType::BeginStation && trackElemType != OpenRCT2::TrackElemType::MiddleStation
            && trackElemType != OpenRCT2::TrackElemType::EndStation)
        {
            for (auto& tunnel : sequenceTunnelGroup)
            {
                if (tunnel.has_value())
                {
                    tunnel.value().height -= 16;
                }
            }
        }
    }
    if (trackStyle == TrackStyle::virginiaReel)
    {
        // this tunnel is flat since RCT1 for some reason, could be removed as standard is slightly better
        if (trackElemType == TrackElemType::FlatToUp25)
        {
            sequenceTunnelGroup[1] = OpenRCT2::TrackMetaData::SequenceTunnel{ 0, 2, TunnelSlope::flat };
        }
        if (trackElemType == TrackElemType::Down25ToFlat)
        {
            sequenceTunnelGroup[1] = OpenRCT2::TrackMetaData::SequenceTunnel{ 0, 0, TunnelSlope::flat };
        }
    }
    return sequenceTunnelGroup;
}

TrackSequencePaintInfo::TrackSequencePaintInfo(
    const OpenRCT2::TrackMetaData::SequenceTunnelGroup& sequenceTunnelGroups, const std::optional<TunnelStyle> tunnelStyle,
    const std::optional<uint8_t> _verticalTunnelHeight)
{
    if (tunnelStyle.has_value())
    {
        for (size_t i = 0; i < sequenceTunnelGroups.size(); i++)
        {
            const auto& tunnel = sequenceTunnelGroups[i];
            if (tunnel.has_value())
            {
                tunnels[i].height = tunnel.value().height;
                tunnels[i].direction = tunnel.value().direction;
                tunnels[i].type = GetTunnelType(tunnelStyle.value(), tunnel.value().slope);
                if (tunnel.value().allowDoors)
                {
                    tunnels[i].flags.set(TrackStyleTunnelFlags::allowDoors);
                }
                if (tunnel.value().slope == TunnelSlope::flatToDown25)
                {
                    tunnels[i].flags.set(TrackStyleTunnelFlags::flatToDown25);
                }
            }
        }
    }

    if (_verticalTunnelHeight.has_value())
    {
        verticalTunnelHeight = *_verticalTunnelHeight;
    }
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
        const auto trackStyle = static_cast<TrackStyle>(trackStyleIndex);

        for (size_t trackElemTypeIndex = 0; trackElemTypeIndex < EnumValue(OpenRCT2::TrackElemType::Count);
             trackElemTypeIndex++)
        {
            const auto trackElemType = static_cast<OpenRCT2::TrackElemType>(trackElemTypeIndex);
            TrackPaintFunction paintFunction = GetTrackPaintFunction(trackStyle, trackElemType);

            if (paintFunction == TrackPaintFunctionDummy)
            {
                trackStylePaintInfo.trackElemTypePaintInfos.emplace_back(paintFunction);
                continue;
            }

            std::array<TrackSequencePaintInfo, OpenRCT2::TrackMetaData::kMaxSequencesPerPiece> sequenceInfo{};

            const auto& ted = OpenRCT2::TrackMetaData::GetTrackElementDescriptor(trackElemType);
            for (uint8_t trackSequenceIndex = 0; trackSequenceIndex < ted.numSequences; trackSequenceIndex++)
            {
                TunnelGroup tunnelGroup = kTrackStyleTunnelInfos[trackStyleIndex].group;
                tunnelGroup = OverrideTunnelGroup(trackStyle, trackElemType, tunnelGroup);
                const auto& sequenceDesc = ted.sequences[trackSequenceIndex];
                const auto& sequenceTunnels = sequenceDesc.tunnels.tunnelGroups[EnumValue(tunnelGroup)];
                const auto sequenceTunnelsOverride = OverrideSequenceTunnelGroup(
                    trackStyle, sequenceTunnels, trackElemType, trackSequenceIndex);
                const auto& tunnelStyle = kTrackStyleTunnelInfos[trackStyleIndex].styles[EnumValue(ted.definition.group)];

                sequenceInfo[trackSequenceIndex] = TrackSequencePaintInfo{ sequenceTunnelsOverride, tunnelStyle,
                                                                           sequenceDesc.tunnels.verticalTunnelHeight };
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
