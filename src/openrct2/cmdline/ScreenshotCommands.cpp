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

#include "../interface/Screenshot.h"
#include "CommandLine.hpp"

static exitcode_t HandleScreenshot(CommandLineArgEnumerator *argEnumerator);

const CommandLineCommand CommandLine::ScreenshotCommands[]
{
    // Main commands
    DefineCommand("", "<file> <output_image> <width> <height> [<x> <y> <zoom> <rotation>]", nullptr, HandleScreenshot),
    DefineCommand("", "<file> <output_image> giant <zoom> <rotation>",                      nullptr, HandleScreenshot),
    CommandTableEnd
};

static exitcode_t HandleScreenshot(CommandLineArgEnumerator *argEnumerator)
{
    const char * * argv = (const char * *)argEnumerator->GetArguments() + argEnumerator->GetIndex();
    sint32 argc = argEnumerator->GetCount() - argEnumerator->GetIndex();
    sint32 result = cmdline_for_screenshot(argv, argc);
    if (result < 0) {
        return EXITCODE_FAIL;
    }
    return EXITCODE_OK;
}
