#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include "core/Exception.hpp"
#include "core/Guard.hpp"
#include "core/Path.hpp"
#include "core/String.hpp"
#include "PlatformEnvironment.h"

extern "C"
{
    #include "openrct2.h"
    #include "platform/platform.h"
}

class PlatformEnvironment : public IPlatformEnvironment
{
private:
    std::string _basePath[4];

public:
    PlatformEnvironment(const std::string basePaths[4])
    {
        for (int i = 0; i < 4; i++)
        {
            _basePath[i] = basePaths[i];
        }
    }

    std::string GetDirectoryPath(DIRBASE base, DIRID did) const override
    {
        const utf8 * basePath = _basePath[base].c_str();
        const utf8 * directoryName;
        switch (base) {
        default:
        case DIRBASE_RCT1:
            throw Exception("Not implemented");
        case DIRBASE_RCT2:
            directoryName = DirectoryNamesRCT2[did];
            break;
        case DIRBASE_OPENRCT2:
        case DIRBASE_USER:
            directoryName = DirectoryNamesOpenRCT2[did];
            break;
        }

        utf8 path[260];
        String::Set(path, sizeof(path), basePath);
        Path::Append(path, sizeof(path), directoryName);
        return std::string(path);
    }

    std::string GetFilePath(PATHID pathid) const override
    {
        const utf8 * basePath = _basePath[DIRBASE_USER].c_str();
        const utf8 * fileName = FileNames[pathid];

        utf8 path[260];
        String::Set(path, sizeof(path), basePath);
        Path::Append(path, sizeof(path), fileName);
        return std::string(path);
    }

private:
    static const char * DirectoryNamesRCT2[];
    static const char * DirectoryNamesOpenRCT2[];
    static const char * FileNames[];
};

IPlatformEnvironment * CreatePlatformEnvironment()
{
    utf8 path[260];
    std::string basePaths[4];
    basePaths[DIRBASE_RCT2] = std::string(gRCT2AddressAppPath);
    platform_get_openrct_data_path(path, sizeof(path));
    basePaths[DIRBASE_OPENRCT2] = std::string(path);
    platform_get_user_directory(path, NULL, sizeof(path));
    basePaths[DIRBASE_USER] = std::string(path);
    return new PlatformEnvironment(basePaths);
}

const char * PlatformEnvironment::DirectoryNamesRCT2[] =
{
    "Data",                 // DIRID_DATA
    "Landscapes",           // DIRID_LANDSCAPE
    nullptr,                // DIRID_LANGUAGE
    nullptr,                // DIRID_LOG_CHAT
    nullptr,                // DIRID_LOG_SERVER
    nullptr,                // DIRID_NETWORK_KEY
    "ObjData",              // DIRID_OBJECT
    "Saved Games",          // DIRID_SAVE
    "Scenarios",            // DIRID_SCENARIO
    nullptr,                // DIRID_SCREENSHOT
    nullptr,                // DIRID_SEQUENCE
    nullptr,                // DIRID_SHADER
    nullptr,                // DIRID_THEME
    "Tracks",               // DIRID_TRACK
};

const char * PlatformEnvironment::DirectoryNamesOpenRCT2[] =
{
    "data",                 // DIRID_DATA
    "landscape",            // DIRID_LANDSCAPE
    "language",             // DIRID_LANGUAGE
    "chatlogs",             // DIRID_LOG_CHAT
    "serverlogs",           // DIRID_LOG_SERVER
    "keys",                 // DIRID_NETWORK_KEY
    "object",               // DIRID_OBJECT
    "save",                 // DIRID_SAVE
    "scenario",             // DIRID_SCENARIO
    "screenshot",           // DIRID_SCREENSHOT
    "sequence",             // DIRID_SEQUENCE
    "shader",               // DIRID_SHADER
    "themes",               // DIRID_THEME
    "track",                // DIRID_TRACK
};

const char * PlatformEnvironment::FileNames[] =
{
    "config.ini",           // PATHID_CONFIG
    "hotkeys.dat",          // PATHID_CONFIG_KEYBOARD
    "objects.idx",          // PATHID_CACHE_OBJECTS
    "tracks.idx",           // PATHID_CACHE_TRACKS
    "groups.json",          // PATHID_NETWORK_GROUPS
    "servers.cfg",          // PATHID_NETWORK_SERVERS
    "users.json",           // PATHID_NETWORK_USERS
    "highscores.dat",       // PATHID_SCORES
};
