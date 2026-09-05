/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

    #include "ServerList.h"

    #include "../Context.h"
    #include "../Diagnostic.h"
    #include "../PlatformEnvironment.h"
    #include "../config/Config.h"
    #include "../core/File.h"
    #include "../core/FileStream.h"
    #include "../core/Guard.hpp"
    #include "../core/Http.h"
    #include "../core/Json.hpp"
    #include "../core/Path.hpp"
    #include "../core/String.hpp"
    #include "../localisation/Language.h"
    #include "../platform/Platform.h"
    #include "Network.h"
    #include "Socket.h"

    #include <numeric>
    #include <optional>

namespace OpenRCT2::Network
{
    int32_t ServerListEntry::CompareTo(const ServerListEntry& other) const
    {
        const auto& a = *this;
        const auto& b = other;

        if (a.Favourite != b.Favourite)
        {
            return a.Favourite ? -1 : 1;
        }

        if (a.Local != b.Local)
        {
            return a.Local ? -1 : 1;
        }

        bool serverACompatible = a.Version == GetVersion();
        bool serverBCompatible = b.Version == GetVersion();
        if (serverACompatible != serverBCompatible)
        {
            return serverACompatible ? -1 : 1;
        }

        if (a.RequiresPassword != b.RequiresPassword)
        {
            return a.RequiresPassword ? 1 : -1;
        }

        if (a.Players != b.Players)
        {
            return a.Players > b.Players ? -1 : 1;
        }

        return String::compare(a.Name, b.Name, true);
    }

    bool ServerListEntry::IsVersionValid() const noexcept
    {
        return Version.empty() || Version == GetVersion();
    }

    bool ServerListEntry::IsCompetitive() const noexcept
    {
        return Kind == ServerKind::competitive;
    }

    std::optional<ServerListEntry> ServerListEntry::FromJson(json_t& server)
    {
        Guard::Assert(server.is_object(), "ServerListEntry::FromJson expects parameter server to be object");

        const auto port = Json::GetNumber<int32_t>(server["port"]);
        const auto name = Json::GetString(server["name"]);
        const auto description = Json::GetString(server["description"]);
        const auto requiresPassword = Json::GetBoolean(server["requiresPassword"]);
        const auto version = Json::GetString(server["version"]);
        const auto players = Json::GetNumber<uint8_t>(server["players"]);
        const auto maxPlayers = Json::GetNumber<uint8_t>(server["maxPlayers"]);
        const auto gameMode = Json::GetString(server["gameMode"]);
        std::string ip;
        // if server["ip"] or server["ip"]["v4"] are values, this will throw an exception, so check first
        if (server["ip"].is_object() && server["ip"]["v4"].is_array())
        {
            ip = Json::GetString(server["ip"]["v4"][0]);
        }

        if (name.empty() || version.empty())
        {
            LOG_VERBOSE("Cowardly refusing to add server without name or version specified.");

            return std::nullopt;
        }

        ServerListEntry entry;

        entry.Address = ip + ":" + std::to_string(port);
        entry.Name = name;
        entry.Description = description;
        entry.Version = version;
        entry.RequiresPassword = requiresPassword;
        entry.Players = players;
        entry.MaxPlayers = maxPlayers;
        if (gameMode == "competitive")
        {
            entry.Kind = ServerKind::competitive;
            entry.CompetitiveProtocol = Json::GetNumber<uint16_t>(server["competitiveProtocol"]);
            entry.CompetitionPhase = Json::GetString(server["phase"]);
            entry.MatchId = Json::GetString(server["matchId"]);
            entry.ScenarioName = Json::GetString(server["scenarioName"]);
            entry.ScenarioFileName = Json::GetString(server["scenarioFileName"]);
            entry.ScenarioHash = Json::GetString(server["scenarioHash"]);
            entry.Victory = Json::GetString(server["victory"]);
            entry.AllowLateJoin = Json::GetBoolean(server["allowLateJoin"]);
        }

        return entry;
    }

