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

#ifndef _DIAGNOSTIC_H_
#define _DIAGNOSTIC_H_

#include "common.h"

typedef enum {
    DIAGNOSTIC_LEVEL_FATAL,
    DIAGNOSTIC_LEVEL_ERROR,
    DIAGNOSTIC_LEVEL_WARNING,
    DIAGNOSTIC_LEVEL_VERBOSE,
    DIAGNOSTIC_LEVEL_INFORMATION,
    DIAGNOSTIC_LEVEL_COUNT
} DiagnosticLevel;

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

extern bool _log_levels[DIAGNOSTIC_LEVEL_COUNT];

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void diagnostic_log(DiagnosticLevel diagnosticLevel, const char *format, ...);
void diagnostic_log_with_location(DiagnosticLevel diagnosticLevel, const char *file, const char *function, sint32 line, const char *format, ...);

#ifdef __cplusplus
}
#endif // __cplusplus

#ifdef _MSC_VER
#define diagnostic_log_macro(level, format, ...)    diagnostic_log_with_location(level, __FILE__, __FUNCTION__, __LINE__, format, ## __VA_ARGS__)
#else
    #define diagnostic_log_macro(level, format, ...)    diagnostic_log_with_location(level, __FILE__, __func__, __LINE__, format, ## __VA_ARGS__)
#endif // _MSC_VER

#define log_fatal(format, ...)      diagnostic_log_macro(DIAGNOSTIC_LEVEL_FATAL, format, ## __VA_ARGS__)
#define log_error(format, ...)      diagnostic_log_macro(DIAGNOSTIC_LEVEL_ERROR, format, ## __VA_ARGS__)
#define log_warning(format, ...)    diagnostic_log_macro(DIAGNOSTIC_LEVEL_WARNING, format, ## __VA_ARGS__)
#define log_verbose(format, ...)    diagnostic_log(DIAGNOSTIC_LEVEL_VERBOSE, format, ## __VA_ARGS__)
#define log_info(format, ...)       diagnostic_log_macro(DIAGNOSTIC_LEVEL_INFORMATION, format, ## __VA_ARGS__)

#endif
