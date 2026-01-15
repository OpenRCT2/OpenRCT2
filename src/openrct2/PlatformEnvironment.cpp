/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

static constexpr const char* kDirectoryNamesRCT2[] = {
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

static constexpr u8string_view kDirectoryNamesOpenRCT2[] = {
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

static constexpr u8string_view kFileNames[] = {
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
    u8string _basePath[kDirBaseCount];
    RCT2Variant _rct2Variant = RCT2Variant::rct2Original;

public:
    explicit PlatformEnvironment(DirBaseValues basePaths)
    {
        for (size_t i = 0; i < kDirBaseCount; i++)
        {
            _basePath[i] = basePaths[i];
        }
    }

    u8string GetDirectoryPath(DirBase base) const override
    {
        return _basePath[EnumValue(base)];
    }

    u8string GetDirectoryPath(DirBase base, DirId did) const override
    {
        auto basePath = GetDirectoryPath(base);
        u8string_view directoryName;
        switch (base)
        {
            default:
            case DirBase::rct1:
                if (basePath.empty())
                    return {};

                directoryName = kDirectoryNamesRCT2[EnumValue(did)];
                break;
            case DirBase::rct2:
                switch (_rct2Variant)
                {
                    case RCT2Variant::rct2Original:
                        directoryName = kDirectoryNamesRCT2[EnumValue(did)];
                        break;
                    case RCT2Variant::rctClassicWindows:
                        directoryName = Platform::kRCTClassicWindowsDataFolder;
                        break;
                    case RCT2Variant::rctClassicMac:
                        directoryName = Platform::kRCTClassicMacOSDataFolder;
                        break;
                    case RCT2Variant::rctClassicPlusMac:
                        directoryName = Platform::kRCTClassicPlusMacOSDataFolder;
                        break;
                }
                break;
            case DirBase::openrct2:
            case DirBase::user:
            case DirBase::config:
                directoryName = kDirectoryNamesOpenRCT2[EnumValue(did)];
                break;
        }

        return Path::Combine(basePath, directoryName);
    }

    u8string GetFilePath(PathId pathid) const override
    {
        auto dirbase = GetDefaultBaseDirectory(pathid);
        auto basePath = GetDirectoryPath(dirbase);
        auto fileName = kFileNames[EnumValue(pathid)];
        return Path::Combine(basePath, fileName);
    }

    u8string FindFile(DirBase base, DirId did, u8string_view fileName) const override
    {
        auto dataPath = GetDirectoryPath(base, did);

        std::string alternativeFilename;
        if (_rct2Variant != RCT2Variant::rct2Original && base == DirBase::rct2 && did == DirId::data)
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
        if (base == DirBase::rct1 && did == DirId::data && !File::Exists(path))
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

    void SetBasePath(DirBase base, u8string_view path) override
    {
        _basePath[EnumValue(base)] = path;

        if (base == DirBase::rct2)
        {
            // This value being empty can be valid on headless.
            auto variant = Platform::classifyGamePath(path);
            if (variant.has_value())
                _rct2Variant = variant.value();
        }
    }

    bool IsUsingClassic() const override
    {
        return _rct2Variant != RCT2Variant::rct2Original;
    }

private:
    static DirBase GetDefaultBaseDirectory(PathId pathid)
    {
        switch (pathid)
        {
            case PathId::config:
            case PathId::configShortcutsLegacy:
            case PathId::configShortcuts:
                return DirBase::config;
            case PathId::cacheObjects:
            case PathId::cacheTracks:
            case PathId::cacheScenarios:
                return DirBase::cache;
            case PathId::scoresRCT2:
                return DirBase::rct2;
            case PathId::changelog:
            case PathId::contributors:
                return DirBase::documentation;
            case PathId::networkGroups:
            case PathId::networkServers:
            case PathId::networkUsers:
            case PathId::scores:
            case PathId::scoresLegacy:
            default:
                return DirBase::user;
        }
    }
};

std::unique_ptr<IPlatformEnvironment> OpenRCT2::CreatePlatformEnvironment(DirBaseValues basePaths)
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
    std::string basePaths[kDirBaseCount];
    basePaths[EnumValue(DirBase::openrct2)] = Platform::GetInstallPath();
    basePaths[EnumValue(DirBase::user)] = Path::Combine(Platform::GetFolderPath(SpecialFolder::userData), subDirectory);
    basePaths[EnumValue(DirBase::config)] = Path::Combine(Platform::GetFolderPath(SpecialFolder::userConfig), subDirectory);
    basePaths[EnumValue(DirBase::cache)] = Path::Combine(Platform::GetFolderPath(SpecialFolder::userCache), subDirectory);
    basePaths[EnumValue(DirBase::documentation)] = Platform::GetDocsPath();

    // Override paths that have been specified via the command line
    if (!gCustomRCT1DataPath.empty())
    {
        basePaths[EnumValue(DirBase::rct1)] = gCustomRCT1DataPath;
    }
    if (!gCustomRCT2DataPath.empty())
    {
        basePaths[EnumValue(DirBase::rct2)] = gCustomRCT2DataPath;
    }
    if (!gCustomOpenRCT2DataPath.empty())
    {
        basePaths[EnumValue(DirBase::openrct2)] = gCustomOpenRCT2DataPath;
    }
    if (!gCustomUserDataPath.empty())
    {
        basePaths[EnumValue(DirBase::user)] = gCustomUserDataPath;
        basePaths[EnumValue(DirBase::config)] = gCustomUserDataPath;
        basePaths[EnumValue(DirBase::cache)] = gCustomUserDataPath;
    }

    if (basePaths[EnumValue(DirBase::documentation)].empty())
    {
        basePaths[EnumValue(DirBase::documentation)] = basePaths[EnumValue(DirBase::openrct2)];
    }

    auto env = CreatePlatformEnvironment(basePaths);

    // Now load the config so we can get the RCT1 and RCT2 paths
    auto configPath = env->GetFilePath(PathId::config);
    Config::SetDefaults();
    if (!Config::OpenFromPath(configPath))
    {
        Config::SaveToPath(configPath);
    }
    if (gCustomRCT1DataPath.empty())
    {
        env->SetBasePath(DirBase::rct1, Config::Get().general.rct1Path);
    }
    if (gCustomRCT2DataPath.empty())
    {
        env->SetBasePath(DirBase::rct2, Config::Get().general.rct2Path);
    }

    // Log base paths
    LOG_VERBOSE("DirBase::rct1    : %s", env->GetDirectoryPath(DirBase::rct1).c_str());
    LOG_VERBOSE("DirBase::rct2    : %s", env->GetDirectoryPath(DirBase::rct2).c_str());
    LOG_VERBOSE("DirBase::openrct2: %s", env->GetDirectoryPath(DirBase::openrct2).c_str());
    LOG_VERBOSE("DirBase::user    : %s", env->GetDirectoryPath(DirBase::user).c_str());
    LOG_VERBOSE("DirBase::config  : %s", env->GetDirectoryPath(DirBase::config).c_str());
    LOG_VERBOSE("DirBase::cache   : %s", env->GetDirectoryPath(DirBase::cache).c_str());

    return env;
}
