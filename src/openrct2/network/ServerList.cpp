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

#include "../core/FileStream.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "ServerList.h"

#include "../platform/platform.h"

extern "C"
{
    bool server_list_read(uint32 * outNumEntries, server_entry * * outEntries)
    {
        log_verbose("server_list_read(...)");

        utf8 path[MAX_PATH];
        platform_get_user_directory(path, nullptr, sizeof(path));
        Path::Append(path, sizeof(path), "servers.cfg");

        uint32 numEntries = 0;
        server_entry * entries = nullptr;
        try
        {
            auto fs = FileStream(path, FILE_MODE_OPEN);
            numEntries = fs.ReadValue<uint32>();
            entries = Memory::AllocateArray<server_entry>(numEntries);

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
        catch (const Exception &)
        {
            Memory::FreeArray(entries, numEntries);
            numEntries = 0;
            entries = nullptr;
        }

        *outNumEntries = numEntries;
        *outEntries = entries;
        return entries != nullptr;
    }

    bool server_list_write(uint32 numEntries, server_entry * entries)
    {
        log_verbose("server_list_write(%d, 0x%p)", numEntries, entries);

        utf8 path[MAX_PATH];
        platform_get_user_directory(path, nullptr, sizeof(path));
        Path::Append(path, sizeof(path), "servers.cfg");

        try
        {
            auto fs = FileStream(path, FILE_MODE_WRITE);
            fs.WriteValue<uint32>(numEntries);

            // Write each server entry
            for (uint32 i = 0; i < numEntries; i++)
            {
                server_entry * serverInfo = &entries[i];
                fs.WriteString(serverInfo->address);
                fs.WriteString(serverInfo->name);
                fs.WriteString(serverInfo->description);
            }
            return true;
        }
        catch (const Exception &)
        {
            return false;
        }
    }
}
