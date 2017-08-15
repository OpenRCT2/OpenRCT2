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

#include <time.h>

#include "../core/Guard.hpp"

extern "C"
{
    #include "../config/Config.h"
    #include "../platform/crash.h"
    #include "../platform/platform.h"
}

#include "../core/Console.hpp"
#include "../core/Memory.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../network/network.h"
#include "../object/ObjectRepository.h"
#include "../OpenRCT2.h"
#include "../Version.h"
#include "CommandLine.hpp"

#ifdef USE_BREAKPAD
#define IMPLIES_SILENT_BREAKPAD ", implies --silent-breakpad"
#else
#define IMPLIES_SILENT_BREAKPAD
#endif // USE_BREAKPAD

#ifndef DISABLE_NETWORK
sint32  gNetworkStart = NETWORK_MODE_NONE;
char gNetworkStartHost[128];
sint32  gNetworkStartPort = NETWORK_DEFAULT_PORT;
char* gNetworkStartAddress = nullptr;

static uint32 _port            = 0;
static char*  _address         = nullptr;
#endif

static bool   _help            = false;
static bool   _version         = false;
static bool   _noInstall       = false;
static bool   _all             = false;
static bool   _about           = false;
static bool   _verbose         = false;
static bool   _headless        = false;
static utf8 * _password        = nullptr;
static utf8 * _userDataPath    = nullptr;
static utf8 * _openrctDataPath = nullptr;
static utf8 * _rct2DataPath    = nullptr;
static bool   _silentBreakpad  = false;

static const CommandLineOptionDefinition StandardOptions[]
{
    { CMDLINE_TYPE_SWITCH,  &_help,            'h', "help",              "show this help message and exit"                            },
    { CMDLINE_TYPE_SWITCH,  &_version,         'v', "version",           "show version information and exit"                          },
    { CMDLINE_TYPE_SWITCH,  &_noInstall,       'n', "no-install",        "do not install scenario if passed"                          },
    { CMDLINE_TYPE_SWITCH,  &_all,             'a', "all",               "show help for all commands"                                 },
    { CMDLINE_TYPE_SWITCH,  &_about,           NAC, "about",             "show information about " OPENRCT2_NAME                      },
    { CMDLINE_TYPE_SWITCH,  &_verbose,         NAC, "verbose",           "log verbose messages"                                       },
    { CMDLINE_TYPE_SWITCH,  &_headless,        NAC, "headless",          "run " OPENRCT2_NAME " headless" IMPLIES_SILENT_BREAKPAD     },
#ifndef DISABLE_NETWORK
    { CMDLINE_TYPE_INTEGER, &_port,            NAC, "port",              "port to use for hosting or joining a server"                },
    { CMDLINE_TYPE_STRING,  &_address,         NAC, "address",           "address to listen on when hosting a server"                 },
#endif
    { CMDLINE_TYPE_STRING,  &_password,        NAC, "password",          "password needed to join the server"                         },
    { CMDLINE_TYPE_STRING,  &_userDataPath,    NAC, "user-data-path",    "path to the user data directory (containing config.ini)"    },
    { CMDLINE_TYPE_STRING,  &_openrctDataPath, NAC, "openrct-data-path", "path to the OpenRCT2 data directory (containing languages)" },
    { CMDLINE_TYPE_STRING,  &_rct2DataPath,    NAC, "rct2-data-path",    "path to the RollerCoaster Tycoon 2 data directory (containing data/g1.dat)" },
#ifdef USE_BREAKPAD
    { CMDLINE_TYPE_SWITCH,  &_silentBreakpad,  NAC, "silent-breakpad",   "make breakpad crash reporting silent"                       },
#endif // USE_BREAKPAD
    OptionTableEnd
};

