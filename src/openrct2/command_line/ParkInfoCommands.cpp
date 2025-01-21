/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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

using namespace OpenRCT2;

// clang-format off
static constexpr CommandLineOptionDefinition kNoOptions[]
{
    kOptionTableEnd
};

static exitcode_t HandleObjectsInfo(CommandLineArgEnumerator *argEnumerator);

const CommandLineCommand CommandLine::kParkInfoCommands[]{
    // Main commands
    DefineCommand("objects", "<savefile>", kNoOptions, HandleObjectsInfo),

    kCommandTableEnd
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

    auto context = OpenRCT2::CreateContext();
    context->Initialise();

    auto stream = OpenRCT2::FileStream(sourcePath, OpenRCT2::FILE_MODE_OPEN);
    ClassifiedFileInfo info;
    if (!TryClassifyFile(&stream, &info))
    {
        Console::Error::WriteLine("Unable to detect file type");
        return EXITCODE_FAIL;
    }

    if (info.Type != FILE_TYPE::PARK && info.Type != FILE_TYPE::SAVED_GAME && info.Type != FILE_TYPE::SCENARIO)
    {
        Console::Error::WriteLine("Invalid file type.");
        return EXITCODE_FAIL;
    }

    auto& objectRepository = context->GetObjectRepository();
    std::unique_ptr<IParkImporter> parkImporter;
    if (info.Type == FILE_TYPE::PARK)
    {
        parkImporter = ParkImporter::CreateParkFile(objectRepository);
    }
    else if (info.Version <= kFileTypeS4Cutoff)
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

    Console::WriteLine("File contains the following objects: ");
    Console::WriteLine();

    constexpr std::array typeToName = {
        "Ride",          "SmallScenery", "LargeScenery", "Walls",           "Banners",          "Paths",
        "PathAdditions", "SceneryGroup", "ParkEntrance", "Water",           "ScenarioText",     "TerrainSurface",
        "TerrainEdge",   "Station",      "Music",        "FootpathSurface", "FootpathRailings",
    };
    constexpr std::array sourceGameToName = {
        "Custom", "WackyWorlds", "TimeTwister", "OpenRCT2Official", "RCT1", "AddedAttractions", "LoopyLandscapes", "", "RCT2",
    };

    for (auto& objType : {
             ObjectType::Ride,
             ObjectType::SmallScenery,
             ObjectType::LargeScenery,
             ObjectType::Walls,
             ObjectType::Banners,
             ObjectType::Paths,
             ObjectType::PathAdditions,
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
        Console::WriteLine("ObjectType: %s, Number of Objects: %d", typeToName[EnumValue(objType)], list.size());
        for (auto& obj : list)
        {
            if (obj.Generation == ObjectGeneration::JSON && obj.Identifier.size() == 0)
            {
                // Empty object slot don't output anything
                continue;
            }
            auto* ori = OpenRCT2::GetContext()->GetObjectRepository().FindObject(obj);
            Console::WriteFormat("%s Object: ", sourceGameToName[EnumValue(ori->GetFirstSourceGame())]);

            std::string name{ obj.GetName() };
            if (obj.Generation == ObjectGeneration::DAT)
            {
                Console::WriteLine("%08X|%s|%08X", obj.Entry.flags, name.c_str(), obj.Entry.checksum);
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
