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
#include "TrackData.h"
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

static constexpr const bool kTrackStyleWide[] = {
    false, // 3DCinema
    true,  // airPoweredVerticalCoaster
    false, // alpineCoaster
    false, // boatHire
    false, // bobsleighCoaster
    false, // carRide
    true,  // chairlift
    false, // circus
    false, // classicStandUpRollerCoaster
    true,  // classicWoodenRollerCoaster
    true,  // classicWoodenTwisterRollerCoaster
    false, // compactInvertedCoaster
    false, // corkscrewRollerCoaster
    false, // crookedHouse
    false, // dinghySlide
    false, // dinghySlideCovered
    false, // dodgems
    false, // enterprise
    false, // facility
    false, // ferrisWheel
    false, // flyingRollerCoaster
    false, // flyingRollerCoasterInverted
    false, // flyingSaucers
    false, // ghostTrain
    true,  // goKarts
    false, // hauntedHouse
    true,  // heartlineTwisterCoaster
    true,  // hybridCoaster
    false, // invertedHairpinCoaster
    false, // invertedImpulseCoaster
    false, // invertedRollerCoaster
    false, // juniorRollerCoaster
    false, // latticeTriangle
    false, // latticeTriangleAlt
    false, // launchedFreefall
    false, // layDownRollerCoasterInverted
    false, // lift
    false, // limLaunchedRollerCoaster
    false, // logFlume
    false, // loopingRollerCoaster
    false, // magicCarpet
    false, // maze
    false, // merryGoRound
    false, // mineRide
    true,  // mineTrainCoaster
    false, // miniGolf
    false, // miniHelicopters
    false, // miniRollerCoaster
    false, // miniSuspendedCoaster
    true,  // miniatureRailway
    false, // monorail
    false, // monorailCycles
    false, // motionSimulator
    false, // multiDimensionRollerCoaster
    false, // multiDimensionRollerCoasterInverted
    false, // observationTower
    true,  // reverseFreefallCoaster
    true,  // reverserRollerCoaster
    true,  // riverRapids
    false, // rotoDrop
    false, // shop
    true,  // sideFrictionRollerCoaster
    false, // singleRailRollerCoaster
    false, // spaceRings
    false, // spiralSlide
    true,  // splashBoats
    false, // standUpRollerCoaster
    false, // steelWildMouse
    false, // steeplechase
    false, // submarineRide
    false, // suspendedMonorail
    false, // suspendedSwingingCoaster
    false, // swingingInverterShip
    false, // swingingShip
    false, // topSpin
    false, // twist
    false, // twisterRollerCoaster
    true,  // virginiaReel
    false, // waterCoaster
    true,  // woodenRollerCoaster
    true,  // woodenWildMouse
};

bool IsTrackStyleWide(const TrackStyle trackStyle, const OpenRCT2::TrackElemType trackElemType, const bool isChain)
{
    const auto& ted = OpenRCT2::TrackMetaData::GetTrackElementDescriptor(trackElemType);

    // These checks are in place of tracks that would supply a list of all their track group and chain widths
    if ((trackStyle == TrackStyle::carRide || trackStyle == TrackStyle::ghostTrain)
        && ted.definition.group == TrackGroup::curveVerySmall)
        return true;

    if ((trackStyle == TrackStyle::woodenRollerCoaster || trackStyle == TrackStyle::classicWoodenRollerCoaster
         || trackStyle == TrackStyle::classicWoodenTwisterRollerCoaster)
        && (ted.definition.group == TrackGroup::verticalLoop || ted.definition.group == TrackGroup::halfLoopMedium
            || ted.definition.group == TrackGroup::halfLoopLarge))
        return false;

    if (trackStyle == TrackStyle::multiDimensionRollerCoaster && isChain)
        return true;

    return kTrackStyleWide[EnumValue(trackStyle)];
}
