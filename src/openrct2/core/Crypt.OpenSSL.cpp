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
#include <vector>
#include <openssl/evp.h>

static void OpenSSLThrowOnBadStatus(const std::string_view& name, int status)
{
    if (status != 1)
    {
        throw std::runtime_error(std::string(name) + " failed: " + std::to_string(status));
    }
}

template<typename TBase>
class OpenSSLHashAlgorithm final : public TBase
{
private:
    const EVP_MD * _type;
    EVP_MD_CTX * _ctx{};
    bool _initialised{};

public:
    OpenSSLHashAlgorithm(const EVP_MD * type)
    {
        _type = type;
        _ctx = EVP_MD_CTX_create();
        if (_ctx == nullptr)
        {
            throw std::runtime_error("EVP_MD_CTX_create failed");
        }
    }

    ~OpenSSLHashAlgorithm()
    {
        EVP_MD_CTX_destroy(_ctx);
    }

    TBase * Clear() override
    {
        if (EVP_DigestInit_ex(_ctx, _type, nullptr) <= 0)
        {
            throw std::runtime_error("EVP_DigestInit_ex failed");
        }
        _initialised = true;
        return this;
    }

    TBase * Update(const void * data, size_t dataLen) override
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
        return this;
    }

    typename TBase::Result Finish() override
    {
        if (!_initialised)
        {
            throw std::runtime_error("No data to hash.");
        }
        _initialised = false;

        typename TBase::Result result;
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

class OpenSSLRsaKey final : public RsaKey
{
public:
    EVP_PKEY * const EvpKey{};

    void SetPrivate(const std::string_view& pem) override { }
    void SetPublic(const std::string_view& pem) override { }
    std::string GetPrivate() override { return ""; }
    std::string GetPublic() override { return ""; }
};

class OpenSSLRsaAlgorithm final : public RsaAlgorithm
{
public:
    std::vector<uint8_t> SignData(const RsaKey& key, const void * data, size_t dataLen) override
    {
        auto evpKey = static_cast<const OpenSSLRsaKey&>(key).EvpKey;
        EVP_MD_CTX * mdctx{};
        try
        {
            mdctx = EVP_MD_CTX_create();
            if (mdctx == nullptr)
            {
                throw std::runtime_error("EVP_MD_CTX_create failed");
            }

            auto status = EVP_DigestSignInit(mdctx, nullptr, EVP_sha256(), nullptr, evpKey);
            OpenSSLThrowOnBadStatus("EVP_DigestSignInit failed", status);

            status = EVP_DigestSignUpdate(mdctx, data, dataLen);
            OpenSSLThrowOnBadStatus("EVP_DigestSignUpdate failed", status);

            // Get required length of signature
            size_t sigLen{};
            status = EVP_DigestSignFinal(mdctx, nullptr, &sigLen);
            OpenSSLThrowOnBadStatus("EVP_DigestSignFinal failed", status);

            // Get signature
            std::vector<uint8_t> signature(sigLen);
            status = EVP_DigestSignFinal(mdctx, signature.data(), &sigLen);
            OpenSSLThrowOnBadStatus("EVP_DigestSignFinal failed", status);

            EVP_MD_CTX_destroy(mdctx);
            return signature;
        }
        catch (const std::exception&)
        {
            EVP_MD_CTX_destroy(mdctx);
            throw;
        }
    }

    bool VerifyData(const RsaKey& key, const void * data, size_t dataLen, const void * sig, size_t sigLen) override
    {
        auto evpKey = static_cast<const OpenSSLRsaKey&>(key).EvpKey;
        EVP_MD_CTX * mdctx{};
        try
        {
            mdctx = EVP_MD_CTX_create();
            if (mdctx == nullptr)
            {
                throw std::runtime_error("EVP_MD_CTX_create failed");
            }

            auto status = EVP_DigestVerifyInit(mdctx, nullptr, EVP_sha256(), nullptr, evpKey);
            OpenSSLThrowOnBadStatus("EVP_DigestVerifyInit", status);

            status = EVP_DigestVerifyUpdate(mdctx, data, dataLen);
            OpenSSLThrowOnBadStatus("EVP_DigestVerifyUpdate", status);

            status = EVP_DigestVerifyFinal(mdctx, (uint8_t*)sig, sigLen);
            if (status != 0 && status != 1)
            {
                OpenSSLThrowOnBadStatus("EVP_DigestVerifyUpdate", status);
            }
            EVP_MD_CTX_destroy(mdctx);
            return status == 0;
        }
        catch (const std::exception&)
        {
            EVP_MD_CTX_destroy(mdctx);
            throw;
        }
    }
};

namespace Hash
{
    std::unique_ptr<Sha1Algorithm> CreateSHA1()
    {
        return std::make_unique<OpenSSLHashAlgorithm<Sha1Algorithm>>(EVP_sha1());
    }

    std::unique_ptr<Sha256Algorithm> CreateSHA256()
    {
        return std::make_unique<OpenSSLHashAlgorithm<Sha256Algorithm>>(EVP_sha256());
    }

    std::unique_ptr<RsaAlgorithm> CreateRSA()
    {
        return std::make_unique<OpenSSLRsaAlgorithm>();
    }

    std::unique_ptr<RsaKey> CreateRSAKey()
    {
        return std::make_unique<OpenSSLRsaKey>();
    }
}

#endif
