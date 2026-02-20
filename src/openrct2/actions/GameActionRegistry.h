/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <optional>

namespace OpenRCT2
{
    enum class GameCommand : int32_t;
}

namespace OpenRCT2::GameActions
{
    class GameAction;

    using GameActionFactory = GameAction* (*)();

    std::optional<GameActionFactory> getFactory(GameCommand command);
    const char* GetName(GameCommand id);
    bool IsValidId(uint32_t id);

} // namespace OpenRCT2::GameActions
