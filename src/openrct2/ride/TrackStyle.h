/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "TrackPaint.h"

#include <cstdint>

enum class TrackStyle : uint8_t
{
    _3DCinema,
    airPoweredVerticalCoaster,
    alpineCoaster,
    boatHire,
    bobsleighCoaster,
    carRide,
    chairlift,
    circus,
    classicStandUpRollerCoaster,
    classicWoodenRollerCoaster,
    classicWoodenTwisterRollerCoaster,
    compactInvertedCoaster,
    corkscrewRollerCoaster,
    crookedHouse,
    dinghySlide,
    dinghySlideCovered,
    dodgems,
    enterprise,
    facility,
    ferrisWheel,
    flyingRollerCoaster,
    flyingRollerCoasterInverted,
    flyingSaucers,
    ghostTrain,
    goKarts,
    hauntedHouse,
    heartlineTwisterCoaster,
    hybridCoaster,
    invertedHairpinCoaster,
    invertedImpulseCoaster,
    invertedRollerCoaster,
    juniorRollerCoaster,
    latticeTriangle,
    latticeTriangleAlt,
    launchedFreefall,
    layDownRollerCoasterInverted,
    lift,
    limLaunchedRollerCoaster,
    logFlume,
    loopingRollerCoaster,
    magicCarpet,
    maze,
    merryGoRound,
    mineRide,
    mineTrainCoaster,
    miniGolf,
    miniHelicopters,
    miniRollerCoaster,
    miniSuspendedCoaster,
    miniatureRailway,
    monorail,
    monorailCycles,
    motionSimulator,
    multiDimensionRollerCoaster,
    multiDimensionRollerCoasterInverted,
    observationTower,
    reverseFreefallCoaster,
    reverserRollerCoaster,
    riverRapids,
    rotoDrop,
    shop,
    sideFrictionRollerCoaster,
    singleRailRollerCoaster,
    spaceRings,
    spiralSlide,
    splashBoats,
    standUpRollerCoaster,
    steelWildMouse,
    steeplechase,
    submarineRide,
    suspendedMonorail,
    suspendedSwingingCoaster,
    swingingInverterShip,
    swingingShip,
    topSpin,
    twist,
    twisterRollerCoaster,
    virginiaReel,
    waterCoaster,
    woodenRollerCoaster,
    woodenWildMouse,

    null = 255,
};

TrackPaintFunction GetTrackPaintFunction(TrackStyle trackStyle, OpenRCT2::TrackElemType trackType);
