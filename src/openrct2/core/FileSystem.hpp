/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

/// @file
/// This file enables access to std::filesystem as `fs` namespace
/// or injects third-party drop-in replacement on build configs
/// where std::filesystem is not available.

#pragma once

// Find out if std::filesystem is available:
#if defined(_MSC_VER) // Visual Studio supports <filesystem>
    #define HAVE_STD_FILESYSTEM 1
#elif defined(__APPLE__) // XCode has the header, but reports error when included.
    #define HAVE_STD_FILESYSTEM 0
#elif defined(__ANDROID__)
    #define HAVE_STD_FILESYSTEM 1
#elif defined(__has_include) // For GCC/Clang check if the header exists.
    #if __has_include(<filesystem>)
        #define HAVE_STD_FILESYSTEM 1
    #else
        #define HAVE_STD_FILESYSTEM 0
    #endif
#else // By default assume not supported.
    #define HAVE_STD_FILESYSTEM 0
#endif

#if HAVE_STD_FILESYSTEM
    #include <filesystem>
namespace fs = std::filesystem;
#else
    #ifdef _WIN32
        #ifndef NOMINMAX
            #define NOMINMAX
        #endif
        #ifndef WIN32_LEAN_AND_MEAN
            #define WIN32_LEAN_AND_MEAN
        #endif
        #define BITMAP WIN32_BITMAP
        #define PATTERN WIN32_PATTERN
    #endif
    #include <filesystem.hpp>
    #ifdef _WIN32
        #undef CreateDirectory
        #undef CreateWindow
        #undef GetMessage
        #undef BITMAP
        #undef PATTERN
    #endif
namespace fs = ghc::filesystem;
#endif

#undef HAVE_STD_FILESYSTEM // Not needed any more, don't make it public.
