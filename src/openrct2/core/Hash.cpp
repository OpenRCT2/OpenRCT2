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

#include "Hash.h"
#include <stdexcept>
#include <string>

// #define __USE_WINSSL__

#ifdef __USE_WINSSL__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincrypt.h>
#else
#include <openssl/evp.h>
#endif

class Sha1Algorithm final : public HashAlgorithm<20>
{
private:
#ifdef __USE_WINSSL__
    HCRYPTPROV _hProv{};
    HCRYPTHASH _hHash{};
#else
    EVP_MD_CTX * _ctx{};
#endif

public:
    Sha1Algorithm()
    {
#ifdef __USE_WINSSL__
        if (!CryptAcquireContextW(&_hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
        {
            auto dwStatus = GetLastError();
            throw std::runtime_error("CryptAcquireContext failed : " + std::to_string(dwStatus));
        }

        if (!CryptCreateHash(_hProv, CALG_SHA1, 0, 0, &_hHash))
        {
            auto dwStatus = GetLastError();
            CryptReleaseContext(_hProv, 0);
            throw std::runtime_error("CryptCreateHash failed : " + std::to_string(dwStatus));
        }
#else
        _ctx = EVP_MD_CTX_create();
        if (_ctx == nullptr)
        {
            throw std::runtime_error("EVP_MD_CTX_create failed");
        }
        if (EVP_DigestInit_ex(_ctx, EVP_sha1(), nullptr) <= 0)
        {
            EVP_MD_CTX_destroy(_ctx);
            throw std::runtime_error("EVP_DigestInit_ex failed");
        }
#endif
    }

    ~Sha1Algorithm()
    {
#ifdef __USE_WINSSL__
        CryptDestroyHash(_hHash);
        CryptReleaseContext(_hProv, 0);
#else
        EVP_MD_CTX_destroy(_ctx);
#endif
    }

    void Clear() override
    {
#ifdef __USE_WINSSL__
        CryptDestroyHash(_hHash);
        if (!CryptCreateHash(_hProv, CALG_SHA1, 0, 0, &_hHash))
        {
            auto dwStatus = GetLastError();
            throw std::runtime_error("CryptCreateHash failed : " + std::to_string(dwStatus));
        }
#else
        if (EVP_DigestInit_ex(_ctx, EVP_sha1(), nullptr) <= 0)
        {
            throw std::runtime_error("EVP_DigestInit_ex failed");
        }
#endif
    }

    void Update(const void * data, size_t dataLen) override
    {
#ifdef __USE_WINSSL__
        if (!CryptHashData(_hHash, (const BYTE *)data, (DWORD)dataLen, 0))
        {
            auto dwStatus = GetLastError();
            throw std::runtime_error("CryptHashData failed: " + std::to_string(dwStatus));
        }
#else
        if (EVP_DigestUpdate(_ctx, data, dataLen) <= 0)
        {
            throw std::runtime_error("EVP_DigestUpdate failed");
        }
#endif
    }

    std::array<uint8_t, 20> Finish() override
    {
#ifdef __USE_WINSSL__
        std::array<uint8_t, 20> result;
        auto cbHash = (DWORD)result.size();
        if (!CryptGetHashParam(_hHash, HP_HASHVAL, result.data(), &cbHash, 0))
        {
            auto dwStatus = GetLastError();
            throw std::runtime_error("CryptGetHashParam failed: " + std::to_string(dwStatus));
        }
        return result;
#else
        std::array<uint8_t, 20> result;
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
#endif
    }
};

namespace Hash
{
    std::unique_ptr<HashAlgorithm<20>> CreateSHA1()
    {
        return std::make_unique<Sha1Algorithm>();
    }

    std::array<uint8_t, 20> SHA1(const void * data, size_t dataLen)
    {
        Sha1Algorithm sha1;
        sha1.Update(data, dataLen);
        return sha1.Finish();
    }
}
