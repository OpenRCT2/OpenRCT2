/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../ride/TrackDesign.h"

#include <vector>

namespace OpenRCT2
{
    struct IStream;
}

namespace RCT2
{
    /**
     * Class to export RollerCoaster Tycoon 2 track designs (*.TD6).
     */
    class T6Exporter final
    {
    public:
        T6Exporter(TrackDesign* trackDesign);

        bool SaveTrack(const utf8* path);
        bool SaveTrack(OpenRCT2::IStream* stream);

    private:
        TrackDesign* _trackDesign;
    };
} // namespace RCT2
