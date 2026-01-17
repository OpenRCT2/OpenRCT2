/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SpriteFile.h"

#include "../../core/FileStream.h"
#include "../../drawing/ImageImporter.h"

namespace OpenRCT2::CommandLine::Sprite
{
    std::optional<SpriteFile> SpriteFile::Open(const utf8* path)
    {
        try
        {
            FileStream stream(path, FileMode::open);

            SpriteFile spriteFile;
            stream.Read(&spriteFile.Header, sizeof(G1Header));

            if (spriteFile.Header.numEntries > 0)
            {
                spriteFile.Entries.reserve(spriteFile.Header.numEntries);

                for (uint32_t i = 0; i < spriteFile.Header.numEntries; ++i)
                {
                    StoredG1Element entry32bit{};
                    stream.Read(&entry32bit, sizeof(entry32bit));
                    G1Element entry{};

                    entry.offset = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(entry32bit.offset));
                    entry.width = entry32bit.width;
                    entry.height = entry32bit.height;
                    entry.xOffset = entry32bit.xOffset;
                    entry.yOffset = entry32bit.yOffset;
                    entry.flags = entry32bit.flags;
                    entry.zoomedOffset = entry32bit.zoomedOffset;
                    spriteFile.Entries.push_back(std::move(entry));
                }
                spriteFile.Data.resize(spriteFile.Header.totalSize);
                stream.Read(spriteFile.Data.data(), spriteFile.Header.totalSize);
            }
            spriteFile.MakeEntriesAbsolute();
            return spriteFile;
        }
        catch (IOException&)
        {
            return std::nullopt;
        }
    }

    void SpriteFile::MakeEntriesAbsolute()
    {
        if (!isAbsolute)
        {
            for (auto& entry : Entries)
                entry.offset += reinterpret_cast<uintptr_t>(Data.data());
        }
        isAbsolute = true;
    }

    void SpriteFile::MakeEntriesRelative()
    {
        if (isAbsolute)
        {
            for (auto& entry : Entries)
                entry.offset -= reinterpret_cast<uintptr_t>(Data.data());
        }
        isAbsolute = false;
    }

    void SpriteFile::AddImage(Drawing::ImageImportResult& image)
    {
        Header.numEntries++;
        // New image will have its data inserted after previous image
        uint8_t* newElementOffset = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(Header.totalSize));
        Header.totalSize += static_cast<uint32_t>(image.Buffer.size());
        Entries.reserve(Header.numEntries);

        {
            ScopedRelativeSpriteFile scopedRelative(*this);
            Data.reserve(Header.totalSize);
            Entries.push_back(image.Element);
            Entries.back().offset = newElementOffset;
            const auto& buffer = image.Buffer;
            std::copy(buffer.begin(), buffer.end(), std::back_inserter(Data));
        }
    }

    void SpriteFile::addPalette(Drawing::PaletteImportResult& palette)
    {
        AddImage(*reinterpret_cast<Drawing::ImageImportResult*>(&palette));
    }

    bool SpriteFile::Save(const utf8* path)
    {
        try
        {
            FileStream stream(path, FileMode::write);
            stream.Write(&Header, sizeof(G1Header));

            if (Header.numEntries > 0)
            {
                ScopedRelativeSpriteFile scopedRelative(*this);

                for (const auto& entry : Entries)
                {
                    StoredG1Element entry32bit{};

                    entry32bit.offset = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(const_cast<uint8_t*>(entry.offset)));
                    entry32bit.width = entry.width;
                    entry32bit.height = entry.height;
                    entry32bit.xOffset = entry.xOffset;
                    entry32bit.yOffset = entry.yOffset;
                    entry32bit.flags = entry.flags;
                    entry32bit.zoomedOffset = entry.zoomedOffset;

                    stream.Write(&entry32bit, sizeof(entry32bit));
                }
                stream.Write(Data.data(), Header.totalSize);
            }
            return true;
        }
        catch (IOException&)
        {
            return false;
        }
    }

} // namespace OpenRCT2::CommandLine::Sprite
