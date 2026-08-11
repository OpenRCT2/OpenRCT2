/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Context.h"
#include "../FileClassifier.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../ParkImporter.h"
#include "../core/Console.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../object/ObjectManager.h"
#include "../park/ParkFile.h"
#include "../scenario/Scenario.h"
#include "../ui/WindowManager.h"
#include "CommandLine.hpp"

#include <memory>

using namespace OpenRCT2::CommandLine;

namespace OpenRCT2
{
    static int32_t _compressLevel = kParkFileSaveCompressionLevel;
    static bool _stripObjects = false;

    // clang-format off
    static constexpr CommandLineOptionDefinition kConvertOptions[]
    {
        { CMDLINE_TYPE_INTEGER, &_compressLevel, 'l', "compress-level", "The compression level to use when writing the converted file" },
        { CMDLINE_TYPE_SWITCH, &_stripObjects, kNAC, "strip-objects", "Do not pack custom objects into the converted file" },
        kOptionTableEnd
    };

    static ExitCode HandleCommandConvert(CommandLineArgEnumerator* argEnumerator);

    const CommandLineCommand CommandLine::kConvertCommands[]{
        // Main commands
        DefineCommand("", "<source> [destination]", kConvertOptions, HandleCommandConvert),
        kCommandTableEnd
    };
    // clang-format on

    static void WriteConvertFromAndToMessage(FileExtension sourceFileType, FileExtension destinationFileType);
    static u8string GetFileTypeFriendlyName(FileExtension fileType);

    static ExitCode HandleCommandConvert(CommandLineArgEnumerator* enumerator)
    {
        ExitCode result = CommandLine::HandleCommandDefault();
        if (result != ExitCode::launch)
        {
            return result;
        }

        // Get the source path
        const utf8* rawSourcePath;
        if (!enumerator->TryPopString(&rawSourcePath))
        {
            Console::Error::WriteLine("Expected a source path.");
            return ExitCode::fail;
        }

        const auto sourcePath = Path::GetAbsolute(rawSourcePath);
        auto sourceFileType = GetFileExtensionType(sourcePath.c_str());

        // Get the destination path
        const utf8* rawDestinationPath;
        if (!enumerator->TryPopString(&rawDestinationPath) || String::startsWith(rawDestinationPath, "-"))
        {
            // if no destination path is provided, convert the park file in-place
            rawDestinationPath = rawSourcePath;
        }

        const auto destinationPath = Path::GetAbsolute(rawDestinationPath);
        auto destinationFileType = GetFileExtensionType(destinationPath.c_str());

        // Validate target type
        if (destinationFileType != FileExtension::park)
        {
            Console::Error::WriteLine("Only conversion to .PARK is supported.");
            return ExitCode::fail;
        }

        // Validate the source type
        switch (sourceFileType)
        {
            case FileExtension::sc4:
            case FileExtension::sv4:
            case FileExtension::sc6:
            case FileExtension::sv6:
                break;
            case FileExtension::park:
                if (destinationFileType == FileExtension::park)
                {
                    Console::Error::WriteLine(
                        "File is already an OpenRCT2 saved game or scenario. Updating file version and recompressing.");
                }
                break;
            default:
                Console::Error::WriteLine("Only conversion from .SC4, .SV4, .SC6, .SV6, or .PARK is supported.");
                return ExitCode::fail;
        }

        // Perform conversion
        WriteConvertFromAndToMessage(sourceFileType, destinationFileType);

        gOpenRCT2Headless = true;
        auto context = CreateContext();
        context->Initialise();

        auto& objManager = context->GetObjectManager();
        auto& gameState = getGameState();

        try
        {
            auto importer = ParkImporter::Create(sourcePath);
            auto loadResult = importer->Load(sourcePath.c_str(), false);

            objManager.LoadObjects(loadResult.RequiredObjects);

            // TODO: Have a separate GameState and exchange once loaded.
            importer->Import(gameState);
        }
        catch (const std::exception& ex)
        {
            Console::Error::WriteLine(ex.what());
            return ExitCode::fail;
        }

        if (sourceFileType == FileExtension::sc4 || sourceFileType == FileExtension::sc6)
        {
            // We are converting a scenario, so reset the park
            ScenarioBegin(gameState);
        }

        try
        {
            auto exporter = std::make_unique<ParkFileExporter>();
            if (!_stripObjects)
            {
                exporter->ExportObjectsList = objManager.GetPackableObjects();
            }

            // HACK remove the main window so it saves the park with the
            //      correct initial view
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->CloseByClass(WindowClass::mainWindow);

            exporter->Export(gameState, destinationPath, static_cast<int16_t>(_compressLevel));
        }
        catch (const std::exception& ex)
        {
            Console::Error::WriteLine(ex.what());
            return ExitCode::fail;
        }

        Console::WriteLine("Conversion successful!");
        return ExitCode::ok;
    }

    static void WriteConvertFromAndToMessage(FileExtension sourceFileType, FileExtension destinationFileType)
    {
        const auto sourceFileTypeName = GetFileTypeFriendlyName(sourceFileType);
        const auto destinationFileTypeName = GetFileTypeFriendlyName(destinationFileType);
        Console::WriteFormat("Converting from a %s to a %s.", sourceFileTypeName.c_str(), destinationFileTypeName.c_str());
        Console::WriteLine();
    }

    static u8string GetFileTypeFriendlyName(FileExtension fileType)
    {
        switch (fileType)
        {
            case FileExtension::sc4:
                return "RollerCoaster Tycoon 1 scenario";
            case FileExtension::sv4:
                return "RollerCoaster Tycoon 1 saved game";
            case FileExtension::sc6:
                return "RollerCoaster Tycoon 2 scenario";
            case FileExtension::sv6:
                return "RollerCoaster Tycoon 2 saved game";
            case FileExtension::park:
                return "OpenRCT2 park";
            default:
                break;
        }

        throw std::invalid_argument(String::stdFormat("Unknown file type: %d", static_cast<int>(fileType)));
    }
} // namespace OpenRCT2
