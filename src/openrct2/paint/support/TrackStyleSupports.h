/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../ride/TrackData.h"
#include "../../ride/TrackStyle.h"

enum class MetalSupportFlags : uint8_t
{
    drawDirection0,
    drawDirection1,
    drawDirection2,
    drawDirection3,
    alternateTiles,
    typeB,
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
