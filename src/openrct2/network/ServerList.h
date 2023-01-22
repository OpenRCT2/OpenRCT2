/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/JsonFwd.hpp"

#include <future>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>
namespace OpenRCT2
{
    struct INetworkEndpoint;

    struct ServerListEntry
    {
        std::string Address;
        std::string Name;
        std::string Description;
        std::string Version;
        bool RequiresPassword{};
        bool Favourite{};
        uint8_t Players{};
        uint8_t MaxPlayers{};
        bool Local{};

        int32_t CompareTo(const ServerListEntry& other) const;
        bool IsVersionValid() const noexcept;

        /**
         * Creates a ServerListEntry object from a JSON object
         *
         * @param json JSON data source - must be object type
         * @return A NetworkGroup object
         * @note json is deliberately left non-const: json_t behaviour changes when const
         */
        static std::optional<ServerListEntry> FromJson(json_t& server);
    };

    class ServerList
    {
    private:
        std::vector<ServerListEntry> _serverEntries;

        void Sort();
        std::vector<ServerListEntry> ReadFavourites() const;
        bool WriteFavourites(const std::vector<ServerListEntry>& entries) const;
        std::future<std::vector<ServerListEntry>> FetchLocalServerListAsync(const INetworkEndpoint& broadcastEndpoint) const;

    public:
        ServerListEntry& GetServer(size_t index);
        size_t GetCount() const;
        void Add(const ServerListEntry& entry);
        void AddRange(const std::vector<ServerListEntry>& entries);
        void AddOrUpdateRange(const std::vector<ServerListEntry>& entries);
        void Clear() noexcept;

        void ReadAndAddFavourites();
        void WriteFavourites() const;

        std::future<std::vector<ServerListEntry>> FetchLocalServerListAsync() const;
        std::future<std::vector<ServerListEntry>> FetchOnlineServerListAsync() const;
        uint32_t GetTotalPlayerCount() const;
    };

    class MasterServerException : public std::exception
    {
    public:
        StringId StatusText;

        MasterServerException(StringId statusText)
            : StatusText(statusText)
        {
        }
    };
} // namespace OpenRCT2
