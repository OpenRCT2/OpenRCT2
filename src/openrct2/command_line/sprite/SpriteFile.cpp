/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SpriteFile.h"

#include "../../core/FileStream.h"

namespace OpenRCT2::CommandLine::Sprite
{
    std::optional<SpriteFile> SpriteFile::Open(const utf8* path)
    {
        try
        {
            FileStream stream(path, FileMode::open);

            SpriteFile spriteFile;
            stream.Read(&spriteFile.Header, sizeof(RCTG1Header));

            if (spriteFile.Header.num_entries > 0)
            {
                spriteFile.Entries.reserve(spriteFile.Header.num_entries);

                for (uint32_t i = 0; i < spriteFile.Header.num_entries; ++i)
                {
                    RCTG1Element entry32bit{};
                    stream.Read(&entry32bit, sizeof(entry32bit));
                    G1Element entry{};

                    entry.offset = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(entry32bit.offset));
                    entry.width = entry32bit.width;
                    entry.height = entry32bit.height;
                    entry.xOffset = entry32bit.x_offset;
                    entry.yOffset = entry32bit.y_offset;
                    entry.flags = entry32bit.flags;
                    entry.zoomedOffset = entry32bit.zoomed_offset;
                    spriteFile.Entries.push_back(std::move(entry));
                }
                spriteFile.Data.resize(spriteFile.Header.total_size);
                stream.Read(spriteFile.Data.data(), spriteFile.Header.total_size);
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

    void SpriteFile::AddImage(ImageImporter::ImportResult& image)
    {
        Header.num_entries++;
        // New image will have its data inserted after previous image
        uint8_t* newElementOffset = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(Header.total_size));
        Header.total_size += static_cast<uint32_t>(image.Buffer.size());
        Entries.reserve(Header.num_entries);

        {
            ScopedRelativeSpriteFile scopedRelative(*this);
            Data.reserve(Header.total_size);
            Entries.push_back(image.Element);
            Entries.back().offset = newElementOffset;
            const auto& buffer = image.Buffer;
            std::copy(buffer.begin(), buffer.end(), std::back_inserter(Data));
        }
    }

    bool SpriteFile::Save(const utf8* path)
    {
        try
        {
            FileStream stream(path, FileMode::write);
            stream.Write(&Header, sizeof(RCTG1Header));

            if (Header.num_entries > 0)
            {
                ScopedRelativeSpriteFile scopedRelative(*this);

                for (const auto& entry : Entries)
                {
                    RCTG1Element entry32bit{};

                    entry32bit.offset = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(const_cast<uint8_t*>(entry.offset)));
                    entry32bit.width = entry.width;
                    entry32bit.height = entry.height;
                    entry32bit.x_offset = entry.xOffset;
                    entry32bit.y_offset = entry.yOffset;
                    entry32bit.flags = entry.flags;
                    entry32bit.zoomed_offset = entry.zoomedOffset;

                    stream.Write(&entry32bit, sizeof(entry32bit));
                }
                stream.Write(Data.data(), Header.total_size);
            }
            return true;
        }
        catch (IOException&)
        {
            return false;
        }
    }

} // namespace OpenRCT2::CommandLine::Sprite