static exitcode_t HandleNoCommand(CommandLineArgEnumerator * enumerator);
static exitcode_t HandleCommandEdit(CommandLineArgEnumerator * enumerator);
static exitcode_t HandleCommandIntro(CommandLineArgEnumerator * enumerator);
static exitcode_t HandleCommandHost(CommandLineArgEnumerator * enumerator);
static exitcode_t HandleCommandJoin(CommandLineArgEnumerator * enumerator);
static exitcode_t HandleCommandSetRCT2(CommandLineArgEnumerator * enumerator);
static exitcode_t HandleCommandScanObjects(CommandLineArgEnumerator * enumerator);

#if defined(_WIN32) && !defined(__MINGW32__)

static bool _removeShell = false;

static const CommandLineOptionDefinition RegisterShellOptions[]
{
    { CMDLINE_TYPE_SWITCH, &_removeShell, 'd', "remove", "remove shell integration" },
};

static exitcode_t HandleCommandRegisterShell(CommandLineArgEnumerator * enumerator);

#endif

static void PrintAbout();
static void PrintVersion();
static void PrintLaunchInformation();

const CommandLineCommand CommandLine::RootCommands[]
{
    // Main commands
#ifndef DISABLE_HTTP
    DefineCommand("",         "<uri>",                  StandardOptions, HandleNoCommand     ),
    DefineCommand("edit",     "<uri>",                  StandardOptions, HandleCommandEdit   ),
#else
    DefineCommand("",         "<path>",                 StandardOptions, HandleNoCommand     ),
    DefineCommand("edit",     "<path>",                 StandardOptions, HandleCommandEdit   ),
#endif
    DefineCommand("intro",    "",                       StandardOptions, HandleCommandIntro  ),
#ifndef DISABLE_NETWORK
    DefineCommand("host",     "<uri>",                  StandardOptions, HandleCommandHost   ),
    DefineCommand("join",     "<hostname>",             StandardOptions, HandleCommandJoin   ),
#endif
    DefineCommand("set-rct2", "<path>",                 StandardOptions, HandleCommandSetRCT2),
    DefineCommand("convert",  "<source> <destination>", StandardOptions, CommandLine::HandleCommandConvert),
    DefineCommand("scan-objects", "<path>",             StandardOptions, HandleCommandScanObjects),
    DefineCommand("handle-uri", "openrct2://.../",      StandardOptions, CommandLine::HandleCommandUri),

#if defined(_WIN32) && !defined(__MINGW32__)
    DefineCommand("register-shell", "", RegisterShellOptions, HandleCommandRegisterShell),
#endif

    // Sub-commands
    DefineSubCommand("screenshot", CommandLine::ScreenshotCommands),
    DefineSubCommand("sprite",     CommandLine::SpriteCommands    ),
    DefineSubCommand("benchgfx",   CommandLine::BenchGfxCommands  ),

    CommandTableEnd
};

const CommandLineExample CommandLine::RootExamples[]
{
    { "./my_park.sv6",                                "open a saved park"                      },
    { "./SnowyPark.sc6",                              "install and open a scenario"            },
    { "./ShuttleLoop.td6",                            "install a track"                        },
#ifndef DISABLE_HTTP
    { "https://openrct2.website/files/SnowyPark.sv6", "download and open a saved park"         },
#endif
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
            _log_levels[DIAGNOSTIC_LEVEL_VERBOSE] = true;
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
    gOpenRCT2NoGraphics = _headless;
    gOpenRCT2SilentBreakpad = _silentBreakpad || _headless;

    if (_userDataPath != nullptr)
    {
        String::Set(gCustomUserDataPath, sizeof(gCustomUserDataPath), _userDataPath);
        Memory::Free(_userDataPath);
    }

    if (_openrctDataPath != nullptr)
    {
        String::Set(gCustomOpenrctDataPath, sizeof(gCustomOpenrctDataPath), _openrctDataPath);
        Memory::Free(_openrctDataPath);
    }

    if (_rct2DataPath != nullptr)
    {
        String::Set(gCustomRCT2DataPath, sizeof(gCustomRCT2DataPath), _rct2DataPath);
        Memory::Free(_rct2DataPath);
    }

    if (_password != nullptr)
    {
        String::Set(gCustomPassword, sizeof(gCustomPassword), _password);
        Memory::Free(_password);
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
        Console::Error::WriteLine("Expected path or URL to a saved park.");
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
        Console::Error::WriteLine("Expected path or URL to a scenario or saved park.");
        return EXITCODE_FAIL;
    }

    gOpenRCT2StartupAction = STARTUP_ACTION_OPEN;
    String::Set(gOpenRCT2StartupActionPath, sizeof(gOpenRCT2StartupActionPath), parkUri);

    gNetworkStart = NETWORK_MODE_SERVER;
    gNetworkStartPort = _port;
    gNetworkStartAddress = _address;

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
        Console::Error::WriteLine("Expected a hostname or IP address to the server to connect to.");
        return EXITCODE_FAIL;
    }

    gNetworkStart = NETWORK_MODE_CLIENT;
    gNetworkStartPort = _port;
    String::Set(gNetworkStartHost, sizeof(gNetworkStartHost), hostname);
    return EXITCODE_CONTINUE;
}

