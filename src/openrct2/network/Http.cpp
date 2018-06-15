/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Http.h"
#include <cstring>
#include <exception>
#include <memory>
#include <thread>

#ifndef DISABLE_HTTP

#include "../Version.h"
#include "../core/Console.hpp"

#ifdef _WIN32
// cURL includes windows.h, but we don't need all of it.
#define WIN32_LEAN_AND_MEAN
#endif
#include <curl/curl.h>

#define OPENRCT2_USER_AGENT "OpenRCT2/" OPENRCT2_VERSION

namespace OpenRCT2::Network::Http
{

Http::Http()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

Http::~Http()
{
    curl_global_cleanup();
}

static size_t writeData(const char * src, size_t size, size_t nmemb, void * userdata)
{
    size_t     realsize = size * nmemb;
    Response * res      = static_cast<Response *>(userdata);
    res->body += std::string(src, src + realsize);

    return realsize;
}

static size_t header_callback(const char * src, size_t size, size_t nitems, void * userdata)
{
    size_t     realsize = nitems * size;
    Response * res      = static_cast<Response *>(userdata);

    auto line = std::string(src, src + realsize);
    auto pos  = line.find(':');
    if (pos != std::string::npos)
    {
        std::string key = line.substr(0, pos);
        // substract 4 chars for ": " and "\r\n"
        std::string value = line.substr(pos + 2, line.size() - pos - 4);
        res->header[key]  = value;
    }

    return realsize;
}

struct WriteThis
{
    const char * readptr;
    size_t       sizeleft;
};

static size_t read_callback(void * dst, size_t size, size_t nmemb, void * userp)
{
    WriteThis * wt          = static_cast<WriteThis *>(userp);
    size_t      buffer_size = size * nmemb;

    if (wt->sizeleft)
    {
        size_t copy_this_much = wt->sizeleft;
        if (copy_this_much > buffer_size)
            copy_this_much = buffer_size;
        memcpy(dst, wt->readptr, copy_this_much);

        wt->readptr += copy_this_much;
        wt->sizeleft -= copy_this_much;
        return copy_this_much;
    }

    return 0;
}

Response Do(const Request & req)
{
    CURL *                curl = curl_easy_init();
    std::shared_ptr<void> _(nullptr, [curl](...) { curl_easy_cleanup(curl); });

    if (!curl)
        std::runtime_error("Failed to initialize curl");

    Response res;

    if (req.method == Method::POST || req.method == Method::PUT)
    {
        WriteThis wt;
        wt.readptr  = req.body.c_str();
        wt.sizeleft = req.body.size();

        curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
        curl_easy_setopt(curl, CURLOPT_READDATA, &wt);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)wt.sizeleft);
    }

    if (req.forceIPv4)
        curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

    if (req.method == Method::POST)
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

    if (req.method == Method::PUT)
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    curl_easy_setopt(curl, CURLOPT_URL, req.url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&res);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void *)&res);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, OPENRCT2_USER_AGENT);

    curl_slist *          chunk = nullptr;
    std::shared_ptr<void> __(nullptr, [chunk](...) { curl_slist_free_all(chunk); });
    for (auto header : req.header)
    {
        std::string hs = header.first + ": " + header.second;
        chunk          = curl_slist_append(chunk, hs.c_str());
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
        throw std::runtime_error("Failed to perform request");
    }

    // gets freed by curl_easy_cleanup
    char * content_type;
    long   code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
    res.status       = static_cast<Status>(code);
    res.content_type = std::string(content_type);

    return res;
}

void DoAsync(const Request & req, std::function<void(Response & res)> fn)
{
    auto thread = std::thread([=]() {
        Response res;
        try
        {
            res = Do(req);
        }
        catch (std::exception & e)
        {
            res.error = e.what();
            return;
        }
        fn(res);
    });
    thread.detach();
}

size_t DownloadPark(const char * url, void ** outData)
{
    // Download park to buffer in memory
    Request request;
    request.url    = url;
    request.method = Method::GET;

    Response res;
    try
    {
        res = Do(request);
        if (res.status != Status::OK)
            std::runtime_error("bad http status");
    }
    catch (std::exception & e)
    {
        Console::Error::WriteLine("Failed to download '%s', cause %s", request.url.c_str(), e.what());
        *outData = nullptr;
        return 0;
    }

    size_t dataSize = res.body.size() - 1;
    void * data     = malloc(dataSize);
    if (data == nullptr)
    {
        Console::Error::WriteLine("Failed to allocate memory for downloaded park.");
        return 0;
    }

    memcpy(data, res.body.c_str(), dataSize);
    *outData = data;

    return dataSize;
}

} // namespace OpenRCT2::Network

#endif
