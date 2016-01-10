#include <time.h>

extern "C"
{
    #include "../openrct2.h"
}

#include "../core/Console.hpp"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "../network/network.h"
#include "CommandLine.hpp"

#ifndef DISABLE_NETWORK
int  gNetworkStart = NETWORK_MODE_NONE;
char gNetworkStartHost[128];
int  gNetworkStartPort = NETWORK_DEFAULT_PORT;
#endif

static bool   _help            = false;
static bool   _version         = false;
static bool   _noInstall       = false;
static bool   _all             = false;
static bool   _about           = false;
static bool   _verbose         = false;
static bool   _headless        = false;
#ifndef DISABLE_NETWORK
static uint32 _port            = 0;
#endif
static utf8 * _userDataPath    = nullptr;
static utf8 * _openrctDataPath = nullptr;

static const CommandLineOptionDefinition StandardOptions[]
{
    { CMDLINE_TYPE_SWITCH,  &_help,            'h', "help",              "show this help message and exit"                            },
    { CMDLINE_TYPE_SWITCH,  &_version,         'v', "version",           "show version information and exit"                          },
    { CMDLINE_TYPE_SWITCH,  &_noInstall,       'n', "no-install",        "do not install scenario if passed"                          },
    { CMDLINE_TYPE_SWITCH,  &_all,             'a', "all",               "show help for all commands"                                 },
    { CMDLINE_TYPE_SWITCH,  &_about,           NAC, "about",             "show information about " OPENRCT2_NAME                      },
    { CMDLINE_TYPE_SWITCH,  &_verbose,         NAC, "verbose",           "log verbose messages"                                       },
    { CMDLINE_TYPE_SWITCH,  &_headless,        NAC, "headless",          "run " OPENRCT2_NAME " headless"                             },
#ifndef DISABLE_NETWORK
    { CMDLINE_TYPE_INTEGER, &_port,            NAC, "port",              "port to use for hosting or joining a server"                },
#endif
    { CMDLINE_TYPE_STRING,  &_userDataPath,    NAC, "user-data-path",    "path to the user data directory (containing config.ini)"    },
    { CMDLINE_TYPE_STRING,  &_openrctDataPath, NAC, "openrct-data-path", "path to the OpenRCT2 data directory (containing languages)" },
    OptionTableEnd
};

static exitcode_t HandleNoCommand(CommandLineArgEnumerator * enumerator);
static exitcode_t HandleCommandEdit(CommandLineArgEnumerator * enumerator);
static exitcode_t HandleCommandIntro(CommandLineArgEnumerator * enumerator);
static exitcode_t HandleCommandHost(CommandLineArgEnumerator * enumerator);
static exitcode_t HandleCommandJoin(CommandLineArgEnumerator * enumerator);

static void PrintAbout();
static void PrintVersion();
static void PrintLaunchInformation();

const CommandLineCommand CommandLine::RootCommands[]
{
    // Main commands
    DefineCommand("",      "<uri>",      StandardOptions, HandleNoCommand   ),
    DefineCommand("edit",  "<uri>",      StandardOptions, HandleCommandEdit ),
    DefineCommand("intro", "",           StandardOptions, HandleCommandIntro),
#ifndef DISABLE_NETWORK
    DefineCommand("host",  "<uri>",      StandardOptions, HandleCommandHost ),
    DefineCommand("join",  "<hostname>", StandardOptions, HandleCommandJoin ),
#endif

    // Sub-commands
    DefineSubCommand("screenshot", CommandLine::ScreenshotCommands),
    DefineSubCommand("sprite",     CommandLine::SpriteCommands    ),

    CommandTableEnd
};

const CommandLineExample CommandLine::RootExamples[]
{
    { "./my_park.sv6",                                "open a saved park"                      },
    { "./SnowyPark.sc6",                              "install and open a scenario"            },
    { "./ShuttleLoop.td6",                            "install a track"                        },
    { "https://openrct2.website/files/SnowyPark.sv6", "download and open a saved park"         },
#ifndef DISABLE_NETWORK
    { "host ./my_park.sv6 --port 11753 --headless",   "run a headless server for a saved park" },
#endif
    ExampleTableEnd
};

exitcode_t CommandLine::HandleCommandDefault()
{
    exitcode_t result = EXITCODE_CONTINUE;

    if (_about)
    {
        PrintAbout();
        result = EXITCODE_OK;
    } else
    {
        if (_verbose)
        {
            _log_levels[DIAGNOSTIC_LEVEL_VERBOSE] = 1;
            PrintLaunchInformation();
        }

        if (_version)
        {
            if (!_verbose)
            {
                PrintVersion();
            }
            result = EXITCODE_OK;
        }
    }

    if (_help)
    {
        CommandLine::PrintHelp(_all);
        result = EXITCODE_OK;
    }

    gOpenRCT2Headless = _headless;

    if (_userDataPath != NULL) {
        String::Set(gCustomUserDataPath, sizeof(gCustomUserDataPath), _userDataPath);
        Memory::Free(_userDataPath);
    }

    if (_openrctDataPath != NULL) {
        String::Set(gCustomOpenrctDataPath, sizeof(gCustomOpenrctDataPath), _openrctDataPath);
        Memory::Free(gCustomOpenrctDataPath);
    }

    return result;
}

