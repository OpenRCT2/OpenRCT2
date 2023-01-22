/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/JsonFwd.hpp"
#include "../drawing/Drawing.h"

#include <memory>
#include <vector>
namespace OpenRCT2
{
    struct Image;
    struct IReadObjectContext;

    struct IStream;

    class ImageTable
    {
    private:
        std::unique_ptr<uint8_t[]> _data;
        std::vector<G1Element> _entries;

        /**
         * Container for a G1 image, additional information and RAII. Used by ReadJson
         */
        struct RequiredImage;
        [[nodiscard]] std::vector<std::pair<std::string, Image>> GetImageSources(
            IReadObjectContext* context, json_t& jsonImages);
        [[nodiscard]] static std::vector<std::unique_ptr<ImageTable::RequiredImage>> ParseImages(
            IReadObjectContext* context, std::string s);
        /**
         * @note root is deliberately left non-const: json_t behaviour changes when const
         */
        [[nodiscard]] static std::vector<std::unique_ptr<ImageTable::RequiredImage>> ParseImages(
            IReadObjectContext* context, std::vector<std::pair<std::string, Image>>& imageSources, json_t& el);
        [[nodiscard]] static std::vector<std::unique_ptr<ImageTable::RequiredImage>> LoadObjectImages(
            IReadObjectContext* context, const std::string& name, const std::vector<int32_t>& range);
        [[nodiscard]] static std::vector<int32_t> ParseRange(std::string s);
        [[nodiscard]] static std::string FindLegacyObject(const std::string& name);
        [[nodiscard]] static std::vector<std::unique_ptr<ImageTable::RequiredImage>> LoadImageArchiveImages(
            IReadObjectContext* context, const std::string& path, const std::vector<int32_t>& range = {});

    public:
        ImageTable() = default;
        ImageTable(const ImageTable&) = delete;
        ImageTable& operator=(const ImageTable&) = delete;
        ~ImageTable();

        void Read(IReadObjectContext* context, OpenRCT2::IStream* stream);
        /**
         * @note root is deliberately left non-const: json_t behaviour changes when const
         */
        bool ReadJson(IReadObjectContext* context, json_t& root);
        const G1Element* GetImages() const
        {
            return _entries.data();
        }
        uint32_t GetCount() const
        {
            return static_cast<uint32_t>(_entries.size());
        }
        void AddImage(const G1Element* g1);
    };
} // namespace OpenRCT2
