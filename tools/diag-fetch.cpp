// Standalone diagnostic: reproduces exactly what ServerList::FetchOnlineServerListAsync does
// (same WinHTTP call pattern, same JSON validation) against a given URL, with full logging,
// to find out where the real client's fetch of the self-hosted master server is actually failing.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>

#include <iostream>
#include <map>
#include <string>

#include <nlohmann/json.hpp>

#pragma comment(lib, "winhttp.lib")

using json = nlohmann::json;

std::wstring ToWide(const std::string& s)
{
    if (s.empty())
        return {};
    int len = MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), nullptr, 0);
    std::wstring result(len, 0);
    MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), result.data(), len);
    return result;
}

int main(int argc, char** argv)
{
    std::string url = argc > 1 ? argv[1] : "http://82.37.200.253:8080";
    std::cout << "Fetching: " << url << "\n";

    URL_COMPONENTS uc{};
    uc.dwStructSize = sizeof(uc);
    uc.dwSchemeLength = static_cast<DWORD>(-1);
    uc.dwHostNameLength = static_cast<DWORD>(-1);
    uc.dwUrlPathLength = static_cast<DWORD>(-1);
    uc.dwExtraInfoLength = static_cast<DWORD>(-1);

    auto wUrl = ToWide(url);
    if (!WinHttpCrackUrl(wUrl.c_str(), 0, 0, &uc))
    {
        std::cout << "WinHttpCrackUrl FAILED: " << GetLastError() << "\n";
        return 1;
    }
    std::wcout << L"  scheme=" << std::wstring(uc.lpszScheme, uc.dwSchemeLength) << L" host="
               << std::wstring(uc.lpszHostName, uc.dwHostNameLength) << L" port=" << uc.nPort
               << L" path=[" << std::wstring(uc.lpszUrlPath, uc.dwUrlPathLength) << L"]\n";

    HINTERNET hSession = WinHttpOpen(
        L"OpenRCT2-diag/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession)
    {
        std::cout << "WinHttpOpen FAILED: " << GetLastError() << "\n";
        return 1;
    }
    WinHttpSetTimeouts(hSession, 10000, 10000, 30000, 30000);

    auto wHost = std::wstring(uc.lpszHostName, uc.dwHostNameLength);
    HINTERNET hConnect = WinHttpConnect(hSession, wHost.c_str(), uc.nPort, 0);
    if (!hConnect)
    {
        std::cout << "WinHttpConnect FAILED: " << GetLastError() << "\n";
        return 1;
    }

    DWORD flags = 0;
    if (lstrcmpiW(std::wstring(uc.lpszScheme, uc.dwSchemeLength).c_str(), L"https") == 0)
        flags = WINHTTP_FLAG_SECURE;

    std::wstring path = uc.dwUrlPathLength > 0 ? std::wstring(uc.lpszUrlPath, uc.dwUrlPathLength) : L"/";
    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect, L"GET", path.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
    if (!hRequest)
    {
        std::cout << "WinHttpOpenRequest FAILED: " << GetLastError() << "\n";
        return 1;
    }

    std::wstring acceptHeader = L"Accept: application/json";
    WinHttpAddRequestHeaders(hRequest, acceptHeader.c_str(), static_cast<ULONG>(-1L), WINHTTP_ADDREQ_FLAG_ADD);

    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
    {
        std::cout << "WinHttpSendRequest FAILED: " << GetLastError() << "\n";
        return 1;
    }

    if (!WinHttpReceiveResponse(hRequest, NULL))
    {
        std::cout << "WinHttpReceiveResponse FAILED: " << GetLastError() << "\n";
        return 1;
    }

    wchar_t statusBuf[32]{};
    DWORD statusLen = sizeof(statusBuf);
    WinHttpQueryHeaders(
        hRequest, WINHTTP_QUERY_STATUS_CODE, WINHTTP_HEADER_NAME_BY_INDEX, statusBuf, &statusLen,
        WINHTTP_NO_HEADER_INDEX);
    std::wcout << L"HTTP status: " << statusBuf << L"\n";

    std::string body;
    DWORD avail = 0;
    do
    {
        if (!WinHttpQueryDataAvailable(hRequest, &avail))
        {
            std::cout << "WinHttpQueryDataAvailable FAILED: " << GetLastError() << "\n";
            break;
        }
        if (avail == 0)
            break;
        std::string chunk(avail, '\0');
        DWORD read = 0;
        if (!WinHttpReadData(hRequest, chunk.data(), avail, &read))
        {
            std::cout << "WinHttpReadData FAILED: " << GetLastError() << "\n";
            break;
        }
        chunk.resize(read);
        body += chunk;
    } while (avail > 0);

    std::cout << "Body length: " << body.size() << "\n";
    std::cout << "Body (first 500 chars): " << body.substr(0, 500) << "\n";

    // Now replicate ServerList.cpp's exact validation.
    auto root = json::parse(body, nullptr, false);
    if (root.is_discarded())
    {
        std::cout << "!! JSON PARSE FAILED\n";
        return 1;
    }
    if (!root.is_object())
    {
        std::cout << "!! root is not an object\n";
        return 1;
    }
    auto jsonStatus = root["status"];
    std::cout << "status field is_number_integer=" << jsonStatus.is_number_integer()
              << " raw=" << jsonStatus.dump() << "\n";
    if (!jsonStatus.is_number_integer())
    {
        std::cout << "!! WOULD THROW: STR_SERVER_LIST_INVALID_RESPONSE_JSON_NUMBER\n";
        return 1;
    }
    int statusVal = jsonStatus.get<int>();
    if (statusVal != 200)
    {
        std::cout << "!! WOULD THROW: STR_SERVER_LIST_MASTER_SERVER_FAILED (status=" << statusVal << ")\n";
        return 1;
    }
    auto jServers = root["servers"];
    if (!jServers.is_array())
    {
        std::cout << "!! WOULD THROW: STR_SERVER_LIST_INVALID_RESPONSE_JSON_ARRAY\n";
        return 1;
    }
    std::cout << "OK - " << jServers.size() << " server(s) would be parsed.\n";
    for (auto& s : jServers)
    {
        std::cout << "  gameMode=" << s.value("gameMode", "<missing>") << " name=" << s.value("name", "<missing>")
                  << "\n";
    }
    return 0;
}
