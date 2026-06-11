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
#include <openrct2/core/EnumUtils.hpp>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/TrackData.h>
#include <openrct2/ride/TrackPieceSelection.h>
#include <openrct2/ride/ted/TrackElemType.h>
#include <openrct2/ride/ted/TrackElementDescriptor.h>

using OpenRCT2::GetNextValidTrackSegments;
using OpenRCT2::TrackElemType;
using OpenRCT2::TrackMetadata::GetTrackElementDescriptor;
using OpenRCT2::TrackMetadata::TrackRoll;

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

    // Mirror of TrackPieceSelection.cpp's internal normalizePostPieceRoll. Used by the symmetry
    // invariant test so it matches GetNextValidTrackSegments's row-matching contract.
    TrackRoll normalizePostPieceRoll(const RideTypeDescriptor& rtd, TrackRoll endRoll)
    {
        if (rtd.flags.has(RtdFlag::hasInvertedVariant) && endRoll == TrackRoll::upsideDown)
            return TrackRoll::none;
        return endRoll;
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
    // multiDimUp90ToInvertedFlatQuarterLoop ends in inverted state (TED.rollEnd=upsideDown).
    // multiDimInvertedFlatToDown90QuarterLoop has rollStart=none and lives in
    // TrackGroup::quarterLoopInvertedDown, which Multi-Dim enables only in its
    // InvertedTrackPaintFunctions drawer.
    //
    // For inverted-variant rides, normalizePostPieceRoll collapses
    // upsideDown→none ONCE (mirroring RideConstruction.cpp:636-645's stored-bank update), so
    // both variant scans match against rollStart=none. The regular-variant scan rejects this
    // candidate because Multi-Dim's regular drawer doesn't enable quarterLoopInvertedDown;
    // the inverted-variant scan accepts it because the inverted drawer does. The candidate's
    // presence proves the inverted-variant Pass B path (normalized roll + inverted-drawer
    // group lookup) reaches it.
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

// -----------------------------------------------------------------------------
// Inverted-variant continuation when the source's TED.rollEnd is upsideDown
// (construction code flips _currentTrackAlternative.inverted; continuations
// are gated against the OPPOSITE drawer than the one that built the source)
// -----------------------------------------------------------------------------

TEST(
    TrackPieceSelectionTest,
    FlyingRollerCoaster_MultiDimInvertedFlatToDown90QuarterLoop_EmitsVerticalContinuationsViaRegularDrawer)
{
    // multiDimInvertedFlatToDown90QuarterLoop has TED.rollEnd=upsideDown and group
    // quarterLoopInvertedDown (enabled in Flying RC's INVERTED drawer). Per
    // RideConstruction.cpp:636-645, placing it on a hasInvertedVariant ride normalizes the
    // stored bank to none AND toggles the alt-inverted flag. So continuations are gated
    // against the REGULAR drawer at the post-piece state (down90, none, ortho). Flying RC's
    // regular drawer enables slopeVertical, so down90 and down90ToDown60 are offered by the
    // construction UI — and must be by this function too. The inverted-drawer scan rejects
    // these (no slopeVertical there); the test proves the regular-drawer scan reaches them.
    auto pieces = GetNextValidTrackSegments(
        rtdOf(RIDE_TYPE_FLYING_ROLLER_COASTER), TrackElemType::multiDimInvertedFlatToDown90QuarterLoop);
    EXPECT_TRUE(contains(pieces, TrackElemType::down90));
    EXPECT_TRUE(contains(pieces, TrackElemType::down90ToDown60));
}

// -----------------------------------------------------------------------------
// Bank gate for flat-banked CURVES — destination's TED.group is uniformly
// TrackGroup::flat (which rides don't enable directly); the gate must use
// flatRollBanking + the curve-class group instead.
// -----------------------------------------------------------------------------

TEST(TrackPieceSelectionTest, LoopingRollerCoaster_LeftEighthBankToDiag_EmitsDiagonalBankedContinuations)
{
    // leftEighthBankToDiag ends at (diag, none, left). Five table rows match the start-state:
    // diagLeftBank, diagLeftBankToFlat, diagLeftBankToDown25, diagLeftBankToUp25,
    // leftEighthBankToOrthogonal — all with definition.group=flat (TrackData.cpp:8178, 8449,
    // 8475, 8531, 8587). Looping RC enables curveLarge + flatRollBanking, which is the
    // correct gate for these flat-banked-curve continuations. The fix forces the bank gate's
    // else branch to consult flatRollBanking (not destTed.group, which would be `flat` and
    // fail enabled()).
    auto pieces = GetNextValidTrackSegments(rtdOf(RIDE_TYPE_LOOPING_ROLLER_COASTER), TrackElemType::leftEighthBankToDiag);
    EXPECT_TRUE(contains(pieces, TrackElemType::diagLeftBank));
    EXPECT_TRUE(contains(pieces, TrackElemType::diagLeftBankToFlat));
    EXPECT_TRUE(contains(pieces, TrackElemType::leftEighthBankToOrthogonal));
}

// -----------------------------------------------------------------------------
// Diagonal+slope+bank restriction must consult slopeCurveLargeBanked (not the
// unbanked slopeCurveLarge) — banked sloped diagonal continuations belong to a
// distinct group from unbanked ones, and rides may enable one without the other.
// -----------------------------------------------------------------------------

TEST(TrackPieceSelectionTest, LSMLaunchedRollerCoaster_LeftEighthBankToDiagUp25_EmitsSlopedDiagonalBankedContinuations)
{
    // leftEighthBankToDiagUp25 ends at (diag, up25, left). Table rows producing
    // diagLeftBankedUp25ToUp25, diagLeftBankedUp25ToLeftBankedFlat, and diagLeftBankedUp25ToFlat
    // all match the source's end-state. The source piece is in TrackGroup::slopeCurveLargeBanked,
    // so it's only buildable on rides that enable that group — LSM Launched RC is the canonical
    // example. The diagonal restriction was gating these on slopeCurveLarge (unbanked); the fix
    // changes it to slopeCurveLargeBanked, the correct banked-curve-on-slope group.
    auto pieces = GetNextValidTrackSegments(
        rtdOf(RIDE_TYPE_LSM_LAUNCHED_ROLLER_COASTER), TrackElemType::leftEighthBankToDiagUp25);
    EXPECT_TRUE(contains(pieces, TrackElemType::diagLeftBankedUp25ToUp25));
    EXPECT_TRUE(contains(pieces, TrackElemType::diagLeftBankedUp25ToLeftBankedFlat));
    EXPECT_TRUE(contains(pieces, TrackElemType::diagLeftBankedUp25ToFlat));
}

TEST(TrackPieceSelectionTest, LSMLaunchedRollerCoaster_RightEighthBankToDiagDown25_EmitsSlopedDiagonalBankedContinuations)
{
    // Right/down mirror of the Bug-C regression: the maintainer explicitly called out both
    // left/right and upward/downward cases. rightEighthBankToDiagDown25 ends at (diag, down25,
    // right). Table rows producing diagRightBankedDown25ToDown25,
    // diagRightBankedDown25ToRightBankedFlat, and diagRightBankedDown25ToFlat match. Same
    // gating as the left/up case but exercises the mirror-image table entries and the
    // down-slope branches of the predicate.
    auto pieces = GetNextValidTrackSegments(
        rtdOf(RIDE_TYPE_LSM_LAUNCHED_ROLLER_COASTER), TrackElemType::rightEighthBankToDiagDown25);
    EXPECT_TRUE(contains(pieces, TrackElemType::diagRightBankedDown25ToDown25));
    EXPECT_TRUE(contains(pieces, TrackElemType::diagRightBankedDown25ToRightBankedFlat));
    EXPECT_TRUE(contains(pieces, TrackElemType::diagRightBankedDown25ToFlat));
}

TEST(TrackPieceSelectionTest, GigaCoaster_LeftEighthBankToDiag_RestrictsBankedSlopeTransitionsWithoutSlopeCurveLargeBanked)
{
    // Negative companion proving the Bug-C distinction between slopeCurveLarge and
    // slopeCurveLargeBanked actually matters. Giga enables slopeCurveLarge but NOT
    // slopeCurveLargeBanked. Under the OLD diag restriction (slopeCurveLarge-gated),
    // banked-slope-transition continuations from a diagonal source would have passed on Giga
    // (since slopeCurveLarge IS enabled, restriction didn't fire). The NEW gate
    // (slopeCurveLargeBanked-gated) correctly restricts them on rides without the banked
    // counterpart.
    //
    // The positive anchors prove the source is processed (we're not testing an empty result
    // by accident); the negative assertions prove the new gate is appropriately strict.
    auto pieces = GetNextValidTrackSegments(rtdOf(RIDE_TYPE_GIGA_COASTER), TrackElemType::leftEighthBankToDiag);
    // Flat-banked continuations don't need slopeCurveLargeBanked (no slope transition).
    EXPECT_TRUE(contains(pieces, TrackElemType::diagLeftBank));
    EXPECT_TRUE(contains(pieces, TrackElemType::diagLeftBankToFlat));
    EXPECT_TRUE(contains(pieces, TrackElemType::leftEighthBankToOrthogonal));
    // These combine bank transition with non-flat pitch end on a diagonal — the diag
    // restriction blocks them without slopeCurveLargeBanked.
    EXPECT_FALSE(contains(pieces, TrackElemType::diagLeftBankToUp25));
    EXPECT_FALSE(contains(pieces, TrackElemType::diagLeftBankToDown25));
}

// -----------------------------------------------------------------------------
// Symmetry invariant: every emitted continuation's start-state must geometrically
// match the source's end-state (modulo bank-flip normalization for inverted-variant
// rides). Catches future drift where a row addition uses mismatched geometry, or a
// predicate change inadvertently emits pieces that don't connect. Each row asserts
// non-emptiness so the invariant alone can't paper over an under-emission regression.
// -----------------------------------------------------------------------------

TEST(TrackPieceSelectionTest, SymmetryInvariant_EmittedContinuationsConnectGeometricallyToSource)
{
    // Source × ride matrix covering each rule family our pipeline encodes. Each pair is
    // (ride, source). Choices favor sources with rich, varied successor sets so the invariant
    // is exercised against many candidates, not just one trivial case.
    struct Case
    {
        ride_type_t ride;
        TrackElemType source;
    };
    const Case cases[] = {
        // hasInvertedVariant rides + sources whose rollEnd triggers the bank-flip normalization
        { RIDE_TYPE_FLYING_ROLLER_COASTER, TrackElemType::multiDimInvertedFlatToDown90QuarterLoop },
        { RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER, TrackElemType::multiDimUp90ToInvertedFlatQuarterLoop },
        // Banked diagonal sources (Bug B / C surface area)
        { RIDE_TYPE_LOOPING_ROLLER_COASTER, TrackElemType::leftEighthBankToDiag },
        { RIDE_TYPE_LOOPING_ROLLER_COASTER, TrackElemType::rightEighthBankToDiag },
        { RIDE_TYPE_LSM_LAUNCHED_ROLLER_COASTER, TrackElemType::leftEighthBankToDiagUp25 },
        { RIDE_TYPE_LSM_LAUNCHED_ROLLER_COASTER, TrackElemType::leftEighthBankToDiagDown25 },
        // Diagonal sloped + non-banked
        { RIDE_TYPE_GIGA_COASTER, TrackElemType::diagFlat },
        { RIDE_TYPE_GIGA_COASTER, TrackElemType::diagUp25 },
        // Covered-variant sources
        { RIDE_TYPE_WATER_COASTER, TrackElemType::flat },
        // Vertical sources
        { RIDE_TYPE_INVERTED_IMPULSE_COASTER, TrackElemType::up90 },
        // Plain orthogonal flat sources across multiple ride families
        { RIDE_TYPE_WOODEN_ROLLER_COASTER, TrackElemType::flat },
        { RIDE_TYPE_MINIATURE_RAILWAY, TrackElemType::leftEighthToDiag },
    };

    for (const auto& c : cases)
    {
        const auto& rtd = rtdOf(c.ride);
        const auto& srcTed = GetTrackElementDescriptor(c.source);
        const auto srcEndPitch = srcTed.definition.pitchEnd;
        const auto srcEndRoll = normalizePostPieceRoll(rtd, srcTed.definition.rollEnd);
        const bool srcEndIsDiag = TrackPieceDirectionIsDiagonal(srcTed.coordinates.rotationEnd);

        const auto pieces = GetNextValidTrackSegments(rtd, c.source);

        // Every matrix row is curated to produce at least one continuation on the chosen ride;
        // an empty result is itself a regression (the symmetry checks below would otherwise
        // silently pass on empty input).
        EXPECT_FALSE(pieces.empty()) << "Source " << EnumValue(c.source) << " on ride " << static_cast<int>(c.ride)
                                     << " unexpectedly produced zero continuations";

        for (auto continuation : pieces)
        {
            const auto& cTed = GetTrackElementDescriptor(continuation);

            EXPECT_EQ(cTed.definition.pitchStart, srcEndPitch)
                << "Continuation " << EnumValue(continuation) << " from source " << EnumValue(c.source) << " on ride "
                << static_cast<int>(c.ride) << ": pitchStart mismatch";
            EXPECT_EQ(cTed.definition.rollStart, srcEndRoll)
                << "Continuation " << EnumValue(continuation) << " from source " << EnumValue(c.source) << " on ride "
                << static_cast<int>(c.ride) << ": rollStart mismatch";
            EXPECT_EQ(TrackPieceDirectionIsDiagonal(cTed.coordinates.rotationBegin), srcEndIsDiag)
                << "Continuation " << EnumValue(continuation) << " from source " << EnumValue(c.source) << " on ride "
                << static_cast<int>(c.ride) << ": rotationBegin diagonality mismatch";
        }
    }
}
