/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GameAction.h"
#include "GuestSetNameAction.hpp"
#include "ParkSetLoanAction.hpp"
#include "ParkSetResearchFundingAction.hpp"
#include "ParkMarketingAction.hpp"
#include "PlaceParkEntranceAction.hpp"
#include "SetParkEntranceFeeAction.hpp"
#include "StaffSetColourAction.hpp"
#include "StaffSetNameAction.hpp"
#include "RideCreateAction.hpp"
#include "RideSetStatus.hpp"
#include "RideSetName.hpp"
#include "RideDemolishAction.hpp"
#include "PlacePeepSpawnAction.hpp"
#include "MazeSetTrackAction.hpp"
#include "SignSetNameAction.hpp"
#include "ParkSetNameAction.hpp"
#include "BannerSetNameAction.hpp"
#include "WallRemoveAction.hpp"

namespace GameActions
{
    void Register()
    {
        Register<SetParkEntranceFeeAction>();
        Register<ParkMarketingAction>();
        Register<ParkSetLoanAction>();
        Register<ParkSetResearchFundingAction>();
        Register<PlaceParkEntranceAction>();
        Register<RideCreateAction>();
        Register<RideSetStatusAction>();
        Register<RideSetNameAction>();
        Register<RideDemolishAction>();
        Register<GuestSetNameAction>();
        Register<StaffSetColourAction>();
        Register<StaffSetNameAction>();
        Register<PlacePeepSpawnAction>();
        Register<MazeSetTrackAction>();
        Register<SignSetNameAction>();
        Register<ParkSetNameAction>();
        Register<BannerSetNameAction>();
        Register<WallRemoveAction>();
    }
} // namespace GameActions
