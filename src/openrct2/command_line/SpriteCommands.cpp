/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../CommandLineSprite.h"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "CommandLine.hpp"

using namespace OpenRCT2;

#define SZ_DEFAULT "default"
#define SZ_CLOSEST "closest"
#define SZ_DITHERING "dithering"

using ImportMode = OpenRCT2::Drawing::ImportMode;

ImportMode gSpriteMode = ImportMode::Default;

static const char* _mode;

// clang-format off
static constexpr CommandLineOptionDefinition kSpriteOptions[]
{
    { CMDLINE_TYPE_STRING, &_mode, 'm', "mode", "the type of sprite conversion <" SZ_DEFAULT "|" SZ_CLOSEST "|" SZ_DITHERING ">" },
    kOptionTableEnd
};

static exitcode_t HandleSprite(CommandLineArgEnumerator *argEnumerator);

const CommandLineCommand CommandLine::kSpriteCommands[]
{
    // Main commands
    DefineCommand("append",       "<spritefile> <input> [x_offset y_offset]", kSpriteOptions, HandleSprite),
    DefineCommand("build",        "<spritefile> <json path> [silent]",        kSpriteOptions, HandleSprite),
    DefineCommand("combine",      "<index file> <image file> <output>",       kSpriteOptions, HandleSprite),
    DefineCommand("create",       "<spritefile>",                             kSpriteOptions, HandleSprite),
    DefineCommand("details",      "<spritefile> [idx]",                       kSpriteOptions, HandleSprite),
    DefineCommand("export",       "<spritefile> <idx> <output>",              kSpriteOptions, HandleSprite),
    DefineCommand("exportall",    "<spritefile> <output directory>",          kSpriteOptions, HandleSprite),
    DefineCommand("exportalldat", "<DAT identifier> <output directory>",      kSpriteOptions, HandleSprite),

    kCommandTableEnd
};
// clang-format on

static exitcode_t HandleSprite(CommandLineArgEnumerator* argEnumerator)
{
    if (String::iequals(_mode, SZ_CLOSEST))
        gSpriteMode = ImportMode::Closest;
    else if (String::iequals(_mode, SZ_DITHERING))
        gSpriteMode = ImportMode::Dithering;
    Memory::Free(_mode);

    const char** argv = const_cast<const char**>(argEnumerator->GetArguments()) + argEnumerator->GetIndex() - 1;
    int32_t argc = argEnumerator->GetCount() - argEnumerator->GetIndex() + 1;
    int32_t result = CommandLineForSprite(argv, argc);
    if (result < 0)
    {
        return EXITCODE_FAIL;
    }
    return EXITCODE_OK;
}
