/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Context.h"
#include "../OpenRCT2.h"
#include "../PlatformEnvironment.h"
#include "../Version.h"
#include "../config/Config.h"
#include "../core/Console.hpp"
#include "../core/File.h"
#include "../core/Guard.hpp"
#include "../core/Memory.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../localisation/Language.h"
#include "../network/network.h"
#include "../object/ObjectRepository.h"
#include "../park/ParkFile.h"
#include "../platform/Crash.h"
#include "../platform/Platform.h"
#include "../scripting/ScriptEngine.h"
#include "CommandLine.hpp"

#include <ctime>
#include <iterator>
#include <string>

using namespace OpenRCT2;

#ifdef USE_BREAKPAD
    #define IMPLIES_SILENT_BREAKPAD ", implies --silent-breakpad"
#else
    #define IMPLIES_SILENT_BREAKPAD
#endif // USE_BREAKPAD

#ifndef DISABLE_NETWORK
int32_t gNetworkStart = NETWORK_MODE_NONE;
std::string gNetworkStartHost;
int32_t gNetworkStartPort = kNetworkDefaultPort;
std::string gNetworkStartAddress;

static uint32_t _port = 0;
static char* _address = nullptr;
#endif

static bool _help = false;
static bool _version = false;
static bool _noInstall = false;
static bool _all = false;
static bool _about = false;
static bool _verbose = false;
static bool _headless = false;
static bool _silentReplays = false;
static u8string _password = {};
static u8string _userDataPath = {};
static u8string _openrct2DataPath = {};
static u8string _rct1DataPath = {};
static u8string _rct2DataPath = {};
static bool _silentBreakpad = false;

// clang-format off
static constexpr CommandLineOptionDefinition kStandardOptions[]
{
    { CMDLINE_TYPE_SWITCH,  &_help,             'h', "help",               "show this help message and exit"                            },
    { CMDLINE_TYPE_SWITCH,  &_version,          'v', "version",            "show version information and exit"                          },
    { CMDLINE_TYPE_SWITCH,  &_noInstall,        'n', "no-install",         "do not install scenario if passed"                          },
    { CMDLINE_TYPE_SWITCH,  &_all,              'a', "all",                "show help for all commands"                                 },
    { CMDLINE_TYPE_SWITCH,  &_about,            kNAC, "about",              "show information about " OPENRCT2_NAME                      },
    { CMDLINE_TYPE_SWITCH,  &_verbose,          kNAC, "verbose",            "log verbose messages"                                       },
    { CMDLINE_TYPE_SWITCH,  &_headless,         kNAC, "headless",           "run " OPENRCT2_NAME " headless" IMPLIES_SILENT_BREAKPAD     },
    { CMDLINE_TYPE_SWITCH,  &_silentReplays,    kNAC, "silent-replays",     "use unobtrusive replays"                                    },
#ifndef DISABLE_NETWORK
    { CMDLINE_TYPE_INTEGER, &_port,             kNAC, "port",               "port to use for hosting or joining a server"                },
    { CMDLINE_TYPE_STRING,  &_address,          kNAC, "address",            "address to listen on when hosting a server"                 },
#endif
    { CMDLINE_TYPE_STRING,  &_password,         kNAC, "password",           "password needed to join the server"                         },
    { CMDLINE_TYPE_STRING,  &_userDataPath,     kNAC, "user-data-path",     "path to the user data directory (containing config.ini)"    },
    { CMDLINE_TYPE_STRING,  &_openrct2DataPath, kNAC, "openrct2-data-path", "path to the OpenRCT2 data directory (containing languages)" },
    { CMDLINE_TYPE_STRING,  &_rct1DataPath,     kNAC, "rct1-data-path",     "path to the RollerCoaster Tycoon 1 data directory (containing data/csg1.dat)" },
    { CMDLINE_TYPE_STRING,  &_rct2DataPath,     kNAC, "rct2-data-path",     "path to the RollerCoaster Tycoon 2 data directory (containing data/g1.dat)" },
#ifdef USE_BREAKPAD
    { CMDLINE_TYPE_SWITCH,  &_silentBreakpad,  kNAC, "silent-breakpad",   "make breakpad crash reporting silent"                       },
#endif // USE_BREAKPAD
    kOptionTableEnd
};

