#include "../core/Console.hpp"
#include "CommandLine.hpp"

const CommandLineCommand * ScreenshotCommandTable;
const CommandLineCommand * SpriteCommandTable;

static bool   _help            = false;
static bool   _version         = false;
static bool   _noInstall       = false;
static bool   _about           = false;
static bool   _verbose         = false;
static bool   _headless        = false;
static uint32 _port            = 0;
static utf8 * _userDataPath    = nullptr;
static utf8 * _openrctDataPath = nullptr;

static const CommandLineOptionDefinition StandardOptions[]
{
    { CMDLINE_TYPE_SWITCH,  &_help,            'h', "help",              "show this help message and exit"                            },
    { CMDLINE_TYPE_SWITCH,  &_version,         'v', "version",           "show version information and exit"                          },
    { CMDLINE_TYPE_SWITCH,  &_noInstall,       'n', "no-install",        "do not install scenario if passed"                          },
    { CMDLINE_TYPE_SWITCH,  &_about,           NAC, "about"     ,        "show information about " OPENRCT2_NAME                      },
    { CMDLINE_TYPE_SWITCH,  &_verbose,         NAC, "verbose",           "log verbose messages"                                       },
    { CMDLINE_TYPE_SWITCH,  &_headless,        NAC, "headless",          "run " OPENRCT2_NAME " headless"                             },
    { CMDLINE_TYPE_INTEGER, &_port,            NAC, "port",              "port to use for hosting or joining a server"                },
    { CMDLINE_TYPE_STRING,  &_userDataPath,    NAC, "user-data-path",    "path to the user data directory (containing config.ini)"    },
    { CMDLINE_TYPE_STRING,  &_openrctDataPath, NAC, "openrct-data-path", "path to the OpenRCT2 data directory (containing languages)" },
    OptionTableEnd
};

static exitcode_t HandleNoCommand(CommandLineArgEnumerator * enumerator);
static exitcode_t HandleCommandIntro(CommandLineArgEnumerator * enumerator);
static exitcode_t HandleCommandHost(CommandLineArgEnumerator * enumerator);
static exitcode_t HandleCommandJoin(CommandLineArgEnumerator * enumerator);

const CommandLineCommand CommandLine::RootCommands[]
{
    // Main commands
    DefineCommand("",      "<uri>",      StandardOptions, HandleNoCommand   ),
    DefineCommand("intro", "",           StandardOptions, HandleCommandIntro),
    DefineCommand("host",  "<uri>",      StandardOptions, HandleCommandHost ),
    DefineCommand("join",  "<hostname>", StandardOptions, HandleCommandJoin ),

    // Sub-commands
    DefineSubCommand("screenshot", ScreenshotCommandTable),
    DefineSubCommand("sprite",     SpriteCommandTable    ),

    CommandTableEnd
};

static const CommandLineExample CommandLine::RootExamples[]
{
    { "./my_park.sv6",                              "open a saved park"                      },
    { "./SnowyPark.sc6",                            "install and open a scenario"            },
    { "./ShuttleLoop.td6",                          "install a track"                        },
    { "http:/openrct2.website/files/SnowyPark.sv6", "download and open a saved park"         },
    { "host ./my_park.sv6 --port 11753 --headless", "run a headless server for a saved park" },
    ExampleTableEnd
};

exitcode_t CommandLine::HandleCommandDefault()
{
    if (_help)
    {
        CommandLine::PrintHelp();
        return EXITCODE_OK;
    }

    return EXITCODE_CONTINUE;
}

exitcode_t HandleNoCommand(CommandLineArgEnumerator * enumerator)
{
    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    // Process URI

    return EXITCODE_CONTINUE;
}

exitcode_t HandleCommandIntro(CommandLineArgEnumerator * enumerator)
{
    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    return EXITCODE_CONTINUE;
}

exitcode_t HandleCommandHost(CommandLineArgEnumerator * enumerator)
{
    const char * parkUri;

    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    if (!enumerator->TryPopString(&parkUri))
    {
        Console::WriteLineError("Expected path or URL to a saved park.");
        CommandLine::PrintUsageFor("host");
        return EXITCODE_FAIL;
    }

    return EXITCODE_CONTINUE;
}

exitcode_t HandleCommandJoin(CommandLineArgEnumerator * enumerator)
{
    const char * hostname;

    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    if (!enumerator->TryPopString(&hostname))
    {
        Console::WriteLineError("Expected a hostname or IP address to the server to connect to.");
        CommandLine::PrintUsageFor("join");
        return EXITCODE_FAIL;
    }

    return EXITCODE_CONTINUE;
}

const CommandLineCommand ScreenshotCommands[]
{
    // Main commands
    DefineCommand("", "<file> <output_image> <width> <height> [<x> <y> <zoom> <rotation>]", nullptr, nullptr),
    DefineCommand("", "<file> <output_image> giant <zoom> <rotation>",                      nullptr, nullptr),
    CommandTableEnd
};

const CommandLineOptionDefinition SpriteOptions[]
{
    { CMDLINE_TYPE_STRING, nullptr, 'm', "mode", "the type of sprite conversion <default|simple|dithering>" },
    OptionTableEnd
};

const CommandLineCommand SpriteCommands[]
{
    // Main commands
    DefineCommand("details", "<spritefile> [idx]",                  SpriteOptions, nullptr),
    DefineCommand("export",  "<spritefile> <idx> <output>",         SpriteOptions, nullptr),
    DefineCommand("build",   "<spritefile> <resourcedir> [silent]", SpriteOptions, nullptr),
    CommandTableEnd
};
