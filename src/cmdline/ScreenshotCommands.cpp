#include "CommandLine.hpp"

const CommandLineCommand CommandLine::ScreenshotCommands[]
{
    // Main commands
    DefineCommand("", "<file> <output_image> <width> <height> [<x> <y> <zoom> <rotation>]", nullptr, nullptr),
    DefineCommand("", "<file> <output_image> giant <zoom> <rotation>",                      nullptr, nullptr),
    CommandTableEnd
};
