#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "CommandLine.hpp"

#define SZ_DEFAULT   "default"
#define SZ_CLOSEST   "closest"
#define SZ_DITHERING "dithering"

extern "C"
{
    int gSpriteMode = 0;

    int cmdline_for_sprite(const char **argv, int argc);
}

static const char * _mode;

static const CommandLineOptionDefinition SpriteOptions[]
{
    { CMDLINE_TYPE_STRING, &_mode, 'm', "mode", "the type of sprite conversion <" SZ_DEFAULT "|" SZ_CLOSEST "|" SZ_DITHERING ">" },
    OptionTableEnd
};

static exitcode_t HandleSprite(CommandLineArgEnumerator *argEnumerator);

const CommandLineCommand CommandLine::SpriteCommands[]
{
    // Main commands
    DefineCommand("details", "<spritefile> [idx]",                  SpriteOptions, HandleSprite),
    DefineCommand("export",  "<spritefile> <idx> <output>",         SpriteOptions, HandleSprite),
    DefineCommand("build",   "<spritefile> <resourcedir> [silent]", SpriteOptions, HandleSprite),
    CommandTableEnd
};

static exitcode_t HandleSprite(CommandLineArgEnumerator *argEnumerator)
{
    if      (String::Equals(_mode, SZ_CLOSEST,   true)) gSpriteMode = 1;
    else if (String::Equals(_mode, SZ_DITHERING, true)) gSpriteMode = 2;
    Memory::Free(_mode);

    const char * * argv = (const char * *)argEnumerator->GetArguments() + argEnumerator->GetIndex() - 1;
    int argc = argEnumerator->GetCount() - argEnumerator->GetIndex() + 1;
    int result = cmdline_for_sprite(argv, argc);
    if (result < 0) {
        return EXITCODE_FAIL;
    }
    return EXITCODE_OK;
}
