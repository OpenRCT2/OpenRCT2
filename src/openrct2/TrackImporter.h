/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "core/IStream.hpp"
#include "ride/TrackDesign.h"

#include <memory>
#include <string>
#include <vector>

/**
 * Interface to import scenarios and saved games.
 */
struct ITrackImporter
{
public:
    virtual ~ITrackImporter() = default;

    virtual bool Load(const utf8* path) = 0;
    virtual bool LoadFromStream(OpenRCT2::IStream* stream) = 0;

    [[nodiscard]] virtual std::unique_ptr<TrackDesign> Import() = 0;
};

namespace OpenRCT2::TrackImporter
{
    [[nodiscard]] std::unique_ptr<ITrackImporter> Create(const std::string& hintPath);
    [[nodiscard]] std::unique_ptr<ITrackImporter> CreateTD4();
    [[nodiscard]] std::unique_ptr<ITrackImporter> CreateTD6();

    bool ExtensionIsRCT1(const std::string& extension);
} // namespace OpenRCT2::TrackImporter
