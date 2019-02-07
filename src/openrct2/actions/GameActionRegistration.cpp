/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "BannerSetNameAction.hpp"
#include "ClearAction.hpp"
#include "ClimateSetAction.hpp"
#include "FootpathRemoveAction.hpp"
#include "GameAction.h"
#include "GuestSetNameAction.hpp"
#include "LargeSceneryRemoveAction.hpp"
#include "MazeSetTrackAction.hpp"
#include "ParkMarketingAction.hpp"
#include "ParkSetLoanAction.hpp"
#include "ParkSetNameAction.hpp"
#include "ParkSetResearchFundingAction.hpp"
#include "PlaceParkEntranceAction.hpp"
#include "PlacePeepSpawnAction.hpp"
#include "RideCreateAction.hpp"
#include "RideDemolishAction.hpp"
#include "RideSetAppearanceAction.hpp"
#include "RideSetColourScheme.hpp"
#include "RideSetName.hpp"
#include "RideSetStatus.hpp"
#include "SetParkEntranceFeeAction.hpp"
#include "SignSetNameAction.hpp"
#include "SignSetStyleAction.hpp"
#include "SmallSceneryRemoveAction.hpp"
#include "StaffSetColourAction.hpp"
#include "StaffSetCostumeAction.hpp"
#include "StaffSetNameAction.hpp"
#include "StaffSetOrdersAction.hpp"
#include "WallRemoveAction.hpp"

namespace GameActions
{
    void Register()
    {
        Register<BannerSetNameAction>();
        Register<ClimateSetAction>();
        Register<FootpathRemoveAction>();
        Register<GuestSetNameAction>();
        Register<MazeSetTrackAction>();
        Register<ParkMarketingAction>();
        Register<ParkSetLoanAction>();
        Register<ParkSetNameAction>();
        Register<ParkSetResearchFundingAction>();
        Register<PlaceParkEntranceAction>();
        Register<PlacePeepSpawnAction>();
        Register<RideCreateAction>();
        Register<RideDemolishAction>();
        Register<RideSetColourSchemeAction>();
        Register<RideSetNameAction>();
        Register<RideSetStatusAction>();
        Register<SetParkEntranceFeeAction>();
        Register<SignSetNameAction>();
        Register<SignSetStyleAction>();
        Register<StaffSetColourAction>();
        Register<StaffSetNameAction>();
        Register<StaffSetOrdersAction>();
        Register<StaffSetCostumeAction>();
        Register<WallRemoveAction>();
        Register<SmallSceneryRemoveAction>();
        Register<LargeSceneryRemoveAction>();
        Register<ClearAction>();
    }
} // namespace GameActions
