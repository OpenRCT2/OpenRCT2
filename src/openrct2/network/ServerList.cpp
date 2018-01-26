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

#include <vector>
#include "../core/FileStream.hpp"
#include "../core/Memory.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../platform/platform.h"
#include "ServerList.h"

std::vector<server_entry> server_list_read()
{
    log_verbose("server_list_read(...)");

    utf8 path[MAX_PATH];
    platform_get_user_directory(path, nullptr, sizeof(path));
    Path::Append(path, sizeof(path), "servers.cfg");

    std::vector<server_entry> entries;
    try
    {
        auto fs = FileStream(path, FILE_MODE_OPEN);
        auto numEntries = fs.ReadValue<uint32>();
        entries.resize(numEntries);

        // Load each server entry
        for (uint32 i = 0; i < numEntries; i++)
        {
            server_entry * serverInfo = &entries[i];
            serverInfo->address = fs.ReadString();
            serverInfo->name = fs.ReadString();
            serverInfo->requiresPassword = false;
            serverInfo->description = fs.ReadString();
            serverInfo->version = String::Duplicate("");
            serverInfo->favourite = true;
            serverInfo->players = 0;
            serverInfo->maxplayers = 0;
        }
    }
    catch (const std::exception &)
    {
        entries = std::vector<server_entry>();
    }
    return entries;
}

bool server_list_write(const std::vector<server_entry> &entries)
{
    log_verbose("server_list_write(%d, 0x%p)", entries.size(), entries.data());

    utf8 path[MAX_PATH];
    platform_get_user_directory(path, nullptr, sizeof(path));
    Path::Append(path, sizeof(path), "servers.cfg");

    try
    {
        auto fs = FileStream(path, FILE_MODE_WRITE);
        fs.WriteValue<uint32>((uint32)entries.size());

        // Write each server entry
        for (const auto &entry : entries)
        {
            fs.WriteString(entry.address);
            fs.WriteString(entry.name);
            fs.WriteString(entry.description);
        }
        return true;
    }
    catch (const std::exception &)
    {
        return false;
    }
}
