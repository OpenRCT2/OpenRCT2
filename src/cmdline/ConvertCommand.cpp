#include "../common.h"
#include "../core/Console.hpp"
#include "../core/Exception.hpp"
#include "../core/Path.hpp"
#include "../rct1/S4Importer.h"
#include "CommandLine.hpp"

extern "C"
{
    #include "../game.h"
    #include "../scenario.h"
    #include "../openrct2.h"
    #include "../interface/window.h"
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
    if (!openrct2_initialise()) {
        Console::Error::WriteLine("Error while initialising OpenRCT2.");
        return EXITCODE_FAIL;
    }

    if (sourceFileType == FILE_EXTENSION_SV4 ||
        sourceFileType == FILE_EXTENSION_SC4)
    {
        auto s4Importer = new S4Importer();
        try
        {
            if (sourceFileType == FILE_EXTENSION_SC4)
            {
                s4Importer->LoadScenario(sourcePath);
            }
            if (sourceFileType == FILE_EXTENSION_SV4)
            {
                s4Importer->LoadSavedGame(sourcePath);
            }

            s4Importer->Import();

            if (sourceFileType == FILE_EXTENSION_SC4)
            {
                // We are converting a scenario, so reset the park
                scenario_begin();
            }
        }
        catch (Exception ex)
        {
            Console::Error::WriteLine(ex.GetMsg());
            return EXITCODE_FAIL;
        }
    }
    else
    {
        if (sourceFileType == FILE_EXTENSION_SC6)
        {
            scenario_load_and_play_from_path(sourcePath);
        }
        if (sourceFileType == FILE_EXTENSION_SV6)
        {
            game_load_save(sourcePath);
        }
    }

    SDL_RWops* rw = SDL_RWFromFile(destinationPath, "wb+");
    if (rw != NULL) {
        // HACK remove the main window so it saves the park with the
        //      correct initial view
        window_close_by_class(WC_MAIN_WINDOW);

        if (destinationFileType == FILE_EXTENSION_SC6)
        {
            scenario_save(rw, 0x80000002);
        }
        else
        {
            scenario_save(rw, 0x80000001);
        }
        SDL_RWclose(rw);
        Console::WriteLine("Conversion successful!");
    }
    else
    {
        Console::Error::WriteLine("Unable to write destination file.");
        return EXITCODE_FAIL;
    }
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
