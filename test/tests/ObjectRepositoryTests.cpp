/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <chrono>
#include <gtest/gtest.h>
#include <openrct2/Context.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/audio/AudioContext.h>
#include <openrct2/core/EnumUtils.hpp>
#include <openrct2/core/File.h>
#include <openrct2/core/FileSystem.hpp>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/Zip.h>
#include <openrct2/object/ObjectRepository.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/ui/UiContext.h>
#include <string>
#include <system_error>
#include <vector>

using namespace OpenRCT2;

namespace
{
    std::vector<uint8_t> CreateMinimalParkObjData(const fs::path& basePath)
    {
        const auto archivePath = (basePath / "source.parkobj").u8string();
        auto archive = Zip::Open(archivePath, ZipAccess::write);
        if (archive == nullptr)
        {
            return {};
        }

        static constexpr std::string_view ObjectJson = R"({
  "id": "openrct2.test.object_repository",
  "objectType": "scenario_meta",
  "version": "1.0",
  "authors": ["OpenRCT2"],
  "strings": {
    "name": {
      "en-GB": "Object Repository Test"
    }
  }
})";

        archive->SetFileData("object.json", std::vector<uint8_t>(ObjectJson.begin(), ObjectJson.end()));
        archive.reset();

        return File::ReadAllBytes(archivePath);
    }

    class ScopedTempDirectory
    {
    public:
        ScopedTempDirectory()
        {
            auto now = std::chrono::steady_clock::now().time_since_epoch().count();
            _path = fs::temp_directory_path() / fs::path("openrct2-object-repository-test-" + std::to_string(now));
            fs::create_directories(_path);
        }

        ~ScopedTempDirectory()
        {
            std::error_code ec;
            fs::remove_all(_path, ec);
        }

        const fs::path& Path() const
        {
            return _path;
        }

    private:
        fs::path _path;
    };
} // namespace

TEST(ObjectRepository, add_json_object_from_file_sanitises_filename)
{
    ScopedTempDirectory tempDirectory;
    const auto repoRoot = fs::u8path(__FILE__).parent_path().parent_path().parent_path();

    DirBaseValues basePaths{};
    basePaths[EnumValue(DirBase::openrct2)] = (repoRoot / "data").u8string();
    basePaths[EnumValue(DirBase::user)] = (tempDirectory.Path() / "user").u8string();
    basePaths[EnumValue(DirBase::cache)] = (tempDirectory.Path() / "cache").u8string();
    basePaths[EnumValue(DirBase::config)] = (tempDirectory.Path() / "config").u8string();
    basePaths[EnumValue(DirBase::documentation)] = (tempDirectory.Path() / "docs").u8string();

    for (const auto& basePath : basePaths)
    {
        if (!basePath.empty())
        {
            fs::create_directories(fs::u8path(basePath));
        }
    }

    gOpenRCT2Headless = true;
    gOpenRCT2NoGraphics = true;

    auto env = CreatePlatformEnvironment(basePaths);
    auto context = CreateContext(std::move(env), Audio::CreateDummyAudioContext(), Ui::CreateDummyUiContext());
    ASSERT_TRUE(context->Initialise());

    const auto objectData = CreateMinimalParkObjData(tempDirectory.Path());
    ASSERT_FALSE(objectData.empty());
    const std::string identifier = "../../escape/payload";
    context->GetObjectRepository().AddObjectFromFile(ObjectGeneration::JSON, identifier, objectData.data(), objectData.size());

    const auto objectsPath = fs::u8path(context->GetPlatformEnvironment().GetDirectoryPath(DirBase::user, DirId::objects));
    const auto expectedPath = fs::u8path(
        Path::Combine(objectsPath.u8string(), Platform::SanitiseFilename(identifier) + ".parkobj"));
    const auto escapedPath = (objectsPath / ".." / ".." / "escape" / "payload.parkobj").lexically_normal();

    EXPECT_TRUE(fs::exists(expectedPath));
    EXPECT_FALSE(fs::exists(escapedPath));
}
