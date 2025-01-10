/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackStyle.h"

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

    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
    DummyGetter,
};
static_assert(std::size(kPaintFunctionMap) == (sizeof(TrackStyle) * 256));

TrackPaintFunction GetTrackPaintFunction(TrackStyle trackStyle, OpenRCT2::TrackElemType trackType)
{
    return kPaintFunctionMap[static_cast<uint8_t>(trackStyle)](trackType);
}
