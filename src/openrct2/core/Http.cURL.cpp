/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if !defined(DISABLE_HTTP) && !defined(_WIN32) && !defined(__ANDROID__)

    #include "Http.h"

    #include "../Version.h"

    #include <chrono>
    #include <cstring>
    #include <memory>
    #include <stdexcept>
    #include <string>

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

    struct TransferActivity
    {
        std::chrono::steady_clock::time_point lastProgress;
        std::chrono::seconds timeout;
        curl_off_t bytesTransferred{};
        bool stalled{};
    };

    /**
     * Aborts the transfer when no bytes have been sent or received for the request's timeout.
     * Unlike a total transfer time limit this never cuts off a slow but progressing download,
     * and unlike cURL's low speed options it does not average over a window of several seconds.
     */
    static int AbortWhenStalled(void* userdata, curl_off_t, curl_off_t dlnow, curl_off_t, curl_off_t ulnow)
    {
        auto* activity = static_cast<TransferActivity*>(userdata);
        auto now = std::chrono::steady_clock::now();
        auto bytesTransferred = dlnow + ulnow;
        if (bytesTransferred != activity->bytesTransferred)
        {
            activity->bytesTransferred = bytesTransferred;
            activity->lastProgress = now;
        }
        else if (now - activity->lastProgress >= activity->timeout)
        {
            activity->stalled = true;
            return 1;
        }
        return 0;
    }

    #if LIBCURL_VERSION_NUM >= 0x075000
    /**
     * Restarts the stall clock once the connection is ready and the request is about to be sent,
     * so that resolving, connecting and the TLS handshake do not eat into the time the server gets
     * to answer. libcurl before 7.80.0 has no such hook and charges those phases to that budget.
     */
    static int RestartStallClock(void* userdata, char*, char*, int, int)
    {
        auto* activity = static_cast<TransferActivity*>(userdata);
        activity->lastProgress = std::chrono::steady_clock::now();
        return CURL_PREREQFUNC_OK;
    }
    #endif

    Response Do(const Request& req)
    {
        try
        {
            CURL* curl = curl_easy_init();
            std::shared_ptr<void> _(nullptr, [curl](...) { curl_easy_cleanup(curl); });

            if (!curl)
                throw std::runtime_error("Failed to initialize curl");

            Response res;
            WriteThis wt;

            if (req.method == Method::post || req.method == Method::put)
            {
                wt.readptr = req.body.c_str();
                wt.sizeleft = req.body.size();

                curl_easy_setopt(curl, CURLOPT_READFUNCTION, ReadCallback);
                curl_easy_setopt(curl, CURLOPT_READDATA, &wt);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(wt.sizeleft));
            }

            if (req.forceIPv4)
                curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

            if (req.method == Method::post)
                curl_easy_setopt(curl, CURLOPT_POST, 1L);

            if (req.method == Method::put)
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

            auto timeoutSeconds = static_cast<long>(std::chrono::duration_cast<std::chrono::seconds>(req.timeout).count());
            TransferActivity activity{ std::chrono::steady_clock::now(), req.timeout };
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeoutSeconds);
            // Timeouts must not be enforced with signals in a multithreaded process.
            curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
            curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, AbortWhenStalled);
            curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &activity);
    #if LIBCURL_VERSION_NUM >= 0x075000
            curl_easy_setopt(curl, CURLOPT_PREREQFUNCTION, RestartStallClock);
            curl_easy_setopt(curl, CURLOPT_PREREQDATA, &activity);
    #endif

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
            if (activity.stalled)
            {
                throw std::runtime_error(
                    "Failed to perform request. No data was transferred for " + std::to_string(timeoutSeconds) + " seconds.");
            }
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
        catch (const std::exception& e)
        {
            Response response;
            response.status = Status::error;
            response.error = e.what();
            return response;
        }
    }

} // namespace OpenRCT2::Http

#endif
