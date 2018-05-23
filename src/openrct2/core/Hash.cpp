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

#if defined(_WIN32) && !defined(__USE_OPENSSL__)
#define __USE_CNG__
#endif

#ifdef __USE_CNG__
// CNG: Cryptography API: Next Generation (CNG)
//      available in Windows Vista onwards.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#else
#include <openssl/evp.h>
#endif

class Sha1Algorithm final : public HashAlgorithm<20>
{
private:
#ifdef __USE_CNG__
    BCRYPT_ALG_HANDLE _hAlg{};
    BCRYPT_HASH_HANDLE _hHash{};
    PBYTE _pbHashObject{};
#else
    EVP_MD_CTX * _ctx{};
#endif

public:
    Sha1Algorithm()
    {
#ifdef __USE_CNG__
        // TODO BCRYPT_HASH_REUSABLE_FLAG only available from Windows 8
        auto status = BCryptOpenAlgorithmProvider(&_hAlg, BCRYPT_SHA1_ALGORITHM, nullptr, BCRYPT_HASH_REUSABLE_FLAG);
        if (!NT_SUCCESS(status))
        {
            throw std::runtime_error("BCryptOpenAlgorithmProvider failed: " + std::to_string(status));
        }

        // Calculate the size of the buffer to hold the hash object
        DWORD cbHashObject{};
        DWORD cbData{};
        status = BCryptGetProperty(_hAlg, BCRYPT_OBJECT_LENGTH, (PBYTE)&cbHashObject, sizeof(DWORD), &cbData, 0);
        if (!NT_SUCCESS(status))
        {
            throw std::runtime_error("BCryptGetProperty failed: " + std::to_string(status));
        }

        // Create a hash
        _pbHashObject = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHashObject);
        if (_pbHashObject == nullptr)
        {
            throw std::bad_alloc();
        }
        status = BCryptCreateHash(_hAlg, &_hHash, _pbHashObject, cbHashObject, nullptr, 0, 0);
        if (!NT_SUCCESS(status))
        {
            throw std::runtime_error("BCryptCreateHash failed: " + std::to_string(status));
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
#ifdef __USE_CNG__
        BCryptCloseAlgorithmProvider(_hAlg, 0);
        BCryptDestroyHash(_hHash);
        HeapFree(GetProcessHeap(), 0, _pbHashObject);
#else
        EVP_MD_CTX_destroy(_ctx);
#endif
    }

    void Clear() override
    {
#ifdef __USE_CNG__
        // Finishing the current digest clears the state ready for a new digest
        Finish();
#else
        if (EVP_DigestInit_ex(_ctx, EVP_sha1(), nullptr) <= 0)
        {
            throw std::runtime_error("EVP_DigestInit_ex failed");
        }
#endif
    }

    void Update(const void * data, size_t dataLen) override
    {
#ifdef __USE_CNG__
        auto status = BCryptHashData(_hHash, (PBYTE)data, (ULONG)dataLen, 0);
        if (!NT_SUCCESS(status))
        {
            throw std::runtime_error("BCryptHashData failed: " + std::to_string(status));
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
#ifdef __USE_CNG__
        std::array<uint8_t, 20> result;
        auto status = BCryptFinishHash(_hHash, result.data(), (ULONG)result.size(), 0);
        if (!NT_SUCCESS(status))
        {
            throw std::runtime_error("BCryptFinishHash failed: " + std::to_string(status));
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