    void ServerList::Sort()
    {
        _serverEntries.erase(
            std::unique(
                _serverEntries.begin(), _serverEntries.end(),
                [](const ServerListEntry& a, const ServerListEntry& b) {
                    if (a.Favourite == b.Favourite)
                    {
                        return String::iequals(a.Address, b.Address);
                    }
                    return false;
                }),
            _serverEntries.end());
        std::sort(_serverEntries.begin(), _serverEntries.end(), [](const ServerListEntry& a, const ServerListEntry& b) {
            return a.CompareTo(b) < 0;
        });
    }

    ServerListEntry& ServerList::GetServer(size_t index)
    {
        return _serverEntries[index];
    }

    size_t ServerList::GetCount() const
    {
        return _serverEntries.size();
    }

    void ServerList::Add(const ServerListEntry& entry)
    {
        _serverEntries.push_back(entry);
        Sort();
    }

    void ServerList::AddRange(const std::vector<ServerListEntry>& entries)
    {
        _serverEntries.insert(_serverEntries.end(), entries.begin(), entries.end());
        Sort();
    }

    void ServerList::AddOrUpdateRange(const std::vector<ServerListEntry>& entries)
    {
        for (auto& existsEntry : _serverEntries)
        {
            auto match = std::find_if(entries.begin(), entries.end(), [&](const ServerListEntry& entry) {
                return existsEntry.Address == entry.Address;
            });
            if (match != entries.end())
            {
                // Keep favourites
                auto fav = existsEntry.Favourite;

                existsEntry = *match;
                existsEntry.Favourite = fav;
            }
        }

        std::vector<ServerListEntry> newServers;
        std::copy_if(entries.begin(), entries.end(), std::back_inserter(newServers), [this](const ServerListEntry& entry) {
            return std::find_if(
                       _serverEntries.begin(), _serverEntries.end(),
                       [&](const ServerListEntry& existsEntry) { return existsEntry.Address == entry.Address; })
                == _serverEntries.end();
        });

        AddRange(newServers);
    }

    void ServerList::Clear() noexcept
    {
        _serverEntries.clear();
    }

    std::vector<ServerListEntry> ServerList::ReadFavourites() const
    {
        LOG_VERBOSE("server_list_read(...)");
        std::vector<ServerListEntry> entries;
        try
        {
            auto& env = GetContext()->GetPlatformEnvironment();
            auto path = env.GetFilePath(PathId::networkServers);
            if (File::Exists(path))
            {
                auto fs = FileStream(path, FileMode::open);
                auto numEntries = fs.ReadValue<uint32_t>();
                for (size_t i = 0; i < numEntries; i++)
                {
                    ServerListEntry serverInfo;
                    serverInfo.Address = fs.ReadString();
                    serverInfo.Name = fs.ReadString();
                    serverInfo.RequiresPassword = false;
                    serverInfo.Description = fs.ReadString();
                    serverInfo.Version.clear();
                    serverInfo.Favourite = true;
                    serverInfo.Players = 0;
                    serverInfo.MaxPlayers = 0;
                    serverInfo.Kind = static_cast<ServerKind>(fs.ReadValue<uint8_t>());
                    entries.push_back(std::move(serverInfo));
                }
            }
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Unable to read server list: %s", e.what());
            entries = std::vector<ServerListEntry>();
        }
        return entries;
    }

    void ServerList::ReadAndAddFavourites()
    {
        _serverEntries.erase(
            std::remove_if(
                _serverEntries.begin(), _serverEntries.end(), [](const ServerListEntry& entry) { return entry.Favourite; }),
            _serverEntries.end());
        auto entries = ReadFavourites();
        AddRange(entries);
    }

    void ServerList::WriteFavourites() const
    {
        // Save just favourite servers
        std::vector<ServerListEntry> favouriteServers;
        std::copy_if(
            _serverEntries.begin(), _serverEntries.end(), std::back_inserter(favouriteServers),
            [](const ServerListEntry& entry) { return entry.Favourite; });
        WriteFavourites(favouriteServers);
    }

    bool ServerList::WriteFavourites(const std::vector<ServerListEntry>& entries) const
    {
        LOG_VERBOSE("server_list_write(%d, 0x%p)", entries.size(), entries.data());

        auto& env = GetContext()->GetPlatformEnvironment();
        auto path = Path::Combine(env.GetDirectoryPath(DirBase::user), u8"servers.cfg");

        try
        {
            auto fs = FileStream(path, FileMode::write);
            fs.WriteValue<uint32_t>(static_cast<uint32_t>(entries.size()));
            for (const auto& entry : entries)
            {
                fs.WriteString(entry.Address);
                fs.WriteString(entry.Name);
                fs.WriteString(entry.Description);
                fs.WriteValue<uint8_t>(static_cast<uint8_t>(entry.Kind));
            }
            return true;
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Unable to write server list: %s", e.what());
            return false;
        }
    }

