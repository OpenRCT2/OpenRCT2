#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#ifdef __cplusplus

#include "../common.h"
#include "../cmdline_sprite.h"

/**
 * Class for enumerating and retrieving values for a set of command line arguments.
 */
class CommandLineArgEnumerator final
{
private:
    const char * const * _arguments;
    uint16               _count;
    uint16               _index;

public:
    const char * const * GetArguments() const { return _arguments; }
    uint16               GetCount()     const { return _count; }
    uint16               GetIndex()     const { return _index; }

    CommandLineArgEnumerator(const char * const * arguments, sint32 count);

    void Reset();
    bool Backtrack();
    bool TryPop();
    bool TryPopInteger(sint32 * result);
    bool TryPopReal(float * result);
    bool TryPopString(const char * * result);
};

typedef sint32 exitcode_t;
typedef exitcode_t (*CommandLineFunc)(CommandLineArgEnumerator *);

enum
{
    EXITCODE_FAIL     = -1,
    EXITCODE_OK       =  0,
    EXITCODE_CONTINUE =  1,
};

struct CommandLineExample
{
    const char * Arguments;
    const char * Description;
};

struct CommandLineOptionDefinition
{
    uint8        Type;
    void *       OutAddress;
    char         ShortName;
    const char * LongName;
    const char * Description;
};

struct CommandLineCommand
{
    const char *                        Name;
    const char *                        Parameters;
    const CommandLineOptionDefinition * Options;
    const CommandLineCommand *          SubCommands;
    CommandLineFunc                     Func;
};

enum
{
    CMDLINE_TYPE_SWITCH,
    CMDLINE_TYPE_INTEGER,
    CMDLINE_TYPE_REAL,
    CMDLINE_TYPE_STRING,
};

constexpr char NAC = '\0';

#define ExampleTableEnd { NULL, NULL }
#define OptionTableEnd  { UINT8_MAX, NULL, NAC, NULL, NULL }
#define CommandTableEnd { NULL, NULL, NULL, NULL }

#define DefineCommand(name, params, options, func) { name, params, options, NULL,            func }
#define DefineSubCommand(name, subcommandtable)    { name, "",     NULL,    subcommandtable, NULL }

namespace CommandLine
{
    extern const CommandLineCommand RootCommands[];
    extern const CommandLineCommand ScreenshotCommands[];
    extern const CommandLineCommand SpriteCommands[];
    extern const CommandLineCommand BenchGfxCommands[];

    extern const CommandLineExample RootExamples[];

    void PrintHelp(bool allCommands = false);
    exitcode_t HandleCommandDefault();

    exitcode_t HandleCommandConvert(CommandLineArgEnumerator * enumerator);
    exitcode_t HandleCommandUri(CommandLineArgEnumerator * enumerator);
}

#endif
