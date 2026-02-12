/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.hpp"

namespace OpenRCT2
{
    struct GameState_t;
}

namespace OpenRCT2::GameActions
{
    // Halts the queue processing until ResumeQueue is called, any calls to ProcessQueue
    // will have no effect during suspension. It has no effect of actions that will not
    // cross the network.
    void SuspendQueue();

    // Resumes queue processing.
    void ResumeQueue();

    void Enqueue(const GameAction* ga, uint32_t tick);
    void Enqueue(GameAction::Ptr&& ga, uint32_t tick);
    void ProcessQueue(GameState_t& gameState);
    void ClearQueue();

    // This should be used if a round trip is to be expected.
    Result Query(const GameAction* action, GameState_t& gameState);
    Result Execute(const GameAction* action, GameState_t& gameState);

    // This should be used from within game actions.
    Result QueryNested(const GameAction* action, GameState_t& gameState);
    Result ExecuteNested(const GameAction* action, GameState_t& gameState);
} // namespace OpenRCT2::GameActions
