/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

namespace OpenRCT2
{
    // Vehicle sprite groups required by track groups are defined in RideEntryGetSupportedTrackPieces()
    enum class TrackGroup : uint8_t
    {
        flat = 0,
        straight,
        stationEnd,
        liftHill,
        liftHillSteep,
        liftHillCurve,
        flatRollBanking,
        verticalLoop,
        slope,
        slopeSteepDown,
        flatToSteepSlope,
        slopeCurve,
        slopeCurveSteep,
        sBend,
        curveVerySmall,
        curveSmall,
        curve,
        curveLarge,
        twist,
        halfLoop,
        corkscrew,
        tower,
        helixUpBankedHalf,
        helixDownBankedHalf,
        helixUpBankedQuarter,
        helixDownBankedQuarter,
        helixUpUnbankedQuarter,
        helixDownUnbankedQuarter,
        brakes,
        onridePhoto,
        waterSplash,
        slopeVertical,
        barrelRoll,
        poweredLift,
        halfLoopLarge,
        slopeCurveBanked,
        logFlumeReverser,
        heartlineRoll,
        reverser,
        reverseFreefall,
        slopeToFlat,
        blockBrakes,
        slopeRollBanking,
        slopeSteepLong,
        curveVertical,
        liftHillCable,
        liftHillCurved,
        quarterLoop,
        spinningTunnel,
        booster,
        inlineTwistUninverted,
        inlineTwistInverted,
        quarterLoopUninvertedUp,
        quarterLoopUninvertedDown,
        quarterLoopInvertedUp,
        quarterLoopInvertedDown,
        rapids,
        flyingHalfLoopUninvertedUp,
        flyingHalfLoopInvertedDown,

        flatRideBase,

        waterfall,
        whirlpool,
        brakeForDrop,
        corkscrewUninverted,
        corkscrewInverted,
        heartlineTransfer,
        miniGolfHole,
        rotationControlToggle,
        slopeSteepUp,

        corkscrewLarge,
        halfLoopMedium,
        zeroGRoll,
        zeroGRollLarge,

        flyingLargeHalfLoopUninvertedUp,
        flyingLargeHalfLoopInvertedDown,
        flyingLargeHalfLoopUninvertedDown,
        flyingLargeHalfLoopInvertedUp,

        flyingHalfLoopUninvertedDown,
        flyingHalfLoopInvertedUp,

        slopeCurveLarge,
        slopeCurveLargeBanked,

        diagBrakes,
        diagBlockBrakes,
        inclinedBrakes,
        diagBooster,

        diagSlopeSteepLong,
        diveLoop,

        diagSlope,
        diagSlopeSteepUp,
        diagSlopeSteepDown,

        count,
    };
} // namespace OpenRCT2
