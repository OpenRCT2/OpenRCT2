/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "VehicleSubpositionData.h"

#include "Vehicle.h"
#include "subposition/SubpositionChairliftBullwheels.hpp"
#include "subposition/SubpositionChairliftGoingBack.hpp"
#include "subposition/SubpositionChairliftGoingOut.hpp"
#include "subposition/SubpositionDefault.hpp"
#include "subposition/SubpositionGoKartsLeftLane.hpp"
#include "subposition/SubpositionGoKartsMovingToLeftLane.hpp"
#include "subposition/SubpositionGoKartsMovingToRightLane.hpp"
#include "subposition/SubpositionGoKartsRightLane.hpp"
#include "subposition/SubpositionMinigolf.hpp"
#include "subposition/SubpositionReverserCoaster.hpp"

namespace OpenRCT2::Subposition
{
    // rct2: 0x008B8F30
    constexpr const VehicleInfoList* const* gTrackVehicleInfo[EnumValue(VehicleTrackSubposition::Count)] = {
        Default::TrackVehicleInfoListDefault,                             // VehicleTrackSubposition::Default
        ChairliftGoingOut::TrackVehicleInfoListChairliftGoingOut,         // VehicleTrackSubposition::ChairliftGoingOut
        ChairliftGoingBack::TrackVehicleInfoListChairliftGoingBack,       // VehicleTrackSubposition::ChairliftGoingBack
        ChairliftBullwheels::TrackVehicleInfoListChairliftEndBullwheel,   // VehicleTrackSubposition::ChairliftEndBullwheel
        ChairliftBullwheels::TrackVehicleInfoListChairliftStartBullwheel, // VehicleTrackSubposition::ChairliftStartBullwheel
        GoKartsLeftLane::TrackVehicleInfoListGoKartsLeftLane,             // VehicleTrackSubposition::GoKartsLeftLane
        GoKartsRightLane::TrackVehicleInfoListGoKartsRightLane,           // VehicleTrackSubposition::GoKartsRightLane
        GoKartsMovingToRightLane::
            TrackVehicleInfoListGoKartsMovingToRightLane, // VehicleTrackSubposition::GoKartsMovingToRightLane
        GoKartsMovingToLeftLane::
            TrackVehicleInfoListGoKartsMovingToLeftLane, // VehicleTrackSubposition::GoKartsMovingToLeftLane
        Minigolf::PlayerPathA::PlayerPathA, // VehicleTrackSubposition::MiniGolfStart9, VehicleTrackSubposition::MiniGolfPathA9
        Minigolf::BallPathA::BallPathA,     // VehicleTrackSubposition::MiniGolfBallPathA10
        Minigolf::PlayerPathB::PlayerPathB, // VehicleTrackSubposition::MiniGolfPathB11
        Minigolf::BallPathB::BallPathB,     // VehicleTrackSubposition::MiniGolfBallPathB12
        Minigolf::PlayerPathC::PlayerPathC, // VehicleTrackSubposition::MiniGolfPathC13
        Minigolf::BallPathC::BallPathC,     // VehicleTrackSubposition::MiniGolfBallPathC14
        ReverserCoaster::FrontBogie::TrackVehicleInfoListReverserRCFrontBogie, // VehicleTrackSubposition::ReverserRCFrontBogie
        ReverserCoaster::RearBogie::TrackVehicleInfoListReverserRCRearBogie,   // VehicleTrackSubposition::ReverserRCRearBogie
    };
} // namespace OpenRCT2::Subposition
