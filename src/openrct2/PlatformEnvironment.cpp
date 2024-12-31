/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PlatformEnvironment.h"

#include "Diagnostic.h"
#include "OpenRCT2.h"
#include "config/Config.h"
#include "core/EnumUtils.hpp"
#include "core/File.h"
#include "core/Path.hpp"
#include "core/String.hpp"
#include "platform/Platform.h"

using namespace OpenRCT2;

static constexpr const char* DirectoryNamesRCT2[] = {
    "Data",        // DATA
    "Landscapes",  // LANDSCAPE
    nullptr,       // LANGUAGE
    nullptr,       // LOG_CHAT
    nullptr,       // LOG_SERVER
    nullptr,       // NETWORK_KEY
    "ObjData",     // OBJECT
    nullptr,       // PLUGIN
    "Saved Games", // SAVE
    "Scenarios",   // SCENARIO
    nullptr,       // SCREENSHOT
    nullptr,       // SEQUENCE
    nullptr,       // SHADER
    nullptr,       // THEME
    "Tracks",      // TRACK
};

static constexpr u8string_view DirectoryNamesOpenRCT2[] = {
    u8"data",             // DATA
    u8"landscape",        // LANDSCAPE
    u8"language",         // LANGUAGE
    u8"chatlogs",         // LOG_CHAT
    u8"serverlogs",       // LOG_SERVER
    u8"keys",             // NETWORK_KEY
    u8"object",           // OBJECT
    u8"plugin",           // PLUGIN
    u8"save",             // SAVE
    u8"scenario",         // SCENARIO
    u8"screenshot",       // SCREENSHOT
    u8"sequence",         // SEQUENCE
    u8"shaders",          // SHADER
    u8"themes",           // THEME
    u8"track",            // TRACK
    u8"heightmap",        // HEIGHTMAP
    u8"replay",           // REPLAY
    u8"desyncs",          // DESYNCS
    u8"crash",            // CRASH
    u8"assetpack",        // ASSET_PACK
    u8"scenario_patches", // SCENARIO_PATCHES
};

static constexpr u8string_view FileNames[] = {
    u8"config.ini",                              // CONFIG
    u8"hotkeys.dat",                             // CONFIG_SHORTCUTS_LEGACY
    u8"shortcuts.json",                          // CONFIG_SHORTCUTS
    u8"objects.idx",                             // CACHE_OBJECTS
    u8"tracks.idx",                              // CACHE_TRACKS
    u8"scenarios.idx",                           // CACHE_SCENARIOS
    u8"groups.json",                             // NETWORK_GROUPS
    u8"servers.cfg",                             // NETWORK_SERVERS
    u8"users.json",                              // NETWORK_USERS
    u8"highscores.dat",                          // SCORES
    u8"scores.dat",                              // SCORES (LEGACY)
    u8"Saved Games" PATH_SEPARATOR "scores.dat", // SCORES (RCT2)
    u8"changelog.txt",                           // CHANGELOG
    u8"plugin.store.json",                       // PLUGIN_STORE
    u8"contributors.md",                         // CONTRIBUTORS
};

class PlatformEnvironment final : public IPlatformEnvironment
{
private:
    u8string _basePath[DIRBASE_COUNT];
    bool _usingRCTClassic{};

public:
    explicit PlatformEnvironment(DIRBASE_VALUES basePaths)
    {
        for (size_t i = 0; i < DIRBASE_COUNT; i++)
        {
            _basePath[i] = basePaths[i];
        }
    }

    u8string GetDirectoryPath(DIRBASE base) const override
    {
        return _basePath[EnumValue(base)];
    }

    u8string GetDirectoryPath(DIRBASE base, DIRID did) const override
    {
        auto basePath = GetDirectoryPath(base);
        u8string_view directoryName;
        switch (base)
        {
            default:
            case DIRBASE::RCT1:
                directoryName = DirectoryNamesRCT2[EnumValue(did)];
                break;
            case DIRBASE::RCT2:
                directoryName = _usingRCTClassic ? "Assets" : DirectoryNamesRCT2[EnumValue(did)];
                break;
            case DIRBASE::OPENRCT2:
            case DIRBASE::USER:
            case DIRBASE::CONFIG:
                directoryName = DirectoryNamesOpenRCT2[EnumValue(did)];
                break;
        }

        return Path::Combine(basePath, directoryName);
    }

    u8string GetFilePath(PATHID pathid) const override
    {
        auto dirbase = GetDefaultBaseDirectory(pathid);
        auto basePath = GetDirectoryPath(dirbase);
        auto fileName = FileNames[EnumValue(pathid)];
        return Path::Combine(basePath, fileName);
    }

    u8string FindFile(DIRBASE base, DIRID did, u8string_view fileName) const override
    {
        auto dataPath = GetDirectoryPath(base, did);

        std::string alternativeFilename;
        if (_usingRCTClassic && base == DIRBASE::RCT2 && did == DIRID::DATA)
        {
            // Special case, handle RCT Classic css ogg files
            if (String::startsWith(fileName, "css", true) && String::endsWith(fileName, ".dat", true))
            {
                alternativeFilename = fileName.substr(0, fileName.size() - 3);
                alternativeFilename.append("ogg");
                fileName = alternativeFilename;
            }
        }

        auto path = Path::ResolveCasing(Path::Combine(dataPath, fileName));
        if (base == DIRBASE::RCT1 && did == DIRID::DATA && !File::Exists(path))
        {
            // Special case, handle RCT1 steam layout where some data files are under a CD root
            auto basePath = GetDirectoryPath(base);
            auto alternativePath = Path::ResolveCasing(Path::Combine(basePath, "RCTdeluxe_install", "Data", fileName));
            if (File::Exists(alternativePath))
            {
                path = alternativePath;
            }
        }

        return path;
    }

