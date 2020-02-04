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
#    include "String.hpp"

#    include <cstdio>
#    include <stdexcept>
#    define WIN32_LEAN_AND_MEAN
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
        auto headerBufferLen = (DWORD)std::size(headerBuffer);
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
        int state = 0;
        int index = 0;
        for (auto c : buffer)
        {
            if (c == '\0')
            {
                state = 0;
                if (index != 0 && wKey.size() != 0)
                {
                    auto key = String::ToUtf8(wKey);
                    auto value = String::ToUtf8(wValue);
                    headers[key] = value;
                }
                wKey.clear();
                wValue.clear();
                index++;
                continue;
            }
            if (state == 0 && c == ':')
            {
                state = 1;
            }
            else if (state == 1 && c == ' ')
            {
                state = 2;
            }
            else if (state == 0)
            {
                wKey.push_back(c);
            }
            else
            {
                state = 2;
                wValue.push_back(c);
            }
        }
        return headers;
    }

    static std::string ReadBody(HINTERNET hRequest)
    {
        DWORD dwSize{};
        std::string body;
        do
        {
            // Check for available data.
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
                ThrowWin32Exception("WinHttpQueryDataAvailable");

            // No more available data.
            if (dwSize == 0)
                break;

            auto offset = body.size();
            body.resize(offset + dwSize);
            auto dst = (LPVOID)&body[offset];

            DWORD dwDownloaded{};
            if (!WinHttpReadData(hRequest, dst, dwSize, &dwDownloaded))
                ThrowWin32Exception("WinHttpReadData");

            // This condition should never be reached since WinHttpQueryDataAvailable
            // reported that there are bits to read.
            if (dwDownloaded == 0)
                break;
        } while (dwSize > 0);
        return body;
    }

    Response Do(const Request& req)
    {
        HINTERNET hSession{}, hConnect{}, hRequest{};
        try
        {
            URL_COMPONENTS url{};
            url.dwStructSize = sizeof(url);
            url.dwSchemeLength = (DWORD)-1;
            url.dwHostNameLength = (DWORD)-1;
            url.dwUrlPathLength = (DWORD)-1;
            url.dwExtraInfoLength = (DWORD)-1;

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

            auto wVerb = GetVerb(req.method);
            auto wQuery = std::wstring(url.lpszUrlPath, url.dwUrlPathLength);
            hRequest = WinHttpOpenRequest(
                hConnect, wVerb, wQuery.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
            if (hRequest == nullptr)
                ThrowWin32Exception("WinHttpOpenRequest");

            for (auto header : req.header)
            {
                auto fullHeader = String::ToWideChar(header.first) + L": " + String::ToWideChar(header.second);
                if (!WinHttpAddRequestHeaders(hRequest, fullHeader.c_str(), (ULONG)-1L, WINHTTP_ADDREQ_FLAG_ADD))
                    ThrowWin32Exception("WinHttpAddRequestHeaders");
            }

            auto reqBody = (LPVOID)req.body.data();
            auto reqBodyLen = (DWORD)req.body.size();
            if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, reqBody, reqBodyLen, reqBodyLen, 0))
                ThrowWin32Exception("WinHttpSendRequest");

            if (!WinHttpReceiveResponse(hRequest, NULL))
                ThrowWin32Exception("WinHttpReceiveResponse");

            auto statusCode = ReadStatusCode(hRequest);
            auto headers = ReadHeaders(hRequest);
            auto body = ReadBody(hRequest);

            Response response;
            response.body = std::move(body);
            response.status = (Status)statusCode;
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
            std::fprintf(stderr, "HTTP request failed: %s\n", e.what());
#    endif
            WinHttpCloseHandle(hSession);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hRequest);
            throw;
        }
    }
} // namespace Http

#endif
