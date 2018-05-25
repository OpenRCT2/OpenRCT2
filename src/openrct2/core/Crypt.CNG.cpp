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
#include <tuple>

// CNG: Cryptography API: Next Generation (CNG)
//      available in Windows Vista onwards.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#include <ncrypt.h>
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

static void CngThrowOnBadStatus(const std::string_view& name, NTSTATUS status)
{
    if (!NT_SUCCESS(status))
    {
        throw std::runtime_error(std::string(name) + " failed: " + std::to_string(status));
    }
}

static void ThrowBadAllocOnNull(const void * ptr)
{
    if (ptr == nullptr)
    {
        throw std::bad_alloc();
    }
}

template<typename TBase>
class CngHashAlgorithm final : public TBase
{
private:
    const wchar_t * _algName;
    BCRYPT_ALG_HANDLE _hAlg{};
    BCRYPT_HASH_HANDLE _hHash{};
    PBYTE _pbHashObject{};
    bool _reusable{};

public:
    CngHashAlgorithm(const wchar_t * algName)
    {
        // BCRYPT_HASH_REUSABLE_FLAG only available from Windows 8
        _algName = algName;
        _reusable = Platform::IsOSVersionAtLeast(6, 2, 0);
        Initialise();
    }

    ~CngHashAlgorithm()
    {
        Dispose();
    }

    TBase * Clear() override
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
        return this;
    }

    TBase * Update(const void * data, size_t dataLen) override
    {
        auto status = BCryptHashData(_hHash, (PBYTE)data, (ULONG)dataLen, 0);
        CngThrowOnBadStatus("BCryptHashData", status);
        return this;
    }

    typename TBase::Result Finish() override
    {
        typename TBase::Result result;
        auto status = BCryptFinishHash(_hHash, result.data(), (ULONG)result.size(), 0);
        CngThrowOnBadStatus("BCryptFinishHash", status);
        return result;
    }

private:
    void Initialise()
    {
        auto flags = _reusable ? BCRYPT_HASH_REUSABLE_FLAG : 0;
        auto status = BCryptOpenAlgorithmProvider(&_hAlg, _algName, nullptr, flags);
        CngThrowOnBadStatus("BCryptOpenAlgorithmProvider", status);

        // Calculate the size of the buffer to hold the hash object
        DWORD cbHashObject{};
        DWORD cbData{};
        status = BCryptGetProperty(_hAlg, BCRYPT_OBJECT_LENGTH, (PBYTE)&cbHashObject, sizeof(DWORD), &cbData, 0);
        CngThrowOnBadStatus("BCryptGetProperty", status);

        // Create a hash
        _pbHashObject = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHashObject);
        ThrowBadAllocOnNull(_pbHashObject);
        status = BCryptCreateHash(_hAlg, &_hHash, _pbHashObject, cbHashObject, nullptr, 0, 0);
        CngThrowOnBadStatus("BCryptCreateHash", status);
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

class CngRsaKey final : public RsaKey
{
public:
    NCRYPT_KEY_HANDLE GetKeyHandle() const { return _hKey; }

    void SetPrivate(const std::string_view& pem) override
    {
        SetKey(pem, true);
    }

    void SetPublic(const std::string_view& pem) override
    {
        SetKey(pem, false);
    }

    std::string GetPrivate() override { return GetKey(true); }

    std::string GetPublic() override { return GetKey(false); }

private:
    NCRYPT_KEY_HANDLE _hKey{};

    void SetKey(const std::string_view& pem, bool isPrivate)
    {
        throw std::runtime_error("Not implemented");
    }

    std::string GetKey(bool isPrivate)
    {
        throw std::runtime_error("Not implemented");
    }
};

class CngRsaAlgorithm final : public RsaAlgorithm
{
public:
    std::vector<uint8_t> SignData(const RsaKey& key, const void * data, size_t dataLen) override
    {
        auto hKey = static_cast<const CngRsaKey&>(key).GetKeyHandle();
        auto [cbHash, pbHash] = HashData(data, dataLen);
        auto [cbSignature, pbSignature] = std::tuple<DWORD, PBYTE>();
        try
        {
            BCRYPT_PKCS1_PADDING_INFO paddingInfo{ BCRYPT_SHA256_ALGORITHM };
            auto status = NCryptSignHash(hKey, &paddingInfo, pbHash, cbHash, NULL, 0, &cbSignature, BCRYPT_PAD_PKCS1);
            CngThrowOnBadStatus("NCryptSignHash", status);
            pbSignature = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbSignature);
            ThrowBadAllocOnNull(pbSignature);
            status = NCryptSignHash(hKey, &paddingInfo, pbHash, cbHash, pbSignature, cbSignature, &cbSignature, BCRYPT_PAD_PKCS1);
            CngThrowOnBadStatus("NCryptSignHash", status);

            auto result = std::vector<uint8_t>(pbSignature, pbSignature + cbSignature);
            HeapFree(GetProcessHeap(), 0, pbSignature);
            return result;
        }
        catch (std::exception&)
        {
            HeapFree(GetProcessHeap(), 0, pbHash);
            HeapFree(GetProcessHeap(), 0, pbSignature);
            throw;
        }
    }

    bool VerifyData(const RsaKey& key, const void * data, size_t dataLen, const void * sig, size_t sigLen) override
    {
        auto hKey = static_cast<const CngRsaKey&>(key).GetKeyHandle();
        auto [cbHash, pbHash] = HashData(data, dataLen);
        auto [cbSignature, pbSignature] = ToHeap(sig, sigLen);

        BCRYPT_PKCS1_PADDING_INFO paddingInfo { BCRYPT_SHA256_ALGORITHM };
        auto status = NCryptVerifySignature(hKey, &paddingInfo, pbHash, cbHash, pbSignature, cbSignature, BCRYPT_PAD_PKCS1);
        HeapFree(GetProcessHeap(), 0, pbSignature);
        return status == ERROR_SUCCESS;
    }

private:
    static std::tuple<DWORD, PBYTE> HashData(const void * data, size_t dataLen)
    {
        auto hash = Hash::SHA256(data, dataLen);
        return ToHeap(hash.data(), hash.size());
    }

    static std::tuple<DWORD, PBYTE> ToHeap(const void * data, size_t dataLen)
    {
        auto cbHash = (DWORD)dataLen;
        auto pbHash = (PBYTE)HeapAlloc(GetProcessHeap(), 0, dataLen);
        ThrowBadAllocOnNull(pbHash);
        std::memcpy(pbHash, data, dataLen);
        return std::make_tuple(cbHash, pbHash);
    }
};

namespace Hash
{
    std::unique_ptr<Sha1Algorithm> CreateSHA1()
    {
        return std::make_unique<CngHashAlgorithm<Sha1Algorithm>>(BCRYPT_SHA1_ALGORITHM);
    }

    std::unique_ptr<Sha256Algorithm> CreateSHA256()
    {
        return std::make_unique<CngHashAlgorithm<Sha256Algorithm>>(BCRYPT_SHA256_ALGORITHM);
    }

    std::unique_ptr<RsaAlgorithm> CreateRSA()
    {
        return std::make_unique<CngRsaAlgorithm>();
    }

    std::unique_ptr<RsaKey> CreateRSAKey()
    {
        return std::make_unique<CngRsaKey>();
    }
}

#endif
