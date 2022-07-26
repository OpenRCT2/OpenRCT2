/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Context.h"
#include "../FileClassifier.h"
#include "../ParkImporter.h"
#include "../core/Console.hpp"
#include "../core/FileStream.h"
#include "../core/Path.hpp"
#include "../object/ObjectRepository.h"
#include "CommandLine.hpp"

// clang-format off
static constexpr const CommandLineOptionDefinition NoOptions[]
{
    OptionTableEnd
};

static exitcode_t HandleObjectsInfo(CommandLineArgEnumerator *argEnumerator);

const CommandLineCommand CommandLine::ParkInfoCommands[]{
    // Main commands
    DefineCommand("objects", "<savefile> <output directory>", NoOptions, HandleObjectsInfo),

    CommandTableEnd
};
// clang-format on

static exitcode_t HandleObjectsInfo(CommandLineArgEnumerator* argEnumerator)
{
    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    // Get the source path
    const utf8* rawSourcePath;
    if (!argEnumerator->TryPopString(&rawSourcePath))
    {
        Console::Error::WriteLine("Expected a source save file path.");
        return EXITCODE_FAIL;
    }

    auto sourcePath = Path::GetAbsolute(rawSourcePath);

    // Get the destination path
    // const utf8* rawDestinationPath;
    // if (!argEnumerator->TryPopString(&rawDestinationPath))
    //{
    //    Console::Error::WriteLine("Expected an output directory.");
    //    return EXITCODE_FAIL;
    //}

    // utf8 directory[MAX_PATH];
    // Path::GetDirectory(directory, MAX_PATH, rawDestinationPath);
    // if (!Path::DirectoryExists(directory))
    //{
    //    Console::Error::WriteLine("Output directory does not exist. Creating...");
    //    Path::CreateDirectory(directory);
    //    if (!Path::DirectoryExists(directory))
    //    {
    //        Console::Error::WriteLine("Output directory failed to create.");
    //        return EXITCODE_FAIL;
    //    }
    //}

    auto context = OpenRCT2::CreateContext();
    context->Initialise();

    auto stream = OpenRCT2::FileStream(sourcePath, OpenRCT2::FILE_MODE_OPEN);
    ClassifiedFileInfo info;
    if (!TryClassifyFile(&stream, &info))
    {
        throw std::runtime_error("Unable to detect file type");
    }

    if (info.Type != FILE_TYPE::PARK && info.Type != FILE_TYPE::SAVED_GAME && info.Type != FILE_TYPE::SCENARIO)
    {
        throw std::runtime_error("Invalid file type.");
    }

    auto& objectRepository = context->GetObjectRepository();
    std::unique_ptr<IParkImporter> parkImporter;
    if (info.Type == FILE_TYPE::PARK)
    {
        parkImporter = ParkImporter::CreateParkFile(objectRepository);
    }
    else if (info.Version <= FILE_TYPE_S4_CUTOFF)
    {
        // Save is an S4 (RCT1 format)
        parkImporter = ParkImporter::CreateS4();
    }
    else
    {
        // Save is an S6 (RCT2 format)
        parkImporter = ParkImporter::CreateS6(objectRepository);
    }
    auto loadResult = parkImporter->LoadSavedGame(sourcePath.c_str());
    std::array<std::string, 17> typeToName = {
        "Ride",        "SmallScenery", "LargeScenery", "Walls",           "Banners",          "Paths",
        "PathBits",    "SceneryGroup", "ParkEntrance", "Water",           "ScenarioText",     "TerrainSurface",
        "TerrainEdge", "Station",      "Music",        "FootpathSurface", "FootpathRailings",
    };
    for (auto& objType : {
             ObjectType::Ride,
             ObjectType::SmallScenery,
             ObjectType::LargeScenery,
             ObjectType::Walls,
             ObjectType::Banners,
             ObjectType::Paths,
             ObjectType::PathBits,
             ObjectType::SceneryGroup,
             ObjectType::ParkEntrance,
             ObjectType::Water,
             ObjectType::ScenarioText,
             ObjectType::TerrainSurface,
             ObjectType::TerrainEdge,
             ObjectType::Station,
             ObjectType::Music,
             ObjectType::FootpathSurface,
             ObjectType::FootpathRailings,
         })
    {
        auto& list = loadResult.RequiredObjects.GetList(objType);
        Console::WriteLine("ObjectType: %s, Number of Objects: %d", typeToName[EnumValue(objType)].c_str(), list.size());
        for (auto& obj : list)
        {
            if (obj.Generation == ObjectGeneration::JSON && obj.Identifier.size() == 0)
            {
                Console::WriteLine("Empty object slot.");
                continue;
            }
            auto* ori = OpenRCT2::GetContext()->GetObjectRepository().FindObject(obj);
            if (ori->GetFirstSourceGame() != ObjectSourceGame::Custom)
            {
                Console::Write("Official Object: ");
            }
            else
            {
                Console::Write("Custom Object: ");
            }
            std::string name{ obj.GetName() };
            if (obj.Generation == ObjectGeneration::DAT)
            {
                Console::WriteLine("%s/%08X", name.c_str(), obj.Entry.checksum);
            }
            else
            {
                Console::WriteLine("%s", name.c_str());
            }
        }
        Console::WriteLine();
    }
    return EXITCODE_OK;
}
