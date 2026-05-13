/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <gtest/gtest.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/TrackPieceSelection.h>
#include <openrct2/ride/ted/TrackElemType.h>

using OpenRCT2::GetNextValidTrackSegments;
using OpenRCT2::TrackElemType;

namespace
{
    bool contains(const std::vector<TrackElemType>& pieces, TrackElemType t)
    {
        return std::find(pieces.begin(), pieces.end(), t) != pieces.end();
    }

    const RideTypeDescriptor& rtdOf(ride_type_t t)
    {
        return GetRideTypeDescriptor(t);
    }
} // namespace

// -----------------------------------------------------------------------------
// Per-piece overrides in Pass B (special-pieces list)
// -----------------------------------------------------------------------------

TEST(TrackPieceSelectionTest, ObservationTower_TowerSection_ExcludesAdditionalTowerBaseWithoutFlag)
{
    // Observation Tower does not set RtdFlag::allowExtraTowerBases, so Pass B's per-piece
    // override must reject TowerBase even though its TrackGroup (tower) is enabled.
    auto pieces = GetNextValidTrackSegments(rtdOf(RIDE_TYPE_OBSERVATION_TOWER), TrackElemType::towerSection);
    EXPECT_FALSE(contains(pieces, TrackElemType::towerBase));
}

TEST(TrackPieceSelectionTest, WoodenRollerCoaster_Flat_IncludesBrakesSpecialPiece)
{
    // Brakes is a special piece; its destination group IS its enablement gate. Wooden RC
    // enables the brakes group, so from a flat source the brakes piece must appear.
    auto pieces = GetNextValidTrackSegments(rtdOf(RIDE_TYPE_WOODEN_ROLLER_COASTER), TrackElemType::flat);
    EXPECT_TRUE(contains(pieces, TrackElemType::brakes));
}

// -----------------------------------------------------------------------------
// Vertical-curve (curveVertical) gating
// -----------------------------------------------------------------------------

TEST(TrackPieceSelectionTest, CompactInvertedCoaster_Up90_ExcludesVerticalQuarterTurnsWithoutCurveVertical)
{
    // Compact Inverted does not enable curveVertical; vertical 1-tile quarter turns must be
    // rejected by Stage-2 even though their TrackGroup (slopeCurveSteep) is enabled.
    auto pieces = GetNextValidTrackSegments(rtdOf(RIDE_TYPE_COMPACT_INVERTED_COASTER), TrackElemType::up90);
    EXPECT_FALSE(contains(pieces, TrackElemType::leftQuarterTurn1TileUp90));
    EXPECT_FALSE(contains(pieces, TrackElemType::rightQuarterTurn1TileUp90));
}

// -----------------------------------------------------------------------------
// Vertical-slope (slopeVertical) same-pitch continuation
// -----------------------------------------------------------------------------

TEST(TrackPieceSelectionTest, InvertedImpulseCoaster_Up90_AllowsContinuationViaSlopeVertical)
{
    // Inverted Impulse enables slopeVertical (same-slope vertical continuation) and
    // curveVertical (curving while vertical). From an up90 source the predicate must allow:
    //   - up90 itself                     (same-pitch continuation, slopeVertical gate)
    //   - up90ToUp60                      (slope transition involving 90°, slopeVertical gate)
    //   - left/rightQuarterTurn1TileUp90  (same-pitch curve at 90°, curveVertical gate)
    // Asserting each piece individually proves both vertical slope and vertical curve gates
    // permit construction; non-empty alone would pass even if all curves were rejected.
    auto pieces = GetNextValidTrackSegments(rtdOf(RIDE_TYPE_INVERTED_IMPULSE_COASTER), TrackElemType::up90);
    EXPECT_TRUE(contains(pieces, TrackElemType::up90));
    EXPECT_TRUE(contains(pieces, TrackElemType::up90ToUp60));
    EXPECT_TRUE(contains(pieces, TrackElemType::leftQuarterTurn1TileUp90));
    EXPECT_TRUE(contains(pieces, TrackElemType::rightQuarterTurn1TileUp90));
}

// -----------------------------------------------------------------------------
// Curve-class gating, independent of destination definition.group
// -----------------------------------------------------------------------------

TEST(TrackPieceSelectionTest, MiniatureRailway_LeftEighthToDiag_IncludesOrthogonalAndDiagonalContinuations)
{
    // Miniature Railway does not enable TrackGroup::flat. The pre-rewrite group-only gate would
    // have excluded diagFlat (group=flat) and leftEighthToOrthogonal (group=flat). The new
    // pipeline gates curve pieces on their curve class (curveLarge for an eighth) so these
    // correctly appear.
    auto pieces = GetNextValidTrackSegments(rtdOf(RIDE_TYPE_MINIATURE_RAILWAY), TrackElemType::leftEighthToDiag);
    EXPECT_TRUE(contains(pieces, TrackElemType::diagFlat));
    EXPECT_TRUE(contains(pieces, TrackElemType::leftEighthToOrthogonal));
    EXPECT_TRUE(contains(pieces, TrackElemType::diagFlatToUp25));
    EXPECT_TRUE(contains(pieces, TrackElemType::diagFlatToDown25));
}

