/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../localisation/Localisation.h"

#define TITLE_COMMAND_SCENARIO_LENGTH 64

struct TitleCommand
{
    uint8_t Type;
    union
    {
        uint8_t SaveIndex; // LOAD (this index is internal only)
        struct             // LOCATION
        {
            uint8_t X;
            uint8_t Y;
        };
        uint8_t Rotations; // ROTATE (counter-clockwise)
        uint8_t Zoom;      // ZOOM
        struct             // FOLLOW
        {
            uint16_t SpriteIndex;
            utf8 SpriteName[USER_STRING_MAX_LENGTH];
        };
        uint8_t Speed;                                // SPEED
        uint16_t Milliseconds;                        // WAIT
        utf8 Scenario[TITLE_COMMAND_SCENARIO_LENGTH]; // LOADSC
    };
};

struct TitleSequenceParkHandle
{
    const utf8* HintPath;
    void* Stream;
};

struct TitleSequence
{
    explicit TitleSequence(const utf8* path);
    TitleSequence() = default;
    ~TitleSequence() = default;

    bool Save();
    bool AddPark(const utf8*, const utf8* name);
    bool RenamePark(size_t index, const utf8* name);
    bool RemovePark(size_t index);
    std::string LegacyScriptWrite();
    void SwapConsecutiveCommands(size_t index);

    // TODO: TitleSequenceParkHandle should also be migrated to RAII
    TitleSequenceParkHandle* GetParkHandle(size_t index);
    void CloseParkHandle(TitleSequenceParkHandle* handle);

    static bool IsLoadCommand(const TitleCommand* command);

    static constexpr const utf8* Extension = ".parkseq";
    static constexpr uint8_t SaveIndexInvalid = std::numeric_limits<uint8_t>::max();

    size_t NumSaves() const
    {
        return Saves.size();
    }
    size_t NumCommands() const
    {
        return Commands.size();
    }

    std::string Path;
    std::string Name;

    std::vector<TitleCommand> Commands;
    std::vector<std::string> Saves;

    bool IsZip = false;
};

enum TITLE_SCRIPT
{
    TITLE_SCRIPT_UNDEFINED = 0xFF,
    TITLE_SCRIPT_WAIT = 0,
    TITLE_SCRIPT_LOCATION,
    TITLE_SCRIPT_ROTATE,
    TITLE_SCRIPT_ZOOM,
    TITLE_SCRIPT_FOLLOW,
    TITLE_SCRIPT_RESTART,
    TITLE_SCRIPT_LOAD,
    TITLE_SCRIPT_END,
    TITLE_SCRIPT_SPEED,
    TITLE_SCRIPT_LOOP,
    TITLE_SCRIPT_ENDLOOP,
    TITLE_SCRIPT_LOADSC,
};
