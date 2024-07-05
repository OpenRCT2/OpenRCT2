/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
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

namespace TrackImporter
{
    std::unique_ptr<ITrackImporter> Create(const std::string& hintPath)
    {
        std::unique_ptr<ITrackImporter> trackImporter;
        std::string extension = Path::GetExtension(hintPath);
        if (ExtensionIsRCT1(extension))
        {
            trackImporter = CreateTD4();
        }
        else if (ExtensionIsRCT2(extension))
        {
            trackImporter = CreateTD6();
        }
        else
        {
            trackImporter = CreateNTDF();
        }
        return trackImporter;
    }

    bool ExtensionIsRCT1(u8string_view extension)
    {
        return String::IEquals(extension, ".td4");
    }

    bool ExtensionIsRCT2(u8string_view extension)
    {
        return String::IEquals(extension, ".td6");
    }
} // namespace TrackImporter