TEST(TrackPieceSelectionTest, WoodenRollerCoaster_LeftEighthToDiag_IncludesDiagonalFlatContinuation)
{
    // Wooden RC enables diagSlope and curveLarge; from a diagonal-ending source the no-curve /
    // no-slope continuation should surface. Exercises the "straight only" path through Pass A.
    auto pieces = GetNextValidTrackSegments(rtdOf(RIDE_TYPE_WOODEN_ROLLER_COASTER), TrackElemType::leftEighthToDiag);
    EXPECT_TRUE(contains(pieces, TrackElemType::diagFlat));
}

// -----------------------------------------------------------------------------
// Large-curve-on-slope gating (slopeCurveLarge + both pitches must be exactly 25°)
// -----------------------------------------------------------------------------

TEST(TrackPieceSelectionTest, MiniatureRailway_Up25_ExcludesLargeCurvesWithoutSlopeCurveLarge)
{
    // Miniature Railway enables curveLarge but not slopeCurveLarge. From an up25 (sloped)
    // source, large quarter turns at the same up25 pitch must be rejected.
    auto pieces = GetNextValidTrackSegments(rtdOf(RIDE_TYPE_MINIATURE_RAILWAY), TrackElemType::up25);
    EXPECT_FALSE(contains(pieces, TrackElemType::leftQuarterTurn5TilesUp25));
    EXPECT_FALSE(contains(pieces, TrackElemType::rightQuarterTurn5TilesUp25));
}

// -----------------------------------------------------------------------------
// 25↔60 slope gating split by direction (slopeSteepUp / slopeSteepDown)
// -----------------------------------------------------------------------------

TEST(TrackPieceSelectionTest, LogFlume_Up25_ExcludesUp25ToUp60WithoutSlopeSteepUp)
{
    // Log Flume enables slopeSteepDown but not slopeSteepUp. From an up25 source the up25→up60
    // transition (pitchStart=up25) is the meaningful candidate the gate must reject.
    auto pieces = GetNextValidTrackSegments(rtdOf(RIDE_TYPE_LOG_FLUME), TrackElemType::up25);
    EXPECT_FALSE(contains(pieces, TrackElemType::up25ToUp60));
}

TEST(TrackPieceSelectionTest, LogFlume_Up60_ExcludesUp60ToUp25WithoutSlopeSteepUp)
{
    // Reverse direction of the same split: from an up60 source the up60→up25 transition
    // (pitchStart=up60) must also be rejected on a ride lacking slopeSteepUp.
    auto pieces = GetNextValidTrackSegments(rtdOf(RIDE_TYPE_LOG_FLUME), TrackElemType::up60);
    EXPECT_FALSE(contains(pieces, TrackElemType::up60ToUp25));
}

TEST(TrackPieceSelectionTest, LogFlume_Down25_IncludesDownwardSteepTransitionsWithSlopeSteepDown)
{
    // Positive companion: the down-direction split allows the supported direction while
    // rejecting the unsupported one.
    auto pieces = GetNextValidTrackSegments(rtdOf(RIDE_TYPE_LOG_FLUME), TrackElemType::down25);
    EXPECT_TRUE(contains(pieces, TrackElemType::down25ToDown60));
}

// -----------------------------------------------------------------------------
// Special pieces that are entirely absent from the canonical tables
// -----------------------------------------------------------------------------

TEST(TrackPieceSelectionTest, VerticalDropRollerCoaster_Flat_ExcludesMazePiece)
{
    // Maze is in neither kNextSelectedPiece nor kSpecialBuildPieces, so it must never appear
    // in a coaster's follow-on set even though its TED group (flat) is broadly enabled.
    auto pieces = GetNextValidTrackSegments(rtdOf(RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER), TrackElemType::flat);
    EXPECT_FALSE(contains(pieces, TrackElemType::maze));
}

// -----------------------------------------------------------------------------
// Inverted-variant handling (bank flip across regular/inverted construction state)
// -----------------------------------------------------------------------------

TEST(TrackPieceSelectionTest, MultiDimensionRollerCoaster_FromInvertedExitQuarterLoop_EmitsInvertedContinuationViaBankFlip)
{
    // multiDimUp90ToInvertedFlatQuarterLoop ends in inverted state (rollEnd=upsideDown).
    // multiDimInvertedFlatToDown90QuarterLoop has rollStart=none and lives in
    // TrackGroup::quarterLoopInvertedDown, which Multi-Dim enables only in its
    // InvertedTrackPaintFunctions drawer.
    //
    // Under the regular variant, source's effectiveRoll = upsideDown, and rollStart=none never
    // matches. Under the inverted variant, the source's rollEnd=upsideDown flips to none,
    // matching rollStart=none AND the candidate's group is enabled in the inverted drawer.
    // Its presence is end-to-end proof of the inverted-variant Pass B path (bank flip +
    // inverted-drawer group lookup).
    auto pieces = GetNextValidTrackSegments(
        rtdOf(RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER), TrackElemType::multiDimUp90ToInvertedFlatQuarterLoop);
    EXPECT_TRUE(contains(pieces, TrackElemType::multiDimInvertedFlatToDown90QuarterLoop));
}