    [[nodiscard]] std::future<std::vector<ServerListEntry>> ServerList::FetchLocalServerListAsync(
        const INetworkEndpoint& broadcastEndpoint) const
    {
        auto broadcastAddress = broadcastEndpoint.GetHostname();
        return std::async(std::launch::async, [broadcastAddress] {
            constexpr auto kReceiveDelayInMs = 10;
            constexpr auto kReceiveWaitInMs = 2000;

            std::string_view msg = kLanBroadcastMsg;
            auto udpSocket = CreateUdpSocket();

            LOG_VERBOSE("Broadcasting %zu bytes to the LAN (%s)", msg.size(), broadcastAddress.c_str());
            auto len = udpSocket->SendData(broadcastAddress, kLanBroadcastPort, msg.data(), msg.size());
            if (len != msg.size())
            {
                throw std::runtime_error("Unable to broadcast server query.");
            }

            std::vector<ServerListEntry> entries;
            for (int i = 0; i < (kReceiveWaitInMs / kReceiveDelayInMs); i++)
            {
                try
                {
                    // Start with initialised buffer in case we receive a non-terminated string
                    char buffer[1024]{};
                    size_t receivedLen{};
                    std::unique_ptr<INetworkEndpoint> endpoint;
                    auto p = udpSocket->ReceiveData(buffer, sizeof(buffer) - 1, &receivedLen, &endpoint);
                    if (p == ReadPacket::success)
                    {
                        auto sender = endpoint->GetHostname();
                        LOG_VERBOSE("Received %zu bytes back from %s", receivedLen, sender.c_str());
                        auto jinfo = Json::FromString(std::string_view(buffer));

                        if (jinfo.is_object())
                        {
                            jinfo["ip"] = { { "v4", { sender } } };

                            auto entry = ServerListEntry::FromJson(jinfo);
                            if (entry.has_value())
                            {
                                entry->Local = true;
                                entries.push_back(std::move(*entry));
                            }
                        }
                    }
                }
                catch (const std::exception& e)
                {
                    LOG_WARNING("Error receiving data: %s", e.what());
                }
                Platform::Sleep(kReceiveDelayInMs);
            }
            return entries;
        });
    }

    [[nodiscard]] std::future<std::vector<ServerListEntry>> ServerList::FetchLocalServerListAsync() const
    {
        return std::async(std::launch::async, [&] {
            // Get all possible LAN broadcast addresses
            auto broadcastEndpoints = GetBroadcastAddresses();

            // Spin off a fetch for each broadcast address
            std::vector<std::future<std::vector<ServerListEntry>>> futures;
            for (const auto& broadcastEndpoint : broadcastEndpoints)
            {
                auto f = FetchLocalServerListAsync(*broadcastEndpoint);
                futures.push_back(std::move(f));
            }

            // Wait and merge all results
            std::vector<ServerListEntry> mergedEntries;
            for (auto& f : futures)
            {
                try
                {
                    auto entries = f.get();
                    mergedEntries.insert(mergedEntries.begin(), entries.begin(), entries.end());
                }
                catch (...)
                {
                    // Ignore any exceptions from a particular broadcast fetch
                }
            }
            return mergedEntries;
        });
    }

