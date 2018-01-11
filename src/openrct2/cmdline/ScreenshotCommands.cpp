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

static ScreenshotOptions options;

// clang-format off
static constexpr const CommandLineOptionDefinition ScreenshotOptionsDef[]
{
    { CMDLINE_TYPE_INTEGER, &options.weather,       NAC, "weather",       "weather to be used (0 = default, 1 = sunny, ..., 6 = thunder)." },
    { CMDLINE_TYPE_SWITCH,  &options.hide_guests,   NAC, "no-peeps",      "hide peeps" },
    { CMDLINE_TYPE_SWITCH,  &options.hide_sprites,  NAC, "no-sprites",    "hide all sprites (e.g. balloons, vehicles, guests)" },
    { CMDLINE_TYPE_SWITCH,  &options.clear_grass,   NAC, "clear-grass",   "set all grass to be clear of weeds" },
    { CMDLINE_TYPE_SWITCH,  &options.mowed_grass,   NAC, "mowed-grass",   "set all grass to be mowed" },
    { CMDLINE_TYPE_SWITCH,  &options.water_plants,  NAC, "water-plants",  "water plants for the screenshot" },
    { CMDLINE_TYPE_SWITCH,  &options.fix_vandalism, NAC, "fix vandalism", "fix vandalism" },
    { CMDLINE_TYPE_SWITCH,  &options.remove_litter, NAC, "remove litter", "remove litter" },
    { CMDLINE_TYPE_SWITCH,  &options.tidy_up_park,  NAC, "tidy-up-park",  "clear grass, water plants, fix vandalism and remove litter" },
    OptionTableEnd
};

static exitcode_t HandleScreenshot(CommandLineArgEnumerator *argEnumerator);

const CommandLineCommand CommandLine::ScreenshotCommands[]
{
    // Main commands
    DefineCommand("", "<file> <output_image> <width> <height> [<x> <y> <zoom> <rotation>]", ScreenshotOptionsDef, HandleScreenshot),
    DefineCommand("", "<file> <output_image> giant <zoom> <rotation>",                      ScreenshotOptionsDef, HandleScreenshot),
    CommandTableEnd
};
// clang-format on

static exitcode_t HandleScreenshot(CommandLineArgEnumerator *argEnumerator)
{
    const char * * argv = (const char * *)argEnumerator->GetArguments() + argEnumerator->GetIndex();
    sint32 argc = argEnumerator->GetCount() - argEnumerator->GetIndex();
    sint32 result = cmdline_for_screenshot(argv, argc, &options);
    if (result < 0) {
        return EXITCODE_FAIL;
    }
    return EXITCODE_OK;
}
