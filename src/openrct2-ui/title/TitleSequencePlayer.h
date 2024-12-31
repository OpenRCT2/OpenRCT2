/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>

struct ITitleSequencePlayer;
struct IScenarioRepository;

namespace OpenRCT2
{
    namespace Title
    {
        [[nodiscard]] std::unique_ptr<ITitleSequencePlayer> CreateTitleSequencePlayer();
    } // namespace Title
} // namespace OpenRCT2
