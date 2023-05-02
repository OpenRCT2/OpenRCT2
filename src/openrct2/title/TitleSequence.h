/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../openrct2/core/IStream.hpp"
#include "Command/End.h"
#include "Command/FollowEntity.h"
#include "Command/LoadPark.h"
#include "Command/LoadScenario.h"
#include "Command/Restart.h"
#include "Command/RotateView.h"
#include "Command/SetLocation.h"
#include "Command/SetSpeed.h"
#include "Command/SetZoom.h"
#include "Command/Wait.h"

#include <memory>
#include <string>
#include <variant>

namespace OpenRCT2::Title
{
    using TitleCommand = std::variant<
        WaitCommand, SetLocationCommand, RotateViewCommand, SetZoomCommand, FollowEntityCommand, RestartCommand,
        LoadParkCommand, EndCommand, SetSpeedCommand, LoadScenarioCommand>;

    struct TitleSequence
    {
        std::string Name;
        std::string Path;

        std::vector<TitleCommand> Commands;
        std::vector<std::string> Saves;

        bool IsZip = false;
    };

    struct TitleSequenceParkHandle
    {
        std::string HintPath;
        std::unique_ptr<OpenRCT2::IStream> Stream;
    };

    constexpr const utf8* TITLE_SEQUENCE_EXTENSION = ".parkseq";
    constexpr uint8_t SAVE_INDEX_INVALID = UINT8_MAX;

    [[nodiscard]] std::unique_ptr<TitleSequence> CreateTitleSequence();
    [[nodiscard]] std::unique_ptr<TitleSequence> LoadTitleSequence(const std::string& path);
    [[nodiscard]] std::unique_ptr<TitleSequenceParkHandle> TitleSequenceGetParkHandle(const TitleSequence& seq, size_t index);

    bool TitleSequenceSave(const TitleSequence& seq);
    bool TitleSequenceAddPark(TitleSequence& seq, const utf8* path, const utf8* name);
    bool TitleSequenceRenamePark(TitleSequence& seq, size_t index, const utf8* name);
    bool TitleSequenceRemovePark(TitleSequence& seq, size_t index);

    bool TitleSequenceIsLoadCommand(const TitleCommand& command);
} // namespace OpenRCT2::Title