    std::future<std::vector<ServerListEntry>> ServerList::FetchOnlineServerListAsync() const
    {
    #ifdef DISABLE_HTTP
        return {};
    #else
        // Always query the official master server, plus a configured masterServerUrl IN ADDITION
        // (not instead) - see NetworkServerAdvertiser for why a competitive listing needs both.
        return std::async(std::launch::async, [this] {
            std::vector<std::string> urls = { kMasterServerURL };
            const std::string customUrl = Config::Get().network.masterServerUrl;
            LOG_INFO("FetchOnlineServerListAsync: configured masterServerUrl = \"%s\" (length %zu)", customUrl.c_str(),
                customUrl.size());
            if (!customUrl.empty() && customUrl != kMasterServerURL)
            {
                urls.push_back(customUrl);
            }

            std::vector<std::future<std::vector<ServerListEntry>>> futures;
            for (const auto& url : urls)
            {
                LOG_INFO("FetchOnlineServerListAsync: querying %s", url.c_str());
                futures.push_back(FetchOnlineServerListAsync(url));
            }

            std::vector<ServerListEntry> mergedEntries;
            for (size_t i = 0; i < futures.size(); i++)
            {
                try
                {
                    auto entries = futures[i].get();
                    LOG_INFO("FetchOnlineServerListAsync: %s returned %zu entr%s", urls[i].c_str(), entries.size(),
                        entries.size() == 1 ? "y" : "ies");
                    mergedEntries.insert(mergedEntries.end(), entries.begin(), entries.end());
                }
                catch (const std::exception& e)
                {
                    // Ignore any exceptions from a particular master server - one being down
                    // shouldn't hide listings from the other.
                    LOG_INFO("FetchOnlineServerListAsync: %s failed: %s", urls[i].c_str(), e.what());
                }
                catch (...)
                {
                    LOG_INFO("FetchOnlineServerListAsync: %s failed with unknown exception", urls[i].c_str());
                }
            }
            return mergedEntries;
        });
    #endif
    }

    std::future<std::vector<ServerListEntry>> ServerList::FetchOnlineServerListAsync(const std::string& masterServerUrl) const
    {
    #ifdef DISABLE_HTTP
        return {};
    #else

        auto p = std::make_shared<std::promise<std::vector<ServerListEntry>>>();
        auto f = p->get_future();

        Http::Request request;
        request.url = masterServerUrl;
        request.method = Http::Method::get;
        request.header["Accept"] = "application/json";
        // Despite DoAsync, the future below is not stored, so it will block the calling thread until the request completes
        // TOOD: Review if this is acceptable
        std::ignore = Http::DoAsync(request, [p, masterServerUrl](Http::Response& response) -> void {
            json_t root;
            try
            {
                LOG_INFO(
                    "FetchOnlineServerListAsync(%s): HTTP status=%d, body length=%zu, body[0..200]=%.200s",
                    masterServerUrl.c_str(), static_cast<int>(response.status), response.body.size(),
                    response.body.c_str());

                if (response.status != Http::Status::ok)
                {
                    throw MasterServerException(STR_SERVER_LIST_NO_CONNECTION);
                }

                root = Json::FromString(response.body);
                if (!root.is_object())
                {
                    throw MasterServerException(STR_SERVER_LIST_INVALID_RESPONSE_JSON_NUMBER);
                }

                auto jsonStatus = root["status"];
                if (!jsonStatus.is_number_integer())
                {
                    throw MasterServerException(STR_SERVER_LIST_INVALID_RESPONSE_JSON_NUMBER);
                }

                auto status = Json::GetNumber<int32_t>(jsonStatus);
                if (status != 200)
                {
                    throw MasterServerException(STR_SERVER_LIST_MASTER_SERVER_FAILED);
                }

                auto jServers = root["servers"];
                if (!jServers.is_array())
                {
                    throw MasterServerException(STR_SERVER_LIST_INVALID_RESPONSE_JSON_ARRAY);
                }

                std::vector<ServerListEntry> entries;
                for (auto& jServer : jServers)
                {
                    if (jServer.is_object())
                    {
                        auto entry = ServerListEntry::FromJson(jServer);
                        if (entry.has_value())
                        {
                            entries.push_back(std::move(*entry));
                        }
                    }
                }

                p->set_value(entries);
            }
            catch (...)
            {
                p->set_exception(std::current_exception());
            }
        });
        return f;
    #endif
    }

    uint32_t ServerList::GetTotalPlayerCount() const
    {
        return std::accumulate(_serverEntries.begin(), _serverEntries.end(), 0, [](uint32_t acc, const ServerListEntry& entry) {
            return acc + entry.Players;
        });
    }

    const char* MasterServerException::what() const noexcept
    {
        static std::string localisedStatusText = LanguageGetString(StatusText);
        return localisedStatusText.c_str();
    }
} // namespace OpenRCT2::Network

#endif
