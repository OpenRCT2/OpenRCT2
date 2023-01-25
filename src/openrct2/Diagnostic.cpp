/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Diagnostic.h"

#include "core/Console.hpp"
#include "core/String.hpp"

#include <cstdarg>
#include <cstdio>

#ifdef __ANDROID__
#    include <android/log.h>
#endif

[[maybe_unused]] static bool _log_location_enabled = true;
bool _log_levels[static_cast<uint8_t>(DiagnosticLevel::Count)] = {
    true, true, true, false, true,
};

static FILE* diagnostic_get_stream(DiagnosticLevel level)
{
    switch (level)
    {
        case DiagnosticLevel::Verbose:
        case DiagnosticLevel::Information:
            return stdout;
        default:
            return stderr;
    }
}

#ifdef __ANDROID__

int _android_log_priority[static_cast<uint8_t>(DiagnosticLevel::Count)] = {
    ANDROID_LOG_FATAL, ANDROID_LOG_ERROR, ANDROID_LOG_WARN, ANDROID_LOG_VERBOSE, ANDROID_LOG_INFO,
};

void DiagnosticLog(DiagnosticLevel diagnosticLevel, const char* format, ...)
{
    va_list args;

    if (!_log_levels[static_cast<uint8_t>(diagnosticLevel)])
        return;

    va_start(args, format);
    __android_log_vprint(_android_log_priority[static_cast<uint8_t>(diagnosticLevel)], "OpenRCT2", format, args);
    va_end(args);
}

void DiagnosticLogWithLocation(
    DiagnosticLevel diagnosticLevel, const char* file, const char* function, int32_t line, const char* format, ...)
{
    va_list args;
    char buf[1024];

    if (!_log_levels[static_cast<uint8_t>(diagnosticLevel)])
        return;

    snprintf(buf, 1024, "[%s:%d (%s)]: ", file, line, function);

    va_start(args, format);
    __android_log_vprint(_android_log_priority[static_cast<uint8_t>(diagnosticLevel)], file, format, args);
    va_end(args);
}

#else

static constexpr const char* _level_strings[] = {
    "FATAL", "ERROR", "WARNING", "VERBOSE", "INFO",
};

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
    if (_log_levels[static_cast<uint8_t>(diagnosticLevel)])
    {
        // Level
        auto prefix = String::StdFormat("%s: ", _level_strings[static_cast<uint8_t>(diagnosticLevel)]);

        // Message
        va_start(args, format);
        auto msg = String::Format_VA(format, args);
        va_end(args);

        DiagnosticPrint(diagnosticLevel, prefix, msg);
    }
}

void DiagnosticLogWithLocation(
    DiagnosticLevel diagnosticLevel, const char* file, const char* function, int32_t line, const char* format, ...)
{
    va_list args;
    if (_log_levels[static_cast<uint8_t>(diagnosticLevel)])
    {
        // Level and source code information
        std::string prefix;
        if (_log_location_enabled)
        {
            prefix = String::StdFormat(
                "%s[%s:%d (%s)]: ", _level_strings[static_cast<uint8_t>(diagnosticLevel)], file, line, function);
        }
        else
        {
            prefix = String::StdFormat("%s: ", _level_strings[static_cast<uint8_t>(diagnosticLevel)]);
        }

        // Message
        va_start(args, format);
        auto msg = String::Format_VA(format, args);
        va_end(args);

        DiagnosticPrint(diagnosticLevel, prefix, msg);
    }
}

#endif // __ANDROID__
