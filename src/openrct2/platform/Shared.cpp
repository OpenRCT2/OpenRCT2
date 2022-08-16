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
#include "../config/Config.h"
#include "../core/File.h"
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

    bool IsRCT2ClassicPath(std::string_view path)
    {
        auto combinedPath = Path::ResolveCasing(Path::Combine(path, u8"Assets", u8"g1.dat"));
        return File::Exists(combinedPath);
    }

    bool OriginalGameDataExists(std::string_view path)
    {
        // Check if path is RCT2
        auto combinedPath = Path::ResolveCasing(Path::Combine(path, u8"Data", u8"g1.dat"));
        if (File::Exists(combinedPath))
            return true;

        // Check if path is RCT Classic
        return IsRCT2ClassicPath(path);
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
