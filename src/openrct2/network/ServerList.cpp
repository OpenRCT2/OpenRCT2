/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../core/FileStream.hpp"
#include "../core/Memory.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../Context.h"
#include "../PlatformEnvironment.h"
#include "../platform/platform.h"
#include "ServerList.h"

using namespace OpenRCT2;

std::vector<server_entry> server_list_read()
{
    log_verbose("server_list_read(...)");
    std::vector<server_entry> entries;
    try
    {
        auto env = GetContext()->GetPlatformEnvironment();
        auto path = env->GetFilePath(PATHID::NETWORK_SERVERS);
        auto fs = FileStream(path, FILE_MODE_OPEN);
        auto numEntries = fs.ReadValue<uint32_t>();
        for (size_t i = 0; i < numEntries; i++)
        {
            server_entry serverInfo;
            serverInfo.address = fs.ReadString();
            serverInfo.name = fs.ReadString();
            serverInfo.requiresPassword = false;
            serverInfo.description = fs.ReadString();
            serverInfo.version = String::Duplicate("");
            serverInfo.favourite = true;
            serverInfo.players = 0;
            serverInfo.maxplayers = 0;
            entries.push_back(std::move(serverInfo));
        }
    }
    catch (const std::exception &e)
    {
        log_error("Unable to read server list: %s", e.what());
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
        fs.WriteValue<uint32_t>((uint32_t)entries.size());
        for (const auto &entry : entries)
        {
            fs.WriteString(entry.address);
            fs.WriteString(entry.name);
            fs.WriteString(entry.description);
        }
        return true;
    }
    catch (const std::exception &e)
    {
        log_error("Unable to write server list: %s", e.what());
        return false;
    }
}
