/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Diagnostic.h"
#include "core/StringTypes.h"

#include <memory>

namespace OpenRCT2
{
    enum class DirBase : size_t
    {
        rct1,          // Base directory for original RollerCoaster Tycoon 1 content.
        rct2,          // Base directory for original RollerCoaster Tycoon 2 content.
        openrct2,      // Base directory for OpenRCT2 installation.
        user,          // Base directory for OpenRCT2 user content.
        config,        // Base directory for OpenRCT2 configuration.
        cache,         // Base directory for OpenRCT2 cache files.
        documentation, // Base directory for OpenRCT2 doc files.
    };
    constexpr size_t kDirBaseCount = 7;
    using DirBaseValues = u8string[kDirBaseCount];

    enum class DirId
    {
        data,             // Contains g1.dat, music etc.
        landscapes,       // Contains scenario editor landscapes (SC6).
        languages,        // Contains language packs.
        chatLogs,         // Contains chat logs.
        serverLogs,       // Contains server logs.
        networkKeys,      // Contains the user's public and private keys.
        objects,          // Contains objects.
        plugins,          // Contains plugins (.js).
        saves,            // Contains saved games (SV6).
        scenarios,        // Contains scenarios (SC6).
        screenshots,      // Contains screenshots.
        sequences,        // Contains title sequences.
        shaders,          // Contains OpenGL shaders.
        themes,           // Contains interface themes.
        trackDesigns,     // Contains track designs.
        heightmaps,       // Contains heightmap data.
        replayRecordings, // Contains recorded replays.
        desyncLogs,       // Contains desync reports.
        crashDumps,       // Contains crash dumps.
        assetPacks,       // Contains asset packs.
        scenarioPatches,  // Contains scenario patches.
    };

    enum class PATHID
    {
        CONFIG,                  // Main configuration (config.ini).
        CONFIG_SHORTCUTS_LEGACY, // Old keyboard shortcuts (hotkeys.cfg)
        CONFIG_SHORTCUTS,        // Shortcut bindings (shortcuts.json)
        CACHE_OBJECTS,           // Object repository cache (objects.idx).
        CACHE_TRACKS,            // Track repository cache (tracks.idx).
        CACHE_SCENARIOS,         // Scenario repository cache (scenarios.idx).
        NETWORK_GROUPS,          // Server groups with permissions (groups.json).
        NETWORK_SERVERS,         // Saved servers (servers.cfg).
        NETWORK_USERS,           // Users and their groups (users.json).
        SCORES,                  // Scenario scores (highscores.dat).
        SCORES_LEGACY,           // Scenario scores, legacy (scores.dat).
        SCORES_RCT2,             // Scenario scores, rct2 (\Saved Games\scores.dat).
        CHANGELOG,               // Notable changes to the game between versions, distributed with the game.
        PLUGIN_STORE,            // Shared storage for plugins.
        CONTRIBUTORS,            // Who has provided work to the game (Contributors.md).
    };

    /**
     * Interface for retrieving paths and other environment related things.
     */
    struct IPlatformEnvironment
    {
        virtual ~IPlatformEnvironment() = default;

        virtual u8string GetDirectoryPath(DirBase base) const = 0;
        virtual u8string GetDirectoryPath(DirBase base, DirId did) const = 0;
        virtual u8string GetFilePath(PATHID pathid) const = 0;
        virtual u8string FindFile(DirBase base, DirId did, u8string_view fileName) const = 0;
        virtual void SetBasePath(DirBase base, u8string_view path) = 0;
        virtual bool IsUsingClassic() const = 0;
    };

    [[nodiscard]] std::unique_ptr<IPlatformEnvironment> CreatePlatformEnvironment(DirBaseValues basePaths);
    [[nodiscard]] std::unique_ptr<IPlatformEnvironment> CreatePlatformEnvironment();

} // namespace OpenRCT2