    void SetBasePath(DIRBASE base, u8string_view path) override
    {
        _basePath[EnumValue(base)] = path;

        if (base == DIRBASE::RCT2)
        {
            _usingRCTClassic = Platform::IsRCTClassicPath(path);
        }
    }

    bool IsUsingClassic() const override
    {
        return _usingRCTClassic;
    }

private:
    static DIRBASE GetDefaultBaseDirectory(PATHID pathid)
    {
        switch (pathid)
        {
            case PATHID::CONFIG:
            case PATHID::CONFIG_SHORTCUTS_LEGACY:
            case PATHID::CONFIG_SHORTCUTS:
                return DIRBASE::CONFIG;
            case PATHID::CACHE_OBJECTS:
            case PATHID::CACHE_TRACKS:
            case PATHID::CACHE_SCENARIOS:
                return DIRBASE::CACHE;
            case PATHID::SCORES_RCT2:
                return DIRBASE::RCT2;
            case PATHID::CHANGELOG:
            case PATHID::CONTRIBUTORS:
                return DIRBASE::DOCUMENTATION;
            case PATHID::NETWORK_GROUPS:
            case PATHID::NETWORK_SERVERS:
            case PATHID::NETWORK_USERS:
            case PATHID::SCORES:
            case PATHID::SCORES_LEGACY:
            default:
                return DIRBASE::USER;
        }
    }
};

std::unique_ptr<IPlatformEnvironment> OpenRCT2::CreatePlatformEnvironment(DIRBASE_VALUES basePaths)
{
    return std::make_unique<PlatformEnvironment>(basePaths);
}

static u8string GetOpenRCT2DirectoryName()
{
#if defined(__ANDROID__)
    return u8"openrct2-user";
#else
    return u8"OpenRCT2";
#endif
}

std::unique_ptr<IPlatformEnvironment> OpenRCT2::CreatePlatformEnvironment()
{
    auto subDirectory = GetOpenRCT2DirectoryName();

    // Set default paths
    std::string basePaths[DIRBASE_COUNT];
    basePaths[EnumValue(DIRBASE::OPENRCT2)] = Platform::GetInstallPath();
    basePaths[EnumValue(DIRBASE::USER)] = Path::Combine(Platform::GetFolderPath(SPECIAL_FOLDER::USER_DATA), subDirectory);
    basePaths[EnumValue(DIRBASE::CONFIG)] = Path::Combine(Platform::GetFolderPath(SPECIAL_FOLDER::USER_CONFIG), subDirectory);
    basePaths[EnumValue(DIRBASE::CACHE)] = Path::Combine(Platform::GetFolderPath(SPECIAL_FOLDER::USER_CACHE), subDirectory);
    basePaths[EnumValue(DIRBASE::DOCUMENTATION)] = Platform::GetDocsPath();

    // Override paths that have been specified via the command line
    if (!gCustomRCT1DataPath.empty())
    {
        basePaths[EnumValue(DIRBASE::RCT1)] = gCustomRCT1DataPath;
    }
    if (!gCustomRCT2DataPath.empty())
    {
        basePaths[EnumValue(DIRBASE::RCT2)] = gCustomRCT2DataPath;
    }
    if (!gCustomOpenRCT2DataPath.empty())
    {
        basePaths[EnumValue(DIRBASE::OPENRCT2)] = gCustomOpenRCT2DataPath;
    }
    if (!gCustomUserDataPath.empty())
    {
        basePaths[EnumValue(DIRBASE::USER)] = gCustomUserDataPath;
        basePaths[EnumValue(DIRBASE::CONFIG)] = gCustomUserDataPath;
        basePaths[EnumValue(DIRBASE::CACHE)] = gCustomUserDataPath;
    }

    if (basePaths[EnumValue(DIRBASE::DOCUMENTATION)].empty())
    {
        basePaths[EnumValue(DIRBASE::DOCUMENTATION)] = basePaths[EnumValue(DIRBASE::OPENRCT2)];
    }

    auto env = OpenRCT2::CreatePlatformEnvironment(basePaths);

    // Now load the config so we can get the RCT1 and RCT2 paths
    auto configPath = env->GetFilePath(PATHID::CONFIG);
    Config::SetDefaults();
    if (!Config::OpenFromPath(configPath))
    {
        Config::SaveToPath(configPath);
    }
    if (gCustomRCT1DataPath.empty())
    {
        env->SetBasePath(DIRBASE::RCT1, Config::Get().general.RCT1Path);
    }
    if (gCustomRCT2DataPath.empty())
    {
        env->SetBasePath(DIRBASE::RCT2, Config::Get().general.RCT2Path);
    }

    // Log base paths
    LOG_VERBOSE("DIRBASE::RCT1    : %s", env->GetDirectoryPath(DIRBASE::RCT1).c_str());
    LOG_VERBOSE("DIRBASE::RCT2    : %s", env->GetDirectoryPath(DIRBASE::RCT2).c_str());
    LOG_VERBOSE("DIRBASE::OPENRCT2: %s", env->GetDirectoryPath(DIRBASE::OPENRCT2).c_str());
    LOG_VERBOSE("DIRBASE::USER    : %s", env->GetDirectoryPath(DIRBASE::USER).c_str());
    LOG_VERBOSE("DIRBASE::CONFIG  : %s", env->GetDirectoryPath(DIRBASE::CONFIG).c_str());
    LOG_VERBOSE("DIRBASE::CACHE   : %s", env->GetDirectoryPath(DIRBASE::CACHE).c_str());

    return env;
}
