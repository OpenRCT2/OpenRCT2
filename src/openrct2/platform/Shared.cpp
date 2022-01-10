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
#else
#    include <unistd.h>
#endif

#include "../Context.h"
#include "../Game.h"
#include "../OpenRCT2.h"
#include "../config/Config.h"
#include "../core/File.h"
#include "../core/FileSystem.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../drawing/LightFX.h"
#include "../localisation/Currency.h"
#include "../localisation/Localisation.h"
#include "../util/Util.h"
#include "../world/Climate.h"
#include "Platform2.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <stdlib.h>
#include <time.h>

#ifdef __APPLE__
#    include <AvailabilityMacros.h>
#    include <mach/mach_time.h>
#    ifndef __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#        error Missing __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ define
#    endif
#endif

#if defined(__APPLE__) && (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 101200)
static mach_timebase_info_data_t _mach_base_info = {};
#endif

#ifdef _WIN32
char* strndup(const char* src, size_t size)
{
    size_t len = strnlen(src, size);
    char* dst = reinterpret_cast<char*>(malloc(len + 1));

    if (dst == nullptr)
    {
        return nullptr;
    }

    dst = reinterpret_cast<char*>(std::memcpy(dst, src, len));
    dst[len] = '\0';
    return dst;
}

static uint32_t _frequency = 0;
static LARGE_INTEGER _entryTimestamp;
#endif // _WIN32

namespace Platform
{
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
        std::string combinedPath = Path::ResolveCasing(Path::Combine(path, "Data", "g1.dat"));
        return File::Exists(combinedPath);
    }

    std::string SanitiseFilename(std::string_view originalName)
    {
#ifdef _WIN32
        static constexpr std::array prohibited = { '<', '>', '*', '\\', ':', '|', '?', '"', '/' };
#else
        static constexpr std::array prohibited = { '/' };
#endif
        auto sanitised = std::string(originalName);
        std::replace_if(
            sanitised.begin(), sanitised.end(),
            [](const std::string::value_type& ch) -> bool {
                return std::find(prohibited.begin(), prohibited.end(), ch) != prohibited.end();
            },
            '_');
        sanitised = String::Trim(sanitised);
        return sanitised;
    }
} // namespace Platform

GamePalette gPalette;

void platform_toggle_windowed_mode()
{
    int32_t targetMode = gConfigGeneral.fullscreen_mode == 0 ? 2 : 0;
    context_set_fullscreen_mode(targetMode);
    gConfigGeneral.fullscreen_mode = targetMode;
    config_save_default();
}

void platform_refresh_video(bool recreate_window)
{
    if (recreate_window)
    {
        context_recreate_window();
    }
    else
    {
        drawing_engine_dispose();
        drawing_engine_init();
        drawing_engine_resize();
    }

    drawing_engine_set_palette(gPalette);
    gfx_invalidate_screen();
}

static void platform_ticks_init()
{
#ifdef _WIN32
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    _frequency = static_cast<uint32_t>(freq.QuadPart / 1000);
    QueryPerformanceCounter(&_entryTimestamp);
#endif
}

uint32_t platform_get_ticks()
{
#ifdef _WIN32
    LARGE_INTEGER pfc;
    QueryPerformanceCounter(&pfc);

    LARGE_INTEGER runningDelta;
    runningDelta.QuadPart = pfc.QuadPart - _entryTimestamp.QuadPart;

    return static_cast<uint32_t>(runningDelta.QuadPart / _frequency);
#elif defined(__APPLE__) && (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 101200)
    return static_cast<uint32_t>(((mach_absolute_time() * _mach_base_info.numer) / _mach_base_info.denom) / 1000000);
#else
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
    {
        log_fatal("clock_gettime failed");
        exit(-1);
    }
    return static_cast<uint32_t>(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
}

void platform_sleep(uint32_t ms)
{
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

#ifndef __ANDROID__
float platform_get_default_scale()
{
    return 1;
}
#endif

void core_init()
{
    static bool initialised = false;
    if (!initialised)
    {
        initialised = true;

#ifdef __ANDROID__
        platform_android_init_class_loader();
#endif // __ANDROID__

        platform_ticks_init();
        bitcount_init();
        mask_init();

#if defined(__APPLE__) && (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 101200)
        kern_return_t ret = mach_timebase_info(&_mach_base_info);
        if (ret != 0)
        {
            log_fatal("Unable to get mach_timebase_info.");
            exit(-1);
        }
#endif
    }
}
