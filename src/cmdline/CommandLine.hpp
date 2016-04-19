#pragma once

extern "C"
{
    #include "../common.h"
}

/**
 * Class for enumerating and retrieving values for a set of command line arguments.
 */
class CommandLineArgEnumerator
{
private:
    const char * const * _arguments;
    uint16               _count;
    uint16               _index;

public:
    const char * const * GetArguments() const { return _arguments; }
    uint16               GetCount()     const { return _count; }
    uint16               GetIndex()     const { return _index; }

    CommandLineArgEnumerator(const char * const * arguments, int count);

    void Reset();
    bool Backtrack();
    bool TryPop();
    bool TryPopInteger(sint32 * result);
    bool TryPopReal(float * result);
    bool TryPopString(const char * * result);
};

typedef int exitcode_t;
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

    extern const CommandLineExample RootExamples[];

    void PrintHelp(bool allCommands = false);
    exitcode_t HandleCommandDefault();

    exitcode_t HandleCommandConvert(CommandLineArgEnumerator * enumerator);
}
