#include "CommandLine.hpp"

static const char * _mode;

static const CommandLineOptionDefinition SpriteOptions[]
{
    { CMDLINE_TYPE_STRING, &_mode, 'm', "mode", "the type of sprite conversion <default|simple|dithering>" },
    OptionTableEnd
};

static exitcode_t Handle(CommandLineArgEnumerator *argEnumerator);

const CommandLineCommand CommandLine::SpriteCommands[]
{
    // Main commands
    DefineCommand("details", "<spritefile> [idx]",                  SpriteOptions, Handle),
    DefineCommand("export",  "<spritefile> <idx> <output>",         SpriteOptions, Handle),
    DefineCommand("build",   "<spritefile> <resourcedir> [silent]", SpriteOptions, Handle),
    CommandTableEnd
};

static exitcode_t Handle(CommandLineArgEnumerator *argEnumerator)
{
    return EXITCODE_OK;
}
