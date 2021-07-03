/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Context.h"
#include "../FileClassifier.h"
#include "../OpenRCT2.h"
#include "../ParkFile.h"
#include "../ParkImporter.h"
#include "../common.h"
#include "../core/Console.hpp"
#include "../core/Path.hpp"
#include "../interface/Window.h"
#include "../object/ObjectManager.h"
#include "../scenario/Scenario.h"
#include "CommandLine.hpp"

#include <memory>

static void WriteConvertFromAndToMessage(uint32_t sourceFileType, uint32_t destinationFileType);
static const utf8* GetFileTypeFriendlyName(uint32_t fileType);

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

    utf8 sourcePath[MAX_PATH];
    Path::GetAbsolute(sourcePath, sizeof(sourcePath), rawSourcePath);
    uint32_t sourceFileType = get_file_extension_type(sourcePath);

    // Get the destination path
    const utf8* rawDestinationPath;
    if (!enumerator->TryPopString(&rawDestinationPath))
    {
        Console::Error::WriteLine("Expected a destination path.");
        return EXITCODE_FAIL;
    }

    utf8 destinationPath[MAX_PATH];
    Path::GetAbsolute(destinationPath, sizeof(sourcePath), rawDestinationPath);
    uint32_t destinationFileType = get_file_extension_type(destinationPath);

    // Validate target type
    if (destinationFileType != FILE_EXTENSION_PARK)
    {
        Console::Error::WriteLine("Only conversion to .PARK is supported.");
        return EXITCODE_FAIL;
    }

    // Validate the source type
    switch (sourceFileType)
    {
        case FILE_EXTENSION_SC4:
        case FILE_EXTENSION_SV4:
            break;
        case FILE_EXTENSION_SC6:
            if (destinationFileType == FILE_EXTENSION_SC6)
            {
                Console::Error::WriteLine("File is already a RollerCoaster Tycoon 2 scenario.");
                return EXITCODE_FAIL;
            }
            break;
        case FILE_EXTENSION_SV6:
            if (destinationFileType == FILE_EXTENSION_SV6)
            {
                Console::Error::WriteLine("File is already a RollerCoaster Tycoon 2 saved game.");
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
        auto loadResult = importer->Load(sourcePath);

        objManager.LoadObjects(loadResult.RequiredObjects);

        importer->Import();
    }
    catch (const std::exception& ex)
    {
        Console::Error::WriteLine(ex.what());
        return EXITCODE_FAIL;
    }

    if (sourceFileType == FILE_EXTENSION_SC4 || sourceFileType == FILE_EXTENSION_SC6)
    {
        // We are converting a scenario, so reset the park
        scenario_begin();
    }

    try
    {
        auto exporter = std::make_unique<ParkFileExporter>();

        // HACK remove the main window so it saves the park with the
        //      correct initial view
        window_close_by_class(WC_MAIN_WINDOW);

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

static void WriteConvertFromAndToMessage(uint32_t sourceFileType, uint32_t destinationFileType)
{
    const utf8* sourceFileTypeName = GetFileTypeFriendlyName(sourceFileType);
    const utf8* destinationFileTypeName = GetFileTypeFriendlyName(destinationFileType);
    Console::WriteFormat("Converting from a %s to a %s.", sourceFileTypeName, destinationFileTypeName);
    Console::WriteLine();
}

static const utf8* GetFileTypeFriendlyName(uint32_t fileType)
{
    switch (fileType)
    {
        case FILE_EXTENSION_SC4:
            return "RollerCoaster Tycoon 1 scenario";
        case FILE_EXTENSION_SV4:
            return "RollerCoaster Tycoon 1 saved game";
        case FILE_EXTENSION_SC6:
            return "RollerCoaster Tycoon 2 scenario";
        case FILE_EXTENSION_SV6:
            return "RollerCoaster Tycoon 2 saved game";
        case FILE_EXTENSION_PARK:
            return "OpenRCT2 park";
    }

    assert(false);
    return nullptr;
}
