/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if !defined(DISABLE_HTTP) && defined(_WIN32)

#    include "Http.h"

#    include "../Version.h"
#    include "../core/Console.hpp"
#    include "String.hpp"

#    include <cstdio>
#    include <stdexcept>
#    include <windows.h>
#    include <winhttp.h>

namespace Http
{
    static constexpr char OPENRCT2_USER_AGENT[] = "OpenRCT2/" OPENRCT2_VERSION;

    static void ThrowWin32Exception(const char* methodName)
    {
        auto errorCode = GetLastError();
        auto msg = String::StdFormat("%s failed, error: %d.", methodName, errorCode);
        throw std::runtime_error(msg);
    }

    static const wchar_t* GetVerb(Method method)
    {
        switch (method)
        {
            case Method::GET:
                return L"GET";
            case Method::POST:
                return L"POST";
            case Method::PUT:
                return L"PUT";
            default:
                throw std::runtime_error("Unsupported verb.");
        }
    }

    static int32_t ReadStatusCode(HINTERNET hRequest)
    {
        wchar_t headerBuffer[32]{};
        auto headerBufferLen = static_cast<DWORD>(std::size(headerBuffer));
        if (!WinHttpQueryHeaders(
                hRequest, WINHTTP_QUERY_STATUS_CODE, L"StatusCode", headerBuffer, &headerBufferLen, WINHTTP_NO_HEADER_INDEX))
        {
            ThrowWin32Exception("WinHttpQueryHeaders");
        }
        auto statusCode = std::stoi(headerBuffer);
        return statusCode < 0 || statusCode > 999 ? 0 : statusCode;
    }

    static std::map<std::string, std::string> ReadHeaders(HINTERNET hRequest)
    {
        DWORD dwSize{};
        WinHttpQueryHeaders(
            hRequest, WINHTTP_QUERY_RAW_HEADERS, WINHTTP_HEADER_NAME_BY_INDEX, NULL, &dwSize, WINHTTP_NO_HEADER_INDEX);

        std::wstring buffer;
        buffer.resize(dwSize);
        if (!WinHttpQueryHeaders(
                hRequest, WINHTTP_QUERY_RAW_HEADERS, WINHTTP_HEADER_NAME_BY_INDEX, buffer.data(), &dwSize,
                WINHTTP_NO_HEADER_INDEX))
            ThrowWin32Exception("WinHttpQueryHeaders");

        std::map<std::string, std::string> headers;
        std::wstring wKey, wValue;

        constexpr int32_t STATE_EXPECT_KEY = 0;
        constexpr int32_t STATE_EXPECT_WHITESPACE_VALUE = 1;
        constexpr int32_t STATE_EXPECT_VALUE = 2;
        int32_t state = STATE_EXPECT_KEY;
        int32_t index = 0;
        for (auto c : buffer)
        {
            if (c == '\0')
            {
                // Ignore first header as that is the HTTP version which
                // we don't really count as a header.
                if (index != 0 && wKey.size() != 0)
                {
                    auto key = String::ToUtf8(wKey);
                    auto value = String::ToUtf8(wValue);
                    headers[key] = std::move(value);
                }
                wKey.clear();
                wValue.clear();
                index++;
                state = STATE_EXPECT_KEY;
                continue;
            }
            if (state == STATE_EXPECT_KEY && c == ':')
            {
                state = STATE_EXPECT_WHITESPACE_VALUE;
            }
            else if (state == 1 && c == ' ')
            {
                state = STATE_EXPECT_VALUE;
            }
            else if (state == STATE_EXPECT_KEY)
            {
                wKey.push_back(c);
            }
            else
            {
                state = STATE_EXPECT_VALUE;
                wValue.push_back(c);
            }
        }
        return headers;
    }

