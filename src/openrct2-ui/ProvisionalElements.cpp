/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ProvisionalElements.h"

#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/interface/Window.h>
#include <openrct2/interface/WindowClasses.h>
#include <openrct2/network/Network.h>
#include <openrct2/profiling/Profiling.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Footpath.h>

using namespace OpenRCT2::Ui::Windows;

namespace OpenRCT2::Ui
{
    void ProvisionalElementsRemove()
    {
        PROFILED_FUNCTION();

        FootpathRemoveProvisionalTemporarily();

        auto* windowMgr = GetWindowManager();
        if (windowMgr->FindByClass(WindowClass::rideConstruction) != nullptr)
        {
            RideRemoveProvisionalTrackPiece();
            RideEntranceExitRemoveGhost();
        }
        // This is in non performant so only make network games suffer for it
        // non networked games do not need this as its to prevent desyncs.
        if ((Network::GetMode() != Network::Mode::none) && windowMgr->FindByClass(WindowClass::trackDesignPlace) != nullptr)
        {
            TrackPlaceClearProvisionalTemporarily();
        }
    }

    void ProvisionalElementsRestore()
    {
        PROFILED_FUNCTION();

        FootpathRestoreProvisional();

        auto* windowMgr = GetWindowManager();
        if (windowMgr->FindByClass(WindowClass::rideConstruction) != nullptr)
        {
            RideRestoreProvisionalTrackPiece();
            RideEntranceExitPlaceProvisionalGhost();
        }
        // This is in non performant so only make network games suffer for it
        // non networked games do not need this as its to prevent desyncs.
        if ((Network::GetMode() != Network::Mode::none) && windowMgr->FindByClass(WindowClass::trackDesignPlace) != nullptr)
        {
            TrackPlaceRestoreProvisional();
        }
    }
} // namespace OpenRCT2::Ui
