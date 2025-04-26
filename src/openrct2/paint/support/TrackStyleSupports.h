/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../ride/TrackData.h"
#include "../../ride/TrackStyle.h"

enum class MetalSupportFlags : uint8_t
{
    drawDirection0,
    drawDirection1,
    drawDirection2,
    drawDirection3,
    alternateTiles,
    noLongCrossbeam,
    drawCap,
    extra,
    useTrackColours,
    fallbackToGeneralSupportHeight,
};

constexpr uint8_t kMetalSupportDrawAll = EnumsToFlags(
    MetalSupportFlags::drawDirection0, MetalSupportFlags::drawDirection1, MetalSupportFlags::drawDirection2,
    MetalSupportFlags::drawDirection3);

struct SequenceMetalSupport
{
    uint16_t flags = 0;
    MetalSupportPlace place = MetalSupportPlace::None;
    int8_t baseHeights[kNumOrthogonalDirections] = { 0, 0, 0, 0 };
    int8_t extraHeights[kNumOrthogonalDirections] = { 0, 0, 0, 0 };
};

constexpr uint8_t kMetalSupportsMaxPerSequence = 2;

using SequenceMetalSupports = SequenceMetalSupport[kMetalSupportsMaxPerSequence];

using TrackElementMetalSupports = SequenceMetalSupports[TrackMetaData::kMaxSequencesPerPiece];

const SequenceMetalSupports& GetMetalSupportsForTrackSequence(
    const TrackStyle trackStyle, const TrackElemType trackElemType, const uint8_t trackSequence);

enum class WoodenSupportFlags : uint8_t
{
    typeB,
    covered,
    useStationColours,
};

struct SequenceWoodenSupport
{
    WoodenSupportSubType subType = WoodenSupportSubType::Null;
    WoodenSupportTransitionType transitionType = WoodenSupportTransitionType::None;
    int8_t height = 0;
    uint8_t flags = 0;
};

using TrackElementWoodenSupports = std::array<SequenceWoodenSupport, TrackMetaData::kMaxSequencesPerPiece>;

const SequenceWoodenSupport& GetWoodenSupportForTrackSequence(
    const TrackStyle trackStyle, const TrackElemType trackElemType, const uint8_t trackSequence);

extern const TrackElementMetalSupports kAlpineCoasterMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kBobsleighCoasterMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kCarRideMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kChairliftMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kClassicStandUpRollerCoasterMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kCompactInvertedCoasterMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kCorkscrewRollerCoasterMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kDinghySlideMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kFlyingRollerCoasterMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kFlyingRollerCoasterInvertedMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kGhostTrainMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kHeartlineTwisterCoasterMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kInvertedHairpinCoasterMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kInvertedImpulseCoasterMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kInvertedRollerCoasterMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kJuniorRollerCoasterMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kLatticeTriangleMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kLatticeTriangleAltMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kLayDownRollerCoasterInvertedMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kLimLaunchedRollerCoasterMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kLogFlumeMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kLoopingRollerCoasterMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kMineRideMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kMineTrainCoasterMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kMiniGolfMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kMiniHelicoptersMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kMiniRollerCoasterMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kMiniSuspendedCoasterMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kMonorailMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kMonorailCyclesMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kMultiDimensionRollerCoasterMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kMultiDimensionRollerCoasterInvertedMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kSingleRailRollerCoasterMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kStandUpRollerCoasterMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kSteelWildMouseMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kSteeplechaseMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kSubmarineRideMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kSuspendedMonorailMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kSuspendedSwingingCoasterMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kTwisterRollerCoasterMetalSupports[EnumValue(TrackElemType::Count)];
extern const TrackElementMetalSupports kWoodenRollerCoasterMetalSupports[EnumValue(TrackElemType::Count)];
