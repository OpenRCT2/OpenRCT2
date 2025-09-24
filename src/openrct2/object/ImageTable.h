/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/JsonFwd.hpp"
#include "../drawing/ImageId.hpp"
#include "ObjectAsset.h"
#include "ResourceTable.h"

#include <optional>

struct G1Element;

namespace OpenRCT2
{
    class ImageSource;
    struct IReadObjectContext;

    class ImageTable : public ResourceTable
    {
    public:
        struct Entry
        {
            SourceKind Kind{};
            std::optional<ObjectAsset> Asset;
            std::optional<uint32_t> PathIndex;
            int32_t X{};
            int32_t Y{};
        };

    private:
        std::vector<Entry> _entries;
        ImageIndex _baseIndex = kImageIndexUndefined;
        size_t _loadedImageCount{};

        std::string _lastImageSourceKey;
        std::unique_ptr<ImageSource> _lastImageSource;

    public:
        ImageTable();
        ~ImageTable();

        std::vector<Entry>& GetEntries();

        /**
         * Read the entries from the given JSON into the table, but do not load anything.
         */
        bool ReadFromJson(IReadObjectContext& context, const json_t& root);

        /**
         * Load all available entries from the given table.
         */
        void LoadFrom(const ImageTable& table, size_t sourceStartIndex, size_t length);

        /**
         * Load all available entries.
         */
        ImageIndex Load();

        /**
         * Unloads all entries that are currently loaded.
         */
        void Unload();

        size_t GetCount() const;
        ImageIndex GetImage(uint32_t index) const;
        void LoadImage(std::vector<G1Element>& elements, size_t index);

    private:
        void ReadImageInfoFromJson(IReadObjectContext& context, const json_t& jImage, Entry& entry);
        void ReadEntryFromJson(IReadObjectContext& context, const json_t& jImage);
        ImageSource* GetOrLoadSource(Entry& entry);
        static std::string GetImageSourceKey(const Entry& entry);
        static std::unique_ptr<ImageSource> CreateImageSource(const Entry& entry);
    };
} // namespace OpenRCT2
