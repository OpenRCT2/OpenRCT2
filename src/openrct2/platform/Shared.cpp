/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../common.h"

#ifdef _WIN32
#    include <windows.h>
#endif

#include "../Context.h"
#include "../Game.h"
#include "../PlatformEnvironment.h"
#include "../config/Config.h"
#include "../core/File.h"
#include "../core/FileIndex.hpp"
#include "../core/Path.hpp"
#include "../localisation/Localisation.h"
#include "Platform.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <time.h>

#ifdef _WIN32
static constexpr std::array _prohibitedCharacters = { '<', '>', '*', '\\', ':', '|', '?', '"', '/' };
#else
static constexpr std::array _prohibitedCharacters = { '/' };
#endif

using OpenRCT2::DIRBASE;
using OpenRCT2::DIRID;

namespace Platform
{
    void CoreInit()
    {
        static bool initialised = false;
        if (!initialised)
        {
            initialised = true;

#ifdef __ANDROID__
            Platform::AndroidInitClassLoader();
#endif // __ANDROID__

            InitTicks();
            bitcount_init();
            mask_init();
        }
    }

    CurrencyType GetCurrencyValue(const char* currCode)
    {
        if (currCode == nullptr || strlen(currCode) < 3)
        {
            return CurrencyType::Pounds;
        }

        for (int32_t currency = 0; currency < EnumValue(CurrencyType::Count); ++currency)
        {
            if (strncmp(currCode, CurrencyDescriptors[currency].isoCode, 3) == 0)
            {
                return static_cast<CurrencyType>(currency);
            }
        }

        return CurrencyType::Pounds;
    }

    rct2_date GetDateLocal()
    {
        auto time = std::time(nullptr);
        auto localTime = std::localtime(&time);

        rct2_date outDate;
        outDate.day = localTime->tm_mday;
        outDate.day_of_week = localTime->tm_wday;
        outDate.month = localTime->tm_mon + 1;
        outDate.year = localTime->tm_year + 1900;
        return outDate;
    }

    rct2_time GetTimeLocal()
    {
        auto time = std::time(nullptr);
        auto localTime = std::localtime(&time);

        rct2_time outTime;
        outTime.hour = localTime->tm_hour;
        outTime.minute = localTime->tm_min;
        outTime.second = localTime->tm_sec;
        return outTime;
    }

    bool OriginalGameDataExists(std::string_view path)
    {
        std::string combinedPath = Path::ResolveCasing(Path::Combine(path, u8"Data", u8"g1.dat"));
        return File::Exists(combinedPath);
    }

    bool RCTCDataExists(u8string_view path)
    {
        u8string combinedPath = Path::ResolveCasing(Path::Combine(path, u8"Assets", u8"g1.dat"));
        return File::Exists(combinedPath);
    }

    u8string CopyRCTCData(u8string_view sourceDir)
    {
        const auto assetDir = Path::Combine(sourceDir, "Assets");
        auto env = OpenRCT2::GetContext()->GetPlatformEnvironment();
        const auto targetDir = env->GetDirectoryPath(DIRBASE::CONFIG, DIRID::RCT2_SOURCE);
        env->SetBasePath(DIRBASE::RCT2, targetDir);

        struct AssetGroup
        {
            u8string Pattern;
            DIRID TargetSubdir;
            u8string NewExtension;
        };

        AssetGroup assetGroups[] = {
            { "*.pob", DIRID::OBJECT, "dat" },
            { "*.td6", DIRID::TRACK, "" },
            { "*.ogg", DIRID::DATA, "" },
            { "*.sea", DIRID::SCENARIO, "sc6" },
        };

        for (const auto& assetGroup : assetGroups)
        {
            auto targetSubdir = env->GetDirectoryPath(DIRBASE::RCT2, assetGroup.TargetSubdir);
            EnsureDirectoryExists(targetSubdir);
            auto objDataPattern = Path::Combine(assetDir, assetGroup.Pattern);
            auto scanner = Path::ScanDirectory(objDataPattern, true);
            while (scanner->Next())
            {
                auto path = std::string(scanner->GetPath());
                auto newFilename = u8string(scanner->GetPathRelative());
                if (!assetGroup.NewExtension.empty())
                {
                    newFilename = newFilename.substr(0, newFilename.length() - 3) + assetGroup.NewExtension;
                }

                if (assetGroup.Pattern == "*.sea")
                {
                    auto data = DecryptSea(fs::u8path(path));
                    File::WriteAllBytes(Path::Combine(targetSubdir, newFilename), data.data(), data.size());
                }
                else
                {
                    File::Copy(path, Path::Combine(targetSubdir, newFilename), true);
                }
            }
        }

        File::Copy(
            Path::Combine(assetDir, "g1.dat"), Path::Combine(env->GetDirectoryPath(DIRBASE::RCT2, DIRID::DATA), "g1.dat"),
            true);

        // Needs to be on for the RCTC scenarios to load.
        gConfigGeneral.allow_loading_with_incorrect_checksum = true;
        config_save_default();

        return targetDir;
    }

    std::string SanitiseFilename(std::string_view originalName)
    {
        auto sanitised = std::string(originalName);
        std::replace_if(
            sanitised.begin(), sanitised.end(),
            [](const std::string::value_type& ch) -> bool {
                return std::find(_prohibitedCharacters.begin(), _prohibitedCharacters.end(), ch) != _prohibitedCharacters.end();
            },
            '_');
        sanitised = String::Trim(sanitised);
        return sanitised;
    }

    bool IsFilenameValid(u8string_view fileName)
    {
        return fileName.find_first_of(_prohibitedCharacters.data(), 0, _prohibitedCharacters.size()) == fileName.npos;
    }

#ifndef __ANDROID__
    float GetDefaultScale()
    {
        return 1;
    }
#endif
} // namespace Platform
