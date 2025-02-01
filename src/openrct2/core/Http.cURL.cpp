/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if !defined(DISABLE_HTTP) && !defined(_WIN32) && !defined(__ANDROID__)

    #include "Http.h"

    #include "../Version.h"
    #include "../core/Console.hpp"

    #include <cstring>
    #include <memory>
    #include <stdexcept>
    #include <thread>

    #if defined(_WIN32) && !defined(WIN32_LEAN_AND_MEAN)
        // cURL includes windows.h, but we don't need all of it.
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <curl/curl.h>

    #define kOpenRCT2UserAgent "OpenRCT2/" kOpenRCT2Version

namespace OpenRCT2::Http
{
    static size_t WriteData(const char* src, size_t size, size_t nmemb, void* userdata)
    {
        size_t realsize = size * nmemb;
        Response* res = static_cast<Response*>(userdata);
        res->body += std::string(src, src + realsize);

        return realsize;
    }

    static size_t HeaderCallback(const char* src, size_t size, size_t nitems, void* userdata)
    {
        size_t realsize = nitems * size;
        Response* res = static_cast<Response*>(userdata);

        auto line = std::string(src, src + realsize);
        auto pos = line.find(':');
        if (pos != std::string::npos)
        {
            std::string key = line.substr(0, pos);
            // subtract 4 chars for ": " and "\r\n"
            std::string value = line.substr(pos + 2, line.size() - pos - 4);
            res->header[key] = value;
        }

        return realsize;
    }

    struct WriteThis
    {
        const char* readptr;
        size_t sizeleft;
    };

    static size_t ReadCallback(void* dst, size_t size, size_t nmemb, void* userp)
    {
        WriteThis* wt = static_cast<WriteThis*>(userp);
        size_t buffer_size = size * nmemb;

        if (wt->sizeleft)
        {
            size_t copy_this_much = wt->sizeleft;
            if (copy_this_much > buffer_size)
                copy_this_much = buffer_size;
            std::memcpy(dst, wt->readptr, copy_this_much);

            wt->readptr += copy_this_much;
            wt->sizeleft -= copy_this_much;
            return copy_this_much;
        }

        return 0;
    }

    Response Do(const Request& req)
    {
        CURL* curl = curl_easy_init();
        std::shared_ptr<void> _(nullptr, [curl](...) { curl_easy_cleanup(curl); });

        if (!curl)
            throw std::runtime_error("Failed to initialize curl");

        Response res;
        WriteThis wt;

        if (req.method == Method::POST || req.method == Method::PUT)
        {
            wt.readptr = req.body.c_str();
            wt.sizeleft = req.body.size();

            curl_easy_setopt(curl, CURLOPT_READFUNCTION, ReadCallback);
            curl_easy_setopt(curl, CURLOPT_READDATA, &wt);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(wt.sizeleft));
        }

        if (req.forceIPv4)
            curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

        if (req.method == Method::POST)
            curl_easy_setopt(curl, CURLOPT_POST, 1L);

        if (req.method == Method::PUT)
            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        curl_easy_setopt(curl, CURLOPT_URL, req.url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void*>(&res));
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, static_cast<void*>(&res));
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, kOpenRCT2UserAgent);

        curl_slist* chunk = nullptr;
        std::shared_ptr<void> __(nullptr, [chunk](...) { curl_slist_free_all(chunk); });
        for (auto header : req.header)
        {
            std::string hs = header.first + ": " + header.second;
            chunk = curl_slist_append(chunk, hs.c_str());
        }
        if (req.header.size() != 0)
        {
            if (chunk == nullptr)
            {
                throw std::runtime_error("Failed to set headers");
            }
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        }

        CURLcode curl_code = curl_easy_perform(curl);
        if (curl_code != CURLE_OK)
        {
            using namespace std::literals;
            throw std::runtime_error(
                "Failed to perform request. curl error code: "s + std::to_string(curl_code) + ": "
                + curl_easy_strerror(curl_code));
        }

        // gets freed by curl_easy_cleanup
        char* content_type;
        long code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
        curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
        res.status = static_cast<Status>(code);
        if (content_type != nullptr)
        {
            res.content_type = std::string(content_type);
        }

        return res;
    }

} // namespace OpenRCT2::Http

#endif
