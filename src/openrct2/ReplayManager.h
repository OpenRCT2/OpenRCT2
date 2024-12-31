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
#include <set>
#include <string>

class GameAction;

namespace OpenRCT2
{
    static constexpr uint32_t k_MaxReplayTicks = 0xFFFFFFFF;

    struct ReplayRecordInfo
    {
        uint16_t Version;
        uint32_t Ticks;
        uint64_t TimeRecorded;
        uint32_t NumCommands;
        uint32_t NumChecksums;
        std::string Name;
        std::string FilePath;
    };

    struct IReplayManager
    {
    public:
        enum class RecordType
        {
            NORMAL,
            SILENT,
        };

        virtual ~IReplayManager() = default;

        virtual void Update() = 0;

        virtual bool IsReplaying() const = 0;
        virtual bool IsRecording() const = 0;
        virtual bool IsNormalising() const = 0;
        virtual bool ShouldDisplayNotice() const = 0;

        virtual void AddGameAction(uint32_t tick, const GameAction* action) = 0;

        virtual bool StartRecording(
            const std::string& name, uint32_t maxTicks = k_MaxReplayTicks, RecordType rt = RecordType::NORMAL)
            = 0;
        virtual bool StopRecording(bool discard = false) = 0;
        virtual bool GetCurrentReplayInfo(ReplayRecordInfo& info) const = 0;

        virtual bool StartPlayback(const std::string& file) = 0;
        virtual bool IsPlaybackStateMismatching() const = 0;
        virtual bool StopPlayback() = 0;

        virtual bool NormaliseReplay(const std::string& inputFile, const std::string& outputFile) = 0;
    };

    [[nodiscard]] std::unique_ptr<IReplayManager> CreateReplayManager();

} // namespace OpenRCT2
