/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "common.h"
#include "core/IStream.hpp"
#include "core/String.hpp"
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

    virtual bool Load(u8string_view path) abstract;
    virtual bool LoadFromStream(OpenRCT2::IStream* stream) abstract;

    [[nodiscard]] virtual std::unique_ptr<TrackDesign> Import() abstract;
};

namespace TrackImporter
{
    [[nodiscard]] std::unique_ptr<ITrackImporter> Create(u8string_view hintPath);
    [[nodiscard]] std::unique_ptr<ITrackImporter> CreateTD4();
    [[nodiscard]] std::unique_ptr<ITrackImporter> CreateTD6();

    bool ExtensionIsRCT1(std::string_view extension);
} // namespace TrackImporter
