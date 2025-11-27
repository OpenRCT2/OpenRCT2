/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/EnumUtils.hpp"
#include "Track.h"
#include "TrackData.h"
#include "TrackPaint.h"

#include <cstdint>
#include <optional>
#include <sfl/static_vector.hpp>

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
    null,
};
constexpr const size_t kTrackStyleCount = 82;

struct TrackSequencePaintInfo
{
private:
    static constexpr int8_t kDoNotSetGeneralSupportHeight = std::numeric_limits<int8_t>::min();

    int8_t generalSupportHeight = kDoNotSetGeneralSupportHeight;

public:
    TrackSequencePaintInfo() {};

    explicit TrackSequencePaintInfo(const std::optional<int32_t> _generalSupportHeight)
    {
        if (_generalSupportHeight.has_value())
        {
            generalSupportHeight = *_generalSupportHeight / kCoordsZStep;
        }
    }

    int32_t getGeneralSupportHeight() const
    {
        return generalSupportHeight * kCoordsZStep;
    }

    bool shouldSetGeneralSupportHeight() const
    {
        return generalSupportHeight != kDoNotSetGeneralSupportHeight;
    }
};

struct TrackElemTypePaintInfo
{
    TrackPaintFunction paintFunction;
    std::array<TrackSequencePaintInfo, OpenRCT2::TrackMetaData::kMaxSequencesPerPiece> sequenceInfo;
};

struct TrackStylePaintInfo
{
    sfl::static_vector<TrackElemTypePaintInfo, EnumValue(OpenRCT2::TrackElemType::Count)> trackElemTypePaintInfos;
};

void CreateTrackStylePaintInfos();
const TrackStylePaintInfo& GetTrackStylePaintInfo(const TrackStyle trackStyle);
