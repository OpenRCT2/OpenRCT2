/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

    #include "NetworkKey.h"

    #include "../Diagnostic.h"
    #include "../core/Crypt.h"
    #include "../core/Guard.hpp"
    #include "../core/IStream.hpp"
    #include "../core/String.hpp"

    #include <vector>

namespace OpenRCT2::Network
{
    Key::Key() = default;
    Key::~Key() = default;

    void Key::Unload()
    {
        _key = nullptr;
    }

    bool Key::Generate()
    {
        try
        {
            _key = Crypt::CreateRSAKey();
            _key->Generate();
            return true;
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Network::Key::Generate failed: %s", e.what());
            return false;
        }
    }

    bool Key::LoadPrivate(IStream* stream)
    {
        Guard::ArgumentNotNull(stream);

        size_t size = static_cast<size_t>(stream->GetLength());
        if (size == static_cast<size_t>(-1))
        {
            LOG_ERROR("unknown size, refusing to load key");
            return false;
        }
        if (size > 4 * 1024 * 1024)
        {
            LOG_ERROR("Key file suspiciously large, refusing to load it");
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
            LOG_ERROR("Network::Key::LoadPrivate failed: %s", e.what());
            return false;
        }
    }

    bool Key::LoadPublic(IStream* stream)
    {
        Guard::ArgumentNotNull(stream);

        size_t size = static_cast<size_t>(stream->GetLength());
        if (size == static_cast<size_t>(-1))
        {
            LOG_ERROR("unknown size, refusing to load key");
            return false;
        }
        if (size > 4 * 1024 * 1024)
        {
            LOG_ERROR("Key file suspiciously large, refusing to load it");
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
            LOG_ERROR("Network::Key::LoadPublic failed: %s", e.what());
            return false;
        }
    }

    bool Key::SavePrivate(IStream* stream)
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
            LOG_ERROR("Network::Key::SavePrivate failed: %s", e.what());
            return false;
        }
    }

    bool Key::SavePublic(IStream* stream)
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
            LOG_ERROR("Network::Key::SavePublic failed: %s", e.what());
            return false;
        }
    }

    std::string Key::PublicKeyString()
    {
        if (_key == nullptr)
        {
            throw std::runtime_error("No key loaded");
        }
        return _key->GetPublic();
    }

    /**
     * @brief Key::PublicKeyHash
     * Computes a short, human-readable (e.g. asciif-ied hex) hash for a given
     * public key. Serves a purpose of easy identification keys in multiplayer
     * overview, multiplayer settings.
     *
     * In particular, any of digest functions applied to a standardised key
     * representation, like PEM, will be sufficient.
     *
     * @return returns a string containing key hash.
     */
    std::string Key::PublicKeyHash()
    {
        try
        {
            std::string key = PublicKeyString();
            if (key.empty())
            {
                throw std::runtime_error("No key found");
            }
            auto hash = Crypt::SHA1(key.c_str(), key.size());
            return String::StringFromHex(hash);
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Failed to create hash of public key: %s", e.what());
        }
        return nullptr;
    }

    bool Key::Sign(const uint8_t* md, const size_t len, std::vector<uint8_t>& signature) const
    {
        try
        {
            auto rsa = Crypt::CreateRSA();
            signature = rsa->SignData(*_key, md, len);
            return true;
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Network::Key::Sign failed: %s", e.what());
            return false;
        }
    }

    bool Key::Verify(const uint8_t* md, const size_t len, const std::vector<uint8_t>& signature) const
    {
        try
        {
            auto rsa = Crypt::CreateRSA();
            return rsa->VerifyData(*_key, md, len, signature.data(), signature.size());
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Network::Key::Verify failed: %s", e.what());
            return false;
        }
    }
} // namespace OpenRCT2::Network

#endif // DISABLE_NETWORK
