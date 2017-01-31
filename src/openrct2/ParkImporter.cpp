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

#include "core/Path.hpp"
#include "core/String.hpp"
#include "ParkImporter.h"

IParkImporter * CreateParkImporterForPath(const std::string &path)
{
    IParkImporter * parkImporter = nullptr;
    std::string extension = Path::GetExtension(path);
    if (String::Equals(extension, ".sc4", true) ||
        String::Equals(extension, ".sv4", true))
    {
        parkImporter = CreateS4Importer();
    }
    else
    {
        parkImporter = CreateS6Importer();
    }
    return parkImporter;
}
