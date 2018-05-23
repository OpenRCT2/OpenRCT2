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

#ifdef __USE_CNG__

#include "Crypt.h"
#include "../platform/Platform2.h"
#include <stdexcept>
#include <string>

// CNG: Cryptography API: Next Generation (CNG)
//      available in Windows Vista onwards.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

class CNGSha1Algorithm final : public Sha1Algorithm
{
private:
    BCRYPT_ALG_HANDLE _hAlg{};
    BCRYPT_HASH_HANDLE _hHash{};
    PBYTE _pbHashObject{};
    bool _reusable{};

public:
    CNGSha1Algorithm()
    {
        // BCRYPT_HASH_REUSABLE_FLAG only available from Windows 8
        _reusable = Platform::IsOSVersionAtLeast(6, 2, 0);
        Initialise();
    }

    ~CNGSha1Algorithm()
    {
        Dispose();
    }

    void Clear() override
    {
        if (_reusable)
        {
            // Finishing the current digest clears the state ready for a new digest
            Finish();
        }
        else
        {
            Dispose();
            Initialise();
        }
    }

    void Update(const void * data, size_t dataLen) override
    {
        auto status = BCryptHashData(_hHash, (PBYTE)data, (ULONG)dataLen, 0);
        if (!NT_SUCCESS(status))
        {
            throw std::runtime_error("BCryptHashData failed: " + std::to_string(status));
        }
    }

    Result Finish() override
    {
        Result result;
        auto status = BCryptFinishHash(_hHash, result.data(), (ULONG)result.size(), 0);
        if (!NT_SUCCESS(status))
        {
            throw std::runtime_error("BCryptFinishHash failed: " + std::to_string(status));
        }
        return result;
    }

private:
    void Initialise()
    {
        auto flags = _reusable ? BCRYPT_HASH_REUSABLE_FLAG : 0;
        auto status = BCryptOpenAlgorithmProvider(&_hAlg, BCRYPT_SHA1_ALGORITHM, nullptr, flags);
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
    }

    void Dispose()
    {
        BCryptCloseAlgorithmProvider(_hAlg, 0);
        BCryptDestroyHash(_hHash);
        HeapFree(GetProcessHeap(), 0, _pbHashObject);

        _hAlg = {};
        _hHash = {};
        _pbHashObject = {};
    }
};

namespace Hash
{
    std::unique_ptr<Sha1Algorithm> CreateSHA1()
    {
        return std::make_unique<CNGSha1Algorithm>();
    }

    Sha1Algorithm::Result SHA1(const void * data, size_t dataLen)
    {
        CNGSha1Algorithm sha1;
        sha1.Update(data, dataLen);
        return sha1.Finish();
    }
}

#endif