static exitcode_t HandleNoCommand(CommandLineArgEnumerator * enumerator);
static exitcode_t HandleCommandEdit(CommandLineArgEnumerator * enumerator);
static exitcode_t HandleCommandIntro(CommandLineArgEnumerator * enumerator);
#ifndef DISABLE_NETWORK
static exitcode_t HandleCommandHost(CommandLineArgEnumerator * enumerator);
static exitcode_t HandleCommandJoin(CommandLineArgEnumerator * enumerator);
#endif
static exitcode_t HandleCommandSetRCT2(CommandLineArgEnumerator * enumerator);
static exitcode_t HandleCommandScanObjects(CommandLineArgEnumerator * enumerator);

#if defined(_WIN32)

static bool _removeShell = false;

static constexpr CommandLineOptionDefinition RegisterShellOptions[]
{
    { CMDLINE_TYPE_SWITCH, &_removeShell, 'd', "remove", "remove shell integration" },
};

static exitcode_t HandleCommandRegisterShell(CommandLineArgEnumerator * enumerator);

#endif

static void PrintAbout();
static void PrintVersion();
static void PrintLaunchInformation();

const CommandLineCommand CommandLine::kRootCommands[]
{
    // Main commands
#ifndef DISABLE_HTTP
    DefineCommand("",         "<uri>",                  kStandardOptions, HandleNoCommand     ),
    DefineCommand("edit",     "<uri>",                  kStandardOptions, HandleCommandEdit   ),
#else
    DefineCommand("",         "<path>",                 kStandardOptions, HandleNoCommand     ),
    DefineCommand("edit",     "<path>",                 kStandardOptions, HandleCommandEdit   ),
#endif
    DefineCommand("intro",    "",                       kStandardOptions, HandleCommandIntro  ),
#ifndef DISABLE_NETWORK
    DefineCommand("host",     "<uri>",                  kStandardOptions, HandleCommandHost   ),
    DefineCommand("join",     "<hostname>",             kStandardOptions, HandleCommandJoin   ),
#endif
    DefineCommand("set-rct2", "<path>",                 kStandardOptions, HandleCommandSetRCT2),
    DefineCommand("convert",  "<source> <destination>", kStandardOptions, CommandLine::HandleCommandConvert),
    DefineCommand("scan-objects", "<path>",             kStandardOptions, HandleCommandScanObjects),
    DefineCommand("handle-uri", "openrct2://.../",      kStandardOptions, CommandLine::HandleCommandUri),

#if defined(_WIN32)
    DefineCommand("register-shell", "", RegisterShellOptions, HandleCommandRegisterShell),
#endif

    // Sub-commands
    DefineSubCommand("screenshot",      CommandLine::kScreenshotCommands       ),
    DefineSubCommand("sprite",          CommandLine::kSpriteCommands           ),
    DefineSubCommand("simulate",        CommandLine::kSimulateCommands         ),
    DefineSubCommand("parkinfo",        CommandLine::kParkInfoCommands         ),
    kCommandTableEnd
};

const CommandLineExample CommandLine::kRootExamples[]
{
    { "./my_park.sv6",                                "open a saved park"                      },
    { "./SnowyPark.sc6",                              "install and open a scenario"            },
    { "./ShuttleLoop.td6",                            "install a track"                        },
#ifndef DISABLE_HTTP
    { "https://example.org/files/ExamplePark.sv6", "download and open a saved park"         },
#endif
#ifndef DISABLE_NETWORK
    { "host ./my_park.sv6 --port 11753 --headless",   "run a headless server for a saved park" },
#endif
    kExampleTableEnd
};
// clang-format on