#endif // DISABLE_NETWORK

static exitcode_t HandleCommandSetRCT2(CommandLineArgEnumerator * enumerator)
{
    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    // Get the path that was passed
    const utf8 * rawPath;
    if (!enumerator->TryPopString(&rawPath))
    {
        Console::Error::WriteLine("Expected a path.");
        return EXITCODE_FAIL;
    }

    utf8 path[MAX_PATH];
    Path::GetAbsolute(path, sizeof(path), rawPath);

    // Check if path exists
    Console::WriteLine("Checking path...");
    if (!platform_directory_exists(path))
    {
        Console::Error::WriteLine("The path '%s' does not exist", path);
        return EXITCODE_FAIL;
    }

    // Check if g1.dat exists (naive but good check)
    Console::WriteLine("Checking g1.dat...");

    utf8 pathG1Check[MAX_PATH];
    String::Set(pathG1Check, sizeof(pathG1Check), path);
    Path::Append(pathG1Check, sizeof(pathG1Check), "Data");
    Path::Append(pathG1Check, sizeof(pathG1Check), "g1.dat");
    if (!platform_file_exists(pathG1Check))
    {
        Console::Error::WriteLine("RCT2 path not valid.");
        Console::Error::WriteLine("Unable to find %s.", pathG1Check);
        return EXITCODE_FAIL;
    }

    // Check user path that will contain the config
    utf8 userPath[MAX_PATH];
    platform_resolve_user_data_path();
    platform_get_user_directory(userPath, nullptr, sizeof(userPath));
    if (!platform_ensure_directory_exists(userPath)) {
        Console::Error::WriteLine("Unable to access or create directory '%s'.", userPath);
        return EXITCODE_FAIL;
    }

    // Update RCT2 path in config
    config_set_defaults();
    config_open_default();
    String::DiscardDuplicate(&gConfigGeneral.rct2_path, path);
    if (config_save_default())
    {
        Console::WriteFormat("Updating RCT2 path to '%s'.", path);
        Console::WriteLine();
        Console::WriteLine("Updated config.ini");
        return EXITCODE_OK;
    }
    else
    {
        Console::Error::WriteLine("Unable to update config.ini");
        return EXITCODE_FAIL;
    }
}

static exitcode_t HandleCommandScanObjects(CommandLineArgEnumerator * enumerator)
{
    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    // IPlatformEnvironment * env = OpenRCT2::SetupEnvironment();
    // IObjectRepository * objectRepository = CreateObjectRepository(env);
    // objectRepository->Construct();
    return EXITCODE_OK;
}

#if defined(_WIN32) && !defined(__MINGW32__)
static exitcode_t HandleCommandRegisterShell(CommandLineArgEnumerator * enumerator)
{
    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    if (!_removeShell)
    {
        platform_setup_file_associations();
    }
    else
    {
        platform_remove_file_associations();
    }
    return EXITCODE_OK;
}
#endif // defined(_WIN32) && !defined(__MINGW32__)

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
#ifdef NO_RCT2
    Console::Write(" (NO_RCT2)");
#endif
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
