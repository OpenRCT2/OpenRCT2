#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "CommandLine.hpp"

#define SZ_DEFAULT   "default"
#define SZ_CLOSEST   "closest"
#define SZ_DITHERING "dithering"

extern "C"
{
	sint32 gSpriteMode = 0;
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
    DefineCommand("append",    "<spritefile> <input>",                SpriteOptions, HandleSprite),
    DefineCommand("build",     "<spritefile> <resourcedir> [silent]", SpriteOptions, HandleSprite),
    DefineCommand("create",    "<spritefile>",                        SpriteOptions, HandleSprite),
    DefineCommand("details",   "<spritefile> [idx]",                  SpriteOptions, HandleSprite),
    DefineCommand("export",    "<spritefile> <idx> <output>",         SpriteOptions, HandleSprite),
    DefineCommand("exportall", "<spritefile> <outputdir>",            SpriteOptions, HandleSprite),
    CommandTableEnd
};

static exitcode_t HandleSprite(CommandLineArgEnumerator *argEnumerator)
{
    if      (String::Equals(_mode, SZ_CLOSEST,   true)) gSpriteMode = 1;
    else if (String::Equals(_mode, SZ_DITHERING, true)) gSpriteMode = 2;
    Memory::Free(_mode);

    const char * * argv = (const char * *)argEnumerator->GetArguments() + argEnumerator->GetIndex() - 1;
    sint32 argc = argEnumerator->GetCount() - argEnumerator->GetIndex() + 1;
    sint32 result = cmdline_for_sprite(argv, argc);
    if (result < 0) {
        return EXITCODE_FAIL;
    }
    return EXITCODE_OK;
}
