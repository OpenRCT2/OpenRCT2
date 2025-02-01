/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

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

constexpr char kNAC = '\0';

constexpr CommandLineExample kExampleTableEnd = CommandLineExample{ nullptr, nullptr };
constexpr CommandLineOptionDefinition kOptionTableEnd = CommandLineOptionDefinition{ UINT8_MAX, nullptr, kNAC, nullptr,
                                                                                     nullptr };
constexpr CommandLineCommand kCommandTableEnd = CommandLineCommand{ nullptr, nullptr, nullptr, nullptr, nullptr };

consteval CommandLineCommand DefineCommand(
    const char* name, const char* params, const CommandLineOptionDefinition* options, const CommandLineFunc func)
{
    return CommandLineCommand{ name, params, options, nullptr, func };
}

consteval CommandLineCommand DefineSubCommand(const char* name, const CommandLineCommand* subcommandtable)
{
    return CommandLineCommand{ name, "", nullptr, subcommandtable, nullptr };
}

namespace OpenRCT2::CommandLine
{
    extern const CommandLineCommand kRootCommands[];
    extern const CommandLineCommand kScreenshotCommands[];
    extern const CommandLineCommand kSpriteCommands[];
    extern const CommandLineCommand kSimulateCommands[];
    extern const CommandLineCommand kParkInfoCommands[];

    extern const CommandLineExample kRootExamples[];

    void PrintHelp(bool allCommands = false);
    exitcode_t HandleCommandDefault();

    exitcode_t HandleCommandConvert(CommandLineArgEnumerator* enumerator);
    exitcode_t HandleCommandUri(CommandLineArgEnumerator* enumerator);
} // namespace OpenRCT2::CommandLine
