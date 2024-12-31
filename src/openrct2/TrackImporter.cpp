/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackImporter.h"

#include "core/Path.hpp"
#include "core/String.hpp"

#include <memory>

namespace OpenRCT2::TrackImporter
{
    std::unique_ptr<ITrackImporter> Create(const std::string& hintPath)
    {
        std::unique_ptr<ITrackImporter> trackImporter;
        std::string extension = Path::GetExtension(hintPath);
        if (ExtensionIsRCT1(extension))
        {
            trackImporter = CreateTD4();
        }
        else
        {
            trackImporter = CreateTD6();
        }
        return trackImporter;
    }

    bool ExtensionIsRCT1(const std::string& extension)
    {
        return String::iequals(extension, ".td4");
    }
} // namespace OpenRCT2::TrackImporter
