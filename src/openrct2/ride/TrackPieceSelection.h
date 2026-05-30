/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "ted/TrackElemType.h"

#include <cstdint>
#include <span>
#include <vector>

// RideTypeDescriptor lives in the global namespace (RideData.h has no enclosing namespace);
// forward-declare it at global scope so a reference here resolves to the real type.
struct RideTypeDescriptor;

namespace OpenRCT2
{
    namespace TrackMetadata
    {
        enum class TrackPitch : uint8_t;
        enum class TrackRoll : uint8_t;
        enum class TrackCurve : uint8_t;
    } // namespace TrackMetadata

    // Sentinel inside the special-pieces list, used by the UI to render group separators in the
    // special-elements dropdown. Core iteration in GetNextValidTrackSegments skips it.
    inline constexpr TrackElemType kSpecialTrackPieceSeparator = TrackElemType::none;

    // Canonical curated list of "special" pieces (loops, towers, helixes, brakes, ...).
    // Includes kSpecialTrackPieceSeparator entries so the UI can detect group boundaries in-line.
    std::span<const TrackElemType> GetSpecialBuildPieces();

    // Resolves a (curve, startsDiagonal, slopeStart, slopeEnd, rollStart, rollEnd) tuple to the
    // matching TrackElemType from the canonical curve/slope/bank transition table. Returns
    // TrackElemType::none if no entry matches.
    TrackElemType GetTrackTypeFromCurve(
        TrackMetadata::TrackCurve curve, bool startsDiagonal, TrackMetadata::TrackPitch startSlope,
        TrackMetadata::TrackPitch endSlope, TrackMetadata::TrackRoll startBank, TrackMetadata::TrackRoll endBank);

    // Returns track pieces buildable from `sourceTrackType` on the given ride type, mirroring the
    // construction code's four-stage pipeline (row select via kNextSelectedPiece, static gates via
    // a transcribed UpdateTrackPieceWidgets predicate, long-base rewrites, covered substitution)
    // across all applicable construction variants (regular / inverted / covered / both). Result is
    // the union; per-variant filtering belongs to a future v2 surface.
    //
    // Honours the enableAllDrawableTrackPieces cheat for Stage-2 gating (since
    // TrackDrawerEntry::SupportsTrackGroup does), but NOT for Stage-4 covered substitution (which
    // matches the UI's raw bitset access at RideConstruction.cpp:5008). Does not consult installed
    // vehicle objects' sprite-set or scenario-disabled groups.
    //
    // `rtd` is fully qualified as `::RideTypeDescriptor` because the type is global, not in OpenRCT2.
    std::vector<TrackElemType> GetNextValidTrackSegments(const ::RideTypeDescriptor& rtd, TrackElemType sourceTrackType);
} // namespace OpenRCT2
