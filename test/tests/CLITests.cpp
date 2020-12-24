
#include "TestData.h"

#include <gtest/gtest.h>
#include <openrct2/core/Path.hpp>
#include <openrct2/CmdlineSprite.h>
#include <fstream>
#include <iterator>
#include <algorithm>

class CommandLineTests : public testing::Test
{
public:
    static std::string SpriteTestDataPath()
    {
        return Path::Combine(TestData::GetBasePath(), "sprites");
    }

    static std::string ManifestFilePath()
    {
        return Path::Combine(SpriteTestDataPath(), "manifest.json");
    }

    static std::string BadManifestFilePath()
    {
        return Path::Combine(SpriteTestDataPath(), "badManifest.json");
    }

    static std::string ExampleSpriteFilePath()
    {
        return Path::Combine(SpriteTestDataPath(), "example.dat");
    }

    static std::string BuildOutputfilePath()
    {
        return Path::Combine(SpriteTestDataPath(), "result.dat");
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
            std::istreambuf_iterator<char>(originalFile.rdbuf()),
            std::istreambuf_iterator<char>(),
            std::istreambuf_iterator<char>(generatedFile.rdbuf())
        );
    }
};

TEST_F(CommandLineTests, cmdline_cmdline_for_sprite_details)
{
    std::string exampleFilePath = ExampleSpriteFilePath();
    const char* detailsCmd[3] = { "details", exampleFilePath.c_str() };

    int32_t result = cmdline_for_sprite(detailsCmd, 2);
    // need to come up with some way to extract stdout/stderr stream if we want to
    // fully test this module
    ASSERT_EQ(result, 1);
}

TEST_F(CommandLineTests, cmdline_cmdline_for_sprite_build)
{
    std::string manifestFilePath = ManifestFilePath();
    std::string outputfilePath = BuildOutputfilePath();
    const char* detailsCmd[3] = { "build", outputfilePath.c_str(), manifestFilePath.c_str() };

    int32_t result = cmdline_for_sprite(detailsCmd, 3);
    ASSERT_EQ(result, 1);
    // compare the resulting output file and assert its identical to expected
    ASSERT_TRUE(CompareSpriteFiles(ExampleSpriteFilePath(), outputfilePath));
}

TEST_F(CommandLineTests, cmdline_cmdline_for_sprite_failed_build)
{
    // now use bad manifest and make sure output file is not edited
    std::string badManifestFilePath = BadManifestFilePath();
    std::string outputfilePath = BuildOutputfilePath();
    const char* detailsCmd[3] = { "build", outputfilePath.c_str(), badManifestFilePath.c_str() };
    int32_t result = cmdline_for_sprite(detailsCmd, 3);
    // check the command failed
    ASSERT_EQ(result, -1);
}
