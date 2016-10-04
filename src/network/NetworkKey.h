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

#ifndef NETWORKKEY_H
#define NETWORKKEY_H

#ifndef DISABLE_NETWORK

#include "../common.h"

#include <SDL_rwops.h>
#include <string>

typedef struct evp_pkey_st EVP_PKEY;
typedef struct evp_pkey_ctx_st EVP_PKEY_CTX;

class NetworkKey final
{
public:
    NetworkKey();
    ~NetworkKey();
    bool Generate();
    bool LoadPrivate(SDL_RWops * file);
    bool LoadPublic(SDL_RWops * file);
    bool SavePrivate(SDL_RWops * file);
    bool SavePublic(SDL_RWops * file);
    std::string PublicKeyString();
    std::string PublicKeyHash();
    void Unload();
    bool Sign(const uint8 * md, const size_t len, char ** signature, size_t * out_size);
    bool Verify(const uint8 * md, const size_t len, const char * sig, const size_t siglen);
private:
    NetworkKey (const NetworkKey &) = delete;
    EVP_PKEY_CTX * _ctx = nullptr;
    EVP_PKEY *     _key = nullptr;
};

#endif // DISABLE_NETWORK

#endif // NETWORKKEY_H
