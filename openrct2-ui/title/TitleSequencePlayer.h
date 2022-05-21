/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include <openrct2/common.h>

struct ITitleSequencePlayer;
struct IScenarioRepository;

namespace OpenRCT2
{
    class GameState;
}

[[nodiscard]] std::unique_ptr<ITitleSequencePlayer> CreateTitleSequencePlayer(OpenRCT2::GameState& gameState);
