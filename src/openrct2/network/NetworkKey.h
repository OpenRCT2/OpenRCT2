/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifndef DISABLE_NETWORK

    #include <cstdint>
    #include <memory>
    #include <string>
    #include <vector>

namespace OpenRCT2
{
    struct IStream;
}

namespace OpenRCT2::Crypt
{
    class RsaKey;
}

namespace OpenRCT2::Network
{
    class Key final
    {
    public:
        Key();
        ~Key();
        bool Generate();
        bool LoadPrivate(IStream* stream);
        bool LoadPublic(IStream* stream);
        bool SavePrivate(IStream* stream);
        bool SavePublic(IStream* stream);
        std::string PublicKeyString();
        std::string PublicKeyHash();
        void Unload();
        bool Sign(const uint8_t* md, size_t len, std::vector<uint8_t>& signature) const;
        bool Verify(const uint8_t* md, size_t len, const std::vector<uint8_t>& signature) const;

    private:
        Key(const Key&) = delete;
        std::unique_ptr<Crypt::RsaKey> _key;
    };
} // namespace OpenRCT2::Network

#endif // DISABLE_NETWORK
