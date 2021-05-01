/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ChecksumStream.h"

namespace OpenRCT2
{
#ifndef DISABLE_NETWORK
    static std::unique_ptr<Crypt::Sha1Algorithm> _spriteHashAlg;

    ChecksumStream::ChecksumStream()
    {
        if (!_spriteHashAlg)
        {
            _spriteHashAlg = Crypt::CreateSHA1();
        }
        _spriteHashAlg->Clear();
    }

    void ChecksumStream::Write(const void* buffer, uint64_t length)
    {
        _spriteHashAlg->Update(buffer, length);
    }

    Crypt::Sha1Algorithm::Result ChecksumStream::Finish()
    {
        return _spriteHashAlg->Finish();
    }
#endif
} // namespace OpenRCT2
