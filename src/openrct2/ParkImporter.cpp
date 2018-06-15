/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

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
