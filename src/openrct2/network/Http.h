/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifndef DISABLE_HTTP

#include <functional>
#include <map>
#include <string>

#include "../common.h"

namespace OpenRCT2::Network::Http
{

enum class Status
{
    OK = 200
};

enum class Method
{
    GET,
    POST,
    PUT
};

struct Response
{
    Status                             status;
    std::string                        content_type;
    std::string                        body   = "";
    std::map<std::string, std::string> header = {};
    std::string                        error  = "";
};

struct Request
{
    std::string                        url;
    std::map<std::string, std::string> header    = {};
    Method                             method    = Method::GET;
    std::string                        body      = "";
    bool                               forceIPv4 = false;
};

struct Http
{
    Http();
    ~Http();
};

Response Do(const Request & req);
void     DoAsync(const Request & req, std::function<void(Response & res)> fn);

/**
 * Download a park via HTTP/S from the given URL into a memory buffer. This is
 * a blocking operation.
 * @param url The URL to download the park from.
 * @param outData The data returned.
 * @returns The size of the data or 0 if the download failed.
 */
size_t DownloadPark(const char * url, void ** outData);

} /* namespace http */

#endif // DISABLE_HTTP
