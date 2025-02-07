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
    MetalSupportPlace place = MetalSupportPlace::TopCorner;
    int8_t baseHeights[kNumOrthogonalDirections] = { 0, 0, 0, 0 };
    int8_t extraHeights[kNumOrthogonalDirections] = { 0, 0, 0, 0 };
};

constexpr uint8_t kMetalSupportsMaxPerSequence = 2;

using SequenceMetalSupports = SequenceMetalSupport[kMetalSupportsMaxPerSequence];

using TrackElementMetalSupports = SequenceMetalSupports[TrackMetaData::kMaxSequencesPerPiece];

const SequenceMetalSupports& GetMetalSupportsForTrackSequence(
    const TrackStyle trackStyle, const TrackElemType trackElemType, const uint8_t trackSequence);
