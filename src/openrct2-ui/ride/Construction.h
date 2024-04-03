/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/ride/RideTypes.h>
#include <openrct2/ride/Track.h>

enum class RideConstructionState : uint8_t;

/**
 * Order of special track elements dropdown. Elements with the same name string must be sequential or they show up twice.
 */
constexpr std::array DropdownOrder = {
    TrackElemType::kEndStation,
    TrackElemType::kSBendLeft,
    TrackElemType::kSBendRight,
    TrackElemType::kLeftVerticalLoop,
    TrackElemType::kRightVerticalLoop,
    TrackElemType::kLeftTwistDownToUp,
    TrackElemType::kLeftTwistUpToDown,
    TrackElemType::kRightTwistDownToUp,
    TrackElemType::kRightTwistUpToDown,
    TrackElemType::kHalfLoopUp,
    TrackElemType::kHalfLoopDown,
    TrackElemType::kLeftCorkscrewUp,
    TrackElemType::kLeftCorkscrewDown,
    TrackElemType::kRightCorkscrewUp,
    TrackElemType::kRightCorkscrewDown,
    TrackElemType::kTowerBase,
    TrackElemType::kTowerSection,
    TrackElemType::kLeftHalfBankedHelixUpSmall,
    TrackElemType::kRightHalfBankedHelixUpSmall,
    TrackElemType::kLeftHalfBankedHelixDownSmall,
    TrackElemType::kRightHalfBankedHelixDownSmall,
    TrackElemType::kLeftHalfBankedHelixUpLarge,
    TrackElemType::kRightHalfBankedHelixUpLarge,
    TrackElemType::kLeftHalfBankedHelixDownLarge,
    TrackElemType::kRightHalfBankedHelixDownLarge,
    TrackElemType::kBooster,
    TrackElemType::kBrakes,
    TrackElemType::kDiagBrakes,
    TrackElemType::kOnRidePhoto,
    TrackElemType::kLeftQuarterBankedHelixLargeUp,
    TrackElemType::kRightQuarterBankedHelixLargeUp,
    TrackElemType::kLeftQuarterBankedHelixLargeDown,
    TrackElemType::kRightQuarterBankedHelixLargeDown,
    TrackElemType::kLeftQuarterHelixLargeUp,
    TrackElemType::kRightQuarterHelixLargeUp,
    TrackElemType::kLeftQuarterHelixLargeDown,
    TrackElemType::kRightQuarterHelixLargeDown,
    TrackElemType::kWaterfall,
    TrackElemType::kRapids,
    TrackElemType::kWatersplash,
    TrackElemType::kWhirlpool,
    TrackElemType::kCableLiftHill,
    TrackElemType::kReverseFreefallSlope,
    TrackElemType::kReverseFreefallVertical,
    TrackElemType::kBrakeForDrop,
    TrackElemType::kLogFlumeReverser,
    TrackElemType::kSpinningTunnel,
    TrackElemType::kLeftBarrelRollUpToDown,
    TrackElemType::kLeftBarrelRollDownToUp,
    TrackElemType::kRightBarrelRollUpToDown,
    TrackElemType::kRightBarrelRollDownToUp,
    TrackElemType::kPoweredLift,
    TrackElemType::kLeftLargeHalfLoopUp,
    TrackElemType::kLeftLargeHalfLoopDown,
    TrackElemType::kRightLargeHalfLoopUp,
    TrackElemType::kRightLargeHalfLoopDown,
    TrackElemType::kLeftFlyerTwistUp,
    TrackElemType::kLeftFlyerTwistDown,
    TrackElemType::kRightFlyerTwistUp,
    TrackElemType::kRightFlyerTwistDown,
    TrackElemType::kFlyerHalfLoopUninvertedUp,
    TrackElemType::kFlyerHalfLoopInvertedDown,
    TrackElemType::kFlyerHalfLoopInvertedUp,
    TrackElemType::kFlyerHalfLoopUninvertedDown,
    TrackElemType::kLeftFlyerCorkscrewUp,
    TrackElemType::kLeftFlyerCorkscrewDown,
    TrackElemType::kRightFlyerCorkscrewUp,
    TrackElemType::kRightFlyerCorkscrewDown,
    TrackElemType::kHeartLineTransferUp,
    TrackElemType::kHeartLineTransferDown,
    TrackElemType::kLeftHeartLineRoll,
    TrackElemType::kRightHeartLineRoll,
    TrackElemType::kMinigolfHoleA,
    TrackElemType::kMinigolfHoleB,
    TrackElemType::kMinigolfHoleC,
    TrackElemType::kMinigolfHoleD,
    TrackElemType::kMinigolfHoleE,
    TrackElemType::kMultiDimInvertedFlatToDown90QuarterLoop,
    TrackElemType::kUp90ToInvertedFlatQuarterLoop,
    TrackElemType::kInvertedFlatToDown90QuarterLoop,
    TrackElemType::kMultiDimUp90ToInvertedFlatQuarterLoop,
    TrackElemType::kMultiDimFlatToDown90QuarterLoop,
    TrackElemType::kMultiDimInvertedUp90ToFlatQuarterLoop,
    TrackElemType::kLeftCurvedLiftHill,
    TrackElemType::kRightCurvedLiftHill,
    TrackElemType::kLeftReverser,
    TrackElemType::kRightReverser,
    TrackElemType::kAirThrustTopCap,
    TrackElemType::kAirThrustVerticalDown,
    TrackElemType::kAirThrustVerticalDownToLevel,
    TrackElemType::kBlockBrakes,
    TrackElemType::kDiagBlockBrakes,
    TrackElemType::kRotationControlToggle,
    TrackElemType::kLeftLargeCorkscrewUp,
    TrackElemType::kLeftLargeCorkscrewDown,
    TrackElemType::kRightLargeCorkscrewUp,
    TrackElemType::kRightLargeCorkscrewDown,
    TrackElemType::kLeftMediumHalfLoopUp,
    TrackElemType::kLeftMediumHalfLoopDown,
    TrackElemType::kRightMediumHalfLoopUp,
    TrackElemType::kRightMediumHalfLoopDown,
    TrackElemType::kLeftZeroGRollUp,
    TrackElemType::kLeftZeroGRollDown,
    TrackElemType::kRightZeroGRollUp,
    TrackElemType::kRightZeroGRollDown,
    TrackElemType::kLeftLargeZeroGRollUp,
    TrackElemType::kLeftLargeZeroGRollDown,
    TrackElemType::kRightLargeZeroGRollUp,
    TrackElemType::kRightLargeZeroGRollDown,
    TrackElemType::kLeftFlyerLargeHalfLoopUninvertedUp,
    TrackElemType::kLeftFlyerLargeHalfLoopInvertedDown,
    TrackElemType::kLeftFlyerLargeHalfLoopInvertedUp,
    TrackElemType::kLeftFlyerLargeHalfLoopUninvertedDown,
    TrackElemType::kRightFlyerLargeHalfLoopUninvertedUp,
    TrackElemType::kRightFlyerLargeHalfLoopInvertedDown,
    TrackElemType::kRightFlyerLargeHalfLoopInvertedUp,
    TrackElemType::kRightFlyerLargeHalfLoopUninvertedDown,
};
constexpr size_t DropdownLength = DropdownOrder.size();

// Update the magic number with the current number of track elements to silence
static_assert(TrackElemType::kCount == 339, "Reminder to add new track element to special dropdown list");

constexpr bool TrackPieceDirectionIsDiagonal(const uint8_t direction)
{
    return direction >= NumOrthogonalDirections;
}

struct SpecialElement
{
    track_type_t TrackType;
    bool Disabled;
};

struct SpecialElementsDropdownState
{
    bool HasActiveElements = false;
    std::vector<SpecialElement> Elements{};
};

SpecialElementsDropdownState BuildSpecialElementsList(
    const Ride& currentRide, uint8_t buildDirection, TrackPitch buildSlope, TrackRoll buildBank, RideConstructionState state);

void RideConstructNew(RideSelection listItem);