// -----------------------------------------------------------------------------
// Covered-variant substitution (Stage 4): base candidate emitted as alternativeType
// when the covered drawer's raw enabledTrackGroups bit allows it
// -----------------------------------------------------------------------------

TEST(TrackPieceSelectionTest, WaterCoaster_Flat_EmitsFlatCoveredViaCoveredDrawer)
{
    // Water Coaster's Covered drawer enables {straight, curve, sBend}. The base candidate
    // `flat` (group=straight) maps to alternativeType `flatCovered`; straight is in the Covered
    // bitset, so the substitution must emit flatCovered alongside the regular flat result.
    auto pieces = GetNextValidTrackSegments(rtdOf(RIDE_TYPE_WATER_COASTER), TrackElemType::flat);
    EXPECT_TRUE(contains(pieces, TrackElemType::flatCovered));
}

TEST(TrackPieceSelectionTest, WaterCoaster_Flat_OmitsCoveredSlopePiecesWhenCoveredDrawerLacksSlope)
{
    // Water Coaster's Regular drawer enables slope (so the flat→up25 transition passes Pass A
    // and the base flatToUp25 is reachable). Its Covered drawer enables only
    // {straight, curve, sBend}; slope is NOT in there, so the covered alternative
    // flatToUp25Covered must NOT be emitted. Asserting the base piece IS present anchors the
    // negative — without it, this test could pass simply because flat→slope generation broke.
    auto pieces = GetNextValidTrackSegments(rtdOf(RIDE_TYPE_WATER_COASTER), TrackElemType::flat);
    EXPECT_TRUE(contains(pieces, TrackElemType::flatToUp25));
    EXPECT_FALSE(contains(pieces, TrackElemType::flatToUp25Covered));
}

// -----------------------------------------------------------------------------
// Long-base rewrite (Stage 3): rides with slopeSteepLong substitute LongBase variants
// -----------------------------------------------------------------------------

TEST(TrackPieceSelectionTest, WoodenRollerCoaster_Flat_RewritesFlatToUp60ToLongBaseVariant)
{
    // Wooden RC enables slopeSteepLong, so the table's flatToUp60 transition must be rewritten
    // to flatToUp60LongBase before emission.
    auto pieces = GetNextValidTrackSegments(rtdOf(RIDE_TYPE_WOODEN_ROLLER_COASTER), TrackElemType::flat);
    EXPECT_TRUE(contains(pieces, TrackElemType::flatToUp60LongBase));
    EXPECT_FALSE(contains(pieces, TrackElemType::flatToUp60));
}

TEST(TrackPieceSelectionTest, WoodenRollerCoaster_Flat_RewritesFlatToDown60ToLongBaseVariant)
{
    auto pieces = GetNextValidTrackSegments(rtdOf(RIDE_TYPE_WOODEN_ROLLER_COASTER), TrackElemType::flat);
    EXPECT_TRUE(contains(pieces, TrackElemType::flatToDown60LongBase));
    EXPECT_FALSE(contains(pieces, TrackElemType::flatToDown60));
}

TEST(TrackPieceSelectionTest, WoodenRollerCoaster_Up60_RewritesUp60ToFlatToLongBaseVariant)
{
    // From an up60 source the level-off transition is also rewritten to its LongBase variant.
    auto pieces = GetNextValidTrackSegments(rtdOf(RIDE_TYPE_WOODEN_ROLLER_COASTER), TrackElemType::up60);
    EXPECT_TRUE(contains(pieces, TrackElemType::up60ToFlatLongBase));
    EXPECT_FALSE(contains(pieces, TrackElemType::up60ToFlat));
}

TEST(TrackPieceSelectionTest, MiniatureRailway_Flat_ExcludesFlatToUp60WithoutSlopeSteepLong)
{
    // Miniature Railway enables neither slopeSteepLong nor flatToSteepSlope, so the flat↔60
    // transition is gated out entirely — neither the base nor the LongBase variant appears.
    auto pieces = GetNextValidTrackSegments(rtdOf(RIDE_TYPE_MINIATURE_RAILWAY), TrackElemType::flat);
    EXPECT_FALSE(contains(pieces, TrackElemType::flatToUp60LongBase));
    EXPECT_FALSE(contains(pieces, TrackElemType::flatToUp60));
}

TEST(TrackPieceSelectionTest, GigaCoaster_DiagFlat_RewritesDiagFlatToUp60ToLongBaseVariant)
{
    // GigaCoaster enables diagSlopeSteepLong, which triggers the separate diagonal rewrite
    // branch. From a diagFlat source the diagonal flat→up60 transition must surface as
    // diagFlatToUp60LongBase, not the base diagFlatToUp60.
    auto pieces = GetNextValidTrackSegments(rtdOf(RIDE_TYPE_GIGA_COASTER), TrackElemType::diagFlat);
    EXPECT_TRUE(contains(pieces, TrackElemType::diagFlatToUp60LongBase));
    EXPECT_FALSE(contains(pieces, TrackElemType::diagFlatToUp60));
}
