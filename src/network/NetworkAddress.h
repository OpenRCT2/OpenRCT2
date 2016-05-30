#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include <memory>
#include <SDL_mutex.h>
#include "NetworkTypes.h"
#include "../common.h"

class NetworkAddress final
{
public:
    enum RESOLVE_STATUS
    {
        RESOLVE_NONE,
        RESOLVE_INPROGRESS,
        RESOLVE_OK,
        RESOLVE_FAILED
    };

    NetworkAddress();
    ~NetworkAddress();

    void Resolve(const char * host, uint16 port);
    void ResolveAsync(const char * host, uint16 port);

    RESOLVE_STATUS GetResult(sockaddr_storage * ss, int * ss_len);

private:
    struct ResolveResult
    {
        RESOLVE_STATUS   status;
        sockaddr_storage ss;
        int              ss_len;
    };

    struct ResolveRequest
    {
        std::string                    Host;
        uint16                         Port;
        std::shared_ptr<ResolveResult> Result;
    };

    /**
     * Store for the async result. A new store is created for every request.
     * Old requests simply write to an old store that will then be
     * automatically deleted by std::shared_ptr.
     */
    std::shared_ptr<ResolveResult> _result;

    /**
     * Mutex so synchronoise the requests.
     */
    SDL_mutex * _resolveMutex;

    static void ResolveWorker(ResolveRequest * req);
};
