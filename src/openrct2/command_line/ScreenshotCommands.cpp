/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Screenshot.h"
#include "CommandLine.hpp"

using namespace OpenRCT2;

static ScreenshotOptions _options;

// clang-format off
static constexpr CommandLineOptionDefinition kScreenshotOptionsDef[]
{
    { CMDLINE_TYPE_INTEGER, &_options.weather,       kNAC, "weather",       "weather to be used (0 = default, 1 = sunny, ..., 6 = thunder)." },
    { CMDLINE_TYPE_SWITCH,  &_options.hide_guests,   kNAC, "no-peeps",      "hide peeps" },
    { CMDLINE_TYPE_SWITCH,  &_options.hide_sprites,  kNAC, "no-sprites",    "hide all sprites (e.g. balloons, vehicles, guests)" },
    { CMDLINE_TYPE_SWITCH,  &_options.clear_grass,   kNAC, "clear-grass",   "set all grass to be clear of weeds" },
    { CMDLINE_TYPE_SWITCH,  &_options.mowed_grass,   kNAC, "mowed-grass",   "set all grass to be mowed" },
    { CMDLINE_TYPE_SWITCH,  &_options.water_plants,  kNAC, "water-plants",  "water plants for the screenshot" },
    { CMDLINE_TYPE_SWITCH,  &_options.fix_vandalism, kNAC, "fix-vandalism", "fix vandalism for the screenshot" },
    { CMDLINE_TYPE_SWITCH,  &_options.remove_litter, kNAC, "remove-litter", "remove litter for the screenshot" },
    { CMDLINE_TYPE_SWITCH,  &_options.tidy_up_park,  kNAC, "tidy-up-park",  "clear grass, water plants, fix vandalism and remove litter" },
    { CMDLINE_TYPE_SWITCH,  &_options.transparent,   kNAC, "transparent",   "make the background transparent" },
    kOptionTableEnd
};

static exitcode_t HandleScreenshot(CommandLineArgEnumerator *argEnumerator);

const CommandLineCommand CommandLine::kScreenshotCommands[]
{
    // Main commands
    DefineCommand("", "<file> <output_image> <width> <height> [<x> <y> <zoom> <rotation>]", kScreenshotOptionsDef, HandleScreenshot),
    DefineCommand("", "<file> <output_image> giant <zoom> <rotation>",                      kScreenshotOptionsDef, HandleScreenshot),
    kCommandTableEnd
};
// clang-format on

static exitcode_t HandleScreenshot(CommandLineArgEnumerator* argEnumerator)
{
    const char** argv = const_cast<const char**>(argEnumerator->GetArguments()) + argEnumerator->GetIndex();
    int32_t argc = argEnumerator->GetCount() - argEnumerator->GetIndex();
    int32_t result = CommandLineForScreenshot(argv, argc, &_options);
    if (result < 0)
    {
        return EXITCODE_FAIL;
    }
    return EXITCODE_OK;
}
