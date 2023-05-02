/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "common.h"

#include <string>

#define OPENRCT2_NAME "OpenRCT2"
#define OPENRCT2_VERSION "0.4.4"

#if defined(__amd64__) || defined(_M_AMD64)
#    define OPENRCT2_ARCHITECTURE "x86-64"
#elif defined(__i386__) || defined(_M_IX86)
#    define OPENRCT2_ARCHITECTURE "x86"
#elif defined(__aarch64__) || defined(_M_ARM64)
#    define OPENRCT2_ARCHITECTURE "AArch64"
#elif defined(__arm__) || defined(_M_ARM)
#    if defined(__ARM_ARCH_7A__)
#        define OPENRCT2_ARCHITECTURE "arm-v7a"
#    else
#        define OPENRCT2_ARCHITECTURE "arm"
#    endif
#elif defined(__powerpc__) || defined(_M_PPC)
#    define OPENRCT2_ARCHITECTURE "PowerPC"
#elif defined(__mips64)
#    define OPENRCT2_ARCHITECTURE "mips64"
#elif defined(__mips__)
#    define OPENRCT2_ARCHITECTURE "mips"
#elif defined(__riscv)
#    define OPENRCT2_ARCHITECTURE "RISC-V"
#endif
#ifdef __EMSCRIPTEN__
#    define OPENRCT2_ARCHITECTURE "Emscripten"
#endif

#ifndef OPENRCT2_ARCHITECTURE
#    error "OPENRCT2_ARCHITECTURE is undefined. Please add identification."
#endif

// Platform
#ifdef _WIN32
#    define OPENRCT2_PLATFORM "Windows"
#endif
#if defined(__linux__) && !defined(__ANDROID__)
#    define OPENRCT2_PLATFORM "Linux"
#endif
#if (defined(__APPLE__) && defined(__MACH__))
#    define OPENRCT2_PLATFORM "macOS"
#endif
#ifdef __FreeBSD__
#    define OPENRCT2_PLATFORM "FreeBSD"
#endif
#ifdef __NetBSD__
#    define OPENRCT2_PLATFORM "NetBSD"
#endif
#ifdef __ANDROID__
#    define OPENRCT2_PLATFORM "Android"
#endif
#ifdef __OpenBSD__
#    define OPENRCT2_PLATFORM "OpenBSD"
#endif
#ifdef __EMSCRIPTEN__
#    define OPENRCT2_PLATFORM "Emscripten"
#endif
#ifndef OPENRCT2_PLATFORM
#    error Unknown platform!
#endif

extern const char gVersionInfoFull[];
extern const char gVersionInfoTag[];
struct NewVersionInfo
{
    std::string tag;
    std::string name;
    std::string changelog;
    std::string url;
};

NewVersionInfo GetLatestVersion();
