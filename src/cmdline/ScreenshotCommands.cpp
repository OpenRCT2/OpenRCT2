extern "C"
{
    #include "../interface/screenshot.h"
}

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
    const char * * argv = (const char * *)argEnumerator->GetArguments() + argEnumerator->GetIndex() - 1;
    int argc = argEnumerator->GetCount() - argEnumerator->GetIndex() + 1;
    int result = cmdline_for_screenshot(argv, argc);
    if (result < 0) {
        return EXITCODE_FAIL;
    }
    return EXITCODE_OK;
}
