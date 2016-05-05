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

#include <SDL2/SDL_rwops.h>
#include <openssl/evp.h>

class NetworkKey
{
public:
    NetworkKey();
    ~NetworkKey();
    bool Generate();
    bool LoadPrivate(SDL_RWops *file);
    bool LoadPublic(SDL_RWops *file);
    bool SavePrivate(SDL_RWops *file);
    bool SavePublic(SDL_RWops *file);
    char *PublicKeyString();
    void Unload();
    bool Sign(const char *md, const size_t len, char **signature, size_t *out_size);
    bool Verify(const char *md, const size_t len, const char* sig, const size_t siglen);
private:
    EVP_PKEY_CTX *m_ctx = nullptr;
    EVP_PKEY *m_key = nullptr;
};

#endif // NETWORKKEY_H
