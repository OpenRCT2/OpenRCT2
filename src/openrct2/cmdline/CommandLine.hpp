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

/**
 * Class for enumerating and retrieving values for a set of command line arguments.
 */
class CommandLineArgEnumerator final
{
private:
    const char* const* _arguments;
    uint16_t _count;
    uint16_t _index;

public:
    const char* const* GetArguments() const
    {
        return _arguments;
    }
    uint16_t GetCount() const
    {
        return _count;
    }
    uint16_t GetIndex() const
    {
        return _index;
    }

    CommandLineArgEnumerator(const char* const* arguments, int32_t count);

    void Reset();
    bool Backtrack();
    bool TryPop();
    bool TryPopInteger(int32_t* result);
    bool TryPopReal(float* result);
    bool TryPopString(const char** result);
};

using exitcode_t = int32_t;
using CommandLineFunc = exitcode_t (*)(CommandLineArgEnumerator*);

enum
{
    EXITCODE_FAIL = -1,
    EXITCODE_OK = 0,
    EXITCODE_CONTINUE = 1,
};

struct CommandLineExample
{
    const char* Arguments;
    const char* Description;
};

struct CommandLineOptionDefinition
{
    uint8_t Type;
    void* OutAddress;
    char ShortName;
    const char* LongName;
    const char* Description;
};

struct CommandLineCommand
{
    const char* Name;
    const char* Parameters;
    const CommandLineOptionDefinition* Options;
    const CommandLineCommand* SubCommands;
    CommandLineFunc Func;
};

enum
{
    CMDLINE_TYPE_SWITCH,
    CMDLINE_TYPE_INTEGER,
    CMDLINE_TYPE_REAL,
    CMDLINE_TYPE_STRING,
};

constexpr char NAC = '\0';

#define ExampleTableEnd                                                                                                        \
    {                                                                                                                          \
        nullptr, nullptr                                                                                                       \
    }
#define OptionTableEnd                                                                                                         \
    {                                                                                                                          \
        UINT8_MAX, nullptr, NAC, nullptr, nullptr                                                                              \
    }
#define CommandTableEnd                                                                                                        \
    {                                                                                                                          \
        nullptr, nullptr, nullptr, nullptr, nullptr                                                                            \
    }

#define DefineCommand(name, params, options, func)                                                                             \
    {                                                                                                                          \
        name, params, options, nullptr, func                                                                                   \
    }
#define DefineSubCommand(name, subcommandtable)                                                                                \
    {                                                                                                                          \
        name, "", nullptr, subcommandtable, nullptr                                                                            \
    }

namespace CommandLine
{
    extern const CommandLineCommand RootCommands[];
    extern const CommandLineCommand ScreenshotCommands[];
    extern const CommandLineCommand SpriteCommands[];
    extern const CommandLineCommand BenchGfxCommands[];
    extern const CommandLineCommand BenchSpriteSortCommands[];
    extern const CommandLineCommand BenchUpdateCommands[];
    extern const CommandLineCommand SimulateCommands[];
    extern const CommandLineCommand ParkInfoCommands[];

    extern const CommandLineExample RootExamples[];

    void PrintHelp(bool allCommands = false);
    exitcode_t HandleCommandDefault();

    exitcode_t HandleCommandConvert(CommandLineArgEnumerator* enumerator);
    exitcode_t HandleCommandUri(CommandLineArgEnumerator* enumerator);
} // namespace CommandLine
