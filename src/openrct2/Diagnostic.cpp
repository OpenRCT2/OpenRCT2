#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include <cstdarg>
#include <cstdio>
#include "core/String.hpp"
#include "Diagnostic.h"

#ifdef __ANDROID__
#include <android/log.h>
#endif

[[maybe_unused]] static bool _log_location_enabled = true;
bool _log_levels[DIAGNOSTIC_LEVEL_COUNT] = { true, true, true, false, true };

static FILE * diagnostic_get_stream(DIAGNOSTIC_LEVEL level)
{
    switch (level) {
    case DIAGNOSTIC_LEVEL_VERBOSE:
    case DIAGNOSTIC_LEVEL_INFORMATION:
        return stdout;
    default:
        return stderr;
    }
}

#ifdef __ANDROID__

int _android_log_priority[DIAGNOSTIC_LEVEL_COUNT] = {ANDROID_LOG_FATAL, ANDROID_LOG_ERROR, ANDROID_LOG_WARN, ANDROID_LOG_VERBOSE, ANDROID_LOG_INFO};

void diagnostic_log(DIAGNOSTIC_LEVEL diagnosticLevel, const char *format, ...)
{
    va_list args;

    if (!_log_levels[diagnosticLevel])
        return;

    va_start(args, format);
    __android_log_vprint(_android_log_priority[diagnosticLevel], "OpenRCT2", format, args);
    va_end(args);
}

void diagnostic_log_with_location(DIAGNOSTIC_LEVEL diagnosticLevel, const char *file, const char *function, sint32 line, const char *format, ...)
{
    va_list args;
    char buf[1024];

    if (!_log_levels[diagnosticLevel])
        return;

    snprintf(buf, 1024, "[%s:%d (%s)]: ", file, line, function);

    va_start(args, format);
    __android_log_vprint(_android_log_priority[diagnosticLevel], file, format, args);
    va_end(args);
}

#else

static constexpr const char * _level_strings[] = {
    "FATAL",
    "ERROR",
    "WARNING",
    "VERBOSE",
    "INFO"
};

void diagnostic_log(DIAGNOSTIC_LEVEL diagnosticLevel, const char *format, ...)
{
    va_list args;
    if (_log_levels[diagnosticLevel])
    {
        // Level
        auto prefix = String::StdFormat("%s: ", _level_strings[diagnosticLevel]);

        // Message
        va_start(args, format);
        auto msg = String::StdFormat_VA(format, args);
        va_end(args);

        auto stream = diagnostic_get_stream(diagnosticLevel);
        fprintf(stream, "%s%s\n", prefix.c_str(), msg.c_str());
    }
}

void diagnostic_log_with_location(DIAGNOSTIC_LEVEL diagnosticLevel, const char *file, const char *function, sint32 line, const char *format, ...)
{
    va_list args;
    if (_log_levels[diagnosticLevel])
    {
        // Level and source code information
        std::string prefix;
        if (_log_location_enabled)
        {
            prefix = String::StdFormat("%s[%s:%d (%s)]: ", _level_strings[diagnosticLevel], file, line, function);
        }
        else
        {
            prefix = String::StdFormat("%s: ", _level_strings[diagnosticLevel]);
        }

        // Message
        va_start(args, format);
        auto msg = String::StdFormat_VA(format, args);
        va_end(args);

        auto stream = diagnostic_get_stream(diagnosticLevel);
        fprintf(stream, "%s%s\n", prefix.c_str(), msg.c_str());
    }
}

#endif // __ANDROID__
