/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TestData.h"

#include <gtest/gtest.h>
#include <openrct2/core/Path.hpp>
#include <openrct2/drawing/ImageImporter.h>
#include <string_view>

using namespace OpenRCT2::Drawing;

class ImageImporterTests : public testing::Test
{
public:
    static std::string GetImagePath(const std::string& name)
    {
        return Path::Combine(TestData::GetBasePath(), u8"images", name.c_str());
    }

    static uint32_t GetHash(void* buffer, size_t bufferLength)
    {
        uint32_t hash = 27;
        for (size_t i = 0; i < bufferLength; i++)
        {
            hash = (13 * hash) + (reinterpret_cast<uint8_t*>(buffer))[i];
        }
        return hash;
    }
};

TEST_F(ImageImporterTests, Import_Logo)
{
    auto logoPath = GetImagePath("logo.png");

    ImageImporter importer;
    auto image = Imaging::ReadFromFile(logoPath, IMAGE_FORMAT::PNG_32);
    auto result = importer.Import(image, 3, 5, ImageImporter::Palette::OpenRCT2, ImageImporter::ImportFlags::RLE);

    ASSERT_EQ(result.Buffer.data(), result.Element.offset);
    ASSERT_EQ(128, result.Element.width);
    ASSERT_EQ(128, result.Element.height);
    ASSERT_EQ(3, result.Element.x_offset);
    ASSERT_EQ(5, result.Element.y_offset);
    ASSERT_EQ(0, result.Element.zoomed_offset);

    // Check to ensure RLE data doesn't change unexpectedly.
    // Update expected hash if change is expected.
    ASSERT_NE(nullptr, result.Buffer.data());
    auto hash = GetHash(result.Buffer.data(), result.Buffer.size());
    ASSERT_EQ(0xCEF27C7D, hash);
}