exitcode_t CommandLine::HandleCommandDefault()
{
    exitcode_t result = EXITCODE_CONTINUE;

    if (_about)
    {
        PrintAbout();
        result = EXITCODE_OK;
    }
    else
    {
        if (_verbose)
        {
            _log_levels[EnumValue(DiagnosticLevel::Verbose)] = true;
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

    if (_help || _all)
    {
        CommandLine::PrintHelp(_all);
        result = EXITCODE_OK;
    }

    gOpenRCT2Headless = _headless;
    gOpenRCT2NoGraphics = _headless;
    gOpenRCT2SilentBreakpad = _silentBreakpad || _headless;

    if (!_userDataPath.empty())
    {
        gCustomUserDataPath = Path::GetAbsolute(_userDataPath);
    }

    if (!_openrct2DataPath.empty())
    {
        gCustomOpenRCT2DataPath = Path::GetAbsolute(_openrct2DataPath);
    }

    if (!_rct1DataPath.empty())
    {
        gCustomRCT1DataPath = Path::GetAbsolute(_rct1DataPath);
    }

    if (!_rct2DataPath.empty())
    {
        gCustomRCT2DataPath = Path::GetAbsolute(_rct2DataPath);
    }

    if (!_password.empty())
    {
        gCustomPassword = _password;
    }

    if (_silentReplays)
    {
        gSilentReplays = _silentReplays;
    }

    return result;
}

exitcode_t HandleNoCommand(CommandLineArgEnumerator* enumerator)
{
    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    const char* parkUri;
    if (enumerator->TryPopString(&parkUri) && parkUri[0] != '-')
    {
        String::set(gOpenRCT2StartupActionPath, sizeof(gOpenRCT2StartupActionPath), parkUri);
        gOpenRCT2StartupAction = StartupAction::Open;
    }

    return EXITCODE_CONTINUE;
}

exitcode_t HandleCommandEdit(CommandLineArgEnumerator* enumerator)
{
    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    const char* parkUri;
    if (!enumerator->TryPopString(&parkUri))
    {
        Console::Error::WriteLine("Expected path or URL to a saved park.");
        return EXITCODE_FAIL;
    }
    String::set(gOpenRCT2StartupActionPath, sizeof(gOpenRCT2StartupActionPath), parkUri);

    gOpenRCT2StartupAction = StartupAction::Edit;
    return EXITCODE_CONTINUE;
}

exitcode_t HandleCommandIntro([[maybe_unused]] CommandLineArgEnumerator* enumerator)
{
    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    gOpenRCT2StartupAction = StartupAction::Intro;
    return EXITCODE_CONTINUE;
}

#ifndef DISABLE_NETWORK

exitcode_t HandleCommandHost(CommandLineArgEnumerator* enumerator)
{
    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    const char* parkUri;
    if (!enumerator->TryPopString(&parkUri))
    {
        Console::Error::WriteLine("Expected path or URL to a scenario or saved park.");
        return EXITCODE_FAIL;
    }

    gOpenRCT2StartupAction = StartupAction::Open;
    String::set(gOpenRCT2StartupActionPath, sizeof(gOpenRCT2StartupActionPath), parkUri);

    gNetworkStart = NETWORK_MODE_SERVER;
    gNetworkStartPort = _port;
    gNetworkStartAddress = String::toStd(_address);

    return EXITCODE_CONTINUE;
}

exitcode_t HandleCommandJoin(CommandLineArgEnumerator* enumerator)
{
    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    const char* hostname;
    if (!enumerator->TryPopString(&hostname))
    {
        Console::Error::WriteLine("Expected a hostname or IP address to the server to connect to.");
        return EXITCODE_FAIL;
    }

    gNetworkStart = NETWORK_MODE_CLIENT;
    gNetworkStartPort = _port;
    gNetworkStartHost = hostname;
    return EXITCODE_CONTINUE;
}

#endif // DISABLE_NETWORK

static exitcode_t HandleCommandSetRCT2(CommandLineArgEnumerator* enumerator)
{
    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    // Get the path that was passed
    const utf8* rawPath;
    if (!enumerator->TryPopString(&rawPath))
    {
        Console::Error::WriteLine("Expected a path.");
        return EXITCODE_FAIL;
    }

    const auto path = Path::GetAbsolute(rawPath);

    // Check if path exists
    Console::WriteLine("Checking path...");
    if (!Path::DirectoryExists(path))
    {
        Console::Error::WriteLine("The path '%s' does not exist", path.c_str());
        return EXITCODE_FAIL;
    }

    // Check if g1.dat exists (naive but good check)
    Console::WriteLine("Checking g1.dat...");

    auto pathG1Check = Path::Combine(path, u8"Data", u8"g1.dat");
    if (!File::Exists(pathG1Check))
    {
        Console::Error::WriteLine("RCT2 path not valid.");
        Console::Error::WriteLine("Unable to find %s.", pathG1Check.c_str());
        return EXITCODE_FAIL;
    }

    // Update RCT2 path in config
    auto env = OpenRCT2::CreatePlatformEnvironment();
    auto configPath = env->GetFilePath(OpenRCT2::PATHID::CONFIG);
    Config::SetDefaults();
    Config::OpenFromPath(configPath);
    Config::Get().general.RCT2Path = path;
    if (Config::SaveToPath(configPath))
    {
        Console::WriteFormat("Updating RCT2 path to '%s'.", path.c_str());
        Console::WriteLine();
        Console::WriteLine("Updated config.ini");
        return EXITCODE_OK;
    }

    Console::Error::WriteLine("Unable to update config.ini");
    return EXITCODE_FAIL;
}

static exitcode_t HandleCommandScanObjects([[maybe_unused]] CommandLineArgEnumerator* enumerator)
{
    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    gOpenRCT2Headless = true;
    gOpenRCT2NoGraphics = true;

    auto context = OpenRCT2::CreateContext();
    auto env = context->GetPlatformEnvironment();
    auto objectRepository = CreateObjectRepository(env);
    objectRepository->Construct(Config::Get().general.Language);
    return EXITCODE_OK;
}

#if defined(_WIN32)
static exitcode_t HandleCommandRegisterShell([[maybe_unused]] CommandLineArgEnumerator* enumerator)
{
    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    if (!_removeShell)
    {
        Platform::SetUpFileAssociations();
    }
    else
    {
        Platform::RemoveFileAssociations();
    }
    return EXITCODE_OK;
}
#endif // defined(_WIN32)

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
    Console::WriteLine("Website:        https://openrct2.io");
    Console::WriteLine("GitHub:         https://github.com/OpenRCT2/OpenRCT2");
    Console::WriteLine("Contributors:   https://github.com/OpenRCT2/OpenRCT2/blob/develop/contributors.md");
    Console::WriteLine("Privacy Policy: https://github.com/OpenRCT2/OpenRCT2/blob/develop/PRIVACY.md");
    Console::WriteLine();
}

static void PrintVersion()
{
    u8string versionInfo = gVersionInfoFull;
    Console::WriteLine(versionInfo.c_str());
    Console::WriteFormat("%s (%s)", OPENRCT2_PLATFORM, OPENRCT2_ARCHITECTURE);
    Console::WriteLine();
    Console::WriteFormat("Network version: %s", NetworkGetVersion().c_str());
    Console::WriteLine();
#ifdef ENABLE_SCRIPTING
    Console::WriteFormat("Plugin API version: %d", OpenRCT2::Scripting::kPluginApiVersion);
    Console::WriteLine();
#else
    Console::WriteFormat("Plugin API not enabled in this build");
    Console::WriteLine();
#endif
    Console::WriteFormat("Current park file version: %d", OpenRCT2::kParkFileCurrentVersion);
    Console::WriteLine();
    Console::WriteFormat("Minimum park file version: %d", OpenRCT2::kParkFileMinVersion);
    Console::WriteLine();
#ifdef USE_BREAKPAD
    Console::WriteFormat("With breakpad support enabled");
    Console::WriteLine();
#else
    Console::WriteFormat("Breakpad support disabled");
    Console::WriteLine();
#endif
}

static void PrintLaunchInformation()
{
    char buffer[256];
    time_t timer;
    struct tm* tmInfo;

    // Print name and version information
    u8string versionInfo = gVersionInfoFull;
    Console::WriteLine(versionInfo.c_str());
    Console::WriteFormat("%s (%s)", OPENRCT2_PLATFORM, OPENRCT2_ARCHITECTURE);
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
