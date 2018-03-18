#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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
#include "core/Path.hpp"
#include "core/String.hpp"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "ParkImporter.h"

ParkLoadResult::ParkLoadResult(PARK_LOAD_ERROR error)
    : Error(error),
      Flag(0)
{
}

ParkLoadResult::ParkLoadResult(PARK_LOAD_ERROR error, const std::vector<rct_object_entry> &missingObjects)
    : Error(error),
      MissingObjects(missingObjects),
      Flag(0)
{
}

ParkLoadResult::ParkLoadResult(PARK_LOAD_ERROR error, const uint8 flag)
    : Error(error),
      Flag(flag)
{
}

ParkLoadResult ParkLoadResult::CreateOK()
{
    return ParkLoadResult(PARK_LOAD_ERROR::PARK_LOAD_ERROR_OK);
}

ParkLoadResult ParkLoadResult::CreateInvalidExtension()
{
    return ParkLoadResult(PARK_LOAD_ERROR::PARK_LOAD_ERROR_INVALID_EXTENSION);
}

ParkLoadResult ParkLoadResult::CreateMissingObjects(const std::vector<rct_object_entry> &missingObjects)
{
    return ParkLoadResult(PARK_LOAD_ERROR::PARK_LOAD_ERROR_MISSING_OBJECTS, missingObjects);
}

ParkLoadResult ParkLoadResult::CreateUnknown()
{
    return ParkLoadResult(PARK_LOAD_ERROR::PARK_LOAD_ERROR_UNKNOWN);
}

ParkLoadResult ParkLoadResult::CreateUnsupportedRCTCflag(uint8 classic_flag)
{
    return ParkLoadResult(PARK_LOAD_ERROR::PARK_LOAD_ERROR_UNSUPPORTED_RCTC_FLAG, classic_flag);
}

PARK_LOAD_ERROR ParkLoadResult_GetError(const ParkLoadResult * t)
{
    return t->Error;
}

size_t ParkLoadResult_GetMissingObjectsCount(const ParkLoadResult * t)
{
    return t->MissingObjects.size();
}

uint8 ParkLoadResult_GetFlag(const ParkLoadResult * t)
{
    return t->Flag;
}

const rct_object_entry * ParkLoadResult_GetMissingObjects(const ParkLoadResult * t)
{
    return t->MissingObjects.data();
}

void ParkLoadResult_Delete(ParkLoadResult * t)
{
    delete t;
}

ParkLoadResult * ParkLoadResult_CreateInvalidExtension()
{
    return new ParkLoadResult(ParkLoadResult::CreateInvalidExtension());
}


namespace ParkImporter
{
    IParkImporter * Create(const std::string &hintPath)
    {
        IParkImporter * parkImporter = nullptr;
        std::string extension = Path::GetExtension(hintPath);
        if (ExtensionIsRCT1(extension))
        {
            parkImporter = CreateS4();
        }
        else
        {
            parkImporter = CreateS6(GetObjectRepository(), GetObjectManager());
        }
        return parkImporter;
    }

    bool ExtensionIsRCT1(const std::string &extension)
    {
        if (String::Equals(extension, ".sc4", true) ||
            String::Equals(extension, ".sv4", true))
        {
            return true;
        }
        return false;
    }

    bool ExtensionIsScenario(const std::string &extension)
    {
        if (String::Equals(extension, ".sc4", true) ||
            String::Equals(extension, ".sc6", true))
        {
            return true;
        }
        return false;
    }
}

void park_importer_load_from_stream(void * stream_c, const utf8 * hintPath_c)
{
    IStream * stream = (IStream *)stream_c;
    std::string hintPath = String::ToStd(hintPath_c);

    bool isScenario = ParkImporter::ExtensionIsScenario(hintPath);

    auto parkImporter = std::unique_ptr<IParkImporter>(ParkImporter::Create(hintPath));
    parkImporter->LoadFromStream(stream, isScenario);
    parkImporter->Import();
}

bool park_importer_extension_is_scenario(const utf8 * extension)
{
    return ParkImporter::ExtensionIsScenario(String::ToStd(extension));
}

