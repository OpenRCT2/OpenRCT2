/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ProvisionalElements.h"

#include <openrct2/interface/Window.h>
#include <openrct2/interface/WindowClasses.h>
#include <openrct2/network/network.h>
#include <openrct2/profiling/Profiling.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/world/Footpath.h>
#include <openrct2-ui/windows/Window.h>

using namespace OpenRCT2::Ui::Windows;

namespace OpenRCT2
{
    void ProvisionalElementsRemove()
    {
        PROFILED_FUNCTION();

        if (gProvisionalFootpath.Flags & PROVISIONAL_PATH_FLAG_1)
        {
            FootpathProvisionalRemove();
            gProvisionalFootpath.Flags |= PROVISIONAL_PATH_FLAG_1;
        }
        if (WindowFindByClass(WindowClass::RideConstruction) != nullptr)
        {
            RideRemoveProvisionalTrackPiece();
            RideEntranceExitRemoveGhost();
        }
        // This is in non performant so only make network games suffer for it
        // non networked games do not need this as its to prevent desyncs.
        if ((NetworkGetMode() != NETWORK_MODE_NONE) && WindowFindByClass(WindowClass::TrackDesignPlace) != nullptr)
        {
            TrackPlaceClearProvisionalTemporarily();
        }
    }

    void ProvisionalElementsRestore()
    {
        PROFILED_FUNCTION();

        if (gProvisionalFootpath.Flags & PROVISIONAL_PATH_FLAG_1)
        {
            gProvisionalFootpath.Flags &= ~PROVISIONAL_PATH_FLAG_1;
            FootpathProvisionalSet(
                gProvisionalFootpath.SurfaceIndex, gProvisionalFootpath.RailingsIndex, gProvisionalFootpath.Position,
                gProvisionalFootpath.Slope, gProvisionalFootpath.ConstructFlags);
        }
        if (WindowFindByClass(WindowClass::RideConstruction) != nullptr)
        {
            RideRestoreProvisionalTrackPiece();
            RideEntranceExitPlaceProvisionalGhost();
        }
        // This is in non performant so only make network games suffer for it
        // non networked games do not need this as its to prevent desyncs.
        if ((NetworkGetMode() != NETWORK_MODE_NONE) && WindowFindByClass(WindowClass::TrackDesignPlace) != nullptr)
        {
            TrackPlaceRestoreProvisional();
        }
    }
} // namespace OpenRCT2
