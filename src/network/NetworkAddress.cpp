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

#ifndef DISABLE_NETWORK

#include <string>
#include <SDL_thread.h>
#include "NetworkAddress.h"

NetworkAddress::NetworkAddress()
{
    _result = std::make_shared<ResolveResult>();
    _result->status = RESOLVE_NONE;
    _resolveMutex = SDL_CreateMutex();
}

NetworkAddress::~NetworkAddress()
{
    SDL_DestroyMutex(_resolveMutex);
}

void NetworkAddress::Resolve(const char * host, uint16 port)
{
    SDL_LockMutex(_resolveMutex);
    {
        // Create a new result store
        _result = std::make_shared<ResolveResult>();
        _result->status = RESOLVE_INPROGRESS;

        // Create a new request
        auto req = new ResolveRequest();
        req->Host = std::string(host == nullptr ? "" : host);
        req->Port = port;;
        req->Result = _result;

        // Resolve synchronously
        ResolveWorker(req);
    }
    SDL_UnlockMutex(_resolveMutex);
}

void NetworkAddress::ResolveAsync(const char * host, uint16 port)
{
    SDL_LockMutex(_resolveMutex);
    {
        // Create a new result store
        _result = std::make_shared<ResolveResult>();
        _result->status = RESOLVE_INPROGRESS;

        // Create a new request
        auto req = new ResolveRequest();
        req->Host = std::string(host);
        req->Port = port;
        req->Result = _result;

        // Spin off a worker thread for resolving the address
        SDL_CreateThread([](void * pointer) -> int
        {
            ResolveWorker((ResolveRequest *)pointer);
            return 0;
        }, 0, req);
    }
    SDL_UnlockMutex(_resolveMutex);
}

NetworkAddress::RESOLVE_STATUS NetworkAddress::GetResult(sockaddr_storage * ss, int * ss_len)
{
    SDL_LockMutex(_resolveMutex);
    {
        const ResolveResult * result = _result.get();
        if (result->status == RESOLVE_OK)
        {
            *ss = result->ss;
            *ss_len = result->ss_len;
        }
        return result->status;
    }
    SDL_UnlockMutex(_resolveMutex);
}

void NetworkAddress::ResolveWorker(ResolveRequest * req)
{
    // Resolve the address
    const char * nodeName = req->Host.c_str();
    std::string serviceName = std::to_string(req->Port);

    addrinfo hints = { 0 };
    hints.ai_family = AF_UNSPEC;
    if (req->Host.empty())
    {
        hints.ai_flags = AI_PASSIVE;
        nodeName = nullptr;
    }

    addrinfo * result;
    getaddrinfo(nodeName, serviceName.c_str(), &hints, &result);

    // Store the result
    ResolveResult * resolveResult = req->Result.get();
    if (result != nullptr)
    {
        resolveResult->status = RESOLVE_OK;
        memcpy(&resolveResult->ss, result->ai_addr, result->ai_addrlen);
        resolveResult->ss_len = result->ai_addrlen;
        freeaddrinfo(result);
    }
    else
    {
        resolveResult->status = RESOLVE_FAILED;
    }
    delete req;
}

#endif
