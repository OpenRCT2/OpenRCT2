// Minimal self-hosted master server for competitive mode, replacing servers.openrct2.io.
// Implements exactly what NetworkServerAdvertiser (register/heartbeat) and ServerList
// (GET listing) expect - see docs/competitive/README.md for why this exists: the real
// public OpenRCT2 master server has a fixed schema and silently drops the custom
// gameMode/competitiveProtocol/matchId fields this fork needs, so competitive listings
// never showed up there.
//
// This is a standalone native tool (no Node/Python/etc required to run it) so anyone
// hosting a competitive match can just double-click competitive-master-server.exe.
// It is functionally identical to competitive-master-server.js - keep both in sync.

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#include <chrono>
#include <cstdio>
#include <iostream>
#include <map>
#include <mutex>
#include <random>
#include <sstream>
#include <string>
#include <thread>

#include <nlohmann/json.hpp>

#pragma comment(lib, "ws2_32.lib")

using json = nlohmann::json;
using Clock = std::chrono::steady_clock;

namespace
{
    constexpr uint16_t kPort = 8080;
    constexpr auto kEntryTimeout = std::chrono::seconds(90); // a bit over the client's 60s heartbeat interval

    struct ServerEntry
    {
        std::string ip;
        uint16_t port{};
        Clock::time_point lastSeen;
        json gameInfo = json::object();
        uint32_t players{};
    };

    std::mutex g_mutex;
    std::map<std::string, ServerEntry> g_servers; // token -> entry

    std::string GenerateToken()
    {
        static constexpr char hex[] = "0123456789abcdef";
        std::random_device rd;
        std::uniform_int_distribution<int> dist(0, 15);
        std::string token(32, '0');
        for (auto& c : token)
            c = hex[dist(rd)];
        return token;
    }

    void SweepExpired()
    {
        const auto now = Clock::now();
        for (auto it = g_servers.begin(); it != g_servers.end();)
        {
            if (now - it->second.lastSeen > kEntryTimeout)
                it = g_servers.erase(it);
            else
                ++it;
        }
    }

    std::string ClientIp(SOCKET client)
    {
        sockaddr_in addr{};
        int addrLen = sizeof(addr);
        if (getpeername(client, reinterpret_cast<sockaddr*>(&addr), &addrLen) == 0)
        {
            char buffer[INET_ADDRSTRLEN]{};
            if (inet_ntop(AF_INET, &addr.sin_addr, buffer, sizeof(buffer)) != nullptr)
                return buffer;
        }
        return "0.0.0.0";
    }

    // Reads one HTTP request off the socket: method, header block, and body (if
    // Content-Length is present). Deliberately minimal - method is all that matters,
    // the path is never inspected, matching the Node reference implementation.
    struct Request
    {
        std::string method;
        std::string body;
    };

    std::string RecvAll(SOCKET client, size_t atLeast)
    {
        std::string data;
        char buffer[4096];
        while (data.size() < atLeast)
        {
            int n = recv(client, buffer, sizeof(buffer), 0);
            if (n <= 0)
                break;
            data.append(buffer, static_cast<size_t>(n));
        }
        return data;
    }

    Request ReadRequest(SOCKET client)
    {
        Request request;
        std::string buffer;
        char chunk[4096];

        size_t headerEnd = std::string::npos;
        while (headerEnd == std::string::npos)
        {
            int n = recv(client, chunk, sizeof(chunk), 0);
            if (n <= 0)
                return request;
            buffer.append(chunk, static_cast<size_t>(n));
            headerEnd = buffer.find("\r\n\r\n");
        }

        std::istringstream headerStream(buffer.substr(0, headerEnd));
        std::string requestLine;
        std::getline(headerStream, requestLine);
        std::istringstream lineStream(requestLine);
        lineStream >> request.method;

        size_t contentLength = 0;
        std::string headerLine;
        while (std::getline(headerStream, headerLine))
        {
            std::string lower = headerLine;
            for (auto& c : lower)
                c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
            if (lower.rfind("content-length:", 0) == 0)
                contentLength = static_cast<size_t>(std::stoul(headerLine.substr(headerLine.find(':') + 1)));
        }

        std::string bodySoFar = buffer.substr(headerEnd + 4);
        if (bodySoFar.size() < contentLength)
            bodySoFar += RecvAll(client, contentLength - bodySoFar.size());
        request.body = bodySoFar.substr(0, contentLength);
        return request;
    }

