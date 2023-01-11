/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Context.h"
#include "../FileClassifier.h"
#include "../OpenRCT2.h"
#include "../ParkImporter.h"
#include "../common.h"
#include "../core/Console.hpp"
#include "../core/Path.hpp"
#include "../interface/Window.h"
#include "../object/ObjectManager.h"
#include "../park/ParkFile.h"
#include "../scenario/Scenario.h"
#include "CommandLine.hpp"

#include <memory>

static void WriteConvertFromAndToMessage(FileExtension sourceFileType, FileExtension destinationFileType);
static u8string GetFileTypeFriendlyName(FileExtension fileType);

exitcode_t CommandLine::HandleCommandConvert(CommandLineArgEnumerator* enumerator)
{
    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    // Get the source path
    const utf8* rawSourcePath;
    if (!enumerator->TryPopString(&rawSourcePath))
    {
        Console::Error::WriteLine("Expected a source path.");
        return EXITCODE_FAIL;
    }

    const auto sourcePath = Path::GetAbsolute(rawSourcePath);
    auto sourceFileType = get_file_extension_type(sourcePath.c_str());

    // Get the destination path
    const utf8* rawDestinationPath;
    if (!enumerator->TryPopString(&rawDestinationPath))
    {
        Console::Error::WriteLine("Expected a destination path.");
        return EXITCODE_FAIL;
    }

    const auto destinationPath = Path::GetAbsolute(rawDestinationPath);
    auto destinationFileType = get_file_extension_type(destinationPath.c_str());

    // Validate target type
    if (destinationFileType != FileExtension::PARK)
    {
        Console::Error::WriteLine("Only conversion to .PARK is supported.");
        return EXITCODE_FAIL;
    }

    // Validate the source type
    switch (sourceFileType)
    {
        case FileExtension::SC4:
        case FileExtension::SV4:
        case FileExtension::SC6:
        case FileExtension::SV6:
            break;
        case FileExtension::PARK:
            if (destinationFileType == FileExtension::PARK)
            {
                Console::Error::WriteLine("File is already an OpenRCT2 saved game or scenario.");
                return EXITCODE_FAIL;
            }
            break;
        default:
            Console::Error::WriteLine("Only conversion from .SC4, .SV4, .SC6 or .SV6 is supported.");
            return EXITCODE_FAIL;
    }

    // Perform conversion
    WriteConvertFromAndToMessage(sourceFileType, destinationFileType);

    gOpenRCT2Headless = true;
    auto context = OpenRCT2::CreateContext();
    context->Initialise();

    auto& objManager = context->GetObjectManager();

    try
    {
        auto importer = ParkImporter::Create(sourcePath);
        auto loadResult = importer->Load(sourcePath.c_str());

        objManager.LoadObjects(loadResult.RequiredObjects);

        importer->Import();
    }
    catch (const std::exception& ex)
    {
        Console::Error::WriteLine(ex.what());
        return EXITCODE_FAIL;
    }

    if (sourceFileType == FileExtension::SC4 || sourceFileType == FileExtension::SC6)
    {
        // We are converting a scenario, so reset the park
        scenario_begin();
    }

    try
    {
        auto exporter = std::make_unique<ParkFileExporter>();

        // HACK remove the main window so it saves the park with the
        //      correct initial view
        window_close_by_class(WindowClass::MainWindow);

        exporter->Export(destinationPath);
    }
    catch (const std::exception& ex)
    {
        Console::Error::WriteLine(ex.what());
        return EXITCODE_FAIL;
    }

    Console::WriteLine("Conversion successful!");
    return EXITCODE_OK;
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
        case FileExtension::SC4:
            return "RollerCoaster Tycoon 1 scenario";
        case FileExtension::SV4:
            return "RollerCoaster Tycoon 1 saved game";
        case FileExtension::SC6:
            return "RollerCoaster Tycoon 2 scenario";
        case FileExtension::SV6:
            return "RollerCoaster Tycoon 2 saved game";
        case FileExtension::PARK:
            return "OpenRCT2 park";
        default:
            break;
    }

    assert(false);
    return nullptr;
}
