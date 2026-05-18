/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <filesystem>
#include <gtest/gtest.h>
#include <openrct2/drawing/G1Element.h>
#include <openrct2/rct1/Csg.h>
#include <openrct2/rct1/Limits.h>

using namespace OpenRCT2;
namespace fs = std::filesystem;

namespace
{
    // The spite resolution functions are exercised against synthetic directory layouts in the system temp dir.
    // Each test gets a fresh empty layout to populate.
    class Rct1CsgTests : public ::testing::Test
    {
    protected:
        fs::path root;

        void SetUp() override
        {
            root = fs::temp_directory_path()
                / ("openrct2-rct1csg-" + std::to_string(::testing::UnitTest::GetInstance()->random_seed()) + "-"
                   + ::testing::UnitTest::GetInstance()->current_test_info()->name());
            fs::remove_all(root);
            fs::create_directories(root);
        }

        void TearDown() override
        {
            std::error_code ec;
            fs::remove_all(root, ec);
        }

        // Create a sparse file of exactly `size` bytes at `path`
        static void CreateSparseFile(const fs::path& path, const uint64_t size)
        {
            fs::create_directories(path.parent_path());
            FILE* f = std::fopen(path.string().c_str(), "wb");
            ASSERT_NE(f, nullptr) << "failed to open " << path;
            ASSERT_EQ(std::fseek(f, size - 1, SEEK_SET), 0);
            ASSERT_EQ(std::fputc(0, f), 0);
            std::fclose(f);
        }
    };
} // namespace

TEST_F(Rct1CsgTests, FindCsg1dat_NotPresentReturnsEmpty)
{
    EXPECT_TRUE(FindCsg1datAtLocation(root.string()).empty());
    EXPECT_FALSE(Csg1datPresentAtLocation(root.string()));
}

TEST_F(Rct1CsgTests, FindCsg1dat_PrimaryNameWins)
{
    CreateSparseFile(root / "Data" / "CSG1.DAT", 16);
    const auto found = FindCsg1datAtLocation(root.string());
    EXPECT_FALSE(found.empty());
    EXPECT_NE(found.find("CSG1.DAT"), std::string::npos);
    EXPECT_TRUE(Csg1datPresentAtLocation(root.string()));
}

TEST_F(Rct1CsgTests, FindCsg1dat_AltNameAlsoMatches)
{
    CreateSparseFile(root / "Data" / "CSG1.1", 16);
    const auto found = FindCsg1datAtLocation(root.string());
    EXPECT_FALSE(found.empty());
    EXPECT_NE(found.find("CSG1.1"), std::string::npos);
}

TEST_F(Rct1CsgTests, FindCsg1idat_NotPresentReturnsEmpty)
{
    EXPECT_TRUE(FindCsg1idatAtLocation(root.string()).empty());
    EXPECT_FALSE(Csg1idatPresentAtLocation(root.string()));
}

TEST_F(Rct1CsgTests, FindCsg1idat_PrimaryLocation)
{
    CreateSparseFile(root / "Data" / "CSG1I.DAT", 16);
    const auto found = FindCsg1idatAtLocation(root.string());
    EXPECT_FALSE(found.empty());
    EXPECT_NE(found.find("Data"), std::string::npos);
    EXPECT_TRUE(Csg1idatPresentAtLocation(root.string()));
}

TEST_F(Rct1CsgTests, FindCsg1idat_RctdeluxeFallback)
{
    CreateSparseFile(root / "RCTdeluxe_install" / "Data" / "CSG1I.DAT", 16);
    const auto found = FindCsg1idatAtLocation(root.string());
    EXPECT_FALSE(found.empty());
    EXPECT_NE(found.find("RCTdeluxe_install"), std::string::npos);
}

TEST_F(Rct1CsgTests, CsgIsUsable_MatchingSizes)
{
    Gx csg = {};
    csg.header.totalSize = RCT1::Limits::kLLCsg1DatFileSize;
    csg.header.numEntries = RCT1::Limits::kNumLLCsgEntries;
    EXPECT_TRUE(CsgIsUsable(csg));
}

TEST_F(Rct1CsgTests, CsgIsUsable_WrongTotalSize)
{
    Gx csg = {};
    csg.header.totalSize = RCT1::Limits::kLLCsg1DatFileSize - 1;
    csg.header.numEntries = RCT1::Limits::kNumLLCsgEntries;
    EXPECT_FALSE(CsgIsUsable(csg));
}

TEST_F(Rct1CsgTests, CsgIsUsable_WrongEntryCount)
{
    Gx csg = {};
    csg.header.totalSize = RCT1::Limits::kLLCsg1DatFileSize;
    csg.header.numEntries = RCT1::Limits::kNumLLCsgEntries - 1;
    EXPECT_FALSE(CsgIsUsable(csg));
}

TEST_F(Rct1CsgTests, CsgAtLocationIsUsable_MissingHeaderReturnsFalse)
{
    EXPECT_FALSE(CsgAtLocationIsUsable(root.string()));
}

TEST_F(Rct1CsgTests, CsgAtLocationIsUsable_MissingDataReturnsFalse)
{
    CreateSparseFile(root / "Data" / "CSG1I.DAT", 16);
    EXPECT_FALSE(CsgAtLocationIsUsable(root.string()));
}

TEST_F(Rct1CsgTests, CsgAtLocationIsUsable_WrongSizesReturnsFalse)
{
    CreateSparseFile(root / "Data" / "CSG1I.DAT", 16);
    CreateSparseFile(root / "Data" / "CSG1.DAT", 16);
    EXPECT_FALSE(CsgAtLocationIsUsable(root.string()));
}

TEST_F(Rct1CsgTests, CsgAtLocationIsUsable_MatchingSizesReturnsTrue)
{
    // Build a layout whose file sizes match exactly what CsgIsUsable expects.
    CreateSparseFile(
        root / "Data" / "CSG1I.DAT", static_cast<uint64_t>(RCT1::Limits::kNumLLCsgEntries) * sizeof(StoredG1Element));
    CreateSparseFile(root / "Data" / "CSG1.DAT", RCT1::Limits::kLLCsg1DatFileSize);
    EXPECT_TRUE(CsgAtLocationIsUsable(root.string()));
}

TEST_F(Rct1CsgTests, RCT1DataPresent_AllConditionsMet)
{
    CreateSparseFile(
        root / "Data" / "CSG1I.DAT", static_cast<uint64_t>(RCT1::Limits::kNumLLCsgEntries) * sizeof(StoredG1Element));
    CreateSparseFile(root / "Data" / "CSG1.DAT", RCT1::Limits::kLLCsg1DatFileSize);
    EXPECT_TRUE(RCT1DataPresentAtLocation(root.string()));
}

TEST_F(Rct1CsgTests, RCT1DataPresent_ShortCircuitsOnMissingFile)
{
    EXPECT_FALSE(RCT1DataPresentAtLocation(root.string()));
}

TEST_F(Rct1CsgTests, RCT1DataPresent_MissingHeaderFile)
{
    CreateSparseFile(root / "Data" / "CSG1.DAT", 16);
    EXPECT_FALSE(RCT1DataPresentAtLocation(root.string()));
}

TEST_F(Rct1CsgTests, RCT1DataPresent_WrongFileSizes)
{
    CreateSparseFile(root / "Data" / "CSG1.DAT", 16);
    CreateSparseFile(root / "Data" / "CSG1I.DAT", 16);
    EXPECT_FALSE(RCT1DataPresentAtLocation(root.string()));
}