    void SendJson(SOCKET client, int status, const json& body)
    {
        const std::string dump = body.dump();
        std::ostringstream response;
        response << "HTTP/1.1 " << status << " OK\r\n"
                  << "Content-Type: application/json\r\n"
                  << "Content-Length: " << dump.size() << "\r\n"
                  << "Connection: close\r\n\r\n"
                  << dump;
        const std::string out = response.str();
        send(client, out.data(), static_cast<int>(out.size()), 0);
    }

    void HandleGet(SOCKET client)
    {
        std::scoped_lock lock(g_mutex);
        SweepExpired();

        json servers = json::array();
        for (const auto& [token, entry] : g_servers)
        {
            json item = entry.gameInfo; // may already carry gameMode, matchId, etc.
            item["ip"] = { { "v4", { entry.ip } }, { "v6", json::array() } };
            item["port"] = entry.port;
            item["players"] = entry.players;
            if (!item.contains("version"))
                item["version"] = "unknown";
            if (!item.contains("requiresPassword"))
                item["requiresPassword"] = false;
            if (!item.contains("gameMode"))
                item["gameMode"] = "competitive";
            servers.push_back(std::move(item));
        }
        // status must be the NUMBER 200 - ServerList.cpp's client parser rejects the response
        // outright (MasterServerException) if "status" is missing or not a number, regardless of
        // HTTP status code. The real master server actually sends "status":"ok" (a string) in this
        // same spot, which fails that same check - not something we can fix from here.
        SendJson(client, 200, { { "servers", servers }, { "status", 200 } });
    }

    void HandlePost(SOCKET client, const std::string& body)
    {
        json parsed = json::parse(body, nullptr, false);
        if (parsed.is_discarded())
            parsed = json::object();

        const auto token = GenerateToken();
        ServerEntry entry;
        entry.ip = parsed.value("address", ClientIp(client));
        entry.port = parsed.value("port", 0);
        entry.lastSeen = Clock::now();

        {
            std::scoped_lock lock(g_mutex);
            g_servers[token] = std::move(entry);
        }
        SendJson(client, 200, { { "status", 200 }, { "token", token } });
    }

    void HandlePut(SOCKET client, const std::string& body)
    {
        json parsed = json::parse(body, nullptr, false);
        if (parsed.is_discarded())
            parsed = json::object();

        const std::string token = parsed.value("token", "");
        std::scoped_lock lock(g_mutex);
        auto it = g_servers.find(token);
        if (it == g_servers.end())
        {
            SendJson(client, 200, { { "status", 401 } });
            return;
        }

        it->second.lastSeen = Clock::now();
        it->second.players = parsed.value("players", 0u);
        if (parsed.contains("gameInfo") && parsed["gameInfo"].is_object())
            it->second.gameInfo = parsed["gameInfo"];
        SendJson(client, 200, { { "status", 200 } });
    }

    void HandleClient(SOCKET client)
    {
        Request request = ReadRequest(client);
        try
        {
            if (request.method == "GET")
                HandleGet(client);
            else if (request.method == "POST")
                HandlePost(client, request.body);
            else if (request.method == "PUT")
                HandlePut(client, request.body);
            else
                SendJson(client, 404, { { "status", 404 } });
        }
        catch (const std::exception& e)
        {
            SendJson(client, 500, { { "status", 500 }, { "message", e.what() } });
        }
        closesocket(client);
    }
} // namespace

namespace
{
    // Double-clicked from Explorer, this process's console window closes the instant main()
    // returns - without this, an early failure (e.g. port already in use) is an invisible flicker.
    int FailWithPause(const std::string& message)
    {
        std::cerr << message << "\n\nPress Enter to close this window...";
        std::cin.get();
        return 1;
    }
} // namespace

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        return FailWithPause("WSAStartup failed");
    }

    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == INVALID_SOCKET)
    {
        return FailWithPause("socket() failed: " + std::to_string(WSAGetLastError()));
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(kPort);

    if (bind(listener, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == SOCKET_ERROR)
    {
        return FailWithPause(
            "bind() failed: " + std::to_string(WSAGetLastError()) + " - is port " + std::to_string(kPort)
            + " already in use (perhaps by another copy of this program)?");
    }

    if (listen(listener, SOMAXCONN) == SOCKET_ERROR)
    {
        return FailWithPause("listen() failed: " + std::to_string(WSAGetLastError()));
    }

    std::cout << "Competitive master server listening on :" << kPort << "\n";
    std::cout << "Leave this window open while hosting. Close it to stop the listing server.\n";

    while (true)
    {
        SOCKET client = accept(listener, nullptr, nullptr);
        if (client == INVALID_SOCKET)
            continue;
        std::thread(HandleClient, client).detach();
    }
}
