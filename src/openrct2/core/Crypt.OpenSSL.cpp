#pragma region Copyright (c) 2018 OpenRCT2 Developers
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

#if defined(_WIN32) && !defined(__USE_OPENSSL__)
#define __USE_CNG__
#endif

#ifndef __USE_CNG__

#include "Crypt.h"
#include <stdexcept>
#include <string>
#include <openssl/evp.h>

class OpenSSLSha1Algorithm final : public Sha1Algorithm
{
private:
    EVP_MD_CTX * _ctx{};
    bool _initialised{};

public:
    OpenSSLSha1Algorithm()
    {
        _ctx = EVP_MD_CTX_create();
        if (_ctx == nullptr)
        {
            throw std::runtime_error("EVP_MD_CTX_create failed");
        }
    }

    ~OpenSSLSha1Algorithm()
    {
        EVP_MD_CTX_destroy(_ctx);
    }

    void Clear() override
    {
        if (EVP_DigestInit_ex(_ctx, EVP_sha1(), nullptr) <= 0)
        {
            throw std::runtime_error("EVP_DigestInit_ex failed");
        }
        _initialised = true;
    }

    void Update(const void * data, size_t dataLen) override
    {
        // Auto initialise
        if (!_initialised)
        {
            Clear();
        }

        if (EVP_DigestUpdate(_ctx, data, dataLen) <= 0)
        {
            throw std::runtime_error("EVP_DigestUpdate failed");
        }
    }

    Result Finish() override
    {
        if (!_initialised)
        {
            throw std::runtime_error("No data to hash.");
        }
        _initialised = false;

        Result result;
        unsigned int digestSize{};
        if (EVP_DigestFinal(_ctx, result.data(), &digestSize) <= 0)
        {
            EVP_MD_CTX_destroy(_ctx);
            throw std::runtime_error("EVP_DigestFinal failed");
        }
        if (digestSize != result.size())
        {
            throw std::runtime_error("Expected digest size to equal " + std::to_string(result.size()));
        }
        return result;
    }
};

namespace Hash
{
    std::unique_ptr<Sha1Algorithm> CreateSHA1()
    {
        return std::make_unique<OpenSSLSha1Algorithm>();
    }

    Sha1Algorithm::Result SHA1(const void * data, size_t dataLen)
    {
        OpenSSLSha1Algorithm sha1;
        sha1.Update(data, dataLen);
        return sha1.Finish();
    }
}

#endif