    static std::string ReadBody(HINTERNET hRequest)
    {
        std::string body;
        DWORD dwRealSize{};
        DWORD dwDownloaded{};
        do
        {
            // Check for available data.
            DWORD dwSizeToRead{};
            if (!WinHttpQueryDataAvailable(hRequest, &dwSizeToRead))
                ThrowWin32Exception("WinHttpQueryDataAvailable");

            // Apparently some servers may report no bytes left to
            // download incorrectly, so still attempt to read...
            if (dwSizeToRead == 0)
                dwSizeToRead = 4096;

            body.resize(dwRealSize + dwSizeToRead);
            auto dst = reinterpret_cast<LPVOID>(&body[dwRealSize]);

            dwDownloaded = 0;
            if (!WinHttpReadData(hRequest, dst, dwSizeToRead, &dwDownloaded))
                ThrowWin32Exception("WinHttpReadData");

            dwRealSize += dwDownloaded;
        } while (dwDownloaded > 0);
        body.resize(dwRealSize);
        body.shrink_to_fit();
        return body;
    }

    Response Do(const Request& req)
    {
        HINTERNET hSession{}, hConnect{}, hRequest{};
        try
        {
            URL_COMPONENTS url{};
            url.dwStructSize = sizeof(url);
            url.dwSchemeLength = static_cast<DWORD>(-1);
            url.dwHostNameLength = static_cast<DWORD>(-1);
            url.dwUrlPathLength = static_cast<DWORD>(-1);
            url.dwExtraInfoLength = static_cast<DWORD>(-1);

            auto wUrl = String::ToWideChar(req.url);
            if (!WinHttpCrackUrl(wUrl.c_str(), 0, 0, &url))
                throw std::invalid_argument("Unable to parse URI.");

            auto userAgent = String::ToWideChar(OPENRCT2_USER_AGENT);
            hSession = WinHttpOpen(
                userAgent.c_str(), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
            if (hSession == nullptr)
                ThrowWin32Exception("WinHttpOpen");

            auto wHostName = std::wstring(url.lpszHostName, url.dwHostNameLength);
            hConnect = WinHttpConnect(hSession, wHostName.c_str(), url.nPort, 0);
            if (hConnect == nullptr)
                ThrowWin32Exception("WinHttpConnect");

            auto dwFlags = 0;
            if (lstrcmpiW(std::wstring(url.lpszScheme, url.dwSchemeLength).c_str(), L"https") == 0)
            {
                dwFlags = WINHTTP_FLAG_SECURE;
            }

            auto wVerb = GetVerb(req.method);
            auto wQuery = std::wstring(url.lpszUrlPath, url.dwUrlPathLength);
            hRequest = WinHttpOpenRequest(
                hConnect, wVerb, wQuery.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, dwFlags);
            if (hRequest == nullptr)
                ThrowWin32Exception("WinHttpOpenRequest");

            for (auto header : req.header)
            {
                auto fullHeader = String::ToWideChar(header.first) + L": " + String::ToWideChar(header.second);
                if (!WinHttpAddRequestHeaders(hRequest, fullHeader.c_str(), static_cast<ULONG>(-1L), WINHTTP_ADDREQ_FLAG_ADD))
                    ThrowWin32Exception("WinHttpAddRequestHeaders");
            }

            auto reqBody = reinterpret_cast<LPVOID>(const_cast<char*>(req.body.data()));
            auto reqBodyLen = static_cast<DWORD>(req.body.size());
            if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, reqBody, reqBodyLen, reqBodyLen, 0))
                ThrowWin32Exception("WinHttpSendRequest");

            if (!WinHttpReceiveResponse(hRequest, NULL))
                ThrowWin32Exception("WinHttpReceiveResponse");

            auto statusCode = ReadStatusCode(hRequest);
            auto headers = ReadHeaders(hRequest);
            auto body = ReadBody(hRequest);

            Response response;
            response.body = std::move(body);
            response.status = static_cast<Status>(statusCode);
            auto it = headers.find("Content-Type");
            if (it != headers.end())
            {
                response.content_type = it->second;
            }
            response.header = std::move(headers);

            WinHttpCloseHandle(hSession);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hRequest);
            return response;
        }
        catch ([[maybe_unused]] const std::exception& e)
        {
#    ifdef DEBUG
            Console::Error::WriteLine("HTTP request failed: %s", e.what());
#    endif
            WinHttpCloseHandle(hSession);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hRequest);
            throw;
        }
    }
} // namespace Http

#endif
