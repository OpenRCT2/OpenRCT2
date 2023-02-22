
#include "TestData.h"

#include <algorithm>
#include <fstream>
#include <gtest/gtest.h>
#include <iterator>
#include <openrct2/CommandLineSprite.h>
#include <openrct2/core/Path.hpp>

class CommandLineTests : public testing::Test
{
public:
    static std::string SpriteTestDataPath()
    {
        return Path::Combine(TestData::GetBasePath(), u8"sprites");
    }

    static std::string ManifestFilePath()
    {
        return Path::Combine(SpriteTestDataPath(), u8"manifest.json");
    }

    static std::string BadManifestFilePath()
    {
        return Path::Combine(SpriteTestDataPath(), u8"badManifest.json");
    }

    static std::string ExampleSpriteFilePath()
    {
        return Path::Combine(SpriteTestDataPath(), u8"example.dat");
    }

    static std::string BuildOutputfilePath()
    {
        return Path::Combine(SpriteTestDataPath(), u8"result.dat");
    }

    static bool CompareSpriteFiles(std::string original, std::string generated)
    {
        std::ifstream originalFile(original, std::ios::binary | std::ifstream::in);
        std::ifstream generatedFile(generated, std::ios::binary | std::ifstream::in);
        if (!(originalFile.is_open() && generatedFile.is_open()))
        {
            return false;
        }
        if (originalFile.tellg() != generatedFile.tellg())
        {
            return false;
        }
        return std::equal(
            std::istreambuf_iterator<char>(originalFile.rdbuf()), std::istreambuf_iterator<char>(),
            std::istreambuf_iterator<char>(generatedFile.rdbuf()));
    }
};

TEST_F(CommandLineTests, command_line_for_sprite_details)
{
    std::string exampleFilePath = ExampleSpriteFilePath();
    const char* detailsCmd[3] = { "details", exampleFilePath.c_str() };

    int32_t result = CommandLineForSprite(detailsCmd, 2);
    // need to come up with some way to extract stdout/stderr stream if we want to
    // fully test this module
    ASSERT_EQ(result, 1);
}

TEST_F(CommandLineTests, command_line_for_sprite_build)
{
    std::string manifestFilePath = ManifestFilePath();
    std::string outputfilePath = BuildOutputfilePath();
    const char* detailsCmd[3] = { "build", outputfilePath.c_str(), manifestFilePath.c_str() };

    int32_t result = CommandLineForSprite(detailsCmd, 3);
    ASSERT_EQ(result, 1);
    // compare the resulting output file and assert its identical to expected
    ASSERT_TRUE(CompareSpriteFiles(ExampleSpriteFilePath(), outputfilePath));
}

TEST_F(CommandLineTests, command_line_for_sprite_failed_build)
{
    // run on correct manifest file
    std::string manifestFilePath = ManifestFilePath();
    std::string outputfilePath = BuildOutputfilePath();
    const char* detailsCmd[3] = { "build", outputfilePath.c_str(), manifestFilePath.c_str() };
    int32_t result = CommandLineForSprite(detailsCmd, 3);
    ASSERT_EQ(result, 1);
    ASSERT_TRUE(CompareSpriteFiles(ExampleSpriteFilePath(), outputfilePath));

    // now use bad manifest and make sure output file is not edited
    std::string badManifestFilePath = BadManifestFilePath();
    detailsCmd[2] = badManifestFilePath.c_str();
    result = CommandLineForSprite(detailsCmd, 3);
    // check the command failed
    ASSERT_EQ(result, -1);
    // validate the target file was unchanged
    ASSERT_TRUE(CompareSpriteFiles(ExampleSpriteFilePath(), outputfilePath));
}
