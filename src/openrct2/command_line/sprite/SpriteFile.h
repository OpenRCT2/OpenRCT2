/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../core/StringTypes.h"
#include "../../drawing/G1Element.h"

#include <optional>

namespace OpenRCT2::Drawing
{
    struct ImageImportResult;
    struct PaletteImportResult;
} // namespace OpenRCT2::Drawing

namespace OpenRCT2::CommandLine::Sprite
{
    class SpriteFile
    {
    public:
        G1Header Header{};
        std::vector<G1Element> Entries;
        std::vector<uint8_t> Data;
        void AddImage(Drawing::ImageImportResult& image);
        void addPalette(Drawing::PaletteImportResult& image);
        bool Save(const utf8* path);
        static std::optional<SpriteFile> Open(const utf8* path);

    private:
        class ScopedRelativeSpriteFile
        {
        private:
            SpriteFile& _SpriteFile;
            bool _WasAbsolute;

        public:
            ScopedRelativeSpriteFile(SpriteFile& sFile)
                : _SpriteFile(sFile)
                , _WasAbsolute(sFile.isAbsolute)
            {
                if (_WasAbsolute)
                    _SpriteFile.MakeEntriesRelative();
            }

            ~ScopedRelativeSpriteFile()
            {
                if (_WasAbsolute)
                    _SpriteFile.MakeEntriesAbsolute();
            }
        };
        bool isAbsolute = false;
        void MakeEntriesAbsolute();
        void MakeEntriesRelative();
    };
} // namespace OpenRCT2::CommandLine::Sprite
