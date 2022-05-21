/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifndef DISABLE_NETWORK

#    include "../common.h"

#    include <memory>
#    include <string>
#    include <vector>

namespace OpenRCT2
{
    struct IStream;
}

namespace Crypt
{
    class RsaKey;
}

class NetworkKey final
{
public:
    NetworkKey();
    ~NetworkKey();
    bool Generate();
    bool LoadPrivate(OpenRCT2::IStream* stream);
    bool LoadPublic(OpenRCT2::IStream* stream);
    bool SavePrivate(OpenRCT2::IStream* stream);
    bool SavePublic(OpenRCT2::IStream* stream);
    std::string PublicKeyString();
    std::string PublicKeyHash();
    void Unload();
    bool Sign(const uint8_t* md, const size_t len, std::vector<uint8_t>& signature) const;
    bool Verify(const uint8_t* md, const size_t len, const std::vector<uint8_t>& signature) const;

private:
    NetworkKey(const NetworkKey&) = delete;
    std::unique_ptr<Crypt::RsaKey> _key;
};

#endif // DISABLE_NETWORK
