#include "CommandLine.hpp"

const CommandLineCommand * ScreenshotCommandTable;
const CommandLineCommand * SpriteCommandTable;

static bool _help;
static bool _version;
static bool _noInstall;

const CommandLineOptionDefinition StandardOptions[]
{
    { CMDLINE_TYPE_SWITCH,  &_help,      'h', "help",              "show this help message and exit"                            },
    { CMDLINE_TYPE_SWITCH,  &_version,   'v', "version",           "show version information and exit"                          },
    { CMDLINE_TYPE_SWITCH,  &_noInstall, 'n', "no-install",        "do not install scenario if passed"                          },
    { CMDLINE_TYPE_SWITCH,  nullptr,     NAC, "no-install",        "show information about " OPENRCT2_NAME                      },
    { CMDLINE_TYPE_SWITCH,  nullptr,     NAC, "verbose",           "log verbose messages"                                       },
    { CMDLINE_TYPE_SWITCH,  nullptr,     NAC, "headless",          "run " OPENRCT2_NAME " headless"                             },
    { CMDLINE_TYPE_INTEGER, nullptr,     NAC, "port",              "port to use for hosting or joining a server"                },
    { CMDLINE_TYPE_STRING,  nullptr,     NAC, "user-data-path",    "path to the user data directory (containing config.ini)"    },
    { CMDLINE_TYPE_STRING,  nullptr,     NAC, "openrct-data-path", "path to the OpenRCT2 data directory (containing languages)" },
    OptionTableEnd
};

const CommandLineCommand RootCommands[]
{
    // Main commands
    DefineCommand("",      "<uri>",      StandardOptions, nullptr),
    DefineCommand("intro", "",           StandardOptions, nullptr),
    DefineCommand("host",  "<uri>",      StandardOptions, nullptr),
    DefineCommand("join",  "<hostname>", StandardOptions, nullptr),

    // Sub-commands
    DefineSubCommand("screenshot", ScreenshotCommandTable),
    DefineSubCommand("sprite",     SpriteCommandTable    ),

    CommandTableEnd
};

const CommandLineExample RootExamples[]
{
    { "./my_park.sv6",                              "open a saved park"                      },
    { "./SnowyPark.sc6",                            "install and open a scenario"            },
    { "./ShuttleLoop.td6",                          "install a track"                        },
    { "http:/openrct2.website/files/SnowyPark.sv6", "download and open a saved park"         },
    { "host ./my_park.sv6 --port 11753 --headless", "run a headless server for a saved park" },
    ExampleTableEnd
};

void HandleNoCommand(CommandLineArgEnumerator * enumerator)
{

}

void HandleCommandIntro(CommandLineArgEnumerator * enumerator)
{

}

void HandleCommandHost(CommandLineArgEnumerator * enumerator)
{
    const char * parkUri;

    if (!enumerator->TryPopString(&parkUri))
    {
        fprintf(stderr, "Expected path or URL to a saved park.");
        CommandLineDisplayUsageFor("host");
    }
}

void HandleCommandJoin(CommandLineArgEnumerator * enumerator)
{
    const char * hostname;

    if (!enumerator->TryPopString(&hostname))
    {
        fprintf(stderr, "Expected a hostname or IP address to the server to connect to.");
        CommandLineDisplayUsageFor("join");
    }
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
