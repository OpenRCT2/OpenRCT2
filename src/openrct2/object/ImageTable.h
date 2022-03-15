/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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

struct Image;
struct IReadObjectContext;
namespace OpenRCT2
{
    struct IStream;
}

class ImageTable
{
private:
    std::vector<uint8_t> _data;
    std::vector<rct_g1_element> _entries;

    struct SubImageTable;
    [[nodiscard]] std::vector<std::pair<std::string, Image>> GetImageSources(IReadObjectContext* context, json_t& jsonImages);
    [[nodiscard]] static ImageTable::SubImageTable ParseImages(IReadObjectContext* context, std::string s);
    /**
     * @note root is deliberately left non-const: json_t behaviour changes when const
     */
    [[nodiscard]] static ImageTable::SubImageTable ParseImages(
        IReadObjectContext* context, std::vector<std::pair<std::string, Image>>& imageSources, json_t& el);
    [[nodiscard]] static ImageTable::SubImageTable LoadObjectImages(
        IReadObjectContext* context, const std::string& name, const std::vector<int32_t>& range);
    [[nodiscard]] static std::vector<int32_t> ParseRange(std::string s);
    [[nodiscard]] static std::string FindLegacyObject(const std::string& name);
    size_t MergeImageData(const std::vector<uint8_t>& data);
    void MergeEntries(const std::vector<rct_g1_element>& handles, size_t dataOffset);
    void AddDataOffsetToEntries(size_t begin, size_t count, uint8_t* dataOffset);

public:
    ImageTable() = default;
    ImageTable(const ImageTable&) = delete;
    ImageTable& operator=(const ImageTable&) = delete;

    void Read(IReadObjectContext* context, OpenRCT2::IStream* stream);
    /**
     * @note root is deliberately left non-const: json_t behaviour changes when const
     */
    bool ReadJson(IReadObjectContext* context, json_t& root);
    // Legacy. Replace all users with GetEntries
    const rct_g1_element* GetImages() const
    {
        return _entries.data();
    }
    const std::vector<rct_g1_element>& GetEntries() const
    {
        return _entries;
    }
    const std::vector<uint8_t>& GetImageData() const
    {
        return _data;
    }
    // Legacy. Replace all users with GetEntries
    uint32_t GetCount() const
    {
        return static_cast<uint32_t>(_entries.size());
    }
    // Overrides the image table allocation singular image instance
    void SetImage(std::vector<rct_g1_element>&& entries, std::vector<uint8_t>&& imageData);
};
