/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Diagnostic.h"

#include "core/Console.hpp"
#include "core/EnumUtils.hpp"
#include "core/String.hpp"

#include <cstdarg>
#include <cstdio>

#ifdef __ANDROID__
    #include <android/log.h>
#endif

using namespace OpenRCT2;

[[maybe_unused]] static bool _log_location_enabled = true;
bool _log_levels[EnumValue(DiagnosticLevel::count)] = {
    true, true, true, false, true,
};

static FILE* diagnostic_get_stream(DiagnosticLevel level)
{
    switch (level)
    {
        case DiagnosticLevel::verbose:
        case DiagnosticLevel::information:
            return stdout;
        default:
            return stderr;
    }
}

#ifdef __ANDROID__

int _android_log_priority[EnumValue(DiagnosticLevel::count)] = {
    ANDROID_LOG_FATAL, ANDROID_LOG_ERROR, ANDROID_LOG_WARN, ANDROID_LOG_VERBOSE, ANDROID_LOG_INFO,
};

void DiagnosticLog(DiagnosticLevel diagnosticLevel, const char* format, ...)
{
    va_list args;

    if (!_log_levels[EnumValue(diagnosticLevel)])
        return;

    va_start(args, format);
    __android_log_vprint(_android_log_priority[EnumValue(diagnosticLevel)], "OpenRCT2", format, args);
    va_end(args);
}

void DiagnosticLogWithLocation(const std::source_location location, DiagnosticLevel diagnosticLevel, const char* format, ...)
{
    va_list args;

    if (!_log_levels[EnumValue(diagnosticLevel)])
        return;

    va_start(args, format);
    __android_log_vprint(_android_log_priority[EnumValue(diagnosticLevel)], location.file_name(), format, args);
    va_end(args);
}

#else

static constexpr const char* kLevelStrings[] = {
    "FATAL", "ERROR", "WARNING", "VERBOSE", "INFO",
};

static void DiagnosticPrint(DiagnosticLevel level, const std::string& msg)
{
    auto stream = diagnostic_get_stream(level);
    if (stream == stdout)
        Console::WriteLine(msg);
    else
        Console::Error::WriteLine(msg);
}

static void DiagnosticPrint(DiagnosticLevel level, const std::string& prefix, const std::string& msg)
{
    auto stream = diagnostic_get_stream(level);
    if (stream == stdout)
        Console::WriteLine("%s%s", prefix.c_str(), msg.c_str());
    else
        Console::Error::WriteLine("%s%s", prefix.c_str(), msg.c_str());
}

void DiagnosticLog(DiagnosticLevel diagnosticLevel, const char* format, ...)
{
    va_list args;
    if (_log_levels[EnumValue(diagnosticLevel)])
    {
        // Level
        auto prefix = std::format("{}: ", kLevelStrings[EnumValue(diagnosticLevel)]);

        // Message
        va_start(args, format);
        auto msg = String::formatVA(format, args);
        va_end(args);

        DiagnosticPrint(diagnosticLevel, prefix, msg);
    }
}

void DiagnosticLogWithLocation(const std::source_location location, DiagnosticLevel diagnosticLevel, const char* format, ...)
{
    va_list args;
    if (_log_levels[EnumValue(diagnosticLevel)])
    {
        // Level and source code information
        std::string prefix;
        if (_log_location_enabled)
        {
            prefix = std::format(
                "{}[{}:{} ({})]: ", kLevelStrings[EnumValue(diagnosticLevel)], location.file_name(), location.line(),
                location.function_name());
        }
        else
        {
            prefix = std::format("{}: ", kLevelStrings[EnumValue(diagnosticLevel)]);
        }

        // Message
        va_start(args, format);
        auto msg = String::formatVA(format, args);
        va_end(args);

        DiagnosticPrint(diagnosticLevel, prefix, msg);
    }
}

void DiagnosticLogFmtArgs(DiagnosticLevel diagnosticLevel, std::string_view line, std::format_args args)
{
    if (_log_levels[EnumValue(diagnosticLevel)])
    {
        std::string msg = std::format("{}: ", kLevelStrings[EnumValue(diagnosticLevel)]);
        std::vformat_to(std::back_inserter(msg), line, args);

        DiagnosticPrint(diagnosticLevel, msg);
    }
}

void DiagnosticLogWithLocationFmtArgs(
    const std::source_location location, DiagnosticLevel diagnosticLevel, std::string_view line, std::format_args args)
{
    if (_log_levels[EnumValue(diagnosticLevel)])
    {
        // Level and source code information
        std::string msg;
        if (_log_location_enabled)
        {
            std::format_to(
                std::back_inserter(msg), "{}[{}:{} ({})]: ", kLevelStrings[EnumValue(diagnosticLevel)], location.file_name(),
                location.line(), location.function_name());
        }
        else
        {
            std::format_to(std::back_inserter(msg), "{}: ", kLevelStrings[EnumValue(diagnosticLevel)]);
        }
        std::vformat_to(std::back_inserter(msg), line, args);

        DiagnosticPrint(diagnosticLevel, msg);
    }
}

#endif // __ANDROID__
