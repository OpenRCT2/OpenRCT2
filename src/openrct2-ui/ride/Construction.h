/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
    TrackElemType::EndStation,
    TrackElemType::SBendLeft,
    TrackElemType::SBendRight,
    TrackElemType::LeftVerticalLoop,
    TrackElemType::RightVerticalLoop,
    TrackElemType::LeftTwistDownToUp,
    TrackElemType::RightTwistDownToUp,
    TrackElemType::LeftTwistUpToDown,
    TrackElemType::RightTwistUpToDown,
    TrackElemType::HalfLoopUp,
    TrackElemType::HalfLoopDown,
    TrackElemType::LeftCorkscrewUp,
    TrackElemType::RightCorkscrewUp,
    TrackElemType::LeftCorkscrewDown,
    TrackElemType::RightCorkscrewDown,
    TrackElemType::TowerBase,
    TrackElemType::TowerSection,
    TrackElemType::LeftHalfBankedHelixUpSmall,
    TrackElemType::RightHalfBankedHelixUpSmall,
    TrackElemType::LeftHalfBankedHelixDownSmall,
    TrackElemType::RightHalfBankedHelixDownSmall,
    TrackElemType::LeftHalfBankedHelixUpLarge,
    TrackElemType::RightHalfBankedHelixUpLarge,
    TrackElemType::LeftHalfBankedHelixDownLarge,
    TrackElemType::RightHalfBankedHelixDownLarge,
    TrackElemType::Booster,
    TrackElemType::Brakes,
    TrackElemType::LeftQuarterBankedHelixLargeUp,
    TrackElemType::RightQuarterBankedHelixLargeUp,
    TrackElemType::LeftQuarterBankedHelixLargeDown,
    TrackElemType::RightQuarterBankedHelixLargeDown,
    TrackElemType::LeftQuarterHelixLargeUp,
    TrackElemType::RightQuarterHelixLargeUp,
    TrackElemType::LeftQuarterHelixLargeDown,
    TrackElemType::RightQuarterHelixLargeDown,
    TrackElemType::Waterfall,
    TrackElemType::Rapids,
    TrackElemType::Watersplash,
    TrackElemType::Whirlpool,
    TrackElemType::CableLiftHill,
    TrackElemType::ReverseFreefallSlope,
    TrackElemType::ReverseFreefallVertical,
    TrackElemType::BrakeForDrop,
    TrackElemType::LogFlumeReverser,
    TrackElemType::SpinningTunnel,
    TrackElemType::LeftBarrelRollUpToDown,
    TrackElemType::RightBarrelRollUpToDown,
    TrackElemType::LeftBarrelRollDownToUp,
    TrackElemType::RightBarrelRollDownToUp,
    TrackElemType::PoweredLift,
    TrackElemType::LeftLargeHalfLoopUp,
    TrackElemType::RightLargeHalfLoopUp,
    TrackElemType::LeftLargeHalfLoopDown,
    TrackElemType::RightLargeHalfLoopDown,
    TrackElemType::LeftFlyerTwistUp,
    TrackElemType::RightFlyerTwistUp,
    TrackElemType::LeftFlyerTwistDown,
    TrackElemType::RightFlyerTwistDown,
    TrackElemType::FlyerHalfLoopUninvertedUp,
    TrackElemType::FlyerHalfLoopInvertedDown,
    TrackElemType::LeftFlyerCorkscrewUp,
    TrackElemType::RightFlyerCorkscrewUp,
    TrackElemType::LeftFlyerCorkscrewDown,
    TrackElemType::RightFlyerCorkscrewDown,
    TrackElemType::HeartLineTransferUp,
    TrackElemType::HeartLineTransferDown,
    TrackElemType::LeftHeartLineRoll,
    TrackElemType::RightHeartLineRoll,
    TrackElemType::MinigolfHoleA,
    TrackElemType::MinigolfHoleB,
    TrackElemType::MinigolfHoleC,
    TrackElemType::MinigolfHoleD,
    TrackElemType::MinigolfHoleE,
    TrackElemType::MultiDimInvertedFlatToDown90QuarterLoop,
    TrackElemType::Up90ToInvertedFlatQuarterLoop,
    TrackElemType::InvertedFlatToDown90QuarterLoop,
    TrackElemType::LeftCurvedLiftHill,
    TrackElemType::RightCurvedLiftHill,
    TrackElemType::LeftReverser,
    TrackElemType::RightReverser,
    TrackElemType::AirThrustTopCap,
    TrackElemType::AirThrustVerticalDown,
    TrackElemType::AirThrustVerticalDownToLevel,
    TrackElemType::BlockBrakes,
    TrackElemType::MultiDimUp90ToInvertedFlatQuarterLoop,
    TrackElemType::MultiDimFlatToDown90QuarterLoop,
    TrackElemType::MultiDimInvertedUp90ToFlatQuarterLoop,
    TrackElemType::RotationControlToggle,
    TrackElemType::LeftLargeCorkscrewUp,
    TrackElemType::RightLargeCorkscrewUp,
    TrackElemType::LeftLargeCorkscrewDown,
    TrackElemType::RightLargeCorkscrewDown,
    TrackElemType::LeftMediumHalfLoopUp,
    TrackElemType::RightMediumHalfLoopUp,
    TrackElemType::LeftMediumHalfLoopDown,
    TrackElemType::RightMediumHalfLoopDown,
    TrackElemType::LeftZeroGRollUp,
    TrackElemType::RightZeroGRollUp,
    TrackElemType::LeftZeroGRollDown,
    TrackElemType::RightZeroGRollDown,
    TrackElemType::LeftLargeZeroGRollUp,
    TrackElemType::RightLargeZeroGRollUp,
    TrackElemType::LeftLargeZeroGRollDown,
    TrackElemType::RightLargeZeroGRollDown,
    TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp,
    TrackElemType::RightFlyerLargeHalfLoopUninvertedUp,
    TrackElemType::LeftFlyerLargeHalfLoopInvertedDown,
    TrackElemType::RightFlyerLargeHalfLoopInvertedDown,
    TrackElemType::LeftFlyerLargeHalfLoopInvertedUp,
    TrackElemType::RightFlyerLargeHalfLoopInvertedUp,
    TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown,
    TrackElemType::RightFlyerLargeHalfLoopUninvertedDown,
    TrackElemType::FlyerHalfLoopInvertedUp,
    TrackElemType::FlyerHalfLoopUninvertedDown,
};
constexpr const size_t DropdownLength = DropdownOrder.size();

// Update the magic number with the current number of track elements to silence
static_assert(TrackElemType::Count == 337, "Reminder to add new track element to special dropdown list");

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
    const Ride& currentRide, uint8_t buildDirection, uint8_t buildSlope, uint8_t buildBank, RideConstructionState state);

void RideConstructNew(RideSelection listItem);
