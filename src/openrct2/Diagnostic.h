/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <format>
#include <source_location>

enum class DiagnosticLevel
{
    fatal,
    error,
    warning,
    verbose,
    information,
    count
};

/**
 * Compile-time debug levels.
 *
 * When compiling, just add -DDEBUG={0,1,2,3} (where 0 means disabled)
 * Regardless of DEBUG value, a set of defines will be created:
 * - DEBUG_LEVEL_1
 * - DEBUG_LEVEL_2
 * - DEBUG_LEVEL_3
 * which you would use like so:
 *
 * #if DEBUG_LEVEL_1
 *     (... some debug code ...)
 *     #if DEBUG_LEVEL_2
 *         (... more debug code ...)
 *    #endif // DEBUG_LEVEL_2
 * #endif // DEBUG_LEVEL_1
 *
 * The defines will be either 0 or 1 so compiler will complain about undefined
 * macro if you forget to include the file, which would not happen if we were
 * only checking whether the define is present or not.
 */

#if defined(DEBUG)
    #if DEBUG > 0
        #define DEBUG_LEVEL_1 1
        #if DEBUG > 1
            #define DEBUG_LEVEL_2 1
            #if DEBUG > 2
                #define DEBUG_LEVEL_3 1
            #else
                #define DEBUG_LEVEL_3 0
            #endif // DEBUG > 2
        #else
            #define DEBUG_LEVEL_3 0
            #define DEBUG_LEVEL_2 0
        #endif // DEBUG > 1
    #else
        #define DEBUG_LEVEL_1 0
        #define DEBUG_LEVEL_2 0
        #define DEBUG_LEVEL_3 0
    #endif // DEBUG > 0
#else
    #define DEBUG_LEVEL_3 0
    #define DEBUG_LEVEL_2 0
    #define DEBUG_LEVEL_1 0
#endif // defined(DEBUG)

extern bool _log_levels[static_cast<uint8_t>(DiagnosticLevel::count)];

void DiagnosticLog(DiagnosticLevel diagnosticLevel, const char* format, ...);
void DiagnosticLogWithLocation(const std::source_location location, DiagnosticLevel diagnosticLevel, const char* format, ...);
void DiagnosticLogFmtArgs(DiagnosticLevel diagnosticLevel, std::string_view line, std::format_args args);
void DiagnosticLogWithLocationFmtArgs(
    const std::source_location location, DiagnosticLevel diagnosticLevel, std::string_view line, std::format_args args);

#define LOG_FATAL(format, ...)                                                                                                 \
    DiagnosticLogWithLocation(std::source_location::current(), DiagnosticLevel::fatal, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...)                                                                                                 \
    DiagnosticLogWithLocation(std::source_location::current(), DiagnosticLevel::error, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...)                                                                                               \
    DiagnosticLogWithLocation(std::source_location::current(), DiagnosticLevel::warning, format, ##__VA_ARGS__)
#define LOG_VERBOSE(format, ...) DiagnosticLog(DiagnosticLevel::verbose, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)                                                                                                  \
    DiagnosticLogWithLocation(std::source_location::current(), DiagnosticLevel::information, format, ##__VA_ARGS__)

template<typename... Args>
void DiagnosticLogFmt(DiagnosticLevel lvl, std::format_string<Args...> fmt, Args&&... args)
{
    DiagnosticLogFmtArgs(lvl, fmt.get(), std::make_format_args(args...));
}

template<typename... Args>
void DiagnosticLogWithLocationFmt(
    const std::source_location loc, DiagnosticLevel lvl, std::format_string<Args...> fmt, Args&&... args)
{
    DiagnosticLogWithLocationFmtArgs(loc, lvl, fmt.get(), std::make_format_args(args...));
}

#define LOG_FATAL_FMT(...) DiagnosticLogWithLocationFmt(std::source_location::current(), DiagnosticLevel::fatal, __VA_ARGS__)
#define LOG_ERROR_FMT(...) DiagnosticLogWithLocationFmt(std::source_location::current(), DiagnosticLevel::error, __VA_ARGS__)
#define LOG_WARNING_FMT(...)                                                                                                   \
    DiagnosticLogWithLocationFmt(std::source_location::current(), DiagnosticLevel::warning, __VA_ARGS__)
#define LOG_VERBOSE_FMT(...) DiagnosticLogFmt(DiagnosticLevel::verbose, __VA_ARGS__)
#define LOG_INFO_FMT(...)                                                                                                      \
    DiagnosticLogWithLocationFmt(std::source_location::current(), DiagnosticLevel::information, __VA_ARGS__)
