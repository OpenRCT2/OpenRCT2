/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

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
    { CMDLINE_TYPE_SWITCH,  &options.fix_vandalism, NAC, "fix-vandalism", "fix vandalism for the screenshot" },
    { CMDLINE_TYPE_SWITCH,  &options.remove_litter, NAC, "remove-litter", "remove litter for the screenshot" },
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
    int32_t argc = argEnumerator->GetCount() - argEnumerator->GetIndex();
    int32_t result = cmdline_for_screenshot(argv, argc, &options);
    if (result < 0) {
        return EXITCODE_FAIL;
    }
    return EXITCODE_OK;
}
