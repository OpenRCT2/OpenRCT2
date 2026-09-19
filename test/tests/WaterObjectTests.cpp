/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <array>
#include <gtest/gtest.h>
#include <memory>
#include <openrct2/Context.h>
#include <openrct2/core/EnumUtils.hpp>
#include <openrct2/core/Json.hpp>
#include <openrct2/core/MemoryStream.h>
#include <openrct2/object/WaterObject.h>

using namespace OpenRCT2;

namespace
{
    class TestReadObjectContext final : public IReadObjectContext
    {
    public:
        std::string_view GetObjectIdentifier() override
        {
            return "test.water";
        }

        bool ShouldLoadImages() override
        {
            return false;
        }

        std::vector<uint8_t> GetData(std::string_view path) override
        {
            return {};
        }

        ObjectAsset GetAsset(std::string_view path) override
        {
            return {};
        }

        void LogVerbose(ObjectError code, const utf8* text) override
        {
        }

        void LogWarning(ObjectError code, const utf8* text) override
        {
        }

        void LogError(ObjectError code, const utf8* text) override
        {
        }
    };

    WaterObjectFlags ReadLegacyFlags(uint16_t rawFlags)
    {
        // 14 unused bytes, uint16_t flags (little-endian), empty string table, empty image table
        std::array<uint8_t, 25> data{};
        data[14] = rawFlags & 0xFF;
        data[15] = rawFlags >> 8;
        data[16] = EnumValue(RCT2LanguageId::end);

        TestReadObjectContext context;
        MemoryStream stream(data.data(), data.size());
        WaterObject object;
        object.ReadLegacy(&context, &stream);
        return static_cast<WaterObjectEntry*>(object.GetLegacyData())->flags;
    }

    WaterObjectFlags ReadJsonFlags(const json_t& properties)
    {
        json_t root = json_t::object();
        root["properties"] = properties;

        TestReadObjectContext context;
        WaterObject object;
        object.ReadJson(&context, root);
        return static_cast<WaterObjectEntry*>(object.GetLegacyData())->flags;
    }
} // namespace

class WaterObjectTest : public testing::Test
{
protected:
    void SetUp() override
    {
        // Reading string tables sorts them using the global localisation service.
        // Initialise() is not needed, so no game data is required.
        _context = CreateContext();
    }

    void TearDown() override
    {
        _context.reset();
    }

    std::unique_ptr<IContext> _context;
};

TEST_F(WaterObjectTest, AllowDucksMatchesLegacyBit)
{
    EXPECT_EQ(EnumToFlag(WaterObjectFlag::allowDucks), 1u);
}

TEST_F(WaterObjectTest, ReadLegacyKeepsRawFlags)
{
    auto noFlags = ReadLegacyFlags(0x0000);
    EXPECT_FALSE(noFlags.has(WaterObjectFlag::allowDucks));
    EXPECT_EQ(noFlags.holder, 0x0000);

    auto allowDucks = ReadLegacyFlags(0x0001);
    EXPECT_TRUE(allowDucks.has(WaterObjectFlag::allowDucks));
    EXPECT_EQ(allowDucks.holder, 0x0001);

    auto otherBit = ReadLegacyFlags(0x0002);
    EXPECT_FALSE(otherBit.has(WaterObjectFlag::allowDucks));
    EXPECT_EQ(otherBit.holder, 0x0002);

    auto allBits = ReadLegacyFlags(0xFFFF);
    EXPECT_TRUE(allBits.has(WaterObjectFlag::allowDucks));
    EXPECT_EQ(allBits.holder, 0xFFFF);
}

TEST_F(WaterObjectTest, ReadJsonAllowDucks)
{
    auto enabled = ReadJsonFlags({ { "allowDucks", true } });
    EXPECT_TRUE(enabled.has(WaterObjectFlag::allowDucks));
    EXPECT_EQ(enabled.holder, 0x0001);

    auto disabled = ReadJsonFlags({ { "allowDucks", false } });
    EXPECT_FALSE(disabled.has(WaterObjectFlag::allowDucks));
    EXPECT_TRUE(disabled.isEmpty());

    auto missing = ReadJsonFlags(json_t::object());
    EXPECT_FALSE(missing.has(WaterObjectFlag::allowDucks));
    EXPECT_TRUE(missing.isEmpty());
}
