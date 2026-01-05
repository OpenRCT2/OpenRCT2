/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SpriteCommands.h"

#include "../../Context.h"
#include "../../OpenRCT2.h"
#include "../../core/Memory.hpp"
#include "../../core/String.hpp"
#include "../../drawing/Drawing.h"
#include "../../object/ObjectFactory.h"
#include "../CommandLine.hpp"

#include <cstring>
#include <optional>

// TODO: Remove when C++20 is enabled and std::format can be used
#include <iomanip>
#include <sstream>

// FIXME: replace with a constexpr that will also work for string interpolation.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define SZ_DEFAULT "default"
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define SZ_CLOSEST "closest"
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define SZ_DITHERING "dithering"

namespace OpenRCT2::CommandLine::Sprite
{
    using namespace OpenRCT2::Drawing;

    static const char* _mode;

    // clang-format off
    static constexpr CommandLineOptionDefinition kSpriteOptions[]
    {
        { CMDLINE_TYPE_STRING, &_mode, 'm', "mode", "the type of sprite conversion <" SZ_DEFAULT "|" SZ_CLOSEST "|" SZ_DITHERING ">" },
        kOptionTableEnd
    };

    static exitcode_t HandleSprite(CommandLineArgEnumerator *argEnumerator);

    const CommandLineCommand kSpriteCommands[]
    {
        // Main commands
        DefineCommand("append",       "<spritefile> <input> [x_offset y_offset]", kSpriteOptions, HandleSprite),
        DefineCommand("build",        "<spritefile> <json path> [silent]",        kSpriteOptions, HandleSprite),
        DefineCommand("combine",      "<index file> <image file> <output>",       kSpriteOptions, HandleSprite),
        DefineCommand("create",       "<spritefile>",                             kSpriteOptions, HandleSprite),
        DefineCommand("details",      "<spritefile> [idx]",                       kSpriteOptions, HandleSprite),
        DefineCommand("export",       "<spritefile> <idx> <output>",              kSpriteOptions, HandleSprite),
        DefineCommand("exportall",    "<spritefile> <output directory>",          kSpriteOptions, HandleSprite),
        DefineCommand("exportobject", "<DAT identifier> <output directory>",      kSpriteOptions, HandleSprite),

        kCommandTableEnd
    };
    // clang-format on

    static exitcode_t HandleSprite(CommandLineArgEnumerator* argEnumerator)
    {
        auto spriteMode = ImportMode::Default;
        if (String::iequals(_mode, SZ_CLOSEST))
            spriteMode = ImportMode::Closest;
        else if (String::iequals(_mode, SZ_DITHERING))
            spriteMode = ImportMode::Dithering;
        Memory::Free(_mode);

        const char** argv = const_cast<const char**>(argEnumerator->GetArguments()) + argEnumerator->GetIndex() - 1;
        int32_t argc = argEnumerator->GetCount() - argEnumerator->GetIndex() + 1;

        gOpenRCT2Headless = true;
        if (argc == 0)
            return -1;

        if (String::iequals(argv[0], "details"))
        {
            return details(argv, argc);
        }

        if (String::iequals(argv[0], "export"))
        {
            return exportSingle(argv, argc);
        }

        if (String::iequals(argv[0], "exportall"))
        {
            return exportAll(argv, argc);
        }

        if (String::iequals(argv[0], "exportobject"))
        {
            return exportObject(argv, argc);
        }

        if (String::iequals(argv[0], "create"))
        {
            return create(argv, argc);
        }

        if (String::iequals(argv[0], "append"))
        {
            return append(argv, argc, spriteMode);
        }

        if (String::iequals(argv[0], "build"))
        {
            return build(argv, argc, spriteMode);
        }

        if (String::iequals(argv[0], "combine"))
        {
            return combine(argv, argc);
        }

        fprintf(stderr, "Unknown sprite command.\n");
        return EXITCODE_FAIL;
    }

    bool SpriteImageExport(const G1Element& spriteElement, u8string_view outPath)
    {
        const size_t pixelBufferSize = static_cast<size_t>(spriteElement.width) * spriteElement.height;
        auto pixelBuffer = std::make_unique<uint8_t[]>(pixelBufferSize);
        auto pixels = pixelBuffer.get();

        RenderTarget rt;
        rt.bits = pixels;
        rt.x = 0;
        rt.y = 0;
        rt.width = spriteElement.width;
        rt.height = spriteElement.height;
        rt.pitch = 0;
        rt.zoom_level = ZoomLevel{ 0 };

        DrawSpriteArgs args(
            ImageId(), PaletteMap::GetDefault(), spriteElement, 0, 0, spriteElement.width, spriteElement.height, pixels);
        GfxSpriteToBuffer(rt, args);

        auto const pixels8 = rt.bits;
        auto const pixelsLen = rt.LineStride() * rt.WorldHeight();
        try
        {
            Image image;
            image.Width = rt.width;
            image.Height = rt.height;
            image.Depth = 8;
            image.Stride = rt.LineStride();
            image.Palette = StandardPalette;
            image.Pixels = std::vector<uint8_t>(pixels8, pixels8 + pixelsLen);
            Imaging::WriteToFile(outPath, image, ImageFormat::png);
            return true;
        }
        catch (const std::exception& e)
        {
            fprintf(stderr, "Unable to write png: %s", e.what());
            return false;
        }
    }

    std::optional<Image> SpriteImageLoad(u8string_view path, ImageImportMeta meta)
    {
        try
        {
            auto format = ImageFormat::png32;
            if (meta.palette == Palette::KeepIndices)
            {
                format = ImageFormat::png;
            }
            return Imaging::ReadFromFile(path, format);
        }
        catch (const std::exception& e)
        {
            fprintf(stderr, "%s\n", e.what());
            return std::nullopt;
        }
    }

    // TODO: Remove when C++20 is enabled and std::format can be used
    std::string PopStr(std::ostringstream& oss)
    {
        auto str = oss.str();
        oss.str("");
        oss.clear();
        return str;
    }
} // namespace OpenRCT2::CommandLine::Sprite
