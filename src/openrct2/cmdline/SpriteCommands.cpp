/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../CmdlineSprite.h"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "CommandLine.hpp"

#define SZ_DEFAULT "default"
#define SZ_CLOSEST "closest"
#define SZ_DITHERING "dithering"
namespace OpenRCT2
{
    using ImportMode = OpenRCT2::Drawing::ImageImporter::ImportMode;

    ImportMode gSpriteMode = ImportMode::Default;

    static const char* _mode;

    // clang-format off
static constexpr const CommandLineOptionDefinition SpriteOptions[]
{
    { CMDLINE_TYPE_STRING, &_mode, 'm', "mode", "the type of sprite conversion <" SZ_DEFAULT "|" SZ_CLOSEST "|" SZ_DITHERING ">" },
    OptionTableEnd
};

static exitcode_t HandleSprite(CommandLineArgEnumerator *argEnumerator);

const CommandLineCommand CommandLine::SpriteCommands[]
{
    // Main commands
    DefineCommand("append",       "<spritefile> <input> [x_offset y_offset]", SpriteOptions, HandleSprite),
    DefineCommand("build",        "<spritefile> <json path> [silent]",        SpriteOptions, HandleSprite),
    DefineCommand("create",       "<spritefile>",                             SpriteOptions, HandleSprite),
    DefineCommand("details",      "<spritefile> [idx]",                       SpriteOptions, HandleSprite),
    DefineCommand("export",       "<spritefile> <idx> <output>",              SpriteOptions, HandleSprite),
    DefineCommand("exportall",    "<spritefile> <output directory>",          SpriteOptions, HandleSprite),
    DefineCommand("exportalldat", "<DAT identifier> <output directory>",      SpriteOptions, HandleSprite),

    CommandTableEnd
};
    // clang-format on

    static exitcode_t HandleSprite(CommandLineArgEnumerator* argEnumerator)
    {
        if (String::Equals(_mode, SZ_CLOSEST, true))
            gSpriteMode = ImportMode::Closest;
        else if (String::Equals(_mode, SZ_DITHERING, true))
            gSpriteMode = ImportMode::Dithering;
        Memory::Free(_mode);

        const char** argv = const_cast<const char**>(argEnumerator->GetArguments()) + argEnumerator->GetIndex() - 1;
        int32_t argc = argEnumerator->GetCount() - argEnumerator->GetIndex() + 1;
        int32_t result = CmdLineForSprite(argv, argc);
        if (result < 0)
        {
            return EXITCODE_FAIL;
        }
        return EXITCODE_OK;
    }
} // namespace OpenRCT2
