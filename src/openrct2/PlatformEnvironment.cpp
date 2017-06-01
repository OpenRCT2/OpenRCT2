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

#include "core/Exception.hpp"
#include "core/Guard.hpp"
#include "core/Path.hpp"
#include "core/String.hpp"
#include "PlatformEnvironment.h"

extern "C"
{
    #include "platform/platform.h"
}

class PlatformEnvironment : public IPlatformEnvironment
{
private:
    std::string _basePath[DIRBASE_COUNT];

public:
    PlatformEnvironment(DIRBASE_VALUES basePaths)
    {
        for (sint32 i = 0; i < 4; i++)
        {
            _basePath[i] = basePaths[i];
        }
    }

    std::string GetDirectoryPath(DIRBASE base, DIRID did) const override
    {
        const utf8 * basePath = _basePath[(size_t)base].c_str();
        const utf8 * directoryName;
        switch (base) {
        default:
        case DIRBASE::RCT1:
        case DIRBASE::RCT2:
            directoryName = DirectoryNamesRCT2[(size_t)did];
            break;
        case DIRBASE::OPENRCT2:
        case DIRBASE::USER:
            directoryName = DirectoryNamesOpenRCT2[(size_t)did];
            break;
        }

        utf8 path[260];
        String::Set(path, sizeof(path), basePath);
        Path::Append(path, sizeof(path), directoryName);
        return std::string(path);
    }

    std::string GetFilePath(PATHID pathid) const override
    {
        const utf8 * fileName = FileNames[(size_t)pathid];
        const utf8 * basePath = _basePath[(size_t)DIRBASE::USER].c_str();
        if (pathid == PATHID::SCORES_RCT2)
        {
            basePath = _basePath[(size_t)DIRBASE::RCT2].c_str();
        }

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

IPlatformEnvironment * CreatePlatformEnvironment(DIRBASE_VALUES basePaths)
{
    return new PlatformEnvironment(basePaths);
}

const char * PlatformEnvironment::DirectoryNamesRCT2[] =
{
    "Data",                 // DATA
    "Landscapes",           // LANDSCAPE
    nullptr,                // LANGUAGE
    nullptr,                // LOG_CHAT
    nullptr,                // LOG_SERVER
    nullptr,                // NETWORK_KEY
    "ObjData",              // OBJECT
    "Saved Games",          // SAVE
    "Scenarios",            // SCENARIO
    nullptr,                // SCREENSHOT
    nullptr,                // SEQUENCE
    nullptr,                // SHADER
    nullptr,                // THEME
    "Tracks",               // TRACK
};

const char * PlatformEnvironment::DirectoryNamesOpenRCT2[] =
{
    "data",                 // DATA
    "landscape",            // LANDSCAPE
    "language",             // LANGUAGE
    "chatlogs",             // LOG_CHAT
    "serverlogs",           // LOG_SERVER
    "keys",                 // NETWORK_KEY
    "object",               // OBJECT
    "save",                 // SAVE
    "scenario",             // SCENARIO
    "screenshot",           // SCREENSHOT
    "sequence",             // SEQUENCE
    "shader",               // SHADER
    "themes",               // THEME
    "track",                // TRACK
};

const char * PlatformEnvironment::FileNames[] =
{
    "config.ini",           // CONFIG
    "hotkeys.dat",          // CONFIG_KEYBOARD
    "objects.idx",          // CACHE_OBJECTS
    "tracks.idx",           // CACHE_TRACKS
    "groups.json",          // NETWORK_GROUPS
    "servers.cfg",          // NETWORK_SERVERS
    "users.json",           // NETWORK_USERS
    "highscores.dat",       // SCORES
    "scores.dat",           // SCORES (LEGACY)
    "Saved Games" PATH_SEPARATOR "scores.dat",  // SCORES (RCT2)
};
