/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/core/EnumUtils.hpp>
#include <openrct2/ride/RideTypes.h>
#include <openrct2/ride/Track.h>

namespace OpenRCT2
{
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
        TrackElemType::LeftTwistUpToDown,
        TrackElemType::RightTwistDownToUp,
        TrackElemType::RightTwistUpToDown,
        TrackElemType::HalfLoopUp,
        TrackElemType::HalfLoopDown,
        TrackElemType::LeftCorkscrewUp,
        TrackElemType::LeftCorkscrewDown,
        TrackElemType::RightCorkscrewUp,
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
        TrackElemType::DiagBooster,
        TrackElemType::Brakes,
        TrackElemType::DiagBrakes,
        TrackElemType::Down25Brakes,
        TrackElemType::OnRidePhoto,
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
        TrackElemType::LeftBarrelRollDownToUp,
        TrackElemType::RightBarrelRollUpToDown,
        TrackElemType::RightBarrelRollDownToUp,
        TrackElemType::PoweredLift,
        TrackElemType::LeftLargeHalfLoopUp,
        TrackElemType::LeftLargeHalfLoopDown,
        TrackElemType::RightLargeHalfLoopUp,
        TrackElemType::RightLargeHalfLoopDown,
        TrackElemType::LeftFlyerTwistUp,
        TrackElemType::LeftFlyerTwistDown,
        TrackElemType::RightFlyerTwistUp,
        TrackElemType::RightFlyerTwistDown,
        TrackElemType::FlyerHalfLoopUninvertedUp,
        TrackElemType::FlyerHalfLoopInvertedDown,
        TrackElemType::FlyerHalfLoopInvertedUp,
        TrackElemType::FlyerHalfLoopUninvertedDown,
        TrackElemType::LeftFlyerCorkscrewUp,
        TrackElemType::LeftFlyerCorkscrewDown,
        TrackElemType::RightFlyerCorkscrewUp,
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
        TrackElemType::MultiDimUp90ToInvertedFlatQuarterLoop,
        TrackElemType::MultiDimFlatToDown90QuarterLoop,
        TrackElemType::MultiDimInvertedUp90ToFlatQuarterLoop,
        TrackElemType::LeftCurvedLiftHill,
        TrackElemType::RightCurvedLiftHill,
        TrackElemType::LeftReverser,
        TrackElemType::RightReverser,
        TrackElemType::AirThrustTopCap,
        TrackElemType::AirThrustVerticalDown,
        TrackElemType::AirThrustVerticalDownToLevel,
        TrackElemType::BlockBrakes,
        TrackElemType::DiagBlockBrakes,
        TrackElemType::RotationControlToggle,
        TrackElemType::LeftLargeCorkscrewUp,
        TrackElemType::LeftLargeCorkscrewDown,
        TrackElemType::RightLargeCorkscrewUp,
        TrackElemType::RightLargeCorkscrewDown,
        TrackElemType::LeftMediumHalfLoopUp,
        TrackElemType::LeftMediumHalfLoopDown,
        TrackElemType::RightMediumHalfLoopUp,
        TrackElemType::RightMediumHalfLoopDown,
        TrackElemType::LeftZeroGRollUp,
        TrackElemType::LeftZeroGRollDown,
        TrackElemType::RightZeroGRollUp,
        TrackElemType::RightZeroGRollDown,
        TrackElemType::LeftLargeZeroGRollUp,
        TrackElemType::LeftLargeZeroGRollDown,
        TrackElemType::RightLargeZeroGRollUp,
        TrackElemType::RightLargeZeroGRollDown,
        TrackElemType::LeftEighthDiveLoopUpToOrthogonal,
        TrackElemType::LeftEighthDiveLoopDownToDiag,
        TrackElemType::RightEighthDiveLoopUpToOrthogonal,
        TrackElemType::RightEighthDiveLoopDownToDiag,
        TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp,
        TrackElemType::LeftFlyerLargeHalfLoopInvertedDown,
        TrackElemType::LeftFlyerLargeHalfLoopInvertedUp,
        TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown,
        TrackElemType::RightFlyerLargeHalfLoopUninvertedUp,
        TrackElemType::RightFlyerLargeHalfLoopInvertedDown,
        TrackElemType::RightFlyerLargeHalfLoopInvertedUp,
        TrackElemType::RightFlyerLargeHalfLoopUninvertedDown,
    };
    constexpr size_t DropdownLength = DropdownOrder.size();

    // Update the magic number with the current number of track elements to silence
    static_assert(EnumValue(TrackElemType::Count) == 349, "Reminder to add new track element to special dropdown list");

    constexpr bool TrackPieceDirectionIsDiagonal(const uint8_t direction)
    {
        return direction >= kNumOrthogonalDirections;
    }

    struct SpecialElement
    {
        OpenRCT2::TrackElemType TrackType;
        bool Disabled;
    };

    struct SpecialElementsDropdownState
    {
        bool HasActiveElements = false;
        std::vector<SpecialElement> Elements{};
    };

    SpecialElementsDropdownState BuildSpecialElementsList(
        const Ride& currentRide, uint8_t buildDirection, TrackPitch buildSlope, TrackRoll buildBank,
        RideConstructionState state);

    void RideConstructNew(RideSelection listItem);
    CoordsXYZD RideGetEntranceOrExitPositionFromScreenPosition(const ScreenCoordsXY& screenCoords);
} // namespace OpenRCT2
