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
#include "Context.h"
#include "core/Path.hpp"
#include "core/String.hpp"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "ParkImporter.h"

namespace ParkImporter
{
    std::unique_ptr<IParkImporter> Create(const std::string &hintPath)
    {
        std::unique_ptr<IParkImporter> parkImporter;
        std::string extension = Path::GetExtension(hintPath);
        if (ExtensionIsRCT1(extension))
        {
            parkImporter = CreateS4();
        }
        else
        {
            auto context = OpenRCT2::GetContext();
            parkImporter = CreateS6(context->GetObjectRepository(), context->GetObjectManager());
        }
        return parkImporter;
    }

    bool ExtensionIsRCT1(const std::string &extension)
    {
        return String::Equals(extension, ".sc4", true) ||
            String::Equals(extension, ".sv4", true);
    }

    bool ExtensionIsScenario(const std::string &extension)
    {
        return String::Equals(extension, ".sc4", true) ||
            String::Equals(extension, ".sc6", true);
    }
}
