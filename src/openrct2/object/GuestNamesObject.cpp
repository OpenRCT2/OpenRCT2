/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GuestNamesObject.h"

#include "../Context.h"
#include "../PlatformEnvironment.h"
#include "../core/Json.hpp"

using namespace OpenRCT2;

void GuestNamesObject::Load()
{
}

void GuestNamesObject::Unload()
{
}

void GuestNamesObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    Guard::Assert(root.is_object(), "GuestNamesObject::ReadJson expects parameter root to be an object");
    PopulateTablesFromJson(context, root);

    Guard::Assert(root["given_names"].is_array(), "GuestNamesObject::ReadJson expects given_names to be an array");
    _givenNames = root["given_names"].get<std::vector<std::string>>();
    std::sort(_givenNames.begin(), _givenNames.end());

    Guard::Assert(root["surnames"].is_array(), "GuestNamesObject::ReadJson expects surnames to be an array");
    _surnames = root["surnames"].get<std::vector<std::string>>();
    std::sort(_surnames.begin(), _surnames.end());
}

std::string GuestNamesObject::GetGivenNameAt(size_t index) const
{
    return _givenNames[index % _givenNames.size()];
}

std::string GuestNamesObject::GetSurnameAt(size_t index) const
{
    return _surnames[index % _surnames.size()];
}