exitcode_t HandleNoCommand(CommandLineArgEnumerator * enumerator)
{
    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    const char * parkUri;
    if (enumerator->TryPopString(&parkUri) && parkUri[0] != '-')
    {
        String::Set(gOpenRCT2StartupActionPath, sizeof(gOpenRCT2StartupActionPath), parkUri);
        gOpenRCT2StartupAction = STARTUP_ACTION_OPEN;
    }

    return EXITCODE_CONTINUE; 
}

exitcode_t HandleCommandEdit(CommandLineArgEnumerator * enumerator)
{
    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    const char * parkUri;
    if (!enumerator->TryPopString(&parkUri))
    {
        Console::WriteLineError("Expected path or URL to a saved park.");
        return EXITCODE_FAIL;
    }
    String::Set(gOpenRCT2StartupActionPath, sizeof(gOpenRCT2StartupActionPath), parkUri);

    gOpenRCT2StartupAction = STARTUP_ACTION_EDIT;
    return EXITCODE_CONTINUE;
}

exitcode_t HandleCommandIntro(CommandLineArgEnumerator * enumerator)
{
    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    gOpenRCT2StartupAction = STARTUP_ACTION_INTRO;
    return EXITCODE_CONTINUE;
}

#ifndef DISABLE_NETWORK

exitcode_t HandleCommandHost(CommandLineArgEnumerator * enumerator)
{
    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    const char * parkUri;
    if (!enumerator->TryPopString(&parkUri))
    {
        Console::WriteLineError("Expected path or URL to a saved park.");
        return EXITCODE_FAIL;
    }

    gOpenRCT2StartupAction = STARTUP_ACTION_OPEN;
    String::Set(gOpenRCT2StartupActionPath, sizeof(gOpenRCT2StartupActionPath), parkUri);

	gNetworkStart = NETWORK_MODE_SERVER;
    gNetworkStartPort = _port;
    return EXITCODE_CONTINUE;
}

exitcode_t HandleCommandJoin(CommandLineArgEnumerator * enumerator)
{
    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    const char * hostname;
    if (!enumerator->TryPopString(&hostname))
    {
        Console::WriteLineError("Expected a hostname or IP address to the server to connect to.");
        return EXITCODE_FAIL;
    }

	gNetworkStart = NETWORK_MODE_CLIENT;
    gNetworkStartPort = _port;
	String::Set(gNetworkStartHost, sizeof(gNetworkStartHost), hostname);
    return EXITCODE_CONTINUE;
}

#endif // DISABLE_NETWORK

static void PrintAbout()
{
    PrintVersion();
    Console::WriteLine();
    Console::WriteLine("OpenRCT2 is an amusement park simulation game based upon the popular game");
    Console::WriteLine("RollerCoaster Tycoon 2, written by Chris Sawyer. It attempts to mimic the ");
    Console::WriteLine("original game as closely as possible while extending it with new features.");
    Console::WriteLine("OpenRCT2 is licensed under the GNU General Public License version 3.0, but");
    Console::WriteLine("includes some 3rd party software under different licenses. See the file");
    Console::WriteLine("\"licence.txt\" shipped with the game for details.");
    Console::WriteLine();
    Console::WriteLine("Website:      https://openrct2.website");
    Console::WriteLine("GitHub:       https://github.com/OpenRCT2/OpenRCT2");
    Console::WriteLine("Contributors: https://github.com/OpenRCT2/OpenRCT2/blob/develop/contributors.md");
    Console::WriteLine();
}

static void PrintVersion()
{
    char buffer[256];
    openrct2_write_full_version_info(buffer, sizeof(buffer));
    Console::WriteLine(buffer);
    Console::WriteFormat("%s (%s)", OPENRCT2_PLATFORM, OPENRCT2_ARCHITECTURE);
    Console::WriteLine();
}

static void PrintLaunchInformation()
{
    char        buffer[256];
    time_t      timer;
    struct tm * tmInfo;

    // Print name and version information
    openrct2_write_full_version_info(buffer, sizeof(buffer));
    Console::WriteFormat("%s", buffer);
    Console::WriteLine();
    Console::WriteFormat("%s (%s)", OPENRCT2_PLATFORM, OPENRCT2_ARCHITECTURE);
    Console::WriteLine();
    Console::WriteFormat("@ %s", OPENRCT2_TIMESTAMP);
    Console::WriteLine();
    Console::WriteLine();

    // Print current time
    time(&timer);
    tmInfo = localtime(&timer);
    strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M:%S", tmInfo);
    Console::WriteFormat("VERBOSE: time is %s", buffer);
    Console::WriteLine();

    // TODO Print other potential information (e.g. user, hardware)
}
