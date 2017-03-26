#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include <memory>
#include "../common.h"
#include "../core/Console.hpp"
#include "../core/Exception.hpp"
#include "../core/Guard.hpp"
#include "../core/Path.hpp"
#include "../ParkImporter.h"
#include "../rct2/S6Exporter.h"
#include "CommandLine.hpp"

extern "C"
{
    #include "../game.h"
    #include "../interface/window.h"
    #include "../OpenRCT2.h"
    #include "../rct2.h"
}

static void WriteConvertFromAndToMessage(uint32 sourceFileType, uint32 destinationFileType);
static const utf8 * GetFileTypeFriendlyName(uint32 fileType);

exitcode_t CommandLine::HandleCommandConvert(CommandLineArgEnumerator * enumerator)
{
    exitcode_t result = CommandLine::HandleCommandDefault();
    if (result != EXITCODE_CONTINUE)
    {
        return result;
    }

    // Get the source path
    const utf8 * rawSourcePath;
    if (!enumerator->TryPopString(&rawSourcePath))
    {
        Console::Error::WriteLine("Expected a source path.");
        return EXITCODE_FAIL;
    }

    utf8 sourcePath[MAX_PATH];
    Path::GetAbsolute(sourcePath, sizeof(sourcePath), rawSourcePath);
    uint32 sourceFileType = get_file_extension_type(sourcePath);

    // Get the destination path
    const utf8 * rawDestinationPath;
    if (!enumerator->TryPopString(&rawDestinationPath))
    {
        Console::Error::WriteLine("Expected a destination path.");
        return EXITCODE_FAIL;
    }

    utf8 destinationPath[MAX_PATH];
    Path::GetAbsolute(destinationPath, sizeof(sourcePath), rawDestinationPath);
    uint32 destinationFileType = get_file_extension_type(destinationPath);

    // Validate target type
    if (destinationFileType != FILE_EXTENSION_SC6 &&
        destinationFileType != FILE_EXTENSION_SV6)
    {
        Console::Error::WriteLine("Only conversion to .SC6 or .SV4 is supported.");
        return EXITCODE_FAIL;
    }

    // Validate the source type
    switch (sourceFileType) {
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
    // if (!openrct2_initialise())
    // {
    //     Console::Error::WriteLine("Error while initialising OpenRCT2.");
    //     return EXITCODE_FAIL;
    // }

    try
    {
        auto importer = std::unique_ptr<IParkImporter>(ParkImporter::Create(sourcePath));
        importer->Load(sourcePath);
        importer->Import();
    }
    catch (const Exception &ex)
    {
        Console::Error::WriteLine(ex.GetMessage());
        return EXITCODE_FAIL;
    }

    if (sourceFileType == FILE_EXTENSION_SC4 ||
        sourceFileType == FILE_EXTENSION_SC6)
    {
        // We are converting a scenario, so reset the park
        scenario_begin();
    }

    try
    {
        auto exporter = std::make_unique<S6Exporter>();

        // HACK remove the main window so it saves the park with the
        //      correct initial view
        window_close_by_class(WC_MAIN_WINDOW);

        exporter->Export();
        if (destinationFileType == FILE_EXTENSION_SC6)
        {
            exporter->SaveScenario(destinationPath);
        }
        else
        {
            exporter->SaveGame(destinationPath);
        }
    }
    catch (const Exception &ex)
    {
        Console::Error::WriteLine(ex.GetMessage());
        return EXITCODE_FAIL;
    }

    Console::WriteLine("Conversion successful!");
    return EXITCODE_OK;
}

static void WriteConvertFromAndToMessage(uint32 sourceFileType, uint32 destinationFileType)
{
    const utf8 * sourceFileTypeName = GetFileTypeFriendlyName(sourceFileType);
    const utf8 * destinationFileTypeName = GetFileTypeFriendlyName(destinationFileType);
    Console::WriteFormat("Converting from a %s to a %s.", sourceFileTypeName, destinationFileTypeName);
    Console::WriteLine();
}

static const utf8 * GetFileTypeFriendlyName(uint32 fileType)
{
    switch (fileType) {
    case FILE_EXTENSION_SC4: return "RollerCoaster Tycoon 1 scenario";
    case FILE_EXTENSION_SV4: return "RollerCoaster Tycoon 1 saved game";
    case FILE_EXTENSION_SC6: return "RollerCoaster Tycoon 2 scenario";
    case FILE_EXTENSION_SV6: return "RollerCoaster Tycoon 2 saved game";
    }

    assert(false);
    return nullptr;
}
