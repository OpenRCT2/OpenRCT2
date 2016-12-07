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

#pragma once

#include <string>
#include "common.h"

enum DIRBASE
{
    DIRBASE_RCT1,           // Base directory for original RollerCoaster Tycoon 1 content.
    DIRBASE_RCT2,           // Base directory for original RollerCoaster Tycoon 2 content.
    DIRBASE_OPENRCT2,       // Base directory for OpenRCT2 installation.
    DIRBASE_USER,           // Base directory for OpenRCT2 user content.
};

enum DIRID
{
    DIRID_DATA,             // Contains g1.dat, music etc.
    DIRID_LANDSCAPE,        // Contains scenario editor landscapes (SC6).
    DIRID_LANGUAGE,         // Contains language packs.
    DIRID_LOG_CHAT,         // Contains chat logs.
    DIRID_LOG_SERVER,       // Contains server logs.
    DIRID_NETWORK_KEY,      // Contains the user's public and private keys.
    DIRID_OBJECT,           // Contains objects.
    DIRID_SAVE,             // Contains saved games (SV6).
    DIRID_SCENARIO,         // Contains scenarios (SC6).
    DIRID_SCREENSHOT,       // Contains screenshots.
    DIRID_SEQUENCE,         // Contains title sequences.
    DIRID_SHADER,           // Contains OpenGL shaders.
    DIRID_THEME,            // Contains interface themes.
    DIRID_TRACK,            // Contains track designs.
};

enum PATHID
{
    PATHID_CONFIG,              // Main configuration (config.ini).
    PATHID_CONFIG_KEYBOARD,     // Keyboard shortcuts. (hotkeys.cfg)
    PATHID_CACHE_OBJECTS,       // Object repository cache (objects.idx).
    PATHID_CACHE_TRACKS,        // Track repository cache (tracks.idx).
    PATHID_NETWORK_GROUPS,      // Server groups with permissions (groups.json).
    PATHID_NETWORK_SERVERS,     // Saved servers (servers.cfg).
    PATHID_NETWORK_USERS,       // Users and their groups (users.json).
    PATHID_SCORES,              // Scenario scores (highscores.dat).
};

/**
 * Interface for retrieving paths and other environment related things.
 */
interface IPlatformEnvironment
{
    virtual ~IPlatformEnvironment() = default;

    virtual std::string GetDirectoryPath(DIRBASE base, DIRID did) const abstract;
    virtual std::string GetFilePath(PATHID pathid) const abstract;
};

IPlatformEnvironment * CreatePlatformEnvironment();
