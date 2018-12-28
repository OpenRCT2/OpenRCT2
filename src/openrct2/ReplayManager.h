/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "common.h"

#include <memory>
#include <set>
#include <string>

struct GameAction;

namespace OpenRCT2
{
    static constexpr uint32_t k_MaxReplayTicks = 0xFFFFFFFF;

    interface IReplayManager
    {
    public:
        virtual ~IReplayManager() = default;

        virtual void Update() = 0;

        virtual bool IsReplaying() const = 0;
        virtual bool IsRecording() const = 0;
        virtual bool IsNormalising() const = 0;

        // NOTE: Will become obsolete eventually once all game actions are done.
        virtual void AddGameCommand(
            uint32_t tick, uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t ebp,
            uint8_t callback)
            = 0;
        virtual void AddGameAction(uint32_t tick, const GameAction* action) = 0;

        virtual bool StartRecording(const std::string& name, uint32_t maxTicks = k_MaxReplayTicks) = 0;
        virtual bool StopRecording() = 0;

        virtual bool StartPlayback(const std::string& file) = 0;
        virtual bool IsPlaybackStateMismatching() const = 0;
        virtual bool StopPlayback() = 0;

        virtual bool NormaliseReplay(const std::string& inputFile, const std::string& outputFile) = 0;
    };

    std::unique_ptr<IReplayManager> CreateReplayManager();

} // namespace OpenRCT2
