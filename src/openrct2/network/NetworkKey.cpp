/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

#    include "NetworkKey.h"

#    include "../Diagnostic.h"
#    include "../core/Crypt.h"
#    include "../core/IStream.hpp"

#    include <vector>

NetworkKey::NetworkKey() = default;
NetworkKey::~NetworkKey() = default;

void NetworkKey::Unload()
{
    _key = nullptr;
}

bool NetworkKey::Generate()
{
    try
    {
        _key = Crypt::CreateRSAKey();
        _key->Generate();
        return true;
    }
    catch (const std::exception& e)
    {
        log_error("NetworkKey::Generate failed: %s", e.what());
        return false;
    }
}

bool NetworkKey::LoadPrivate(OpenRCT2::IStream* stream)
{
    Guard::ArgumentNotNull(stream);

    size_t size = static_cast<size_t>(stream->GetLength());
    if (size == static_cast<size_t>(-1))
    {
        log_error("unknown size, refusing to load key");
        return false;
    }
    if (size > 4 * 1024 * 1024)
    {
        log_error("Key file suspiciously large, refusing to load it");
        return false;
    }

    std::string pem(size, '\0');
    stream->Read(pem.data(), pem.size());

    try
    {
        _key = Crypt::CreateRSAKey();
        _key->SetPrivate(pem);
        return true;
    }
    catch (const std::exception& e)
    {
        log_error("NetworkKey::LoadPrivate failed: %s", e.what());
        return false;
    }
}

bool NetworkKey::LoadPublic(OpenRCT2::IStream* stream)
{
    Guard::ArgumentNotNull(stream);

    size_t size = static_cast<size_t>(stream->GetLength());
    if (size == static_cast<size_t>(-1))
    {
        log_error("unknown size, refusing to load key");
        return false;
    }
    if (size > 4 * 1024 * 1024)
    {
        log_error("Key file suspiciously large, refusing to load it");
        return false;
    }

    std::string pem(size, '\0');
    stream->Read(pem.data(), pem.size());

    try
    {
        _key = Crypt::CreateRSAKey();
        _key->SetPublic(pem);
        return true;
    }
    catch (const std::exception& e)
    {
        log_error("NetworkKey::LoadPublic failed: %s", e.what());
        return false;
    }
}

bool NetworkKey::SavePrivate(OpenRCT2::IStream* stream)
{
    try
    {
        if (_key == nullptr)
        {
            throw std::runtime_error("No key loaded");
        }
        auto pem = _key->GetPrivate();
        stream->Write(pem.data(), pem.size());
        return true;
    }
    catch (const std::exception& e)
    {
        log_error("NetworkKey::SavePrivate failed: %s", e.what());
        return false;
    }
}

bool NetworkKey::SavePublic(OpenRCT2::IStream* stream)
{
    try
    {
        if (_key == nullptr)
        {
            throw std::runtime_error("No key loaded");
        }
        auto pem = _key->GetPublic();
        stream->Write(pem.data(), pem.size());
        return true;
    }
    catch (const std::exception& e)
    {
        log_error("NetworkKey::SavePublic failed: %s", e.what());
        return false;
    }
}

std::string NetworkKey::PublicKeyString()
{
    if (_key == nullptr)
    {
        throw std::runtime_error("No key loaded");
    }
    return _key->GetPublic();
}

/**
 * @brief NetworkKey::PublicKeyHash
 * Computes a short, human-readable (e.g. asciif-ied hex) hash for a given
 * public key. Serves a purpose of easy identification keys in multiplayer
 * overview, multiplayer settings.
 *
 * In particular, any of digest functions applied to a standardised key
 * representation, like PEM, will be sufficient.
 *
 * @return returns a string containing key hash.
 */
std::string NetworkKey::PublicKeyHash()
{
    try
    {
        std::string key = PublicKeyString();
        if (key.empty())
        {
            throw std::runtime_error("No key found");
        }
        auto hash = Crypt::SHA1(key.c_str(), key.size());

        std::string result;
        result.reserve(hash.size() * 2);
        for (auto b : hash)
        {
            char buf[3];
            snprintf(buf, 3, "%02x", b);
            result.append(buf);
        }
        return result;
    }
    catch (const std::exception& e)
    {
        log_error("Failed to create hash of public key: %s", e.what());
    }
    return nullptr;
}

bool NetworkKey::Sign(const uint8_t* md, const size_t len, std::vector<uint8_t>& signature) const
{
    try
    {
        auto rsa = Crypt::CreateRSA();
        signature = rsa->SignData(*_key, md, len);
        return true;
    }
    catch (const std::exception& e)
    {
        log_error("NetworkKey::Sign failed: %s", e.what());
        return false;
    }
}

bool NetworkKey::Verify(const uint8_t* md, const size_t len, const std::vector<uint8_t>& signature) const
{
    try
    {
        auto rsa = Crypt::CreateRSA();
        return rsa->VerifyData(*_key, md, len, signature.data(), signature.size());
    }
    catch (const std::exception& e)
    {
        log_error("NetworkKey::Verify failed: %s", e.what());
        return false;
    }
}

#endif // DISABLE_NETWORK
