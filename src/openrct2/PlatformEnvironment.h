/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include <string>
#include "common.h"

namespace OpenRCT2
{
    enum class DIRBASE : int32_t
    {
        RCT1,               // Base directory for original RollerCoaster Tycoon 1 content.
        RCT2,               // Base directory for original RollerCoaster Tycoon 2 content.
        OPENRCT2,           // Base directory for OpenRCT2 installation.
        USER,               // Base directory for OpenRCT2 user content.
        CONFIG,             // Base directory for OpenRCT2 configuration.
        CACHE,              // Base directory for OpenRCT2 cache files.
        DOCUMENTATION,      // Base directory for OpenRCT2 doc files.
    };
    constexpr int32_t DIRBASE_COUNT = 7;
    using DIRBASE_VALUES = std::string[DIRBASE_COUNT];

    enum class DIRID
    {
        DATA,               // Contains g1.dat, music etc.
        LANDSCAPE,          // Contains scenario editor landscapes (SC6).
        LANGUAGE,           // Contains language packs.
        LOG_CHAT,           // Contains chat logs.
        LOG_SERVER,         // Contains server logs.
        NETWORK_KEY,        // Contains the user's public and private keys.
        OBJECT,             // Contains objects.
        SAVE,               // Contains saved games (SV6).
        SCENARIO,           // Contains scenarios (SC6).
        SCREENSHOT,         // Contains screenshots.
        SEQUENCE,           // Contains title sequences.
        SHADER,             // Contains OpenGL shaders.
        THEME,              // Contains interface themes.
        TRACK,              // Contains track designs.
    };

    enum class PATHID
    {
        CONFIG,             // Main configuration (config.ini).
        CONFIG_KEYBOARD,    // Keyboard shortcuts. (hotkeys.cfg)
        CACHE_OBJECTS,      // Object repository cache (objects.idx).
        CACHE_TRACKS,       // Track repository cache (tracks.idx).
        CACHE_SCENARIOS,    // Scenario repository cache (scenarios.idx).
        MP_DAT,             // Mega Park data, Steam RCT1 only (\RCTdeluxe_install\Data\mp.dat)
        NETWORK_GROUPS,     // Server groups with permissions (groups.json).
        NETWORK_SERVERS,    // Saved servers (servers.cfg).
        NETWORK_USERS,      // Users and their groups (users.json).
        SCORES,             // Scenario scores (highscores.dat).
        SCORES_LEGACY,      // Scenario scores, legacy (scores.dat).
        SCORES_RCT2,        // Scenario scores, rct2 (\Saved Games\scores.dat).
        CHANGELOG,          // Notable changes to the game between versions, distributed with the game.
    };

    /**
     * Interface for retrieving paths and other environment related things.
     */
    interface IPlatformEnvironment
    {
        virtual ~IPlatformEnvironment() = default;

        virtual std::string GetDirectoryPath(DIRBASE base) const abstract;
        virtual std::string GetDirectoryPath(DIRBASE base, DIRID did) const abstract;
        virtual std::string GetFilePath(PATHID pathid) const abstract;
        virtual void SetBasePath(DIRBASE base, const std::string &path) abstract;
    };

    std::unique_ptr<IPlatformEnvironment> CreatePlatformEnvironment(DIRBASE_VALUES basePaths);
    std::unique_ptr<IPlatformEnvironment> CreatePlatformEnvironment();

} // namespace OpenRCT2
